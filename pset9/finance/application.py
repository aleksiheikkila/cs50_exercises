import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, represents_int, password_fulfills_criteria

from datetime import datetime

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter. Formats a string as "usd"
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


def get_cash(user_id: int) -> float:
    rows = db.execute("SELECT cash FROM users where id = ?", user_id)
    if len(rows) != 1:
        return apology("User not found??? (you shouldn't end up here)", 400)

    return rows[0]["cash"]


def get_portfolio(user_id: int, get_only_holdings: bool = False):
    """Returns portfolio_data (list of dicts), cash available, and grand total value of the portfolio for the given user_id"""

    # TODO: might get heavy over time
    # For current user, get all transactions. Group by symbol and sum the shares
    # Keep those where the sum is > 0
    rows = db.execute(
        "SELECT symbol, sum(shares) as shares FROM transactions WHERE user_id = ? GROUP BY symbol HAVING sum(shares) > 0", user_id)

    grand_total = 0
    portfolio_data = []  # list of dicts
    for holding_dict in rows:
        symbol_data = {"symbol": holding_dict["symbol"],
                       "shares": holding_dict["shares"]}
        quote = lookup(holding_dict["symbol"])
        if quote:
            symbol_data["current_price"] = quote["price"]
            symbol_data["total_value"] = round(quote["price"] * holding_dict["shares"], 2)
            grand_total += quote["price"] * holding_dict["shares"]
        portfolio_data.append(symbol_data)

    if get_only_holdings:
        return portfolio_data, None, None

    # Get cash available
    cash = get_cash(user_id)

    grand_total += cash

    return portfolio_data, cash, grand_total


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user_id = session.get("user_id")
    portfolio_data, cash, grand_total = get_portfolio(user_id)

    return render_template("portfolio.html",
                           portfolio_data=portfolio_data,
                           cash=round(cash, 2),
                           grand_total=round(grand_total, 2))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        # do buy
        # validate input
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")
        if not symbol:
            return apology("must provide stock symbol", 400)
        if not shares:
            return apology("must provide number of shares to buy", 400)
        shares = shares.strip()
        if not represents_int(shares):
            return apology("must provide positive integer for shares", 400)
        shares = int(shares)
        if shares < 1:
            return apology("must provide positive integer for shares", 400)

        # Check that symbol is real
        quote_data = lookup(symbol)
        if not quote_data:
            return apology(f"Symbol {symbol} not found", 400)

        # price per share
        share_price = quote_data["price"]

        # cash available to the user
        user_id = session.get("user_id")
        if not user_id:
            return apology("Not logged in??? (you shouldn't end up here)", 400)

        # check that user has enough cash
        rows = db.execute("SELECT cash FROM users where id = ?", user_id)
        if len(rows) != 1:
            return apology("User not found??? (you shouldn't end up here)", 400)

        cash = rows[0]["cash"]
        cash_after = cash - (shares * share_price)
        if cash_after < 0:
            return apology(f"Not enough cash to complete the purchase (cash: {cash}, needed: {shares * share_price}", 400)

        # OK, complete the purchase
        db.execute("INSERT INTO transactions (user_id, symbol, shares, share_price, total_value, type) VALUES(?, ?, ?, ?, ?, ?)",
                   user_id, symbol, shares, share_price, round(shares * share_price, 2), "B")
        # uses now current_timestamp default at the DB side

        # update cash
        db.execute("UPDATE users SET cash = ? where id = ?", cash_after, user_id)

        return redirect("/")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    # Get transactions
    user_id = session.get("user_id")
    transaction_data = db.execute("SELECT * FROM transactions WHERE user_id = ? ORDER BY ts ASC", user_id)

    return render_template("history.html", transaction_data=transaction_data)

    return apology("TODO")


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 400)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]
        # Added also name
        session["username"] = rows[0]["username"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    symbol = request.form.get("symbol")
    if request.method == "POST":
        if not symbol:
            return apology("Must provide a stock symbol", 400)
        quote_data = lookup(symbol)

        # TODO: Need to check something?
        if not quote_data:
            # Nothing returned / not found
            # flash(f"Symbol {symbol} not found!")
            # return redirect("/quote")
            return apology(f"Symbol {symbol} not found", 400)

        return render_template("quoted.html", quote_data=quote_data)

    else:
        return render_template("quote.html")


def validate_password(password, confirmation):
    """Passwords can be None or string"""
    # A bit hacky fix...
    # Ensure password was submitted
    if not password or password != confirmation:
        return False, "Must provide password / passwords need to match", 400
    else:
        return True, None, None


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("Must provide username", 400)

        # Ensure password was submitted
        pwd = request.form.get("password")
        valid, msg, code = validate_password(pwd, request.form.get("confirmation"))  # tODO: hacky... same in change password
        if not valid:
            return apology(msg, code)
        #if not password_fulfills_criteria(pwd):
        #    flash("Password must be at least 8 characters long, with at least one lowercase, one uppercase, one digit")
        #    return redirect("/change_password")

        # Is username free?
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))
        if len(rows) > 0:
            return apology("Username already in use", 400)

        # Insert into DB
        rows = db.execute("INSERT INTO users (username, hash) VALUES(?, ?)",
                          request.form.get("username"),
                          generate_password_hash(pwd))

        # TODO: or sign in
        return redirect("/login")

    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    user_id = session.get("user_id")
    portfolio_data, _, _ = get_portfolio(user_id, get_only_holdings=True)

    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")
        if not symbol:
            return apology("Must provide a symbol", 400)
        if not shares:
            return apology("must provide number of shares to sell", 400)
        shares = shares.strip()
        if not represents_int(shares):
            return apology("must provide positive integer for shares", 400)
        shares = int(shares)
        if shares < 1:
            return apology("must provide positive integer for shares", 400)

        # make sure that user has that many shares
        symbol_found = False
        for holding_dict in portfolio_data:
            if holding_dict["symbol"] == symbol:
                symbol_found = True
                if holding_dict["shares"] < shares:
                    return apology(f"You don't own that many shares (you own: {holding_dict['shares']}, tried to sell: {shares}", 400)
                break
        if not symbol_found:
            return apology(f"You don't seem to own shares in {symbol}", 400)

        # Execute the sale

        quote_data = lookup(symbol)
        if not quote_data:
            return apology(f"Symbol {symbol} not found", 400)
        share_price = quote_data["price"]

        cash_after = get_cash(user_id) + shares * share_price

        #db.execute("INSERT INTO transactions (user_id, symbol, shares, ts) VALUES(?, ?, ?, ?)", user_id, symbol, -1 * shares, datetime.now())
        db.execute("INSERT INTO transactions (user_id, symbol, shares, share_price, total_value, type) VALUES(?, ?, ?, ?, ?, ?)",
                   user_id, symbol, -1 * shares, share_price, round(-1 * shares * share_price, 2), "S")
        # uses default ts = CURRENT_TIMESTAMP at DB side

        # reduce cash
        db.execute("UPDATE users SET cash = ? where id = ?", cash_after, user_id)

        return redirect("/")

    else:
        return render_template("sell.html", portfolio_data=portfolio_data)


@app.route("/change_password", methods=["POST", "GET"])
@login_required
def change_password():
    if request.method == "POST":
        user_id = session.get("user_id")
        if not user_id:
            return apology("Not logged in??? (you shouldn't end up here)", 400)

        pwd = request.form.get("password")
        valid, msg, code = validate_password(pwd, request.form.get("confirmation"))
        if not valid:
            return apology(msg, code)
        #if not password_fulfills_criteria(pwd):
        #    flash("Password must be at least 8 characters long, with at least one lowercase, one uppercase, one digit")
        #    return redirect("/change_password")

        # Insert into DB
        db.execute("UPDATE users SET hash = ? where id = ?", generate_password_hash(pwd), user_id)

        flash(f"Password changed!")
        return redirect("/")

    else:
        return render_template("change_password.html")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)


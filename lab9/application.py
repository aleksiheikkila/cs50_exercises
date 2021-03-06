import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")

@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":

        # TODO: Add the user's entry into the database
        # Does not check for the validity of the given date

        db.execute("INSERT INTO birthdays (name, month, day) VALUES(?, ?, ?)",
                    request.form.get("name"),
                    request.form.get("month"),
                    request.form.get("day"))

        return redirect("/")

    else:

        # TODO: Display the entries in the database on index.html
        # Query DB and pass the data to the index page
        birthdays = db.execute("SELECT * FROM birthdays")

        return render_template("index.html", birthdays=birthdays)

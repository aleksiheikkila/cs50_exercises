from cs50 import get_int


def main():
    # get int
    number = get_int("Number: ")
    number_str = str(number)

    # There is a minimum length
    if len(number_str) < 13:
        print("INVALID")
        return

    # Calculate the luhn sum
    luhn_sum = 0
    for i, c in enumerate(number_str[::-1]):
        if i % 2 == 0:
            luhn_sum += int(c)
        else:
            doubled = 2 * int(c)
            if doubled > 9:
                luhn_sum += (1 + (doubled - 10))
            else:
                luhn_sum += doubled

    # Check for validity as per Luhns algo
    if (luhn_sum % 10) != 0:
        print("INVALID")
        return

    # Check card type
    first_two = number_str[:2]

    if first_two in ("34", "37"):
        print("AMEX")
    elif first_two in ("51", "52", "53", "54", "55"):
        print("MASTERCARD")
    elif first_two[0] == "4":
        print("VISA")
    else:
        # For this exercise, consider rest invalid
        print("INVALID")


if __name__ == "__main__":
    main()

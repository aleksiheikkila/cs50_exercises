
def coleman_liau_index(L: float, S: float) -> float:
    return (0.0588 * L - 0.296 * S - 15.8)


def main():
    text = input("Gimme some text: ")

    letters = words = sentences = 0

    # not very pythonic, but works
    in_word = False
    for c in text:
        if c.isalpha():
            letters += 1
            in_word = True

        elif c in (".", "?", "!"):
            sentences += 1
            in_word = True

        elif c.isspace() and in_word:
            words += 1
            in_word = False

    # Add the last word if needed
    words += in_word

    # print the stats
    print(f"{letters} letter(s)")
    print(f"{words} word(s)")
    print(f"{sentences} sentence(s)")

    # Calculate L and S
    # L: L is the average number of letters per 100 words
    L = letters / (words / 100.0)

    # S: S is the average number of sentences per 100 words
    S = sentences / (words / 100.0)

    grade = round(coleman_liau_index(L, S))

    # print the resulting grade
    if (grade < 1):
        print("Before Grade 1")
    elif (grade >= 16):
        print("Grade 16+")
    else:
        print(f"Grade {grade}")


if __name__ == "__main__":
    main()
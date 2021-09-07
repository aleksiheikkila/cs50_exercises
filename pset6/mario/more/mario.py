from cs50 import get_int

if __name__ == "__main__":
    # Get positive int [1, 8]
    while True:
        h = get_int("Height: ")
        if (1 <= h <= 8):
            break

    # Print the pyramid
    for i in range(h):
        print(" " * (h-i-1) + "#" * (i+1) + "  " + "#" * (i+1))
        # print("".join([" " * (h-i-1), "#" * (i+1), "  ", "#" * (i+1)]))
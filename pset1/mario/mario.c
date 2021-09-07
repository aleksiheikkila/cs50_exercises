#include <stdio.h>
#include <cs50.h>


// Prototype
void print_n_chars(int n, char c);


int main(void)
{
    // Get height
    int h;
    do
    {
        h = get_int("Height: ");
    }
    while (h < 1 || h > 8);

    // Print out the pyramid
    for (int i = 0; i < h; i++)
    {
        // First, the empty spaces before the hashes
        print_n_chars(h - i - 1, ' ');
        print_n_chars(i + 1, '#');

        // Gap in the middle
        printf("  ");

        // Then the right side
        print_n_chars(i + 1, '#');
        //print_n_chars(h - i - 1, ' ');

        // Finally newline
        printf("\n");
    }
}


void print_n_chars(int n, char c)
{
    // Prints the char c n times, as a side effect
    while (n > 0)
    {
        printf("%c", c);
        n--;
    }
}
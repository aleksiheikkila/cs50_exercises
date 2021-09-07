#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


// Prototypes
bool is_letter(char c);
bool is_end_of_sentence(char c);
float coleman_liau_index(float L, float S);

int main(void)
{
    string text = get_string("Gimme some text: ");
    int N = strlen(text);

    int letters = 0;
    int words = 0;
    int sents = 0;

    bool in_word = false;

    // loop
    for (int i = 0; i < N; i++)
    {

        if (is_letter(text[i]))
        {
            letters += 1;
            in_word = true;
        }
        else if (is_end_of_sentence(text[i]))
        {
            sents += 1;
            in_word = true;
        }
        else if (text[i] == ' ' && in_word)
        {
            words += 1;
            in_word = false;
        }
    }
    // Add the last word if needed
    if (in_word)
    {
        words += 1;
    }

    printf("%i letter(s)\n", letters);
    printf("%i word(s)\n", words);
    printf("%i sentence(s)\n", sents);

    // Calculate L and S
    // L: L is the average number of letters per 100 words
    float L = letters / (words / 100.0);

    // S: S is the average number of sentences per 100 words
    float S = sents / (words / 100.0);

    int grade = round(coleman_liau_index(L, S));

    if (grade < 1)
    {
        printf("Before Grade 1\n");

    }
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }
}

bool is_letter(char c)
{
    // either ascii upper or lowercase a-z
    int i = (int) c;
    // TODO: Compare just using the chars
    if (i >= 65 && i <= 122)
    {
        return true;

    }
    else
    {
        return false;

    }
}

bool is_end_of_sentence(char c)
{
    if (c == '.' || c == '!' || c == '?')
    {
        return true;
    }
    else
    {
        return false;

    }

}


float coleman_liau_index(float L, float S)
{
    return (0.0588 * L - 0.296 * S - 15.8);

}
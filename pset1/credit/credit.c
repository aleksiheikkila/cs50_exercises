#include <stdio.h>
#include <cs50.h>


int main(void)
{
    // Get card number to test

    long orig_number = get_long("Number: ");

    // Not long enough
    if (orig_number < (1e12))
    {
        printf("INVALID\n");
        return 0;
    }


    // Luhns algorithm
    int i = 0;
    int sum_of_doubled_digits = 0;
    int sum_of_other_digits = 0;

    long number = orig_number;
    while (number)
    {
        // Grab the current digit
        int digit = number % 10;

        // if every other, starting from the second last
        if (i % 2 == 1)
        {
            // Handle the case where the doubled digit is 10 or more (we need to add the separate digits!)
            if (2 * digit > 9)
            {
                sum_of_doubled_digits += 1 + (2 * digit - 10);
            }
            else
            {
                sum_of_doubled_digits += 2 * digit;
            }
        }

        // not one of those that needs to be doubled
        else
        {
            sum_of_other_digits += digit;
        }

        // Move to next digit
        number /= 10;
        i++;
    }

    if ((sum_of_doubled_digits + sum_of_other_digits) % 10 != 0)
    {
        printf("INVALID\n");
        return 0;
    }

    else
    {
        // Valid, but which card type?
        printf("VALID\n");
        // Get first to digits to determine card type
        string card_type = "kukkuu";
        number = orig_number;

        while (number > 100)
        {
            number /= 10;
        }
        printf("First 2 digits: %li\n", number);

        if (number == 34 || number == 37)
        {
            card_type = "AMEX";
        }
        else if (number >= 51 && number <= 55)
        {
            card_type = "MASTERCARD";
        }
        else if (number / 10 == 4)
        {
            card_type = "VISA";
        }
        else
        {
            printf("INVALID\n");
            return 0;
        }
        printf("%s\n", card_type);

    }


}
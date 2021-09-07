#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // Get start and end sizes for the population
    int start_population;
    int end_population;

    do
    {
        start_population = get_int("Start size: ");
    }
    while (start_population < 9);

    do
    {
        end_population = get_int("End size: ");
    }
    while (end_population < start_population);

    // Variables for tracking the population growth
    int pop = start_population;
    int years = 0;

    // Check how many years are needed for the population to grow to (or above) the given end value:
    while (pop < end_population)
    {
        pop = pop + (pop / 3) - (pop / 4);
        years++;
    }

    // Print out the result
    printf("Years: %i\n", years);

}
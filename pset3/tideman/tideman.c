#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
// locked[i][j] being true represents the existence of an edge pointing from candidate i to candidate j; false means there is no edge.
// adjacency matrix
bool locked[MAX][MAX];  // 9x9 2d array

// Each pair has a winner, loser (these are indices for those)
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates (names)
string candidates[MAX];
// pairs, with one side preferred over another (indices)
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool cycle(int end, int cycle_start);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs (initial graph is empty)
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        // ranks[i] is the index of the candidate who is the ith preference for the voter.
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        // ranks are passed into the record_preference function, whose job it is to take those ranks and update the global preferences variable.
        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // If name is a match for the name of a valid candidate,
    //then you should update the ranks array to indicate that the voter has the candidate as their rank preference
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            // You may assume that no two candidates will have the same name.
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    // The function is called once for each voter, and takes as argument the ranks array,
    // (recall that ranks[i] is the voter’s ith preference, where ranks[0] is the first preference). ranks[i] is the index

    // The function should update the global preferences array to add the current voter’s preferences.
    // Recall that preferences[i][j] should represent the number of voters who prefer candidate i over candidate j

    // You may assume that every voter will rank each of the candidates.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    // go thru the preferences

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // it's a tie, do not add
            if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }

            // i preferred
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
            }
            // j preferred:
            else
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
            }

            pair_count++;
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Using bubble sort. Compare pair x to pair x+1
    // max pair_count - 1 rounds...
    // this is the number of "rounds"
    for (int i = 0; i < pair_count - 1; i++)
    {
        bool no_swaps_done = true;  // for early stopping

        for (int j = 0; j < pair_count - 2 - i; j++)
        {
            if (preferences[pairs[j].winner][pairs[j].loser] < preferences[pairs[j + 1].winner][pairs[j + 1].loser])
            {
                // then swap
                pair tmp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = tmp;
                no_swaps_done = false;
            }
        }
        // early stopping
        if (no_swaps_done)
        {
            return;
        }
    }
    // the pairs array will be sorted from largest preference to smallest
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    for (int i = 0; i < pair_count; i++)
    {
        // If wont create a cycle, lock the pair
        if (!cycle(pairs[i].loser, pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    // winner is the source of the graph
    for (int i = 0; i < candidate_count; i++)
    {
        //
        int arrows_to_node = 0;

        for (int j = 0; j < candidate_count; j++)
        {
            // i == j case is false
            if (locked[j][i] == true)
            {
                arrows_to_node++;
                break;
            }
        }
        // Winner:
        if (arrows_to_node == 0)
        {
            printf("%s\n", candidates[i]);
            return;
        }

    }

    return;
}


// Test if creates a cycle
bool cycle(int end, int cycle_start)
{
    // Recursive
    // BASE CASE: Return true if creates a cycle in the graphs
    if (cycle_start == end)
    {
        return true;
    }

    // Loop through candidates
    // if there is a way from the current end to somewhere, follow that path to check if there will be a route all the way to the start
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[end][i])
        {
            // hop from end to i, and recurse to cycle function again
            if (cycle(i, cycle_start))
            {
                return true;
            }
        }
    }
    return false;
}
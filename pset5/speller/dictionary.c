// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "dictionary.h"


// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;


// Number of buckets in hash table
const unsigned int N = 10000;  // TODO: set this

// Hash table
node *table[N];

// Size of the dictionary
unsigned int dict_size = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // TODO
    // Put word to lowercase before hashing (dictionary is in lowercase)
    char word_copy[strlen(word) + 1];
    strcpy(word_copy, word);
    for (int i = 0; i < strlen(word) + 1; i++)
    {
        word_copy[i] = tolower(word_copy[i]);
    }
    //word_copy[strlen(word)] = '\0';
    int tbl_idx = hash(word_copy);
    node *n = table[tbl_idx];

    while (n != NULL)
    {
        if (strcmp(n->word, word_copy) == 0)  // case sensitive comparison now ok
        {
            return true;
        }
        n = n->next;
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // BAsed on https://stackoverflow.com/questions/7666509/hash-function-for-string
    // djb2 by Dan Bernstein.
    unsigned long hash = 5381;
    int c;

    while ((c = *word++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash % N;
}

// Loads dictionary into memory, returning true if successful, else false
// dictionary is the path to the dictionary text file
// returns true if loaded OK
bool load(const char *dictionary)
{
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        printf("Unable to open file\n");
        return false;
    }

    char word[LENGTH + 1];

    //printf("Init table...\n");
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    int tbl_idx;

    //while (fgets(word, LENGTH, file))
    while (fscanf(file, "%s", word) != EOF)
    {
        // get rid of the trailing newline character
        //word[strcspn(word, "\r\n")] = 0;

        tbl_idx = hash(word);

        // create new node
        node *newnode = malloc(sizeof(node));
        if (newnode == NULL)
        {
            printf("Couldn't alloc memory for new node\n");
            unload();
            return false;
        }

        strcpy(newnode->word, word);
        newnode->next = table[tbl_idx];
        table[tbl_idx] = newnode;
        dict_size++;
    }

    fclose(file);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    return dict_size;  // from global scope
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    node *curr;
    node *next;
    for (int i = 0; i < N; i++)
    {
        next = table[i];
        while (next != NULL)
        {
            curr = next;
            next = curr->next;
            free(curr);
        }
    }
    return true;
}

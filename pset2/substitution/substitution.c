#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Substitution cipher

string encrypt(string plaintext, string key);

int main(int argc, string argv[])
{
    // check input validity
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }
    string key = argv[1];

    if (strlen(key) != 26)
    {
        printf("Key must contain 26 characters.\n");
        return 1;
    }

    // check that only valid characters in the key, and no duplicates
    int key_len = strlen(key);
    int count[26] = {0}; // array of zeros!

    for (int i = 0; i < key_len; i++)
    {
        if (key[i] < 65 || key[i] > 122)
        {
            printf("Key must contain 26 characters.\n");
            return 1;
        }
        int idx = toupper(key[i]) - 'A';
        count[idx] += 1;
        if (count[idx] > 1)
        {
            printf("Key must not contain duplicates.\n");
            return 1;
        }
    }


    // Ask for the plaintext
    string plaintext = get_string("plaintext: ");

    // encrypt
    string ciphertext = encrypt(plaintext, argv[1]);

    // Print encrypted
    printf("ciphertext: %s\n", ciphertext);
    return 0;
}

string encrypt(string plaintext, string key)
{
    int N = strlen(plaintext);
    char *ciphertext = malloc(N + 1);


    for (int i = 0; i < N; i++)
    {
        char c = plaintext[i];

        // if not alphabet, leave it as is
        if (c < 65 || c > 122)
        {
            ciphertext[i] = c;
        }
        else
        {
            // substitute, keep case
            int idx = toupper(c) - 'A';

            if (isupper(c))
            {
                ciphertext[i] = toupper(key[idx]);
            }
            else
            {
                ciphertext[i] = tolower(key[idx]);
            }

        }
    }

    ciphertext[N] = '\0';
    return ciphertext;
}
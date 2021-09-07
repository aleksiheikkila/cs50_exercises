#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef uint8_t BYTE;

const int BLOCK_SZ = 512;  // 512 bytes


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    FILE *source = fopen(argv[1], "r");
    if (source == NULL)
    {
        printf("Couldnt open %s for reading\n", argv[1]);
        return 1;
    }

    int img_nbr = 0;

    // buffer holds BLOCK_SZ bytes from the source
    BYTE buffer[BLOCK_SZ];
    FILE *destination = NULL;

    // Read one block to the buffer
    while (fread(&buffer, sizeof(BYTE), BLOCK_SZ, source))
    {
        // Check if we are starting a new file
        // Fourth byte: "1110 xxxx" as int: from 224 to 239
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] >= 224 && buffer[3] <= 239)
        {
            // if we had file open, it's now ready to be closed
            if (destination != NULL)
            {
                fclose(destination);
                destination = NULL;
            }

            // Open a new output file
            // Create filename

            // Stop if unexpectedly many images were found
            if (img_nbr >= 100)
            {
                printf("More than 100 imgs found!\n");
                return 0;
            }

            //char filename[sizeof "./recovered_jpegs/img_99.jpeg"];
            char filename[7];
            //sprintf(filename, "./recovered_jpegs/img_%02d.jpeg", img_nbr);
            sprintf(filename, "%03d.jpg", img_nbr);
            //printf("Opening a file: %s\n", filename);

            // Open the output file
            destination = fopen(filename, "w");
            if (destination == NULL)
            {
                fclose(source);
                printf("Could not create %s.\n", argv[2]);
                return 1;
            }

            img_nbr++;  // For the next one
        }

        if (destination == NULL)
        {
            // The beginning of the first jpeg file not found, just read the next block
            continue;
        }

        // Write the data to the output file
        fwrite(&buffer, sizeof(BYTE), BLOCK_SZ, destination);


    }

    // Close input file and output file, if open
    fclose(source);
    if (destination != NULL)
    {
        fclose(destination);
    }

    return 0;
}
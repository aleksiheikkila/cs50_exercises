#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <string.h>  // for memcpy(??)


// Prototypes
int combine_gs(int gx, int gy);


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int avg_px_val;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            RGBTRIPLE *px = &image[h][w];
            avg_px_val = (int) round((px->rgbtBlue + px->rgbtRed + px->rgbtGreen) / 3.0);

            // Set average value to each channel
            px->rgbtBlue  = avg_px_val;
            px->rgbtRed   = avg_px_val;
            px->rgbtGreen = avg_px_val;
        }
    }
    return;
}


// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // swap between left and right
    // row by row

    for (int row = 0; row < height; row++)
    {
        int left = 0;
        int right = width - 1;  // last pixel
        for (; left < right;)
        {
            // swap
            RGBTRIPLE tmp = image[row][right];
            image[row][right] = image[row][left];
            image[row][left] = tmp;

            left++;
            right--;
        }
    }
    return;
}


// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // The new value of each pixel would be the average of the values of all of the pixels that are
    // within 1 row and column of the original pixel (forming a 3x3 box)

    // Make a copy of the image, so that we can save the values and also keep the original
    RGBTRIPLE orig_image[height][width];
    memcpy(orig_image, image, height * width * sizeof(RGBTRIPLE));

    // deltas for moving to the nearby pixel coordinates
    int DELTAS[9][2];
    int idx = 0;

    for (int dx = -1; dx < 2; dx++)
    {
        for (int dy = -1; dy < 2; dy++)
        {
            DELTAS[idx][0] = dx;  // width dimension delta
            DELTAS[idx][1] = dy;
            idx++;
        }
    }

    int dx, dy;
    int x, y;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            float num_vals = 0.0;  // how many values where there for the average... [4, 9]
            // made it a float to prevent integer division before rounding...
            int red = 0;
            int green = 0;
            int blue = 0;

            for (int i = 0; i < 9; i++)
            {
                dx = DELTAS[i][0];
                dy = DELTAS[i][1];

                // x, y are the pixel coords
                x = w + dx;
                y = h + dy;

                // skip if outside of image
                if (x < 0 || x >= width || y < 0 || y >= height)
                {
                    continue;
                }
                else
                {
                    red   += orig_image[y][x].rgbtRed;
                    green += orig_image[y][x].rgbtGreen;
                    blue  += orig_image[y][x].rgbtBlue;
                    num_vals++;
                }
            }

            // After all valid deltas considered, calculate the averages
            image[h][w].rgbtRed   = round(red / num_vals);
            image[h][w].rgbtGreen = round(green / num_vals);
            image[h][w].rgbtBlue  = round(blue / num_vals);
        }
    }
    return;
}

// Detect edges
// The edges function should take an image and highlight the edges between objects, according to the Sobel operator.
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Make a copy of the image, so that we can save the values and also keep the original
    RGBTRIPLE orig_image[height][width];
    memcpy(orig_image, image, height * width * sizeof(RGBTRIPLE));

    // Values are: dh, dw, coeff... positive directions are down and right
    // So Gx[9][3]
    const int Gxs[9][3] =
    {
        {-1, -1, -1},
        {-1, 0, 0},
        {-1, 1, 1},
        {0, -1, -2},
        {0, 0, 0},
        {0, 1, 2},
        {1, -1, -1},
        {1, 0, 0},
        {1, 1, 1}
    };

    const int Gys[9][3] =
    {
        {-1, -1, -1},
        {-1, 0, -2},
        {-1, 1, -1},
        {0, -1, 0},
        {0, 0, 0},
        {0, 1, 0},
        {1, -1, 1},
        {1, 0, 2},
        {1, 1, 1}
    };


    int dx, dy;
    int gx, gy;
    int x, y;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            int red_x = 0;
            int green_x = 0;
            int blue_x = 0;

            int red_y = 0;
            int green_y = 0;
            int blue_y = 0;

            for (int i = 0; i < 9; i++)
            {
                dy = Gxs[i][0];
                dx = Gxs[i][1];  // These are the same for both Gx and Gy

                gx  = Gxs[i][2];
                gy  = Gys[i][2];

                // x, y are the pixel coords
                x = w + dx;
                y = h + dy;

                if (x < 0 || x >= width || y < 0 || y >= height)
                {
                    // There is a black border, meaning the pixel values are all zero. Contribution to the weighed sum is zero
                    continue;
                }
                else
                {
                    red_x   += gx * orig_image[y][x].rgbtRed;
                    green_x += gx * orig_image[y][x].rgbtGreen;
                    blue_x  += gx * orig_image[y][x].rgbtBlue;

                    red_y   += gy * orig_image[y][x].rgbtRed;
                    green_y += gy * orig_image[y][x].rgbtGreen;
                    blue_y  += gy * orig_image[y][x].rgbtBlue;
                }
            }

            // After all valid deltas considered:
            image[h][w].rgbtRed   = combine_gs(red_x, red_y);
            image[h][w].rgbtGreen = combine_gs(green_x, green_y);
            image[h][w].rgbtBlue  = combine_gs(blue_x, blue_y);
        }
    }
    return;
}

int combine_gs(int gx, int gy)
{
    // takes Gx and Gy values, combines them with proper rounding and capping applied in the end
    float rst = round(sqrt(pow(gx, 2) + pow(gy, 2)));

    // cap to 255
    if (rst > 255)
    {
        rst = 255;
    }
    return (int) rst;
}
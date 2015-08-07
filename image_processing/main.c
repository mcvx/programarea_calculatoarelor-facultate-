#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bmp.h"
#include "utils.h"

#define DATA_FILE "input.txt"
#define IMAGE_FILE "input.bmp"

/* -------- Points structs and functions -------- */
typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    int count;
    Point *data;
} PointQueue;

// Returns a new point
Point newPoint(int x, int y);

// Adds a new point to the point queue
void pushPoint(PointQueue *queue, Point p);

/* --------- Image processing functions --------- */
// Checks if a pixel is in a range of colors
inline int validPixel(bmp_pixel *ref, bmp_pixel *offset, bmp_pixel *pixel);

// Generates a new cluster starting from <start> point and returns the point queue
inline PointQueue generateCluster(Point start, bmp_pixel *ref, bmp_pixel *offset, bmp_pixel **imageData, int **mask);

// Crops a <cluster> from an <image> and exports it to file
inline void cropCluster(PointQueue *cluster, int clusterNr, bmp_image *image);

// Blurs a <cluster> in <bluredImage> using <auxImage> for swap
inline void blurCluster(PointQueue *cluster, bmp_image *bluredImage, bmp_image *auxImage);

int main(void)
{
    FILE *inputFile;						// input data file
    bmp_pixel pixelRef, pixelOffset;		// input pixels
    double p;								// input cluster procent

    /* --------------- Read input data -----------------*/
	int r, g, b;

    inputFile = fopen(DATA_FILE, "r");

    // Check if the file was opened
    if (!inputFile)
    {
        printf ("Error opening input file\n");
        exit(EXIT_FAILURE);
    }

    // Read reference pixel
    fscanf(inputFile, "%d %d %d\n", &r, &g, &b);
    pixelRef.r = r;
    pixelRef.g = g;
    pixelRef.b = b;

    // Read offset pixel
    fscanf(inputFile, "%d %d %d\n", &r, &g, &b);
    pixelOffset.r = r;
    pixelOffset.g = g;
    pixelOffset.b = b;

    // Read procent
    fscanf(inputFile, "%lf", &p);

    fclose(inputFile);

    /* --------- Open image and create mask ---------- */
    bmp_image *image = openBmpImage(IMAGE_FILE);

    int i;
    int **imageMask;	// Matrix used for marking clusters

    // Allocate the image mask matrix
    imageMask = calloc(image->infoHeader.height, sizeof(int*));

    for (i = 0; i < image->infoHeader.height; i++)
    {
        imageMask[i] = calloc(image->infoHeader.width, sizeof(int));
    }

    /* ----------------------------------------------- */
    FILE *outputFile;		// Output file for requirement 1

    int j;
    int clusterNr;			// Clusters count
    int *clustersSize;		// Array containing the size of each cluster

    Point startPoint;		// Starting point for generating cluster
    PointQueue pQueue;		// Cluster's point queue

    bmp_image *bluredImage;	// Blured image for requirement 2
    bmp_image *auxImage;	// Auxiliary image used as swap for bluring

    clusterNr = 0;
    clustersSize = calloc(0, sizeof(int));

    // Copy the original image
    auxImage = copyBmpImage(image);
    bluredImage = copyBmpImage(image);
    bluredImage->infoHeader.biXPelsPerMeter = bluredImage->infoHeader.biYPelsPerMeter = 0;

    // Go through the image matrix of pixels
    for (i = 0; i < image->infoHeader.height; i++)
    {
        for (j = 0; j < image->infoHeader.width; j++)
        {
    		// Check if the current pixel is valid
            if (validPixel(&pixelRef, &pixelOffset, &image->data[i][j]) && !imageMask[i][j])
            {
                startPoint.x = i;
                startPoint.y = j;

                // Start generating a cluster from the start point, which is a valid point
                pQueue = generateCluster(startPoint, &pixelRef, &pixelOffset, image->data, imageMask);

                // Check if the cluster has valid size
                if (pQueue.count >= p * image->infoHeader.width * image->infoHeader.height)
                {
                    clusterNr++;

                    // Add the cluster size in the array of sizes
                    clustersSize = realloc(clustersSize, clusterNr * sizeof(int));
                    clustersSize[clusterNr - 1] = pQueue.count;

                    cropCluster(&pQueue, clusterNr, image);
                    blurCluster(&pQueue, bluredImage, auxImage);
                }

                // Clear the cluster point queue
                free(pQueue.data);
            }
        }
    }

    // Sort the array of cluster's sizes
    qsort(clustersSize, clusterNr, sizeof(int), compareNumbers);

    // Print the result for requirement 1 to the file
    outputFile = fopen("output.txt", "w");
    for (i = 0; i < clusterNr; i++)
    {
        fprintf(outputFile, "%d ", clustersSize[i]);
    }
    fclose(outputFile);

    // Export the blured image for requirement 2 to file
    exportBmpImage("output_blur.bmp", bluredImage);

    /* ----------------- Free memory ----------------- */
    free(clustersSize);
    freeMatrix((void*)imageMask, image->infoHeader.height);
    freeBmpImage(image);
    freeBmpImage(bluredImage);
    freeBmpImage(auxImage);

    return 0;
}

Point newPoint(int x, int y)
{
    Point p;

    p.x = x;
    p.y = y;

    return p;
}

void pushPoint(PointQueue *queue, Point p)
{
    queue->count++;
    queue->data = realloc(queue->data, queue->count * sizeof(Point));
    queue->data[queue->count - 1] = p;
}

int validPixel(bmp_pixel *ref, bmp_pixel *offset, bmp_pixel *pixel)
{
    if (abs(ref->b - pixel->b) > offset->b)
    {
        return 0;
    }

    if (abs(ref->g - pixel->g) > offset->g)
    {
        return 0;
    }

    if (abs(ref->r - pixel->r) > offset->r)
    {
        return 0;
    }

    return 1;
}

PointQueue generateCluster(Point start, bmp_pixel *ref, bmp_pixel *offset, bmp_pixel **imageData, int **mask)
{
    int i, x, y;
    PointQueue pQueue;

    // Allocate the point queue
    pQueue.count = 1;
    pQueue.data = calloc(1, sizeof(Point));

    // Add the start point to the queue
    pQueue.data[0] = start;
    mask[start.x][start.y] = 1;

    // For each point in the queue search for vaild points in it's neighbourhood
    // and also add them to the queue
    for (i = 0; i < pQueue.count; i++)
    {
        x = pQueue.data[i].x;
        y = pQueue.data[i].y;

        if (!mask[x - 1][y] && validPixel(ref, offset, &imageData[x - 1][y]))
        {
            pushPoint(&pQueue, newPoint(x - 1, y));
            mask[x - 1][y] = 1;
        }

        if (!mask[x + 1][y] && validPixel(ref, offset, &imageData[x + 1][y]))
        {
            pushPoint(&pQueue, newPoint(x + 1, y));
            mask[x + 1][y] = 1;
        }

        if (!mask[x][y + 1] && validPixel(ref, offset, &imageData[x][y + 1]))
        {
            pushPoint(&pQueue, newPoint(x, y + 1));
            mask[x][y + 1] = 1;
        }

        if (!mask[x][y - 1] && validPixel(ref, offset, &imageData[x][y - 1]))
        {
            pushPoint(&pQueue, newPoint(x, y - 1));
            mask[x][y - 1] = 1;
        }
    }

    return pQueue;
}

void cropCluster(PointQueue *cluster, int clusterNr, bmp_image* image)
{
    int i, j;
    int top, right, bottom, left;	// Crop rectangle's bounds
    int width, height, padding;		// Crop image properties
    bmp_image *cropedImage;			// Output image
	FILE *cropedImageFile;			// Output image file
    char cropedImageName[128];		// Output image name

    // Init the bounds to min/max values
    top = cluster[0].data->x;
    left = cluster[0].data->y;
    bottom = right = 0;

    // Go through the cluster points and find the bounds 
    for (i = 0; i < cluster->count; i++)
    {
        if (cluster->data[i].x > bottom)
        {
            bottom = cluster->data[i].x;
        }

        if (cluster->data[i].x < top)
        {
            top = cluster->data[i].x;
        }

        if (cluster->data[i].y > right)
        {
            right = cluster->data[i].y;
        }

        if (cluster->data[i].y < left)
        {
            left = cluster->data[i].y;
        }
    }

    // Compute output image properties
    width = right - left + 1;
    height = bottom - top + 1;
    padding = width % 4 ? 4 - 3 * width % 4 : 0;

	// Alloc and init the croped image
    cropedImage = calloc(1, sizeof(bmp_image));
    initImage(cropedImage);

	// Set image header properties
    cropedImage->infoHeader.width = width;
    cropedImage->infoHeader.height = height;
    cropedImage->infoHeader.biSizeImage = 3 * width * height + height * padding;
    cropedImage->fileHeader.bfSize = sizeof(bmp_infoheader) + sizeof(bmp_fileheader) + cropedImage->infoHeader.biSizeImage;

    // Set the name of the image
    sprintf(cropedImageName, "output_crop%d.bmp", clusterNr);

    // Open image file
    cropedImageFile = fopen(cropedImageName, "wb");

    fwrite(&cropedImage->fileHeader, sizeof(bmp_fileheader), 1, cropedImageFile);
    fwrite(&cropedImage->infoHeader, sizeof(bmp_infoheader), 1, cropedImageFile);

    // Export the cluster's pixels to file
    for (i = 0; i < height; i++)
    {
        fwrite(&image->data[top + i][left], sizeof(bmp_pixel), width, cropedImageFile);
        for (j = 1; j <= padding; j++)
        {
            fputc(0, cropedImageFile);
        }
    }

    freeBmpImage(cropedImage);
    fclose(cropedImageFile);
}

void blurCluster(PointQueue *cluster, bmp_image *bluredImage, bmp_image *auxImage)
{
    bmp_pixel topP, rightP, bottomP, leftP;	// Neighbour pixels
    int i, k, x, y;

    // Repeat the bluring algorithm 100 times
    for (k = 1; k <= 100; k++)
    {
    	// Copy the current cluster's pixels to the swap image
        for (i = 0; i < cluster->count; i++)
        {
            x = cluster->data[i].x;
            y = cluster->data[i].y;
            auxImage->data[x][y] = bluredImage->data[x][y];
        }

        // Go through cluster's pixels
        for (i = 0; i < cluster->count; i++)
        {
        	// Init the neighbour pixels with the current pixel
            x = cluster->data[i].x;
            y = cluster->data[i].y;
            topP = leftP = rightP = bottomP = auxImage->data[x][y];

            // Check if the neighbour pixels are out of bounds
            if (x > 0)
            {
                topP = auxImage->data[x - 1][y];
            }

            if (y < bluredImage->infoHeader.width - 1)
            {
                rightP = auxImage->data[x][y + 1];
            }

            if (x < bluredImage->infoHeader.height - 1)
            {
                bottomP = auxImage->data[x + 1][y];
            }

            if (y > 0)
            {
                leftP = auxImage->data[x][y - 1];
            }

            // Blur the pixels in the destination image
            bluredImage->data[x][y].b = (topP.b + rightP.b + bottomP.b + leftP.b) / 4;
            bluredImage->data[x][y].g = (topP.g + rightP.g + bottomP.g + leftP.g) / 4;
            bluredImage->data[x][y].r = (topP.r + rightP.r + bottomP.r + leftP.r) / 4;
        }
    }
}

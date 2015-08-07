#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

bmp_image* openBmpImage(char *path)
{
    FILE *imageFile;
    bmp_image *image;
    int i, padding;

    imageFile = fopen(path, "rb");

    // Check if the file was opened
    if (!imageFile)
    {
        printf ("Error opening image\n");
        exit(EXIT_FAILURE);
    }

    image = calloc(1, sizeof(bmp_image));

    fread(&image->fileHeader, sizeof(bmp_fileheader), 1, imageFile);
    fread(&image->infoHeader, sizeof(bmp_infoheader), 1, imageFile);

    // Compute the pixels matrix padding and alloc the matrix
    padding = image->infoHeader.width % 4 ? 4 - 3 * image->infoHeader.width % 4 : 0;
    image->data = (bmp_pixel**) calloc(image->infoHeader.height, sizeof(bmp_pixel*));

    // Seek to the position of the pixels matrix
    fseek(imageFile, image->fileHeader.imageDataOffset, SEEK_SET);

    // Go through the pixels matrix line by line
    for (i = 0; i < image->infoHeader.height; i++)
    {
        // Alloc a line of pixels
        image->data[i] = calloc(image->infoHeader.width, sizeof(bmp_pixel));

        // Read the line of pixels
        fread(image->data[i], sizeof(bmp_pixel), image->infoHeader.width, imageFile);

        // Seek over the padding and go to the start of the next line
        fseek(imageFile, padding, SEEK_CUR);
    }

    fclose(imageFile);

    return image;
}

void exportBmpImage(char *path, bmp_image *image)
{
    FILE *imageFile;
    int i, j, padding;

    // Open image file and compute padding
    imageFile = fopen(path, "wb");
    padding = image->infoHeader.width % 4 ? 4 - 3 * image->infoHeader.width % 4 : 0;

    fwrite(&image->fileHeader, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(&image->infoHeader, sizeof(bmp_infoheader), 1, imageFile);

    // Go through the pixels matrix line by line
    for (i = 0; i < image->infoHeader.height; i++)
    {
        // Write the line of pixels
        fwrite(image->data[i], sizeof(bmp_pixel), image->infoHeader.width, imageFile);

        // Write the padding
        for (j = 1; j <= padding; j++)
        {
            fputc(0, imageFile);
        }
    }

    fclose(imageFile);
}

bmp_image* copyBmpImage(bmp_image *source)
{
    int i;
    bmp_image *image;

    // Alloc a new image
    image = calloc(1, sizeof(bmp_image));
    image->data = calloc(source->infoHeader.height, sizeof(bmp_pixel*));

    // Copy headers
    image->fileHeader = source->fileHeader;
    image->infoHeader = source->infoHeader;

    // Copy the pixels matrix lines
    for (i = 0; i < source->infoHeader.height; i++)
    {
        image->data[i] = calloc(source->infoHeader.width, sizeof(bmp_pixel));
        memcpy(image->data[i], source->data[i], source->infoHeader.width * sizeof(bmp_pixel));
    }

    return image;
}

void initImage(bmp_image *image)
{
    image->fileHeader.fileMarker1 = 'B';
    image->fileHeader.fileMarker2 = 'M';
    image->fileHeader.unused1 = 0;
    image->fileHeader.unused2 = 0;
    image->fileHeader.imageDataOffset = sizeof(bmp_infoheader) + sizeof(bmp_fileheader);

    image->infoHeader.biSize = sizeof(bmp_infoheader);
    image->infoHeader.planes = 1;
    image->infoHeader.bitPix = 24;
    image->infoHeader.biCompression = 0;
    image->infoHeader.biXPelsPerMeter = 0;
    image->infoHeader.biYPelsPerMeter = 0;
    image->infoHeader.biClrUsed = 0;
    image->infoHeader.biClrImportant = 0;

    image->data = NULL;
}

void freeBmpImage(bmp_image *image)
{
	if (image->data)
	{
	    freeMatrix((void*)image->data, image->infoHeader.height);
	    free(image);
    }
}

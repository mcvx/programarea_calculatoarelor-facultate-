// Informs the compiler to not add padding for these structs
#pragma pack(1)

typedef struct
{
    unsigned char  fileMarker1; /* 'B' */
    unsigned char  fileMarker2; /* 'M' */
    unsigned int   bfSize; /* File's size */
    unsigned short unused1;
    unsigned short unused2;
    unsigned int   imageDataOffset; /* Offset to the start of image data */
} bmp_fileheader;

typedef struct
{
    unsigned int   biSize; /* Size of the info header - 40 bytes */
    signed int     width; /* Width of the image */
    signed int     height; /* Height of the image */
    unsigned short planes;
    unsigned short bitPix;
    unsigned int   biCompression;
    unsigned int   biSizeImage; /* Size of the image data */
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} bmp_infoheader;

typedef struct
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
} bmp_pixel;

typedef struct
{
    bmp_fileheader fileHeader;
    bmp_infoheader infoHeader;
    bmp_pixel **data;
} bmp_image;

// Open bmp image from file
bmp_image* openBmpImage(char *path);

// Export bmp image to file
void exportBmpImage(char *path, bmp_image *image);

// Copy bmp image from memory to memory
bmp_image* copyBmpImage(bmp_image *source);

// Init bmp image with default header and file info values
void initImage(bmp_image *image);

// Free a bmp image from memory
void freeBmpImage(bmp_image *image);

#pragma pack()

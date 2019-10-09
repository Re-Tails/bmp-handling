/**
 * BMP-related data types based on Microsoft's own.
 */

#include <stdint.h>

/**
 * Common Data Types 
 *
 * Data types in this section are aliases for C/C++ 
 * primitive data types.
 *
 * Adapted from https://msdn.microsoft.com/en-us/library/cc230309
 */
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

/**
 * BITMAPFILEHEADER
 *
 * The BITMAPFILEHEADER structure contains information about the type, size,
 * and layout of a file that contains a DIB [device-independent bitmap].
 *
 * Adapted from https://msdn.microsoft.com/en-us/library/dd183374
 */
typedef struct 
{ 
    WORD bfType; 
    DWORD bfSize; 
    WORD bfReserved1; 
    WORD bfReserved2; 
    DWORD bfOffBits; 
} __attribute__((__packed__)) 
BITMAPFILEHEADER; 

/**
 * BITMAPINFOHEADER
 *
 * The BITMAPINFOHEADER structure contains information about the 
 * dimensions and color format of a DIB [device-independent bitmap].
 *
 * Adapted from https://msdn.microsoft.com/en-us/library/dd183376
 */
typedef struct
{
    DWORD biSize; 
    LONG biWidth; 
    LONG biHeight; 
    WORD biPlanes; 
    WORD biBitCount; 
    DWORD biCompression; 
    DWORD biSizeImage; 
    LONG biXPelsPerMeter; 
    LONG biYPelsPerMeter; 
    DWORD biClrUsed; 
    DWORD biClrImportant; 
} __attribute__((__packed__))
BITMAPINFOHEADER; 

/**
 * RGBTRIPLE
 *
 * The RGBTRIPLE structure describes a color consisting of 
 * relative intensities of red, green, and blue. 
 *
 * Adapted from https://msdn.microsoft.com/en-us/library/dd162939
 */
typedef struct
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;


/**
 * BMPIMAGE
 *
 * The BMPIMAGE structure describes a bitmap file consists 
 * of BITMAPFILEHEADER, BITMAPINFOHEADER and the image. 
 *
 */
typedef struct
{   
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char* image;
} __attribute__((__packed__))
BMPIMAGE;

typedef struct __attribute__((__packed__))
{
    unsigned char id[2];
    unsigned int bmpSize;
    unsigned char reserved[4];
    unsigned int bmpDataOff;
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short nColorPlanes;
    unsigned short bpp;
    unsigned int compression;
    unsigned int imageSize;
    unsigned int xRes;
    unsigned int yRes;
    unsigned int nColors; /* number of colors, used for compression */
    unsigned int nImportantColors; /* used for compression */
} BMPHeader;

typedef struct __attribute__((__packed__))
{
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} ImageRGBPixel;

typedef struct __attribute__((__packed__))
{
    unsigned char  runValue;
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} CompressedRGBPixel;
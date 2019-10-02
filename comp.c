#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Stores pixel value for uncompressed image */
typedef struct __attribute__((__packed__))
{
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} ImageRGBPixel;

/* Stores pixel value for compressed image */
typedef struct __attribute__((__packed__))
{
    unsigned char  runValue;
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} CompressedRGBPixel;

/* Stores image header */
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
    unsigned int nColors;
    unsigned int nImportantColors;
} BMPHeader;

/* Compression and Decompression Function */
void runLengthEncoding(char inFileName[], char outFileName[]);
void runLengthDecoding(char inFileName[], char outFileName[]);

int main()
{
    runLengthEncoding("Input_Image.bmp", "Output_Image");
    runLengthDecoding("Output_Image", "Reconstructed_Image.bmp");
    /* getchar(); */
    return 0;
}

void writeCompressedPixel(FILE* filePointer, unsigned char r, unsigned char g, unsigned char b, unsigned char count)
{
    CompressedRGBPixel temp;
    temp.r = r;
    temp.g = g;
    temp.b = b;
    temp.runValue = count;

    fwrite(&temp,sizeof(CompressedRGBPixel),1, filePointer);
}

void printMetaData(BMPHeader header)
{
    printf("BMP Id: %c%c \n", header.id[0], header.id[1]);
    printf("BMP File Size: %u bytes\n", header.bmpSize);
    printf("Reserved: %x%x%x%x bytes\n", header.reserved[0], header.reserved[1], header.reserved[2], header.reserved[3]);
    printf("BMP Data Offset: %u\n", header.bmpDataOff);
    printf("Header Size: %u\n", header.headerSize);
    printf("width: %u\n", header.width);
    printf("height: %u\n", header.height);
    printf("Color planes: %u\n", header.nColorPlanes);
    printf("Bits per pixel: %u\n", header.bpp);
    printf("Compression: %u\n", header.compression);
    printf("Image Size: %u\n", header.imageSize);
    printf("X Resolution: %u\n", header.xRes);
    printf("Y Resolution: %u\n", header.yRes);
    printf("N Colors: %u\n", header.nColors);
    printf("N Important Colors: %u\n", header.nImportantColors);
}

void runLengthEncoding(char inFileName[], char outFileName[])
{
    int i;
    int j;
    /* Header Variables */
    BMPHeader header;

    /* For debugging */
    int FailStatus = 0;

    /* File Handlers */
    ImageRGBPixel **image;

    /* Reading file and checking header details */
    FILE* inImgFile = fopen(inFileName, "rb");
    FILE* outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        /* Reading the file */
        fseek(inImgFile, 0, SEEK_SET);
        fread(&header, sizeof(BMPHeader), 1, inImgFile);

        for(i = 0; i < sizeof(BMPHeader); i++)
        {
            printf("%x ", *(&(header.id[0])+i) & 0xff);
        }

        printf("\n");
        printMetaData(header);

        /* Calculating total number of bytes in Image Pixel Array */
        printf("Number of Bytes Before Compression: %u\n", header.height*header.width*3);

        /* Setting offset to start of pixel data */
        fseek(inImgFile, header.bmpDataOff, SEEK_SET);

       /* Allocating memory to image array as per the loaded image */
        image = (ImageRGBPixel**)malloc(header.height * sizeof(ImageRGBPixel*));
        if (!image)
        {
            printf("Error while allocating memory\n");
        }
        else
        {
            for (i = 0; i < header.height; i++)
            {
                image[i] = (ImageRGBPixel*)malloc(header.width * sizeof(ImageRGBPixel));
                if (!image[i])
                {
                    printf("Error while allocating memory\n");
                    FailStatus=1;
                }
            }
        }

        /* For debugging purposes */
        if(!FailStatus)
        {
           printf("Successful memory allocation\n");
        }

        /* Read image pixels */
        unsigned char padding = (((header.width * 3) % 4) == 0) ? 0 : 4- ((header.width * 3) % 4);

        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fread(&image[i][j], sizeof(unsigned char), sizeof(ImageRGBPixel), inImgFile);
            }
            fseek(inImgFile, padding, SEEK_CUR);
        }

        /* Run Length Encoding Algorithm */
        fseek(outImgFile, sizeof(BMPHeader) + sizeof(unsigned int), SEEK_SET);
        ImageRGBPixel current;
        current.r = 0;
        current.g = 0;
        current.b = 0;
        unsigned int pixel_count = 0;
        unsigned int colorCount = 0;

        for (i = 0; i < header.height; i++)
        {
            for (j = 0; j < header.width; j++)
            {
                if (i == 0 && j == 0)
                {
                        current = image[i][j];
                        pixel_count = 1;
                        colorCount = 1;
                }
                else if ((image[i][j].r == current.r) && (image[i][j].g == current.g) && (image[i][j].b == current.b))
                {
                    pixel_count++;
                }
                else
                {
                    writeCompressedPixel(outImgFile, current.r, current.g, current.b, pixel_count);
                    pixel_count = 1;
                    colorCount++;
                    current = image[i][j];
                }

                if (i == header.height - 1 && j == header.width -1)
                {
                    writeCompressedPixel(outImgFile, current.r, current.g, current.b, pixel_count);
                }
            }
        }

        printf("Size after compression: %u \n", colorCount*4);
        printf("Compression percentage: %f\n", 100.0*(colorCount*4)/(header.height*header.width*3));

        fseek(outImgFile, 0, SEEK_SET);
        fwrite(&header, sizeof(BMPHeader), 1, outImgFile);
        fwrite(&colorCount, 4, 1, outImgFile);

        /* ------- */

        /* Freeing data and closing files */
        if(image) free(image);
        fclose(inImgFile);
        fclose(outImgFile);
    }
    printf("\nCompression Complete!!!\n");
}

void runLengthDecoding(char inFileName[], char outFileName[])
{
    /* Header Variables */
    int i;
    int j;
    BMPHeader header;
    unsigned int colorCount;

    /* For debugging */
    int FailStatus = 0;

    /*File Handlers */
    ImageRGBPixel *compressedImage;

    /* Reading file and checking header details */
    FILE* inImgFile = fopen(inFileName, "rb");
    FILE* outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        /* Reading the file */
        fseek(inImgFile, 0, SEEK_SET);
        fread(&header, sizeof(BMPHeader), 1, inImgFile);

        printMetaData(header);

        fseek(inImgFile, sizeof(BMPHeader), SEEK_SET);
        fread(&colorCount, sizeof(unsigned int), 1, inImgFile);
        printf("Color Count: %u\n", colorCount);

       /* Allocating memory to image array as per the loaded image */
        compressedImage = (ImageRGBPixel*)malloc(header.width*header.height * sizeof(ImageRGBPixel));
        if (!compressedImage)
        {
            printf("Error while allocating memory\n");
            FailStatus = 1;
        }

        /* For debugging purposes */
        if(!FailStatus)
        {
           printf("Successful memory allocation\n");
        }

        /* Read image pixels */
        unsigned int pixel_index = 0;
        for (i = 0; i < colorCount; i++)
        {
            CompressedRGBPixel temp_compressed;
            fread(&temp_compressed, sizeof(unsigned char), sizeof(CompressedRGBPixel), inImgFile);

            for (j = 0; j < temp_compressed.runValue; j++)
            {
                compressedImage[pixel_index].r = temp_compressed.r;
                compressedImage[pixel_index].g = temp_compressed.g;
                compressedImage[pixel_index].b = temp_compressed.b;
                pixel_index++;
            }
        }

        fwrite(&header, sizeof(BMPHeader), 1, outImgFile);

        unsigned char pad = 0;
        unsigned char padding = (((header.width * 3) % 4) == 0) ? 0 : 4- ((header.width * 3) % 4);
        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fwrite(&compressedImage[(i*header.width)+j], sizeof(unsigned char), sizeof(ImageRGBPixel), outImgFile);
            }
            for (j = 0; j < padding; j++) {
                fwrite(&pad, sizeof(unsigned char), 1, outImgFile);
            }
        }

        /* Freeing data and closing files */
        if(compressedImage) free(compressedImage);
        fclose(inImgFile);
        fclose(outImgFile);
    }
    printf("\nDecompression Complete!!!\n");
}

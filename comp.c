#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "bmp.h"

/* Miscellaneous Variables */
int i, j, k;

/* Reading the BMP File */
FILE* inImgFile;
FILE* outImgFile;

/* Stores pixel value for uncompressed image */
typedef struct __attribute__((__packed__)) {
    unsigned char  b;
    unsigned char  g;
    unsigned char  r; 
} ImageRGBPixel;

/* Stores pixel value for compressed image */
typedef struct __attribute__((__packed__)) {
    unsigned char  runValue;
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
} CompressedRGBPixel;

/* Compression and Decompression Function */
void runLengthEncoding();
void runLengthDecoding();

int main()
{
    runLengthEncoding();
    runLengthDecoding();
    getchar();
    return 0;
}

void runLengthEncoding()
{
    char inFileName[] = "Input_Image.bmp"; /*Change this to change input file name */
    char outFileName[] = "Output_Image.bmp"; /*Change this to change output file name*/

    /* Header Variables */
    uint32_t total=0;
    uint16_t bpp, offset=0, nColorPlanes;
    uint8_t id[2];
    uint8_t reserved[4];
    uint32_t bmpSize=0, bmpDataOff=0, width, height, headerSize, compression, imageSize,xRes, yRes,nColors,nImportantColors;
    int numBytes = 0;

    /* For debugging */
    bool FailStatus = false;

    /* File Handlers */
    ImageRGBPixel **image;
    CompressedRGBPixel **compressedImage;

    /* Reading file and checking header details */
    inImgFile = fopen(inFileName, "rb");
    outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        /* Reading the file */
        offset = 0;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&id, 1, 2, inImgFile);
        printf("BMP Id: %c%c \n",id[0],id[1]);

        /*  Getting size of BMP File by setting offset to 2 */
        offset = 2;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&bmpSize, 4, 1, inImgFile);
        printf("BMP File Size: %u bytes\n",bmpSize);

        /* Getting reserved bits */
        offset = 6;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&reserved, 1, 4, inImgFile);
        printf("Reserved: %x%x%x%x bytes\n",reserved[0],reserved[1],reserved[2],reserved[3]);

        /* Getting bitmap data offset */
        offset = 10;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&bmpDataOff, 4, 1, inImgFile);
        printf("BMP Data Offset: %u\n",bmpDataOff);

        /*  Getting header size */
        offset = 14;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&headerSize, 4, 1, inImgFile);
        printf("Header Size: %u\n",headerSize);

        /* Using offset of 18 and 22 getting height and width of image */
        fseek(inImgFile, 18, SEEK_SET);
        fread(&width, 4, 1, inImgFile);
        fread(&height, 4, 1, inImgFile);
        printf("width: %u\n",width);
        printf("height: %u\n",height);

        /* Getting number of Color Planes */
        offset = 26;
        fseek(inImgFile, offset, SEEK_SET);
        fread(&nColorPlanes, 2, 1, inImgFile);

        /* Number of bits per pixel */
        offset=28;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&bpp, 2, 1, inImgFile);

        /* Compression Method */
        offset=30;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&compression, 4, 1, inImgFile);

        /* Image Size */
        offset=34;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&imageSize, 4, 1, inImgFile);

        /* X Resolution */
        offset=38;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&xRes, 4, 1, inImgFile);

        /* Y Resolution */
        offset=42;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&yRes, 4, 1, inImgFile);

        /* N Colors */
        offset=46;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&nColors, 4, 1, inImgFile);

        /* N Important Colors */
        offset=50;
        fseek(inImgFile, offset, SEEK_CUR);
        fread(&nImportantColors, 4, 1, inImgFile);

        /* Calculating total number of bytes in Image Pixel Array */
        numBytes = (bmpSize - bmpDataOff) / 3;
        printf("Number of Bytes Before Compression: %d\n",numBytes);

        /* Setting offset to start of pixel data */
        fseek(inImgFile, bmpDataOff, SEEK_SET);

       /* Allocating memory to image array as per the loaded image */
        image = (ImageRGBPixel**)malloc(height * sizeof(ImageRGBPixel*));
        if (!image)
        {
            printf("Error while allocating memory\n");
        }
        else
        {
            for (i = 0; i < height; i++)
            {
                image[i] = (ImageRGBPixel*)malloc(width * sizeof(ImageRGBPixel));
                if (!image[i])
                {
                    printf("Error while allocating memory\n");
                    FailStatus=true;
                }
            }
        }
        /* For debugging purposes
        if(!FailStatus)
        {
           printf("Successful memory allocation\n");
        } */
        for (i = height-1; i >= 0; i--)
        {
            for (j = 0; j < width; j++)
            {
                fread(&image[i][j], sizeof(unsigned char), sizeof(ImageRGBPixel), inImgFile);
            }
        }

        /* Run Length Encoding Algorithm */

        compressedImage = (CompressedRGBPixel**)malloc(height * sizeof(CompressedRGBPixel*));
        for (i = 0; i < height; i++)
        {
            compressedImage[i] = (CompressedRGBPixel*)malloc((width*2) * sizeof(CompressedRGBPixel));
        }

        int runLen=0;

        for (i = 0; i < height; i++)
        {
            k=0;
            for (j = 0; j < width; j++)
            {
                compressedImage[i][k].r=image[i][j].r;
                compressedImage[i][k].g=image[i][j].g;
                compressedImage[i][k].b=image[i][j].b;
                runLen=1;
                total++;
                while((j+1) < width)
                {
                    if((image[i][j].r == image[i][j+1].r) &&
                      (image[i][j].g == image[i][j+1].g) &&
                      (image[i][j].b == image[i][j+1].b))
                    {
                        runLen++;
                        j++;
                    }
                    else
                    {
                        break;
                    }
                }
                compressedImage[i][k++].runValue=runLen;
            }
        }
        printf("Total Bytes After Compression: %u\n",total);
        printf("Compression ratio = %f%%\n", 100.0 - (bmpDataOff + total) * 100.0 / bmpSize);

        bmpSize=total+bmpDataOff;
        printf("Size after compression: %u \n", bmpSize);
        compression=0x0001;
        char header[40];
        sprintf(header,"%c%c%u%c%c%c%c%u%u%u%u%"PRIu16"%"PRIu16"%u%u%u%u%u%u",
                id[0],id[1],bmpSize,reserved[0],reserved[1],reserved[2],reserved[3],
                bmpDataOff,headerSize,width,height,nColorPlanes,bpp,compression,imageSize,
                xRes,yRes,nColors,nImportantColors
                );
        printf(header);
        /* Writing the file */
        fwrite(header,sizeof(header)-1, 1, outImgFile);
        fwrite(compressedImage,sizeof(compressedImage),total, outImgFile);
        /* ------- */


        /* Freeing data and closing files */
        if(image) free(image);
        if(compressedImage) free(compressedImage);
        fclose(inImgFile);
        fclose(outImgFile);
    }
    printf("\nCompression Complete!!!\n");
}

void runLengthDecoding()
{
    char inFileName[] = "Output_Image.bmp"; /*Change this to change input file name */
    char outFileName[] = "Reconstructed_Image.bmp"; /*Change this to change output file name */
    /* Header Variables */
    uint32_t total=0;
    uint16_t bpp, offset=0, nColorPlanes;
    uint8_t id[2];
    uint8_t reserved[4];
    uint32_t bmpSize=0, bmpDataOff=0, width, height, headerSize, compression, imageSize,xRes, yRes,nColors,nImportantColors;
    int numBytes = 0;

    /* For debugging */
    bool FailStatus = false;

    /*File Handlers */
   /*  ImageRGBPixel **reconstructedImage; */
    CompressedRGBPixel **compressedImage;

    /* Reading file and checking header details */
    inImgFile = fopen(inFileName, "rb");
    outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
}

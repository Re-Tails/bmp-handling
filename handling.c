#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

int validate(char* inName)
{
    /* open and validate FILE* input */
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return 1;
    }

    /* read inFile's BITMAPFILEHEADER */
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFileP);

    /* read inFile's BITMAPINFOHEADER */
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFileP);

    /* check compatibility based on MS Docs */
    if (bf.bfType != 0x4d42 || /* BM stored in little-endian */
        bf.bfOffBits != 54 || /* 14 + 40 */
        bi.biSize != 40 || /* size of BITMAPINFOHEADER */
        bi.biBitCount != 24 || /* 24-bit BMP */
        bi.biCompression != 0) /* uncompressed */
    {
        fclose(inFileP);
        fprintf(stderr, "Unsupported file format.\n");
        return 2;
    }
    fclose(inFileP);
    return 0;
}

void encrypt(char* inName)
{
    char* outName = strcat(inName, "_encrypted");
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return 1;
    }

    /* create and validate FILE* output */
    FILE* outFileP = fopen(outName, "w");
    if (outFileP == NULL)
    {
        fclose(inFileP);
        fprintf(stderr, "Could not create %s.\n", 
                strcat(outName, "_encrypted"));
        return 2;
    }

    /* read inFile's BITMAPFILEHEADER */
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFileP);

    /* read inFile's BITMAPINFOHEADER */
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFileP);

    /* headers are the exact same */
   fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outFileP);
   fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outFileP);

    RGBTRIPLE triple;
    int row, col;
    int num = 0;
    int biHeight = abs(bi.biHeight);
    for(row = 0; row < biHeight; row++)
    {
        for(col = 0; col < bi.biWidth; col++)
        {
            fread(&triple, sizeof(RGBTRIPLE), 1, inFileP);
            changeColour(&triple, (num * row) % 2, (num * row) % 3,
                        num * row * col);
            num += abs(row - col) + 1;
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
    }

}

void changeColour(RGBTRIPLE* triple, int sign, int colour, int offset)
{
    if (sign % 2 == 0)
    {
        switch(colour)
        {
            case 0:
                triple->rgbtBlue = \
                    abs(triple->rgbtBlue - offset) % 256;
                break;
            case 1:
                triple->rgbtGreen = \
                    abs(triple->rgbtBlue - offset) % 256;
                break;
            case 2:
            triple->rgbtRed = \
                    abs(triple->rgbtBlue - offset) % 256;
                break;
        }
    }
    else
    {
        switch(colour)
        {
            case 0:
                triple->rgbtBlue = \
                    (triple->rgbtBlue + offset) % 256;
                break;
            case 1:
                triple->rgbtGreen = \
                    (triple->rgbtBlue + offset) % 256;
                break;
            case 2:
            triple->rgbtRed = \
                    (triple->rgbtBlue + offset) % 256;
                break;
        }
    } 
}
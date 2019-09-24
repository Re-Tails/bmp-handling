#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

#define BF_TYPE 0x4d42; /* BM stored in little-endian */
#define BF_OFFBITS 54; /* 14 + 40 */
#define BI_SIZE 40; /* size of BITMAPINFOHEADER */
#define BI_BITCOUNT 24; /* 24-bit BMP */
#define BI_COMPRESSION 0; /* uncompressed */

int validate(char* inName);
int encrypt(char* inName, int pass[]);
void changeColour(RGBTRIPLE* triple, int sign, int colour, int offset);

int validate(char* inName)
{
    /* open and validate FILE* input */
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return 1;
    }

    /* read inFileP's BITMAPFILEHEADER */
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFileP);

    /* read inFileP's BITMAPINFOHEADER */
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
int encrypt(char* inName, int pass[])
{
    char* outName = "test_encrypter.bmp";
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

    /* read inFileP's BITMAPFILEHEADER */
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFileP);

    /* read inFileP's BITMAPINFOHEADER */
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFileP);

    /* headers are the exact same */
   fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outFileP);
   fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outFileP);

    RGBTRIPLE triple;
    int row, col, pad;
    int num = 0;
    int biHeight = abs(bi.biHeight);
    int nPad = (4 - bi.biWidth * sizeof(RGBTRIPLE)) % 4;
    for(row = 0; row < biHeight; row++)
    {
        for(col = 0; col < bi.biWidth; col++)
        {
            fread(&triple, sizeof(RGBTRIPLE), 1, inFileP);
            num += (abs(row - col) + 1) % 50;
            changeColour(&triple, pass[num] % 2, pass[num + 1] % 3,
                        pass[num + 2]);
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
        fseek(inFileP, nPad, SEEK_CUR);
        for(pad = 0; pad < nPad; pad++)
        {
            fputc(0x00, outFileP);
        }
    }
    return 0;
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
                triple->rgbtGreen = \
                    abs(triple->rgbtGreen + offset) % 256;
                break;
            case 1:
                triple->rgbtGreen = \
                    abs(triple->rgbtBlue - offset) % 256;
                triple->rgbtRed = \
                    abs(triple->rgbtBlue + offset) % 256;
                break;
            case 2:
            triple->rgbtRed = \
                    abs(triple->rgbtBlue - offset) % 256;
            triple->rgbtBlue = \
                    abs(triple->rgbtBlue + offset) % 256;
                break;
        }
    }
    else
    {
        switch(colour)
        {
            case 0:
                triple->rgbtBlue = \
                    abs(triple->rgbtBlue + offset) % 256;
                triple->rgbtGreen = \
                    abs(triple->rgbtGreen - offset) % 256;
                break;
            case 1:
                triple->rgbtGreen = \
                    abs(triple->rgbtBlue + offset) % 256;
                triple->rgbtRed = \
                    abs(triple->rgbtBlue - offset) % 256;
                break;
            case 2:
            triple->rgbtRed = \
                    abs(triple->rgbtBlue + offset) % 256;
            triple->rgbtBlue = \
                    abs(triple->rgbtBlue - offset) % 256;
                break;
        }
    } 
}
int main(void)
{
    return 0;
}
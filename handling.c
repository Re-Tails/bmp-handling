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
void encryptColour(RGBTRIPLE* triple, int sign, int colour, int offset);
void decryptColour(RGBTRIPLE* triple, int sign, int colour, int offset);
char* encrypt(char* inName, int pass[], int length);
char* decrypt(char* inName, int pass[], int length);

/**
 * FUNCTION:
 * pass back file name without extension
 * 
 * IN:
 * full name of the input file "example.bmp"
 * 
 * OUT:
 * name of the file without the extension "example"
 * 
 * NOTE:
 * need to free the strings passed back
 * 
 **/
char* fileName(char* inName, int length)
{
    char* output = malloc(sizeof(char) * (length - 3));
    int i;
    for(i = 0; i < length; i++)
    {
        if(inName[i] == '.')
        {
            strncpy(output, inName, i);
            output[i] = '\0';
        }
    }
    return output;
}

/**
 * 
 * 
 * 
 * 
 **/
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

/**
 * 
 * 
 * 
 * 
 **/
char* encrypt(char* inName, int pass[], int length)
{
    char* outName = \
        strcat(fileName(inName, strlen(inName) - 1), "_encrypted.bmp");
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return NULL;
    }

    /* create and validate FILE* output */
    FILE* outFileP = fopen(outName, "w");
    if (outFileP == NULL)
    {
        fclose(inFileP);
        fprintf(stderr, "Could not create %s.\n", 
                strcat(outName, "_encrypted.bmp"));
        return NULL;
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
            num += (col + row) % 50;
            encryptColour(&triple, \
                        pass[num % length] % 2, \
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length]);
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
        fseek(inFileP, nPad, SEEK_CUR);
        for(pad = 0; pad < nPad; pad++)
        {
            fputc(0x00, outFileP);
        }
    }
    fclose(inFileP);
    fclose(outFileP);
    return outName;
}

/**
 * 
 * 
 * 
 * 
 **/
void encryptColour(RGBTRIPLE* triple, int sign, int colour, int offset)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    /*
    printf("0: %d %d %d  %d\n", *blue, *green, *red, offset);
    */
    if (sign % 2 == 0)
    {
        switch(colour)
        {
            case 0:
                *blue = \
                    abs(*blue - offset) % 256;
                *green = \
                    abs(*green + offset) % 256;
                break;
            case 1:
                *green = \
                    abs(*green - offset) % 256;
                *red = \
                    abs(*red + offset) % 256;
                break;
            case 2:
                *red = \
                    abs(*red - offset) % 256;
                *blue = \
                    abs(*blue + offset) % 256;
                break;
        }
    }
    else
    {
        switch(colour)
        {
            case 0:
                *blue = \
                    abs(*blue + offset) % 256;
                *green = \
                    abs(*green - offset) % 256;
                break;
            case 1:
                /* printf("#: %d %d %d  %d\n", *blue, *green, *red, offset); */
                *green = \
                    abs(*green + offset) % 256;
                *red = \
                    abs(*red - offset) % 256;
                /* printf("1: %d %d %d  %d\n", *blue, *green, *red, offset); */
                break;
            case 2:
                *red = \
                    abs(*red + offset) % 256;
                *blue = \
                    abs(*blue - offset) % 256;
                break;
        }
    }
    /* printf("1: %d %d %d  %d\n", *blue, *green, *red, offset); */
}

/**
 * 
 * 
 * 
 * 
 **/
char* decrypt(char* inName, int pass[], int length)
{
    char* decryptedName = \
        strcat(fileName(inName, strlen(inName)), "_decrypted.bmp");
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return NULL;
    }

    /* create and validate FILE* output */
    FILE* outFileP = fopen(decryptedName, "w");
    if (outFileP == NULL)
    {
        fclose(inFileP);
        fprintf(stderr, "Could not create %s.\n", 
                strcat(decryptedName, "_decrypted"));
        return NULL;
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
            num += (col + row) % 50;
            decryptColour(&triple, \
                        pass[num % length] % 2, \
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length]);
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
        fseek(inFileP, nPad, SEEK_CUR);
        for(pad = 0; pad < nPad; pad++)
        {
            fputc(0x00, outFileP);
        }
    }
    fclose(inFileP);
    fclose(outFileP);
    return decryptedName;
}

void decryptColour(RGBTRIPLE* triple, int sign, int colour, int offset)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    /*
    printf("m: %d %d %d  %d\n", *blue, *green, *red, offset);
    */
    if (sign % 2 == 0)
    {
        switch(colour)
        {
            case 0:
                if(offset < *blue)
                    *blue = (*blue + offset) % 256;
                else
                    *blue = abs(*blue - offset) % 256;
                if(*green - offset > 0)
                    *green = (*green - offset) % 256;
                else
                    *green = abs(*green - offset + 256) % 256;
                
                break;
            case 1:
                if(offset < *green)
                    *green = (*green + offset) % 256;
                else
                    *green = abs(*green - offset) % 256;
                if(*red - offset > 0)
                    *red = (*red - offset) % 256;
                else
                    *red = abs(*red - offset + 256) % 256;
                
                break;
            case 2:
                if(offset < *red)
                    *red = (*red + offset) % 256;
                else
                    *red = abs(*red - offset) % 256;
                if(*blue - offset > 0)
                    *blue = (*blue - offset) % 256;
                else
                    *blue = abs(*blue - offset + 256) % 256;
                
                break;
        }
    }
    else
    {
        switch(colour)
        {
            case 0:
                if(offset < *green)
                    *green = (*green + offset) % 256;
                else
                    *green = abs(*green - offset) % 256;
                if(*blue - offset > 0)
                    *blue = (*blue - offset) % 256;
                else
                    *blue = abs(*blue - offset + 256) % 256;
                break;
            case 1:
                if(offset < *red)
                {
                    *red = (*red + offset) % 256;
                }
                else
                {
                    *red = abs(*red - offset) % 256;
                }
                if(*green - offset > 0)
                    *green = (*green - offset) % 256;
                else
                    *green = abs(*green - offset + 256) % 256;
                break;
            case 2:
                if(offset < *blue)
                    *blue = (*blue + offset) % 256;
                else
                    *blue = abs(*blue - offset) % 256;
                if(*red - offset > 0)
                    *red = (*red - offset) % 256;
                else
                    *red = abs(*red - offset + 256) % 256;
                break;
        }
    }
    /*
    printf("f: %d %d %d  %d\n", *blue, *green, *red, offset);
    */
}

int main(void)
{
    int password[] = {100,200,200,200,200};
    validate("index.bmp");
    encrypt("index.bmp", password, 5);
    decrypt("index_encrypted.bmp", password, 5);
    return 0;
}
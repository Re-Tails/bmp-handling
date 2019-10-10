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
void encryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);
void decryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);
char* encrypt(char* outName, char* inName, int pass[], int length, int debug);
char* decrypt(char* outName, char* inName, int pass[], int length, int debug);

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
/*
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
*/
/**
 * returns 0 only if the image is compatible with the program
 * 
 * IN: 
 * char* inName: name of the file to check
 * 
 * OUT:
 * int: 
 *  0 valid
 *  1 could not open, invalid
 *  2 could open but still invalid
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
 * encrypts the passed in image based on password
 * 
 * IN:
 * char* inName: filename to encrypt
 * int pass[]: password to use
 * int length: length of password
 * int debug: 1 enable; 0 disable
 * 
 * OUT:
 * char*: name of the encrypted file
 * 
 **/
char* encrypt(char* outName, char* inName, int pass[], int length, int debug)
{
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
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length], \
                        debug);
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
 * Changes the colour of a pixel during encryption
 * 
 * IN:
 * RGBTRIPLE* triple: the pixel to be changed
 * int sign: uses + or -
 * int colour: the colour value that will not change
 * int offset: offset to be changed
 * int debug: 1 enable; 0 disable
 * 
 * OUT:
 * none
 * 
 **/
void encryptColour(RGBTRIPLE* triple, int colour, int offset, int debug)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    if(debug == 1)
        printf("0: %d %d %d  %d\n", *blue, *green, *red, offset);
    switch(colour)
    {
        case 0:
            *blue = \
                abs(*blue + offset % 256);
            *green = \
                abs(*green + offset) % 256;
            break;
        case 1:
            *green = \
                abs(*green + offset) % 256;
            *red = \
                abs(*red + offset) % 256;
            break;
        case 2:
            *red = \
                abs(*red + offset) % 256;
            *blue = \
                abs(*blue + offset) % 256;
            break;
    }
    if(debug == 1)
        printf("1: %d %d %d  %d\n", *blue, *green, *red, offset);
}

/**
 * decrypts the passed in image based on password
 * 
 * IN:
 * char* inName: filename to decrypt
 * int pass[]: password to use
 * int length: length of password
 * int debug: 1 enable; 0 disable
 * 
 * OUT:
 * char*: name of the decrypted file
 * 
 * 
 * 
 **/
char* decrypt(char* outName, char* inName, int pass[], int length, int debug)
{
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
                strcat(outName, "_decrypted"));
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
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length], \
                        debug);
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
 * Changes the colour of a pixel during decryption
 * 
 * IN:
 * RGBTRIPLE* triple: the pixel to be changed
 * int sign: uses + or -
 * int colour: the colour value that will not change
 * int offset: offset to be changed
 * int debug: 1 enable; 0 disable
 * 
 * OUT:
 * none
 * 
 **/
void decryptColour(RGBTRIPLE* triple, int colour, int offset, int debug)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    if(debug == 1)
        printf("0: %d %d %d  %d\n", *blue, *green, *red, offset);

    switch(colour)
    {
        case 0:
            if(*blue - offset > 0)
                *blue = *blue - offset;
            else
                *blue = (*blue - offset + 256) % 256;
            if(*green - offset > 0)
                *green = *green - offset;
            else
                *green = (*green - offset + 256) % 256;
            
            break;
        case 1:
            if(*green - offset > 0)
                *green = *green - offset;
            else
                *green = (*green - offset + 256) % 256;
            if(*red - offset > 0)
                *red = *red - offset;
            else
                *red = (*red - offset + 256) % 256;
            
            break;
        case 2:
            if(*red - offset > 0)
                *red = *red - offset;
            else
                *red = (*red - offset + 256) % 256;
            if(*blue - offset > 0)
                *blue = *blue - offset;
            else
                *blue = (*blue - offset + 256) % 256;
            break;
    }
    if(debug == 1)
        printf("1: %d %d %d  %d\n", *blue, *green, *red, offset);
}

int main(void)
{
    int password[] = {100,100,100,100,100};
    validate("pointillist.bmp");
    
    encrypt("pointillist_e.bmp", "pointillist.bmp", password, 5, 0);
    
    printf("\n");

    decrypt("pointillist_e_d.bmp", "pointillist_e.bmp", password, 5, 0);
    
    return 0;
}
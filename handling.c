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
        fprintf(stderr, "Could not create %s.\n", strcat(outName, "_encrypted"));
        return 2;
    }

    /* read from inFile and write to outFile 
    * headers are the exact same
    * change the RGB of the pixels
    */

   fread(outFileP, sizeof(BITMAPFILEHEADER), 1, inFileP);
   fread(outFileP, sizeof(BITMAPINFOHEADER), 1, inFileP);
}
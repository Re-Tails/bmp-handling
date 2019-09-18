#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char *LoadBMP(char *filename, BITMAPINFOHEADER *BMPHeader);

int main(void)
{
	printf("Hello world\n");


	/*To test LoadBMP, please change to false if not in use*/
	if (1 == 1)
	{
		BITMAPINFOHEADER bitmapInfoHeader;
		unsigned char *image;
		image = LoadBMP("MARBLES.BMP", &bitmapInfoHeader);
		printf
		printf("%s\n", image);
	}
	
	return 0;
}


/*
* Reads a BMP picture into memory
* INPUTS:
*	char* filename: Name of file to open
*	BITMAPINFOHEADER: Contains info about the picture
* RETURNS:
*	unsigned char*: the actual picture
* TODO:
*	Increased robustness, error messages
* https://en.wikipedia.org/wiki/BMP_file_format
*/
unsigned char* LoadBMP(char* filename, BITMAPINFOHEADER* bitmapInfoHeader)
{
	FILE *BMP_p;
	/*Contains necessary info to read the file*/
	BITMAPFILEHEADER bitmapFileHeader;
	/*The actual picture*/
	unsigned char* bitmapImage;

	BMP_p = fopen(filename, "rb");
	/*Check that file exists*/
	if (BMP_p == NULL)
	{
		printf("File does not exist\n");
		return NULL;
	}

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	/*Move to beginning of bits*/
	fseek(BMP_p, bitmapFileHeader.bfOffBits, SEEK_SET);
	/*Allocate memory for the image*/
	bitmapImage = (unsigned char*) malloc((*bitmapInfoHeader).biSizeImage);
	fread(&bitmapImage, (*bitmapInfoHeader).biSizeImage, 1, BMP_p);

	fclose(BMP_p);
	return bitmapImage;
}

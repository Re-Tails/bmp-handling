#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

BMPIMAGE LoadBMP(char* filename);
void SaveBMP(char* filename, BMPIMAGE* bitmapImage);

int main(void)
{
	printf("Hello world\n");


	/*To test LoadBMP and SaveBMP, please change to false if not in use*/
	if (1 == 1)
	{
		BMPIMAGE bitmapImage;
		bitmapImage = LoadBMP("MARBLES.BMP");
		SaveBMP("SPY.BMP", &bitmapImage);
	}
	
	return 0;
}


/*
* Reads a BMP picture into memory
* INPUTS:
*	char* filename: Name of file to open
*	BMPIMAGE: Contains all info about the picture
* RETURNS:
* -
* TODO:
*	Increased robustness, error messages
* https://en.wikipedia.org/wiki/BMP_file_format
*/

BMPIMAGE LoadBMP(char* filename)
{
	FILE *BMP_p;
	BMPIMAGE *bitmapImage = malloc(sizeof(*bitmapImage));
	/*The actual picture*/

	BMP_p = fopen(filename, "rb");
	/*Check that file exists*/
	if (BMP_p == NULL)
	{
		printf("File does not exist\n");
	}

	fread(&bitmapImage->bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fread(&bitmapImage->bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	/*Move to beginning of bits*/
	fseek(BMP_p, bitmapImage->bitmapFileHeader.bfOffBits, SEEK_SET);
	/*Allocate memory for the image*/
	bitmapImage->image = malloc(sizeof(bitmapImage->image) * bitmapImage->bitmapInfoHeader.biSizeImage);
	fread(&bitmapImage->image, (*bitmapImage).bitmapInfoHeader.biSizeImage, 1, BMP_p);
	fclose(BMP_p);
	return *bitmapImage;
}



void SaveBMP(char* filename, BMPIMAGE* bitmapImage)
{
	FILE *BMP_p;

	BMP_p = fopen(filename, "wb");

	fwrite(&bitmapImage->bitmapFileHeader, sizeof(bitmapImage->bitmapFileHeader), 1, BMP_p);
	fwrite(&bitmapImage->bitmapInfoHeader, sizeof(bitmapImage->bitmapInfoHeader), 1, BMP_p);
	fwrite(bitmapImage->image, bitmapImage->bitmapInfoHeader.biSizeImage, 1, BMP_p);
	
	fclose(BMP_p);
}
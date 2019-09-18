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
		bitmapImage = LoadBMP("LAND.BMP");
		printf("%d\n", bitmapImage.bitmapInfoHeader.biSize);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biHeight);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biWidth);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biPlanes);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biBitCount);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biCompression);
		printf("%d\n", bitmapImage.bitmapInfoHeader.biSizeImage);

		SaveBMP("SP2.BMP", &bitmapImage);
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
	int elements_read;

	elements_read = fread(&bitmapImage->bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	printf("%d\n", elements_read);
	elements_read = fread(&bitmapImage->bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	printf("%d\n", elements_read);
	/*Move to beginning of bits*/
	/*fseek(BMP_p, bitmapImage->bitmapFileHeader.bfOffBits, SEEK_SET);*/
	/*Allocate memory for the image*/
	printf("SIZE: %d\n", bitmapImage->bitmapInfoHeader.biSizeImage);
	bitmapImage->image = (unsigned char*) malloc(bitmapImage->bitmapInfoHeader.biSizeImage);
	printf("test");
	fread(bitmapImage->image, bitmapImage->bitmapInfoHeader.biSizeImage, 1, BMP_p);
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
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

/* IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
struct Node_t
{
    int data;
    char name[10];
    struct Node_t* next;
};

BMPIMAGE LoadBMP(char* filename);
void SaveBMP(char* filename, BMPIMAGE bitmapImage);
void FreeBMP(BMPIMAGE bitmapImage);
void Save_History(struct Node_t **top, int x, char name[]);
int Is_Empty(struct Node_t *top);
void Pop(struct Node_t **top);
void Delete_History(struct Node_t **top);
void Print_History(struct Node_t *top);






int main(void)
{

	struct Node_t *top = NULL;
	Save_History(&top, 1, "test.bmp");
	Save_History(&top, 4, "test.bmp");
	Save_History(&top, 2, "hello.bmp");
	Save_History(&top, 3, "why.bmp");
	Print_History(top);
	Delete_History(&top);


	/*To test LoadBMP and SaveBMP, please change to false if not in use*/
	if (1 == 0)
	{
		BMPIMAGE bitmapImage;
		bitmapImage = LoadBMP("6x6_24bit.bmp");
		SaveBMP("SPY.BMP", bitmapImage);
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
	BMPIMAGE bitmapImage;
	/*The actual picture*/

	BMP_p = fopen(filename, "rb");
	/*Check that file exists*/
	if (BMP_p == NULL)
	{
		printf("File does not exist\n");
	}
	
	fread(&bitmapImage.bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fread(&bitmapImage.bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	/*Move to beginning of bits*/
	/*fseek(BMP_p, bitmapImage->bitmapFileHeader.bfOffBits, SEEK_SET); Does not work? */
	/*Allocate memory for the image*/
	bitmapImage.image = (unsigned char*) malloc(bitmapImage.bitmapInfoHeader.biSizeImage);
	/*FREE MEMORY*/
	fread(bitmapImage.image, bitmapImage.bitmapInfoHeader.biSizeImage, 1, BMP_p);
	fclose(BMP_p);
	return bitmapImage;
}



void SaveBMP(char* filename, BMPIMAGE bitmapImage)
{
	FILE *BMP_p;

	BMP_p = fopen(filename, "wb");

	fwrite(&bitmapImage.bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fwrite(&bitmapImage.bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	fwrite(bitmapImage.image, bitmapImage.bitmapInfoHeader.biSizeImage, 1, BMP_p);
	
	fclose(BMP_p);
}


void FreeBMP(BMPIMAGE bitmapImage)
{
    free(bitmapImage.image);
}



void Save_History(struct Node_t **top, int x, char name[])
{
	struct Node_t* node = NULL;
	node = (struct Node_t*) malloc(sizeof(struct Node_t));

	node->data = x;
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (name[i] !='.')
		{
			node->name[i] = name[i];
		}
	}
	node->next = *top;
	*top = node;
} 

int Is_Empty(struct Node_t *top)
{
	if (top == NULL)
	{
		return 1;
	}
	return 0;
}

void Pop(struct Node_t **top)
{
	struct Node_t *node;
	node = *top;
	*top = (*top)->next;
	free(node);
}

void Delete_History(struct Node_t **top)
{
	if (Is_Empty(*top) == 0)
	{
		Pop(top);
		Delete_History(top);
	}
}

void Print_History(struct Node_t *top)
{
	if (Is_Empty(top) == 0)
	{
		switch (top->data)
        {
            case 1: printf("Compressed ");
                    break;
            case 2: printf("Decompressed ");
                    break;
            case 3: printf("Encrypted ");
                    break;
            case 4: printf("Decrypted ");
                    break;
        }
        printf("%s.BMP\n", top->name);
		Print_History(top->next);
	}
}
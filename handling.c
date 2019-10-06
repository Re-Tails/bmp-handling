#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

/* 
* Structure for linked list, limit on file length now, gonna fix it a bit better
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
void Print_Node(struct Node_t *top);
void Search_Action(struct Node_t *top, int Search_Number);



int main(void)
{

	struct Node_t *top = NULL;
	Save_History(&top, 1, "test.bmp");
	Save_History(&top, 4, "test.bmp");
	Save_History(&top, 2, "hello.bmp");
	Save_History(&top, 3, "why.bmp");
	Save_History(&top, 4, "test2.bmp");
	Print_History(top);
	printf("------------------\n");
	Search_Action(top, 4);
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
* Can be useful if you want to perform an operation while keeping the whole file in memory
* INPUTS:
*	char* filename: Name of file to open
* RETURNS:
* 	BMPIMAGE: Contains all info about the picture
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
	
	/*Read the two headers*/
	fread(&bitmapImage.bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fread(&bitmapImage.bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	/*Allocate memory for the image*/
	bitmapImage.image = (unsigned char*) malloc(bitmapImage.bitmapInfoHeader.biSizeImage);
	/*FREE MEMORY*/
	fread(bitmapImage.image, bitmapImage.bitmapInfoHeader.biSizeImage, 1, BMP_p);
	fclose(BMP_p);
	return bitmapImage;
}

/*
* Saves a BMP picture to a file
* Can be useful together with previous function to copy paste image
* INPUTS:
*	char* filename: Name of file to save to
* 	BMPIMAGE: Contains all info about the picture
* RETURNS:
* TODO:
*	Increased robustness, error messages
* https://en.wikipedia.org/wiki/BMP_file_format
*/

void SaveBMP(char* filename, BMPIMAGE bitmapImage)
{
	FILE *BMP_p;

	BMP_p = fopen(filename, "wb");

	fwrite(&bitmapImage.bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BMP_p);
	fwrite(&bitmapImage.bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BMP_p);
	fwrite(bitmapImage.image, bitmapImage.bitmapInfoHeader.biSizeImage, 1, BMP_p);
	
	fclose(BMP_p);
}


/*
*Free up the space used by the picture, since it's a variable length
*INPUTS:
*	BMPIMAGE: The image to free up
*/

void FreeBMP(BMPIMAGE bitmapImage)
{
    free(bitmapImage.image);
}


/*
*Saves a command to the history
*The number input represents one operation, could be done in a better way
*INPUT:
*	struct Node_t **: The uppermost node in the linked list
*	int: The command that was used, 1-Compressed, 2-Decompressed, 3-Encrypted, 4-Decrypted
*	char[]: Name of the file the operation was performed on
*/
void Save_History(struct Node_t **top, int x, char name[])
{
	struct Node_t* node = NULL;
	/*Allocate memory*/
	node = (struct Node_t*) malloc(sizeof(struct Node_t));

	node->data = x;
	int i;
	for (i = 0; i < 10; ++i)
	{
		/*Save name before .bmp*/
		if (name[i] !='.')
		{
			node->name[i] = name[i];
		}
	}
	/*Make the new node point to previous top*/
	node->next = *top;
	/*Return the new node as top*/
	*top = node;
} 

/*
*Helper function to check if history is empty
*INPUT:
*	Node_t*: Uppermost node
*RETURN:
*	Int: 1 if empty, else 0
*/
int Is_Empty(struct Node_t *top)
{
	if (top == NULL)
	{
		return 1;
	}
	return 0;
}

/*
*Deletes the uppermoost node
*INPUT:
*	Node_t**: Pointer to upper node
*/
void Pop(struct Node_t **top)
{
	struct Node_t *node;
	node = *top;
	*top = (*top)->next;
	free(node);
}

/*
*Deletes all history by recursive methods
*INPUT:
*	Node_t**: Upper node
*/
void Delete_History(struct Node_t **top)
{
	/*If the history isn't empty, delete and call again on next node*/
	if (Is_Empty(*top) == 0)
	{
		Pop(top);
		Delete_History(top);
	}
}


/*
*Prints all history in the linked list, starting at latest action
*INPUT:
*	Node_t*: Top node
*/
void Print_History(struct Node_t *top)
{
	if (Is_Empty(top) == 0)
	{
		Print_Node(top);
        /*Call print again for the next node*/
		Print_History(top->next);
	}
}

/*
*Prints a single node
*INPUT:
*	Node_t*: The node
*/
void Print_Node(struct Node_t *top)
{
	/*Depending on number saved, print which commando was used*/
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
        /*Add .bmp*/
        printf("%s.BMP\n", top->name);
}

void Search_Action(struct Node_t *top, int Search_Number)
{
	if (Is_Empty(top) == 0)
	{
		if (top->data == Search_Number)
		{
			Print_Node(top);
		}
		Search_Action(top->next, Search_Number);
	}
}
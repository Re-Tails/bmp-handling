#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

/* 
* Structure for linked list, limit on file length now, gonna fix it a bit better
*/
struct Node_t
{
    int data;
<<<<<<< Updated upstream
    char name[10];
=======
    char in_name[20];
    char out_name[20];
>>>>>>> Stashed changes
    struct Node_t* next;
};

BMPIMAGE LoadBMP(char* filename);
void SaveBMP(char* filename, BMPIMAGE bitmapImage);
void FreeBMP(BMPIMAGE bitmapImage);
void Save_History(struct Node_t **top, int x, char in_name[], char out_name[]);
int Is_Empty(struct Node_t *top);
void Pop(struct Node_t **top);
void Delete_History(struct Node_t **top);
void Print_History(struct Node_t *top);
void Print_Node(struct Node_t *top);
void Search_Action(struct Node_t *top, int Search_Number);
<<<<<<< Updated upstream
=======
void Search_File(struct Node_t *top, char filename[]);
void runLengthEncoding(char inFileName[], char outFileName[]);
void runLengthDecoding(char inFileName[], char outFileName[]);
void decryptColour(RGBTRIPLE* triple, int sign, int colour, int offset, int debug);

int main(void)
{
    struct Node_t* history = NULL;

    int exit;
    exit = 0;
    while(exit == 0) 
    {
    /*to print the menu list and call functions and allow the program to run multiple instances without closing */
        char inName[20];
        char outName[20];
        int choice = 0;
        int pass[50] = {0};
        printf("\ntype 1 for encryption\n\ntype 2 for decryption\n\ntype 3 for compression\n\ntype 4 for decompresion\n\n");
        printf("type 5 for compresion and encryption\n\ntype 6 for decompresion and decryption\n\ntype 7 to view history");
        printf("\n\ntype 8 to exit>");

        
        scanf("%d", &choice);
        
        if (choice == 1)
        {           
    /* to allow a choice to be made by user to encrypt an image */
            printf("\nenter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            validate(inName);
            printf("option 1 encryption chosen\n");
            password(pass);
            encrypt(outName, inName, pass, 10, 0);
            printf("%s created.", outName);
            Save_History(&history, 1, inName, outName);
        }
        else if (choice == 2)
        {
            /* to allow a choice to be made by user to decrypt an image*/
            printf("option 1 encryption chosen\n");
        printf("\nenter infile.bmp>");
            scanf("%s", inName);
            validate(inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            password(pass); 
            decrypt(outName, inName, pass, 10, 0);
            printf("%s created.", outName);
            Save_History(&history, 2, inName, outName);

        }
        
        else if (choice ==3)
        {
            /* to allow a choice to be made by user to compress an image */
            printf("\nenter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            printf("option 3 compression chosen\n");
            runLengthEncoding(inName, outName);
            Save_History(&history, 3, inName, outName);

        }
        else if (choice ==4)
        {
            /* to allow a choice to be made by user to decompress an image */
            printf("\nenter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            printf("option 4 decompresion chosen\n");
            runLengthDecoding(inName, outName);
            Save_History(&history, 4, inName, outName);

        }
/*
        else if (choice == 5){
         to allow a choice to be made by user to encrypt and compres an image
            printf("option 5 compresion and encryption chosen");
            inName = LoadBMP(* filename);
            password(* pass);
            validate(* inName);
            inName= encrypt(&inName, &pass[50]);
            filename= strcat(inName,"_encrypted.bmp");
            runLengthEncoding( inFileName[],  outFileName[]);;
            FreeBMP(BMPIMAGE bitmapImage);
            Save_History(&history, 5, inName, outName);

        }
        else if (choice == 6){
        to allow a choice to be made by user to decrypt and decompress
            printf("option 6 decompression and decryption chosen");
            inName = LoadBMP(* filename);
            decompress;
            password(* pass);
            validate(* inName);
            inName = decrypt;
            filename= strcat(inName,"_encrypted.bmp");
            SaveBMP(&filename, & bitmapImage);
            FreeBMP(&bitmapImage);
            Save_History(&history, 6, inName, outName);

        }
*/
        else if (choice ==7)
        {
            printf("\ntype 1 to view all history\n\ntype 2 to search for action\n\n");
            printf("type 3 to search for file\n\ntype 4 to delete history\n\ntype anything else to exit>");

            scanf("%d", &choice);

            if (choice==1)
            {
                printf("Latest action:\n");
                Print_History(history);
            }
            else if (choice==2)
            {
                printf("\ntype 1 to show encryptions\ntype 2 to show decryptions\ntype 3 to show compressions\n");
                printf("type 4 to show decompressions\ntype 5 to show combined encryptions and compressions\n");
                printf("type 6 for combined decryptions and decompressions>");
                scanf("%d", &choice);
                printf("Latest action:\n");
                Search_Action(history, choice);
            }
            else if (choice==3)
            {
                printf("\nwhich file do you want to search for>");
                scanf("%s", inName);
                printf("Latest action:\n");
                Search_File(history, inName);
            }
            else if (choice==4)
            {
                Delete_History(&history);
            }
        

        }
        else if (choice == 8)
        {
            /* to allow a choice to be made by user to exit the program*/
            printf("exiting %d\n", exit);
            Delete_History(&history);
            exit = 1;
        }
        else
        {
            /* to alert the user if they enter an incorect value*/
            printf("invalid input\n");
        }
        
    }
    printf("%d", exit);
    return 0;
};
>>>>>>> Stashed changes



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
<<<<<<< Updated upstream
	FILE *BMP_p;

	BMP_p = fopen(filename, "wb");
=======
/*collect string from user make into numbers make 50 length through repetition
of password */
    char password [10];
    
    int I, Length, error, Q;

    error =0;
    
    Length =1;
    
    while (error <(Length))
    {
        /*to recive the input of user and make numerical and ensure only valid characters are used*/
        
        printf("enter pasword using alphanumeric characters\n");
        
        scanf("%s",password);
              
        Length = strlen(password) ;     
        
        error = 0;
             
        for (I=0; I<= (Length-1); I++){
            
            /* to increment I*/
    
            if (((password[I] >=48 && password[I] <=57) || (password[I] >=65 && password[I] <= 90) || (password[I] >= 97 && password[I] <= 122)) && (Length >3 && Length<=10)){
                /* to check if each character is valid and that it is of desired lenght*/
                error++;
                
            };
                
        };
      
        if (error != (Length)){
            
            /*error mesage for pasword*/
        
            printf("invalid password (cannot contain specical characters) and must be between 3 and 10 character long\n");
            
            error=0;
        };
    };
        
        
        I=0;

        for (Q=0; Q<= 50; Q++){
            /* to make pasword 50 lenght*/
            
            pass[Q] = password[I];
            
            I++;

            printf("%d", password[I]);
            
            if (I == Length){
                
                /*to make pasword restart when it reaches the end*/
                
                I=0;
            };  
            
        };
        
};
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream

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
=======
void Save_History(struct Node_t **top, int x, char inname[], char outname[])
{
    struct Node_t* node = NULL;
    node = (struct Node_t*) malloc(sizeof(struct Node_t));
    int length;
    length = strlen(inname);
    printf("%d\n", length);
    node->data = x;
    int i;
    for (i = 0; i < length; ++i)
    {        
        node->in_name[i] = inname[i];
        node->in_name[i+1] = '\0';
    }

    length = strlen(outname);
    for (i = 0; i < length; ++i)
    {
        node->out_name[i] = outname[i];
        node->out_name[i+1] = '\0';
    }

    node->next = *top;
    *top = node;
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
=======
/*
*Prints all history in the linked list, starting at latest action
*INPUT:
*   Node_t*: Top node
*/
void Print_History(struct Node_t *top)
{
    if (Is_Empty(top) == 0)
    {
        Print_Node(top);
        /*Call print again for the next node*/
        if (Is_Empty(top->next) == 0)
        {
            Print_History(top->next);
        }
    }
}

/*
*Prints a single node
*INPUT:
*   Node_t*: The node
*/
void Print_Node(struct Node_t *top)
{
    /*Depending on number saved, print which commando was used*/
    switch (top->data)
    {
        case 1: printf("Encrypted ");
                break;
        case 2: printf("Decrypted ");
                break;
        case 3: printf("Compressed ");
                break;
        case 4: printf("Decompressed ");
                break;
        case 5: printf("Compressed and encrypted ");
                break;
        case 6: printf("Decompressed and decrypted ");
                break;
    }
    printf("%s into %s\n", top->in_name, top->out_name);
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

void Search_File(struct Node_t *top, char filename[])
{    
    if (Is_Empty(top) == 0)
    {
        if (strcmp(top->in_name, filename) == 0 || strcmp(top->out_name, filename) == 0)
        {
            Print_Node(top);
        }
        Search_File(top->next, filename);
    }
}
>>>>>>> Stashed changes

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
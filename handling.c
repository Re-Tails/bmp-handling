#include <stdio.h> /*printf, scanf,fopen, fclose, fprintf*/
#include <stdlib.h> /* malloc, free*/
#include <string.h> /* strcat, strlen*/
#include "bmp.h"

struct Node_t
{
    int data;
    char in_name[20];
    char out_name[20];
    struct Node_t* next;
};

#define BF_TYPE 0x4d42 /* BM stored in little-endian */
#define BF_OFFBITS 54 /* 14 + 40 */
#define BI_SIZE 40 /* size of BITMAPINFOHEADER */
#define BI_BITCOUNT 24 /* 24-bit BMP */
#define BI_COMPRESSION 0 /* uncompressed */
#define MAX_FILENAME_LENGTH 50

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

void password(int pass[]);
void compress(char* image);
void decrypt(char* outName, char* inName, int pass[], int length, int debug);
void decompress(char* image);
void encryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);
int validate(char* inName);
void encrypt(char* outName, char* inName, int pass[], int length, int debug);
void Save_History(struct Node_t **top, int x, char in_name[], char out_name[]);
int Is_Empty(struct Node_t *top);
void Pop(struct Node_t **top);
void Delete_History(struct Node_t **top);
void Print_History(struct Node_t *top);
void Print_Node(struct Node_t *top);
void Search_Action(struct Node_t *top, int Search_Number);
void Search_File(struct Node_t *top, char filename[]);
void runLengthEncoding(char inFileName[], char outFileName[], int debug);
void runLengthDecoding(char inFileName[], char outFileName[], int debug);
void decryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);

int main(int argc, char* argv[])
{
    struct Node_t* history = NULL;
    int exit = 0;
	int debug = 0;
    if(argc > 1)
    {
        if(strcmp(argv[1], "-d") == 0)
        {
            debug = 1;
            printf("Debug mode enabled\n");
            printf("USE WITH LARGE IMAGES WILL"
                    "CAUSE LONG PROCESSING TIMES!\n");
        }
    }
    while(exit == 0) 
    {
	/*to print the menu list and call functions and 
    allow the program to run multiple instances without closing */
        char inName[MAX_FILENAME_LENGTH];
        char outName[MAX_FILENAME_LENGTH];
        int choice = 0;
        int pass[50] = {0};
        printf("type 1 for encryption\n"
                "type 2 for decryption\n"
                "type 3 for compression\n"
                "type 4 for decompresion\n"
                "type 5 to view history\n"
                "type 6 to exit\n>");
        
        scanf("%d", &choice);
        
        if (choice == 1)
        {			
        /* to allow a choice to be made by user to encrypt an image */
            printf("option 1 encryption chosen\n");
            printf("enter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            validate(inName);
            printf("option 1 encryption chosen\n");
            password(pass);
            encrypt(outName, inName, pass, 10, debug);
            printf("%s created.\n", outName);
            Save_History(&history, 1, inName, outName);
        }
        else if (choice == 2)
        {
			/* to allow a choice to be made by user to decrypt an image*/
            printf("option 2 decryption chosen\n");
	        printf("enter infile.bmp>");
            scanf("%s", inName);
            validate(inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            password(pass); 
            decrypt(outName, inName, pass, 10, debug);
            printf("%s created.\n", outName);
            Save_History(&history, 2, inName, outName);

        }
        
        else if (choice == 3)
        {
            /* to allow a choice to be made by user to compress an image */
            printf("option 3 compression chosen\n");
            printf("enter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            runLengthEncoding(inName, outName, debug);
            printf("%s created.\n", outName);
            Save_History(&history, 3, inName, outName);
        }
        else if (choice == 4)
        {
            /* to allow a choice to be made by user to decompress an image */
            printf("option 4 decompression chosen\n");
            printf("enter infile.bmp>");
            scanf("%s", inName);
            printf("enter outfile.bmp>");
            scanf("%s", outName);
            printf("option 4 decompresion chosen\n");
            runLengthDecoding(inName, outName, debug);
            printf("%s created.\n", outName);
            Save_History(&history, 4, inName, outName);

        }

        else if (choice == 5)
        {
            /* allow user to access the history functinality */
            printf("type 1 to view all history\n"
                    "type 2 to search for action\n"
                    "type 3 to search for file\n"
                    "type 4 to delete history\n"
                    "type anything else to exit\n"
                    ">");

            scanf("%d", &choice);

            if (choice==1)
            {
                printf("Latest action:\n");
                Print_History(history);
            }
            else if (choice==2)
            {
                printf("type 1 to show encryptions\n"
                        "type 2 to show decryptions\n"
                        "type 3 to show compressions\n"
                        "type 4 to show decompressions\n"
                        ">");
                scanf("%d", &choice);
                printf("Latest action:\n");
                Search_Action(history, choice);
            }
            else if (choice==3)
            {
                printf("which file do you want to search for>");
                scanf("%s", inName);
                printf("Latest action:\n");
                Search_File(history, inName);
            }
            else if (choice==4)
            {
                Delete_History(&history);
            }
        }
        else if (choice == 6)
        {
			/* to allow a choice to be made by user to exit the program*/
            printf("exiting %s\n", argv[0]);
            exit = 1;
        }
        else
        {
			/* to alert the user if they enter an incorect value*/
            printf("invalid input\n");
        }
    }
    return 0;
};

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
void encrypt(char* outName, char* inName, int pass[], int length, int debug)
{
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return;
    }

    /* create and validate FILE* output */
    FILE* outFileP = fopen(outName, "w");
    if (outFileP == NULL)
    {
        fclose(inFileP);
        fprintf(stderr, "Could not create %s.\n", 
                strcat(outName, "_encrypted.bmp"));
        return;
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
    /* run through each pixel row in the image */
    for(row = 0; row < biHeight; row++)
    {
        /* run through each pixel in the row */
        for(col = 0; col < bi.biWidth; col++)
        {
            /* pass the pixel into triple struct */
            fread(&triple, sizeof(RGBTRIPLE), 1, inFileP);
            num += (col + row) % 50;
            /* change the colour of the pixel - encrypt */
            encryptColour(&triple, \
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length], \
                        debug);
            /* pass the processed ixel back into the new image */
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
        /* handle padding bytes in most BMPs */
        fseek(inFileP, nPad, SEEK_CUR);
        for(pad = 0; pad < nPad; pad++)
        {
            fputc(0x00, outFileP);
        }
    }
    fclose(inFileP);
    fclose(outFileP);
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
    /* colour value decides which colour value is kept the same */
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
void decrypt(char* outName, char* inName, int pass[], int length, int debug)
{
    FILE* inFileP = fopen(inName, "r");
    if (inFileP == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inName);
        return;
    }

    /* create and validate FILE* output */
    FILE* outFileP = fopen(outName, "w");
    if (outFileP == NULL)
    {
        fclose(inFileP);
        fprintf(stderr, "Could not create %s.\n", 
                strcat(outName, "_decrypted"));
        return;
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
    /* run through each pixel row in the image */
    for(row = 0; row < biHeight; row++)
    {
        /* run through each pixel in the row */
        for(col = 0; col < bi.biWidth; col++)
        {
            /* pass the pixel into triple struct */
            fread(&triple, sizeof(RGBTRIPLE), 1, inFileP);
            num += (col + row) % 50;
            /* change the colour of the pixel - decrypt */
            decryptColour(&triple, \
                        pass[(num + 1) % length] % 3, \
                        pass[(num + 2) % length], \
                        debug);
            /* pass the processed ixel back into the new image */
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFileP);
        }
        /* handle padding bytes in most BMPs */
        fseek(inFileP, nPad, SEEK_CUR);
        for(pad = 0; pad < nPad; pad++)
        {
            fputc(0x00, outFileP);
        }
    }
    fclose(inFileP);
    fclose(outFileP);
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
    /* colour value decides which colour value is kept the same */
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


void password(int pass[])
{
/*collect string from user make into numbers 
make 50 length through repetition of password */
    char password [10];
    
    int I, Length, error, Q;

	error =0;
	
	Length =1;
    
    while (error <(Length))
    {
		/*to recive the input of user and make numerical and 
        ensure only valid characters are used*/
		
        printf("enter pasword using alphanumeric characters\n");
        
        scanf("%s",password);
              
        Length = strlen(password) ; 	
        
        error = 0;
             
        for (I=0; I<= (Length-1); I++){
			
			/* to increment I*/
    
            if (((password[I] >=48 && password[I] <=57) || \
                (password[I] >=65 && password[I] <= 90) || \
                (password[I] >= 97 && password[I] <= 122)) \
                && (Length >3 && Length<=10))
            {
                /* to check if each character is valid 
                and that it is of desired length*/
                error++;
                
            };
                
        };
      
        if (error != (Length)){
			
			/*error mesage for pasword*/
        
            printf("invalid password (cannot contain specical characters)"
                    "and must be between 4 and 10 character long\n");
            
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

/**
 * Saves an element to the history
 * 
 * IN:
 * struct Node_t **top: pointer to the top element of history
 * int x: represents the users action
 * char inname[]: name of the infile
 * char outname[]: name of the outfile
 * 
 * OUT:
 * none
 * 
 **/
void Save_History(struct Node_t **top, int x, char inname[], char outname[])
{
    /*Initiate new node and allocate memory*/
    struct Node_t* node = NULL;
    node = (struct Node_t*) malloc(sizeof(struct Node_t));
    int length;
    length = strlen(inname);
    node->data = x;
    int i;
    /*Copy name of in- and out-file*/
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

    /*Make the new node point to the previous top-node*/
    node->next = *top;
    *top = node;
} 

/**
 * Checks if pointer points to an element
 * 
 * IN:
 * struct Node_t *top: pointer to the top element of history
 *
 * OUT:
 * int: 1 if it is empty, 0 otherwise
 * 
 **/
int Is_Empty(struct Node_t *top)
{
    if (top == NULL)
    {
        return 1;
    }
    return 0;
}

/**
 * Deletes a single node, the one that was latest inserted in the history
 * 
 * IN:
 * struct Node_t **top: pointer to the history
 * 
 * OUT:
 * none
 * 
 **/
void Pop(struct Node_t **top)
{
    /*Create a new node to temporarily hold the one to be deleted*/
    struct Node_t *node;
    node = *top;
    /*Return the second node as new top node*/
    *top = (*top)->next;
    /*Free the previous top node*/
    free(node);
}

/**
 * Recursively deletes all elements in the history
 * 
 * IN:
 * struct Node_t **top: pointer to the top element of history
 * 
 * OUT:
 * none
 * 
 **/
void Delete_History(struct Node_t **top)
{
    /*Delete until the history is empty*/
    if (Is_Empty(*top) == 0)
    {
        Pop(top);
        Delete_History(top);
    }
}

/**
 * Prints all elements in the history, starting with the latest
 * 
 * IN:
 * struct Node_t **top: pointer to the top element of history
 *
 * OUT:
 * none
 * 
 **/
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
*IN:
*   Node_t*: The node to be printed
*
*OUT;
*none
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
    }
    printf("%s into %s\n", top->in_name, top->out_name);
}

/**
 * Prints all elements matching a specific action
 * 
 * IN:
 * struct Node_t **top: pointer to the top element of history
 * int Search_Number: represents the users action
 * 
 * OUT:
 * none
 * 
 **/
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

/**
 * Prints all elements matching a specific file
 * 
 * IN:
 * struct Node_t **top: pointer to the top element of history
 * char filename[]: name of the file
 * 
 * OUT:
 * none
 * 
 **/
void Search_File(struct Node_t *top, char filename[])
{    
    if (Is_Empty(top) == 0)
    {
    	/*Check if filename matches*/
        if (strcmp(top->in_name, filename) == 0 ||\
            strcmp(top->out_name, filename) == 0)
        {
            Print_Node(top);
        }
        Search_File(top->next, filename);
    }
}
  

void writeCompressedPixel(FILE* filePointer, unsigned char r, unsigned char g,\
                        unsigned char b, unsigned char count)
{
    CompressedRGBPixel temp;
    temp.r = r;
    temp.g = g;
    temp.b = b;
    temp.runValue = count;

    fwrite(&temp,sizeof(CompressedRGBPixel),1, filePointer);
}

void printMetaData(BMPHeader header)
{
    printf("BMP Id: %c%c \n", header.id[0], header.id[1]);
    printf("BMP File Size: %u bytes\n", header.bmpSize);
    printf("Reserved: %x%x%x%x bytes\n", header.reserved[0],\
            header.reserved[1], header.reserved[2], header.reserved[3]);
    printf("BMP Data Offset: %u\n", header.bmpDataOff);
    printf("Header Size: %u\n", header.headerSize);
    printf("width: %u\n", header.width);
    printf("height: %u\n", header.height);
    printf("Color planes: %u\n", header.nColorPlanes);
    printf("Bits per pixel: %u\n", header.bpp);
    printf("Compression: %u\n", header.compression);
    printf("Image Size: %u\n", header.imageSize);
    printf("X Resolution: %u\n", header.xRes);
    printf("Y Resolution: %u\n", header.yRes);
    printf("N Colors: %u\n", header.nColors);
    printf("N Important Colors: %u\n", header.nImportantColors);
}



void runLengthEncoding(char inFileName[], char outFileName[], int debug)
{
    int i;
    int j;
    /* Header Variables */
    BMPHeader header;

    /* For debugging */
    int FailStatus = 0;

    /* File Handlers */
    ImageRGBPixel **image;

    /* Reading file and checking header details */
    FILE* inImgFile = fopen(inFileName, "rb");
    FILE* outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        /* Reading the file */
        fseek(inImgFile, 0, SEEK_SET);
        fread(&header, sizeof(BMPHeader), 1, inImgFile);

        if(debug)
        {
            for(i = 0; i < sizeof(BMPHeader); i++)
            {
                printf("%x ", *(&(header.id[0])+i) & 0xff);
            }
        }

        printf("\n");
        if(debug)
            printMetaData(header);

        /* Calculating total number of bytes in Image Pixel Array */
        if(debug)
        {
            printf("Number of Bytes Before Compression: %u\n", \
                    header.height*header.width*3);
        }
        /* Setting offset to start of pixel data */
        fseek(inImgFile, header.bmpDataOff, SEEK_SET);

       /* Allocating memory to image array as per the loaded image */
        image = (ImageRGBPixel**)malloc(header.height *\
                sizeof(ImageRGBPixel*));
        if (!image)
        {
            printf("Error while allocating memory\n");
        }
        else
        {
            for (i = 0; i < header.height; i++)
            {
                image[i] = (ImageRGBPixel*)malloc(header.width *\
                            sizeof(ImageRGBPixel));
                if (!image[i])
                {
                    printf("Error while allocating memory\n");
                    FailStatus=1;
                }
            }
        }

        /* For debugging purposes */
        if(!FailStatus)
        {
           printf("Successful memory allocation\n");
        }

        /* Read image pixels */
        unsigned char padding;
        if(((header.width * 3) % 4) == 0)
            padding = 0;
        else
            padding = 4- ((header.width * 3) % 4);

        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fread(&image[i][j], sizeof(unsigned char), \
                        sizeof(ImageRGBPixel), inImgFile);
            }
            fseek(inImgFile, padding, SEEK_CUR);
        }

        /* Run Length Encoding Algorithm */
        fseek(outImgFile, sizeof(BMPHeader) + sizeof(unsigned int), SEEK_SET);
        ImageRGBPixel current;
        current.r = 0;
        current.g = 0;
        current.b = 0;
        unsigned int pixel_count = 0;
        unsigned int colorCount = 0;

        for (i = 0; i < header.height; i++)
        {
            for (j = 0; j < header.width; j++)
            {
                if (i == 0 && j == 0)
                {
                        current = image[i][j];
                        pixel_count = 1;
                        colorCount = 1;
                }
                else if ((image[i][j].r == current.r) &&\
                         (image[i][j].g == current.g) &&\
                         (image[i][j].b == current.b))
                {
                    pixel_count++;
                }
                else
                {
                    writeCompressedPixel(outImgFile, current.r,\
                                        current.g, current.b, pixel_count);
                    pixel_count = 1;
                    colorCount++;
                    current = image[i][j];
                }

                if (i == header.height - 1 && j == header.width -1)
                {
                    writeCompressedPixel(outImgFile, current.r, current.g, \
                                        current.b, pixel_count);
                }
            }
        }

        printf("Size after compression: %u \n", colorCount*4);
        printf("Compression percentage: %f\n", \
                100.0*(colorCount*4)/(header.height*header.width*3));

        fseek(outImgFile, 0, SEEK_SET);
        fwrite(&header, sizeof(BMPHeader), 1, outImgFile);
        fwrite(&colorCount, 4, 1, outImgFile);

        /* ------- */

        /* Freeing data and closing files */
        if(image) free(image);
        fclose(inImgFile);
        fclose(outImgFile);
    }
    printf("\nCompression Complete!!!\n");
}

void runLengthDecoding(char inFileName[], char outFileName[], int debug)
{
    /* Header Variables */
    int i;
    int j;
    BMPHeader header;
    unsigned int colorCount;

    /* For debugging */
    int FailStatus = 0;

    /*File Handlers */
    ImageRGBPixel *compressedImage;

    /* Reading file and checking header details */
    FILE* inImgFile = fopen(inFileName, "rb");
    FILE* outImgFile = fopen(outFileName, "wb");
    if (inImgFile == NULL || outImgFile == NULL)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        /* Reading the file */
        fseek(inImgFile, 0, SEEK_SET);
        fread(&header, sizeof(BMPHeader), 1, inImgFile);
        if(debug)
            printMetaData(header);

        fseek(inImgFile, sizeof(BMPHeader), SEEK_SET);
        fread(&colorCount, sizeof(unsigned int), 1, inImgFile);
        if(debug)
            printf("Color Count: %u\n", colorCount);

       /* Allocating memory to image array as per the loaded image */
        compressedImage = (ImageRGBPixel*)malloc(header.width * header.height\
                            * sizeof(ImageRGBPixel));
        if (!compressedImage)
        {
            printf("Error while allocating memory\n");
            FailStatus = 1;
        }

        /* For debugging purposes */
        if(!FailStatus)
        {
           printf("Successful memory allocation\n");
        }

        /* Read image pixels */
        unsigned int pixel_index = 0;
        for (i = 0; i < colorCount; i++)
        {
            CompressedRGBPixel temp_compressed;
            fread(&temp_compressed, sizeof(unsigned char),\
                    sizeof(CompressedRGBPixel), inImgFile);

            for (j = 0; j < temp_compressed.runValue; j++)
            {
                compressedImage[pixel_index].r = temp_compressed.r;
                compressedImage[pixel_index].g = temp_compressed.g;
                compressedImage[pixel_index].b = temp_compressed.b;
                pixel_index++;
            }
        }

        fwrite(&header, sizeof(BMPHeader), 1, outImgFile);

        unsigned char pad = 0;
        unsigned char padding;
        if(((header.width * 3) % 4) == 0)
            padding = 0;
        else
            padding = 4- ((header.width * 3) % 4);
        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fwrite(&compressedImage[(i*header.width)+j], \
                    sizeof(unsigned char), sizeof(ImageRGBPixel), outImgFile);
            }
            for (j = 0; j < padding; j++) {
                fwrite(&pad, sizeof(unsigned char), 1, outImgFile);
            }
        }

        /* Freeing data and closing files */
        if(compressedImage) free(compressedImage);
        fclose(inImgFile);
        fclose(outImgFile);
    }
    printf("\nDecompression Complete!!!\n");
}

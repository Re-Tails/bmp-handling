#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

struct Node_t
{
    int data;
    char name[10];
    struct Node_t* next;
};

#define BF_TYPE 0x4d42; /* BM stored in little-endian */
#define BF_OFFBITS 54; /* 14 + 40 */
#define BI_SIZE 40; /* size of BITMAPINFOHEADER */
#define BI_BITCOUNT 24; /* 24-bit BMP */
#define BI_COMPRESSION 0; /* uncompressed */

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

void password(char* pass);
void compress(char* image);
char* decrypt(char* inName, int pass[], int length, int debug);
void decompress(char* image);
void encryptColour(RGBTRIPLE* triple, int sign, int colour, int offset, int debug);
int validate(char* inName);
char* encrypt(char* inName, int pass[], int length, int debug);
BMPIMAGE LoadBMP(char* filename);
void SaveBMP(char* filename, BMPIMAGE bitmapImage);
void FreeBMP(BMPIMAGE bitmapImage);
void Save_History(struct Node_t **top, int x, char name[]);
int Is_Empty(struct Node_t *top);
void Pop(struct Node_t **top);
void Delete_History(struct Node_t **top);
void Print_History(struct Node_t *top);
void runLengthEncoding(char inFileName[], char outFileName[]);
void runLengthDecoding(char inFileName[], char outFileName[]);
void decryptColour(RGBTRIPLE* triple, int sign, int colour, int offset, int debug);
char* fileName(char* inName, int length);

int main(void)
{
    do
    {
		/*to print the menu list and call functions and allow the program to run multiple instances without closing */
        char* filename;
        char* inName;
        char change[14]="_encrypted.bmp";
        int choice;
        BMPIMAGE bitmapImage;
        int pass[50];
        int exit = 0;
        printf("type 1 for encryption\n\n type 2 for decryption\n\n type 3 for compression\n\n type 4 for decompresion\n\n");
        printf("type 5 for compresion and encryption\n\n type 6 for decompresion and decryption\n\n type 7 to exit>");
        
        scanf("%d", &choice);
        
        if (choice == 1){
			/* to allow a choice to be made by user to encrypt an image */
            printf("option 1 encryption chosen");
            bitmapImage = LoadBMP(filename);
            inName = *filename;
            password(&pass[50]);
            validate(&inName); 
            encrypt(&inName, &pass[50], 50, 0);
            filename = strcat(&inName,change);
            SaveBMP(filename, bitmapImage);
            FreeBMP(bitmapImage);
        }
        else if (choice ==2){
			/* to allow a choice to be made by user to decrypt an image*/
            printf("option 2 decryption chosen");
            bitmapImage=LoadBMP(filename);
            password(&pass[50]);
            inName = decrypt(pass ,&bitmapImage, 50, 0);
            filename = strcat(inName,"_encrypted.bmp");
            SaveBMP(filename, bitmapImage);
            FreeBMP(bitmapImage);
        }
        else if (choice ==3){
			/* to allow a choice to be made by user to compress an image */
            printf("option 3 compression chosen");
            LoadBMP(filename);
            runLengthEncoding( inFileName[],  outFileName[]);
            FreeBMP(bitmapImage);
        }
        else if (choice ==4){
			/* to allow a choice to be made by user to decompress an image */
            printf("option 4 decompresion chosen");
            LoadBMP(filename);
            runLengthDecoding(inFileName[], outFileName[]);
            SaveBMP(filename, bitmapImage);
            FreeBMP(bitmapImage);
        }
        else if (choice ==5){
			/* to allow a choice to be made by user to encrypt and compres an image */
            printf("option 5 compresion and encryption chosen");
            inName = LoadBMP(* filename);
            password(* pass);
            validate(* inName);
            inName= encrypt(&inName, &pass[50]);
            filename= strcat(inName,"_encrypted.bmp");
            runLengthEncoding( inFileName[],  outFileName[]);;
            FreeBMP(BMPIMAGE bitmapImage);
        }
        else if (choice = 6){
			/* to allow a choice to be made by user to decrypt and decompress*/
            printf("option 6 decompression and decryption chosen");
            inName = LoadBMP(* filename);
            decompress;
            password(* pass);
            validate(* inName);
            inName = decrypt;
            filename= strcat(inName,"_encrypted.bmp");
            SaveBMP(&filename, & bitmapImage);
            FreeBMP(&bitmapImage);
        }
        else if (choice = 7){
			/* to allow a choice to be made by user to exit the program*/
            printf("exiting");
            exit =1
        }
        else{
			/* to alert the user if they enter an incorect value*/
            printf("invalid input");
        }
        
    }while(exit != 1);
    return 0;
};

void encryptColour(RGBTRIPLE* triple, int sign, int colour, int offset, \
                    int debug)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    if(debug)
        printf("0: %d %d %d  %d\n", *blue, *green, *red, offset);

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
    if(debug)
        printf("1: %d %d %d  %d\n", *blue, *green, *red, offset);
}

char* decrypt(char* inName, int pass[], int length, int debug)
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
    return decryptedName;
}

void decryptColour(RGBTRIPLE* triple, int sign, int colour, int offset, \
                    int debug)
{
    BYTE* blue = &(triple->rgbtBlue);
    BYTE* green = &(triple->rgbtGreen);
    BYTE* red = &(triple->rgbtRed);
    if(debug)
        printf("0: %d %d %d  %d\n", *blue, *green, *red, offset);
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
    if(debug)
        printf("1: %d %d %d  %d\n", *blue, *green, *red, offset);
}

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

void password(char* pass){

/*collect string from user make into numbers make 50 length through repetition
of password */
    char password [10];
    
    int NumPass[50], I, Length, error, Q;
    
    while (error <(Length)){
		
		/*to recive the input of user and make numerical and ensure only valid characters are used*/
    
        printf("enter pasword using alphanumeric characters\n");
        
        scanf("%s",password);
              
        Length = strlen(password) ; 	
        
        error = 0;
        
        for (I=0; I<= (Length-1); I++){
			
			/* to increment I*/
    
            if (((password[I] >=48 && password[I] <=57) || (password[I] >=65 && password[I] <= 90) || (password[I] >= 97 && password[I] <= 122)) && (Length >3 && Length<=10)){
                /* to check if ech character is valid and that it is of desired lenght*/
                error++;
                
            };
                
        };
        /*printf("error");*/
        if (error != (Length)){
			/*error mesage for pasword*/
        
            printf("invalid password (cannot contain specical characters) and must be between 3 and 10 character long\n");
            
            error=0;
        };
    };
        
        
        I=0;
        
        /*printf("NUM");*/
        for (Q=0; Q<= 50; Q++){
			
			/* to make pasword 50 lenght*/
            
            NumPass[Q] = password[I];
            
            I++;
            
            if (I == Length){
				
				/*to make pasword restart when it reaches the end*/
                
                I=0;
            };	
            
        };
        
    pass=NumPass;

};

BMPIMAGE LoadBMP(char* filename)
{
    scanf("%s", &filename);
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
  
char* encrypt(char* inName, int pass[], int length, int debug)
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

void writeCompressedPixel(FILE* filePointer, unsigned char r, unsigned char g, unsigned char b, unsigned char count)
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
    printf("Reserved: %x%x%x%x bytes\n", header.reserved[0], header.reserved[1], header.reserved[2], header.reserved[3]);
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

void runLengthEncoding(char inFileName[], char outFileName[])
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

        for(i = 0; i < sizeof(BMPHeader); i++)
        {
            printf("%x ", *(&(header.id[0])+i) & 0xff);
        }

        printf("\n");
        printMetaData(header);

        /* Calculating total number of bytes in Image Pixel Array */
        printf("Number of Bytes Before Compression: %u\n", header.height*header.width*3);

        /* Setting offset to start of pixel data */
        fseek(inImgFile, header.bmpDataOff, SEEK_SET);

       /* Allocating memory to image array as per the loaded image */
        image = (ImageRGBPixel**)malloc(header.height * sizeof(ImageRGBPixel*));
        if (!image)
        {
            printf("Error while allocating memory\n");
        }
        else
        {
            for (i = 0; i < header.height; i++)
            {
                image[i] = (ImageRGBPixel*)malloc(header.width * sizeof(ImageRGBPixel));
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
        unsigned char padding = (((header.width * 3) % 4) == 0) ? 0 : 4- ((header.width * 3) % 4);

        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fread(&image[i][j], sizeof(unsigned char), sizeof(ImageRGBPixel), inImgFile);
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
                else if ((image[i][j].r == current.r) && (image[i][j].g == current.g) && (image[i][j].b == current.b))
                {
                    pixel_count++;
                }
                else
                {
                    writeCompressedPixel(outImgFile, current.r, current.g, current.b, pixel_count);
                    pixel_count = 1;
                    colorCount++;
                    current = image[i][j];
                }

                if (i == header.height - 1 && j == header.width -1)
                {
                    writeCompressedPixel(outImgFile, current.r, current.g, current.b, pixel_count);
                }
            }
        }

        printf("Size after compression: %u \n", colorCount*4);
        printf("Compression percentage: %f\n", 100.0*(colorCount*4)/(header.height*header.width*3));

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

void runLengthDecoding(char inFileName[], char outFileName[])
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

        printMetaData(header);

        fseek(inImgFile, sizeof(BMPHeader), SEEK_SET);
        fread(&colorCount, sizeof(unsigned int), 1, inImgFile);
        printf("Color Count: %u\n", colorCount);

       /* Allocating memory to image array as per the loaded image */
        compressedImage = (ImageRGBPixel*)malloc(header.width*header.height * sizeof(ImageRGBPixel));
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
            fread(&temp_compressed, sizeof(unsigned char), sizeof(CompressedRGBPixel), inImgFile);

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
        unsigned char padding = (((header.width * 3) % 4) == 0) ? 0 : 4- ((header.width * 3) % 4);
        for (i = header.height-1; i >= 0; i--)
        {
            for (j = 0; j < header.width; j++)
            {
                fwrite(&compressedImage[(i*header.width)+j], sizeof(unsigned char), sizeof(ImageRGBPixel), outImgFile);
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

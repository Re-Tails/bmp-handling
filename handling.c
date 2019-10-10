#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

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

void password(int pass[]);
void compress(char* image);
void decrypt(char* outName, char* inName, int pass[], int length, int debug);
void decompress(char* image);
void encryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);
int validate(char* inName);
void encrypt(char* outName, char* inName, int pass[], int length, int debug);
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
void decryptColour(RGBTRIPLE* triple, int colour, int offset, int debug);

int main(void)
{
	int exit;
	exit = 0;
    while(exit == 0) 
    {
		/*to print the menu list and call functions and allow the program to run multiple instances without closing */
        char inName[20];
        char outName[20];
        printf("enter infile>");
        scanf("%s", inName);
        printf("enter outfile>");
        scanf("%s", outName);
        /*char change[14]="_encrypted.bmp";*/
        int choice;
        /*BMPIMAGE bitmapImage;*/
        int pass[5] = {100,100,100,100,100};
        printf("type 1 for encryption\n\n type 2 for decryption\n\n type 3 for compression\n\n type 4 for decompresion\n\n");
        printf("type 5 for compresion and encryption\n\n type 6 for decompresion and decryption\n\n type 7 to exit>");
        
        scanf("%d", &choice);
        
        if (choice == 1)
        {			
			/* to allow a choice to be made by user to encrypt an image */
            printf("option 1 encryption chosen\n");
            /* password(pass); */
            validate(inName); 
            encrypt(outName, inName, pass, 5, 0);
            printf("%s created.", outName);
        }
        else if (choice == 2)
        {
			/* to allow a choice to be made by user to decrypt an image*/
            printf("option 1 encryption chosen\n");
            /* password(pass); */
            validate(inName); 
            decrypt(outName, inName, pass, 5, 0);
            printf("%s created.", outName);
        }
        
        else if (choice ==3)
        {
			/* to allow a choice to be made by user to compress an image */
            printf("option 3 compression chosen\n");
            runLengthEncoding(inName, outName);
        }
        else if (choice ==4)
        {
			/* to allow a choice to be made by user to decompress an image */
            printf("option 4 decompresion chosen\n");
            runLengthDecoding(inName, outName);
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
        }
*/
        else if (choice == 7)
        {
			/* to allow a choice to be made by user to exit the program*/
            printf("exiting %d\n", exit);
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


void password(int pass[])
{
/*collect string from user make into numbers make 50 length through repetition
of password */
    char password [10];
    
    int NumPass[50], I, Length, error, Q;
    
    while (error <(Length))
    {
		/*to recive the input of user and make numerical and ensure only valid characters are used*/
		
        printf("enter pasword using alphanumeric characters\n");
        
        scanf("%s",password);
              
        Length = strlen(password) ; 	
        
        error = 0;
        
        /*printf("%d",Length);*/
        
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
    scanf("%s", filename);
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

//--------------------
//  main.c
//  QuadTree Project
//
//  Created by Alex Grace on 14/03/2015.
//  Copyright (c) 2015 Alex Grace. All rights reserved.
//--------------------


//--------------------
// Include
//--------------------
// Header files that include basic functions for the program to run.
//--------------------

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


//--------------------
// Define
//--------------------
// Defines key values and gives them a name for easy reference in the code. Also in the future makes it easy to change the value throughtout the code.
//--------------------

#define MinImageWidth   1
#define MaxImageWidth   64
#define Black   0
#define White   1
#define Mixed   2


//--------------------
// Structures
//--------------------
// The structure for the node on the quadtree. Contains the coordinates, width, value (if the node is black, white or mixed), pointers to the four child nodes which also have the same structure, and the pixel array contained within the node.
//--------------------

typedef struct node
{
    int PosX;
    int PosY;
    int Width;
    int Value;
    struct node* Child[4];
    int** PixelArray;
} Node;


//--------------------
//Function Declaration
//--------------------
//Delaring functions used in the code.
//--------------------

void ReadFile ( char *file_name, Node *node);
void SetNode(Node* node,int PosX, int PosY, int Width, int Value, int** PixelArray);
int PowerOfTwo (int x);
void BuildQuadTree(Node *node);
int CheckColour (Node *node, int Top, int Bottom, int Left, int Right);
int** CreateTempArray (Node *node, int Top, int Bottom, int Left, int Right, int mid);


//--------------------
// Main
//--------------------
// The main function is the designated start of the program which calls all the following functions in the program. The main is passed the file which is identified when the program is called.
//--------------------

int main ( int argc, char *argv [] )
{
    Node RootNode;
    
    if ( argc != 2)
    {
        puts ("Error with input file. Program exit");
        exit(-1);
    }
    
    ReadFile( argv[1], &RootNode);
    BuildQuadTree(&RootNode);
}


//--------------------
// Functions
//--------------------
// Contains all the functions used in the program
//--------------------

// ReadFile
// This is the first function called by main(). This function is passed the input file from main. The function starts by reading the file and dynamically allocating memory for a buffer based on the size of the file and then stores the content of the file in the buffer. Next the program reads through and stores the relevant information starting with the PixelWidth and then the NumberofBlack and then the coordinates. Memory is dynamically allocated based on the PixelWidth to store the values of the coordinates; at first all the pixels are made white but when the program reads the coordinates it changes the relevant pixels to black. The node is then set using the function SetNode. At the end of the function the array and buffer are freed and the file is closed.
// The function makes multiples checks on the information provided to make sure the information is within boundries, to save time and make sure the program can function normally. The checks are, that there is content in the file, the image width is a power of two, the image is within the max and min width which is defined above, that the image is not just white or just black and that the number of black pixels is less that the total number of pixels and more than 0 and the final check is that the number of black pixels given in the file is the number of black pixels counted by the program.

void ReadFile ( char *file_name, Node* node ){
    
    int CountedNumberofBlack=1, Counter=0, i, j, a, PixelWidth, XValue, YValue, b, val, PowerofTwo, NumberofBlack;
    long StringSize, ReadSize;
    int** OriginalArray;
    char *buffer = NULL;
    FILE *fp;
    
    fp = fopen(file_name,"r");
    
    if (fp == NULL){
        puts("Error reading file. Program exit");
        exit (-1);
    }
    
    if (fp)
    {
        fseek(fp,0,SEEK_END);
        StringSize = ftell(fp);
        rewind(fp);
        buffer = (char*) malloc (sizeof(char) * (StringSize + 1) );
        ReadSize = fread(buffer,sizeof(char),StringSize,fp);
        buffer[StringSize] = '\0';
        if(buffer[StringSize-1] == '\n')
        {
            buffer[StringSize-1] = '\0';
        }
        if (StringSize != ReadSize) {
            free(buffer);
            buffer =NULL;
            puts("Error storing file. Program exit");
            exit(-1);
        }
    }
    
    char *str=buffer, *p=str;
    
    while (*p) {
        if (*p== '\n') {
            CountedNumberofBlack++;
            p++;
        }
        else if (isdigit(*p)) {
            val = strtol(p, &p, 10);
            if (Counter==0) {
                PixelWidth = val;
                
                PowerofTwo=PowerOfTwo(PixelWidth);
                if (PowerofTwo != 1){
                    puts("The image width is not a power of 2. Program exit");
                    exit (-1);
                }
                if (PixelWidth<MinImageWidth) {
                    puts("The image is too small: it is less than the minimum image width. Program exit");
                    exit (-1);
                }
                if (PixelWidth>MaxImageWidth) {
                    puts("The image is too large: it is more than maximum image width. Program exit");
                    exit (-1);
                }
                
                Counter++;
                OriginalArray = (int**) malloc(PixelWidth*sizeof(int*));
                for(i = 0; i < PixelWidth; i++)
                {
                    OriginalArray[i] = (int*) malloc(PixelWidth*sizeof(int));
                    for ( j = 0; j < PixelWidth; j++ )
                    {
                        OriginalArray[i][j] = White;
                    }
                }
            }
            
            else if (Counter==1) {
                NumberofBlack=val;
                
                if (NumberofBlack>(PixelWidth*PixelWidth)) {
                    puts("There are more black pixels than the size of the image. Program exit");
                    exit (-1);
                }
                if (NumberofBlack==(PixelWidth*PixelWidth)) {
                    puts("The image is only black pixels");
                    exit (-1);
                }
                if (NumberofBlack==0) {
                    puts("The image is only white pixels");
                    exit (-1);
                }
                if (NumberofBlack<0) {
                    puts("There are less than 0 black pixels. Program exit");
                    exit (-1);
                }
                
                Counter++;
            }
            
            else if (Counter%2 == 0) {
                XValue=val;
                Counter++;
            }
            
            else {
                YValue=val;
                if (XValue<0 || XValue>=PixelWidth || YValue<0 || YValue>=PixelWidth) {
                    printf("Coordinate (%i,%i) is outside of the bounds of the image width. Program exit\n", XValue, YValue);
                    exit (-1);
                }
                OriginalArray[XValue][YValue]=Black;
                Counter++;
            }
        }
        else {
            p++;
        }
    }
    CountedNumberofBlack=CountedNumberofBlack-2;
    
    if (NumberofBlack!= CountedNumberofBlack) {
        puts("The given number of black pixels is not the same as the counted number of black pixels. Program exit ");
        exit (0);
    }
    
    b=PixelWidth-1;
    
    SetNode(node, 0, b, PixelWidth, 2, OriginalArray);
    free(buffer);
    for ( a=0; a < PixelWidth; a++)
    {
        free(OriginalArray[a]);
    }
    free(OriginalArray);
    fclose(fp);
    return;
}


// PowerOfTwo
// The power of two function is used to check if x which is passed to the function is a power of two. The function uses a while loop which loops checking x is still even and that x is greater than 1, whilst each loop it divides x by 2. When the loop finishes it returns the value of x. This is then used in the calling function to check x is equal to one.

int PowerOfTwo (int x)
{
    while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
        x /= 2;
    return x;
}


// SetNode
// This function is used to set information passed to the function on the node that is passed. The first parameter is the node that the information will be set on. The PosX, PosY, Width, Value is copied to the passed node. Next the four child nodes memory is allocated. Finally memory for the pixel array is dynamically allocated based on width and then the 2d Pixel array passed to the function is copied to the new PixelArray.

void SetNode(Node* newNode, int PosX, int PosY, int Width, int Value, int** Pixels)
{
    int i,j,k;
    
    newNode->PosX=PosX;
    newNode->PosY=PosY;
    newNode->Width= Width;
    newNode->Value= Value;
    
    for (k = 0; k < 4; k++)
        newNode->Child[k] = (Node*) malloc(sizeof(Node));
    
    
    newNode->PixelArray = (int**) malloc(Width*sizeof(int*));
    for(i = 0; i < Width; i++)
    {
        newNode->PixelArray[i] = (int*) malloc(Width*sizeof(int));
        for ( j = 0; j < Width; j++ )
        {
            newNode->PixelArray[i][j] = Pixels[i][j];
            
        }
    }
}


// BuildQuadTree
// Called recursively
// This functions only parameter is node being passed to it. At the start of the function there are a few calculations for variables which are used later. The main part of the function is the for loop which switches between 4 cases; each case is for a different quadrant of the pixel array. The case calls the CheckColour function to find out if that quadrant is Black, White or Mixed and copies that to Value. The temporary array for that quadrant is then created. After a few more calculations to find the coordinates for that quadrant the node is set with the relevant values and the array and then the temporary array is freed. If value is black then the coordinates and width is printed and this case breaks. If value is mixed the BuildQuadTree function is called recursively, this time with the child of the node as the root node. Finally if value is white nothing is called or printed and this case breaks.

void BuildQuadTree(Node *node)
{
    int x, Value, mid, uppermid, edge, a, PreviousPosX,PreviousPosY, PosX, PosY;
    int** TempArray;
            
    PreviousPosX=node->PosX;
    PreviousPosY=node->PosY;
    uppermid=((node->Width)/2);
    mid=uppermid-1;
    edge=node->Width-1;
    
    for (x=0; x< 4; x++){
        switch(x){
                
            case 0: //NW
                Value= CheckColour(node, 0, mid, 0, mid);
                TempArray = CreateTempArray(node, 0, mid, 0, mid, uppermid);
                PosX=PreviousPosX;
                PosY=PreviousPosY-uppermid;
                SetNode(node->Child[0], PosX, PosY, uppermid, Value, TempArray);
                for ( a=0; a < uppermid; a++)
                {
                    free(TempArray[a]);
                }
                free(TempArray);
                if (Value==Black)
                    printf("Position (%i,%i), Width %i, is all black\n", PosX,PosY, uppermid);
                if (Value==Mixed)
                    BuildQuadTree(node->Child[0]);
                break;
                
            case 1: // NE
                Value= CheckColour(node, 0, mid, uppermid, edge);
                TempArray = CreateTempArray(node, 0, mid, uppermid, edge, uppermid);
                PosX=PreviousPosX+uppermid;
                PosY=PreviousPosY-uppermid;
                SetNode(node->Child[1], PosX , PosY , uppermid, Value, TempArray);
                for ( a=0; a < uppermid; a++)
                {
                    free(TempArray[a]);
                }
                free(TempArray);
                if (Value==Black)
                    printf("Position (%i,%i), Width %i, is all black\n", PosX,PosY, uppermid);
                if (Value==Mixed)
                    BuildQuadTree(node->Child[1]);
                break;
                
            case 2: //SW
                Value= CheckColour(node, uppermid, edge, 0, mid);
                TempArray = CreateTempArray(node, uppermid, edge, 0, mid, uppermid);
                PosX=PreviousPosX;
                PosY=PreviousPosY;
                SetNode(node->Child[2], PosX, PosY, uppermid, Value, TempArray);
                for ( a=0; a < uppermid; a++)
                {
                    free(TempArray[a]);
                }
                free(TempArray);
                if (Value==Black)
                    printf("Position (%i,%i), Width %i, is all black\n", PosX,PosY, uppermid);
                if (Value==Mixed)
                    BuildQuadTree(node->Child[2]);
                break;
                
            case 3: //SE
                Value= CheckColour(node, uppermid, edge, uppermid, edge);
                TempArray = CreateTempArray(node, uppermid, edge, uppermid, edge, uppermid);
                PosX=PreviousPosX+uppermid;
                PosY=PreviousPosY;
                SetNode(node->Child[3], PosX, PosY, uppermid, Value, TempArray);
                for ( a=0; a < uppermid; a++)
                {
                    free(TempArray[a]);
                }
                free(TempArray);
                if (Value==Black)
                    printf("Position (%i,%i), Width %i, is all black\n", PosX,PosY, uppermid);
                if (Value==Mixed)
                    BuildQuadTree(node->Child[3]);
                break;
        }
    }
}


// CheckColour
// This function takes the node that is being passed to it and between top, bottom, left and right, it counts the number of black pixels and white pixels, then if there are no white pixels it returns black, if there are no black pixels it returns white and if there are both lack and white pixels it returns mixed.

int CheckColour (Node *node, int Top, int Bottom, int Left, int Right)
{
    int i,j,BlackCount=0, WhiteCount=0;
    
    for ( j = Top; j <= Bottom; j++ )
    {
        for ( i = Left; i <= Right; i++ )
        {
            if (node->PixelArray[i][j]==White){
                WhiteCount++;
            }
            else if (node->PixelArray[i][j]==Black){
                BlackCount ++;
            }
        }
    }
    
    if (WhiteCount==0){
        return Black;
    }
    if (BlackCount==0){
        return White;
    }
    return Mixed;
}


// CreateTempArray
// This function creates a temporary array for the quadrant that has called the function and copies the array values from that quadrant into the temporary array. First the function dynamically allocates a 2d array in memory for the width of the quadrant. The second half of the function copies the quadrant pixel values into the new temporary array, because they have have different coordinates this has to be done with two for loops and a counter inside the for loops. The function returns the new temporary array.

int** CreateTempArray (Node *node, int Top, int Bottom, int Left, int Right, int mid)
{
    int i=0,j=0,x,y,z;
    
    
    int** TempArray = (int**) malloc(mid*sizeof(int*));
    for(z = 0; z < mid; z++)
    {
        TempArray[z] = (int*) malloc(mid*sizeof(int));
    }
    
    for ( y=Top ; y <= Bottom; y++ )
    {
        for ( x=Left ; x <= Right; x++ )
        {
            
            TempArray[i][j] = node->PixelArray[x][y];
            
            i=i+1;
            if (i==mid)
            {
                j=j+1;
                i=0;
            }
        }
    }
    return TempArray;
}
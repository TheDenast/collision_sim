#include "VgaDrawer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

const  unsigned  int PIXEL_REGISTER_OFFSET = 0xC8000000; // Offset for pixel registers

unsigned  int  pixeloffset, value;

void VgaDrawer::PaintPixel(int posx, int posy, int color){ // Paints a pixel given coordinates and color
    pixeloffset = ((posx << 1) | (posy << 10));
    RegisterWrite(pBase, pixeloffset, color);
}

int VgaDrawer::CheckPixel(int posx, int posy){ // Checks if the pixel is already "taken"
    pixeloffset = ((posx << 1) | (posy << 10));
    int k = RegisterRead(pBase,pixeloffset);
    if (k != 0){
            return 1; // That pixel is already taken
        }
    else{
        return 0;     // That pixel is free
    }
}

/**
 * Draws a line
 * Direction: 
 *  1 - horizontal (goes right)
 *  2 - vertical   (goes down)
*/
void VgaDrawer::PaintLine(int posx, int posy, int direct, int length, int color){
    int k1;
    for (int k = 0; k<length; k++){
        if (direct == 1){
            PaintPixel((posx+k),posy,color);
        }
        else if (direct == 2){
            PaintPixel(posx,(posy+k),color);
        }
        else {
            cout<<"Wrong direction"<<endl;
            break;
        }
    }
}

/**
 * Draws a line given color and first and last point
*/
void VgaDrawer::Paint2PL(int x1, int x2, int y1, int y2, int color){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float stp, x, y;
    x=x1; y=y1;

    if (abs(dx)>abs(dy)){
        stp = abs(dx);
    }
    else {
        stp = abs(dy);
    }

    float xinc = dx/stp;
    float yinc = dy/stp;

    for (int k = 0; k<stp;k++){
        x=x+xinc;
        y=y+yinc;
        PaintPixel(int(x),int(y),color);
    }
}

/**
 * Paints a shape
 * Shapes:
 *  1 - square
 *  2 - triangle
 *  3 - old square
 *  4 - rumbus (unfinished)
*/
void VgaDrawer::PaintShape(int posx, int posy, int shape, int size, int color){
    switch (shape){
        case (1):
            Paint2PL(posx,posx,posy,(posy+size),color);
            Paint2PL(posx,(posx+size),posy,posy,color);
            Paint2PL((posx),(posx+size),(posy+size),(posy+size),color);
            Paint2PL((posx+size),(posx+size),(posy),(posy+size-1),color);
            break;
        case (2):
            Paint2PL(posx,(posx+size),(posy+size),(posy+size),color);
            Paint2PL(posx,(posx+0.5*size),(posy+size),posy,color);
            Paint2PL((posx+0.5*size),(posx+size),posy,(posy+size),color);
            break;
        case (3): // Test old square
            for (int k = 0; k<size; k++){  // Old square snippet
                PaintLine(posx,(posy+k),1,size,color);
            }
            break;
        case (4): //Rumbus (unfinished)
            if ( size % 2 != 0){
                size++;
            }
            for (int k=0; k<size;k++){
                if (k<size*0.5){

                    
                }
                if (k>size*0.5){

                }
            }
    }
}

void VgaDrawer::ClearScreen(){ // Clears the screen completely
    for (int k = 0; k<239; k++){
        PaintLine(0,k,1,319,0);
    }
}
	
VgaDrawer::VgaDrawer()
{
    ClearScreen();
}
	
VgaDrawer::~VgaDrawer()
{
	ClearScreen();
}

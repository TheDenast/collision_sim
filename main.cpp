#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "DE1SoCfpga.h"
#include "SevenSegment.h"
#include "SoundPlayer.h" 
#include "VgaDrawer.h"
#include <cmath>
#include <cstdio>
#include <ctime>

using namespace std;
const  unsigned  int MPCORE_PRIV_TIMER_LOAD_OFFSET =      0x37FEC600; // Timer register
const  unsigned  int MPCORE_PRIV_TIMER_COUNTER_OFFSET =   0x37FEC604; // Timer register
const  unsigned  int MPCORE_PRIV_TIMER_CONTROL_OFFSET =   0x37FEC608; // Timer register
const  unsigned  int MPCORE_PRIV_TIMER_INTERRUPT_OFFSET = 0x37FEC60C; // Timer register

const  unsigned  int DATA_REGISTER_OFFSET = 0x37200060;        // Data register
const  unsigned  int DIREC_REGISTER_OFFSET = 0x37200064;       // Direction register
const  unsigned  int INTRMASK_REGISTER_OFFSET = 0x37200068;    // Interruptmask register
const  unsigned  int EDCAP_REGISTER_OFFSET = 0x3720006C;       // Edgecapture register

const  unsigned  int PIXEL_REGISTER_OFFSET = 0xC8000000; // Offset for pixel registers

int KEY_CHECK;

class Object{ // A class that stores all info about each individual object
    public:
    int posx, posy, vx, vy, size, shape, countx, county, count_checkx, count_checky, color;
};

int ydiff, xdiff, vx1, vx2, vy1, vy2;

int obj_count; // Current number of objects
int obj_size;  // Current size of object array

bool collis;   // Shows that there was a collision
int  coll_obj; // Object that participated in the latest collision
int  coll_diry; // 1 - above, 2 - below
int  coll_dirx; // 1 - right, 2 - left


void Object_Grow();   // Function that grows the object array
void Object_Shrink(); // Function that shrinks the object array



void Collision_Checker(Object obj[], int c, int total); // Checks if there are collisions

int main() { 
    cout << "Program Starting...!" << endl;

    SoundPlayer *player = new SoundPlayer;   // All nessesary objects are declared here
    VgaDrawer *vga = new VgaDrawer;
    SevenSegment *display = new SevenSegment;
    Object *obj = new Object[15];

    int vx, vy, bt1, bt, ch, color, play, shape, size, key1,sound;
    
    int counter = 20000000; // Counter optimised for 10 fps rendering
    sound = 1;

    obj_count = 0; // Current number of objects on screen
    obj_size = 10; // Current size of array of objects 
    bt1=0;         // Used to avoid multiple button readings
    KEY_CHECK = 0; // Used to avoid multiple keypad readings

    display->RegisterWrite(display->pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, counter);
    display->RegisterWrite(display->pBase, MPCORE_PRIV_TIMER_CONTROL_OFFSET, 3); // Prepares the timer

    display->RegisterWrite(display->pBase, DIREC_REGISTER_OFFSET, 0b1010101000000000000001);  // Seting D0, D9, D11, D13, D14 Pins as an output (0x6A01)

    cout<<"\n---Main controls:---"
    "\nButtons: 3 2 1 0\n0. Add new object to the display\n1. Delete the last added object\n2. Turn on/off sounds\n3. Terminate program\n\n";
    

    while (1){
        
        if (display->RegisterRead(display->pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET) != 0){ // Main timer loop
            display->RegisterWrite(display->pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, 1);
            bt = display->PushButtonGet(display->pBase); // Reads current button state
            if (bt != bt1 && bt != 0){           // Checks that there are no multiple button readings
                for (int k=0;k<4;k++){
                    int c = (bt >> k) & 1;
                    if (c==1){
                        ch = k;
                        break; 
                    }
                }
                switch (ch){ // Main menu
                    case 0:
                        cout<<"\n---New object---"<<endl;
                        cout<<"Availible shapes:\n1. Square\n2. Triangle\n3. Filled square\n4. Rumbus\n\n";
                        cout<<"Enter the shape of the object";

                        key1=0;
                        while (1){
                        int key = display->Keypad_input(); // Use of the keypad
                            if (key != key1 && key !=0){
                            obj[obj_count].shape = key;
                            key1=key;
                            break;
                            }
                        }

                        cout<<"\nAvailible colors:\n1. Blue\n2. Green\n3. Red\n\n";
                        cout<<"Enter the color of the object: ";
                        cin>>color;

                        switch (color){
                            case 1:
                                obj[obj_count].color=0xf;
                                break;
                            case 2:
                                obj[obj_count].color=0x7E0;
                                break;
                            case 3:
                                obj[obj_count].color=0xF800;
                                break; 
                        }

                        cout<<"Enter the size of the object: ";
                        cin >> obj[obj_count].size;

                        cout<<"Enter the horizontal velocity of the object (in 10 pix/sec): ";
                        cin >> vx;

                        cout<<"Enter the vertical velocity of the object (in 10 pix/sec): ";
                        cin >> vy;

                        obj[obj_count].vx=vx;
                        obj[obj_count].vy=vy;
                        obj[obj_count].count_checkx=1;
                        obj[obj_count].count_checky=1;

                        #if 0
                        if (abs(vx)>=10){
                            obj[obj_count].vx = vx/10;
                            obj[obj_count].count_checkx=1;
                        }
                        else{
                            obj[obj_count].vx=int(pow(vx,0));
                            obj[obj_count].count_checkx=abs(vx);
                        }
                        
                        if (abs(vy)>=10){
                            obj[obj_count].vy = vy/10;
                            obj[obj_count].count_checky=1;
                        }
                        else{
                            obj[obj_count].vy=int(pow(vy,0));
                            obj[obj_count].count_checkx=abs(vy);
                        }
                        #endif

                        obj[obj_count].posx=160; // "Spawn" coordinates for the object
                        obj[obj_count].posy=119; // "Spawn" coordinates for the object
                        obj[obj_count].countx=0;
                        obj[obj_count].county=0;
                        obj_count++;
                        cout<<"\nObject "<<obj_count<<" successfully created"<<endl;
                        break;
                    case 1:
                        cout<<"\n---Object Deletion---"<<endl;
                        cout<<"There are "<<obj_count<<" objects currently on screen.\nObject "<<obj_count<<" successfully deleted."<<endl;
                        vga->PaintShape(obj[(obj_count-1)].posx,obj[(obj_count-1)].posy,obj[(obj_count-1)].shape,obj[(obj_count-1)].size,0);
                        obj_count=obj_count-1; // Does not actually remove the object from the array 
                        cout<<"There are now "<<obj_count<<" objects currently on screen"<<endl;
                        break;
                    case 2:
                        sound=-sound;
                        break;
                    case 3:
                        cout<<"\n---Termination---"<<endl;
                        delete display;
                        delete player;
                        delete vga;
                        return 0;
                }
            }
            bt1=bt;
            
            clock_t start = clock();
            for (int k = 0; k<obj_count; k++){ // Goes through each object 
                obj[k].countx++; // Increments counters used for movement
                obj[k].county++; // Increments counters used for movement
                vga->PaintShape(obj[k].posx,obj[k].posy,obj[k].shape,obj[k].size,0); // Removes the shape from screen before movement

                if (obj[k].countx==obj[k].count_checkx){ // Checks if the shape is ready to be shifted 
                    obj[k].posx=obj[k].posx+obj[k].vx;   // Moves coordinates of the shape
                    obj[k].countx=0;
                }
                if (obj[k].county==obj[k].count_checky){ // Checks if the shape is ready to be shifted
                    obj[k].posy=obj[k].posy+obj[k].vy;   // Moves coordinates of the shape
                    obj[k].county=0;
                }

                if (((obj[k].posx+obj[k].size)>=319) || (obj[k].posx<=0)){ // Checks screen collision for left and right
                    cout<<"---Collision! (screen)---"<<endl;
                    obj[k].posx=obj[k].posx-obj[k].vx;
                    obj[k].vx=-obj[k].vx;
                    obj[k].countx=0;
                    obj[k].county=0;
                    play = 1; // Indicates that the sound should be played
                }
                else if(((obj[k].posy+obj[k].size)>=239) || (obj[k].posy<=0)){ // Checks screen collision for top and bottom
                    cout<<"---Collision! (screen)---"<<endl;                   
                    obj[k].posy=obj[k].posy-obj[k].vy;
                    obj[k].vy=-obj[k].vy;
                    obj[k].county=0;
                    obj[k].countx=0;
                    play = 1; // Indicates that the sound should be played
                }

                Collision_Checker(obj, k, obj_count); // Checks for collisions with other shapes
                if(collis == true){ // If collision happened, moves both shapes accordingly
                    if (coll_dirx == 1){  //Right
                        obj[k].posx=obj[k].posx-obj[k].vx;
                        obj[coll_obj].posx=obj[coll_obj].posx+obj[coll_obj].vx;
                    }
                    if (coll_dirx == 2){  //Left
                        obj[k].posx=obj[k].posx+obj[k].vx;
                        obj[coll_obj].posx=obj[coll_obj].posx-obj[coll_obj].vx;
                    }
                    if (coll_diry == 1){ //Above
                        obj[k].posy=obj[k].posy+obj[k].vy;
                        obj[coll_obj].posy=obj[coll_obj].posy-obj[coll_obj].vy;
                    }
                    if (coll_diry == 2){ //Below
                        obj[k].posy=obj[k].posy-obj[k].vy;
                        obj[coll_obj].posy=obj[coll_obj].posy+obj[coll_obj].vy;
                    }
                    
                    obj[k].vx=vx1;
                    obj[k].vy=vy1;
                    obj[k].count_checkx=1;
                    obj[k].count_checky=1;

                    obj[coll_obj].vx=vx2;
                    obj[coll_obj].vy=vy2;
                    obj[coll_obj].count_checkx=1;
                    obj[coll_obj].count_checky=1;

                    obj[k].county=0;
                    obj[k].countx=0;
                    obj[coll_obj].county=0;
                    obj[coll_obj].countx=0;
                    play = 1;
                    vga->PaintShape(obj[k].posx,obj[k].posy,obj[k].shape,obj[k].size,0);
                    vga->PaintShape(obj[coll_obj].posx,obj[coll_obj].posy,obj[coll_obj].shape,obj[coll_obj].size,0);
                }

                if (collis != true){
                vga->PaintShape(obj[k].posx,obj[k].posy,obj[k].shape,obj[k].size,obj[k].color); // Paints the shape with it's final coordinates
                }

                if (play == 1 && sound>0){  // Plays a sound if there was a collision and the sound is ON
                    player->PlayNote(0,0.05);
                }
                play=0;
            }
            clock_t end = clock();
            double elapsedtime = double(end-start);
            cout << "Total Time Measured: "<< elapsedtime << " seconds" << endl << endl;
        }
    }
}

void Collision_Checker(Object obj[], int c, int total){ // Function that checks if this shape has collided with any other shape

    int x21, x22, y21, y22; 
    int x11 = obj[c].posx;
    int x12 = obj[c].posx + obj[c].size;
    int y11 = obj[c].posy;
    int y12 = obj[c].posy + obj[c].size;

    int px1,px2,py1,py2, size2;
    int size1 = obj[c].size;
    vx1 = obj[c].vx;
    vy1 = obj[c].vy;

    for (int k = 0; k<total; k++){ //Runs through each shape
        
        if (k!=c){ // Checks that it doesn't detect collision with itself, which is pointless 

            x21=obj[k].posx;
            x22=obj[k].posx + obj[k].size;
            y21=obj[k].posy;
            y22=obj[k].posy + obj[k].size;

            size2 = obj[k].size;
            vx2 = obj[k].vx;
            vy2 = obj[k].vy;

            if (!(x11>x22 || x21>x12) && !(y11>y22 || y21>y12)){ // Checks for collision. If the other shape is neither above/below or on the right/left,
                cout<<"---Collision! (object)---"<<endl;         // there had to be a collision between them
                collis = true; // Sets the collision indicator to true
                coll_obj = k;


                px1 = size1*vx1+size2*vx2;
                py1 = size1*vy1+size2*vy2;

                vx1=0.5*px1/size1;
                vx2=0.5*px1/size2;

                vy1=0.5*py1/size1;
                vy2=0.5*py1/size2;

                if(y12>y21){      // Below
                    ydiff=y12-y21;
                    coll_diry=2;
                    if(x12>x21){  // Right
                        xdiff=x12-x21;
                        coll_dirx=1;
                    }
                    if (x11<x22){ // Left
                        xdiff=x22-x11;
                        coll_dirx=2;
                    }
                    else{
                        xdiff=0;
                    }
                }
                if(y11 < y22){    // Above
                    ydiff=y22-y11;
                    coll_diry=1;
                    if(x12>x21){  // Right
                        xdiff=x12-x21;
                        coll_dirx=1;
                    }
                    if (x11<x22){ //Left
                        xdiff=x22-x11;
                        coll_dirx=2;
                    }
                    else{
                        xdiff=0;
                    }
                }
                else {
                    ydiff=0;
                }

            }
            else{ // If there was no collision
                collis = false;
            }
        }
    }
} 

#if 0 // Yet unfinished functions for array growth / shrinkage
void Object_Grow(){
    cout<<"DEBUG: Object Array growing"<<endl;
    Object *nobj[obj_size*2] = new Object;       // Initialize new array
    for (int k=0;k<obj_count;k++){               // Move elements from old array
        nobj[k]->posx=obj[k]->posx;
        nobj[k]->posy=obj[k]->posy;
        nobj[k]->vx=obj[k]->vx;
        nobj[k]->vy=obj[k]->vy;
        nobj[k]->size=obj[k]->size;
        nobj[k]->shape=obj[k]->shape;
    }
    obj_size = obj_size*2;                  // Double size variable
    delete obj;                             // Delete old array
    obj=nobj;                               // Replace old array with the new array
}

void Object_Shrink(){
    cout<<"DEBUG: Object Array shrimnking"<<endl;
}

#endif
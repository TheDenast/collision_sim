#include "SoundPlayer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

const  unsigned  int MPCORE_PRIV_TIMER_LOAD_OFFSET =      0x37FEC600; 
const  unsigned  int MPCORE_PRIV_TIMER_COUNTER_OFFSET =   0x37FEC604;
const  unsigned  int MPCORE_PRIV_TIMER_CONTROL_OFFSET =   0x37FEC608;
const  unsigned  int MPCORE_PRIV_TIMER_INTERRUPT_OFFSET = 0x37FEC60C;

const  unsigned  int DATA_REGISTER_OFFSET = 0x37200060;        // Data register
const  unsigned  int DIREC_REGISTER_OFFSET = 0x37200064;       // Direction register
const  unsigned  int INTRMASK_REGISTER_OFFSET = 0x37200068;    // Interruptmask register
const  unsigned  int EDCAP_REGISTER_OFFSET = 0x3720006C;       // Edgecapture register

int turns;      // Used to run note a certain amount of times in order to acheive required total time
int turns_check;// Usd to determine of the note has been played for enough time 
int counter;    // Self explanatory
int value1;     // Used for timer loop writing 1/0

const int notes[13] = {227272,191113,170262,151688,143172,127553,113636,101238,95557,85131,75843,71587,63776};
// Above is the array of all notes going in the following order: 
// A4  C5  D5  E5  F5  G5  A5  B5  C6  D6  E6  F6  G6
// 0   1   2   3   4   5   6   7   8   9   10  11  12   

void SoundPlayer::PlayAll(){
 
    RegisterWrite(pBase, MPCORE_PRIV_TIMER_CONTROL_OFFSET, 3); // prepare timer
    for (int k=1; k<13; k++){ // Runs 12 times for all notes
        value1 = 0;
        turns_check = 0; 
        counter = notes[k]; 
        turns = 100000000 / counter; // determines how many times this note should run the timer

        RegisterWrite(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, counter); // Sets the counter

        Display_Note(k); // Displays the current note

        while(1){ // Infinite loop for timer
            if (RegisterRead(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET) != 0){
                RegisterWrite(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, 1); // reset timer flag bit
                RegisterWrite(pBase, DATA_REGISTER_OFFSET,value1); // Sets speaker on/off
                value1 = value1 ^ 0x1; // Switches the variable
                turns_check++;         // Counts turns
            }
            if (turns_check==turns){ // When run for 0.5s, breaks loop
                break;
            }
        }
        Hex_ClearAll(); // Clears all displays
    }

    RegisterWrite(pBase, DATA_REGISTER_OFFSET,0);                  // In case the speaker is still on, turns it off
    RegisterWrite(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, 20000000); // Returns counter to initial value
}

void SoundPlayer::PlayNote(int note, double time){

    value1 = 0;
    turns_check = 0; 
    counter = notes[note];                  // Fetches an appropriate note from the list
    turns = (time * 200000000) / counter;   // Calculates the number of times we have to run the note to reach required time

    RegisterWrite(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, counter);
    RegisterWrite(pBase, MPCORE_PRIV_TIMER_CONTROL_OFFSET, 3); // Prepares the timer 

    Display_Note(note); // Displays the note

    while(1){
        if (RegisterRead(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET) != 0){
            RegisterWrite(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, 1); // reset timer flag bit
            RegisterWrite(pBase, DATA_REGISTER_OFFSET,value1);  // Sets speaker on/off
            value1 = value1 ^ 0x1;  // Switches the variable
            turns_check++;          // Counts turns
        }
        if (turns_check==turns){ // When run for required time, breaks loop
            break;
        }
    }
    Hex_ClearAll();                                                // Clears all displays
    RegisterWrite(pBase, DATA_REGISTER_OFFSET,0);                  // In case the speaker is still on, turns it off

    RegisterWrite(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, 10000000);
}


SoundPlayer::SoundPlayer()
{
	
}
	
SoundPlayer::~SoundPlayer()
{
	
}


#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H
#include "DE1SoCfpga.h"

using namespace std;
	
class SevenSegment : public DE1SoCfpga{
	
    private:
	unsigned int reg0_OFFSET;
	unsigned int reg1_OFFSET;
    unsigned  int  reg0_hexValue;
    unsigned  int  reg1_hexValue;

    public:
    void Hex_ClearAll();
    void Hex_ClearSpecific(int  index);
    void Hex_WriteSpecific(int index, int value);
    void Hex_WriteNumber(int number);
    void Hex_WriteDecNumber(int number);
    int  Display_translator(int value);
    void Display_Note(int note);
	int  ReadAllSwitches(char *pBase);
	int  Read1Switch(char *pBase,int switchNum);
    int  Keypad();
    int  Keypad_translate(int key);
    int  Keypad_input();
    int  PushButtonGet(char *pBase);
    SevenSegment();
    ~SevenSegment();
};
#endif
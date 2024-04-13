#include "SevenSegment.h"  
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <bitset>
#include <cmath>

using namespace std;

    const  unsigned  int MPCORE_PRIV_TIMER_LOAD_OFFSET  =  0x37FEC600; 
    const  unsigned  int MPCORE_PRIV_TIMER_COUNTER_OFFSET  =  0x37FEC604;
    const  unsigned  int MPCORE_PRIV_TIMER_CONTROL_OFFSET = 0x37FEC608;
    const  unsigned  int MPCORE_PRIV_TIMER_INTERRUPT_OFFSET = 0x37FEC60C; 

    unsigned  int  reg0_hexValue;
    unsigned  int  reg1_hexValue;

    unsigned  int  initialvalueLoadMPCore;
    unsigned  int  initialvalueControlMPCore;
    unsigned  int  initialvalueInterruptMPCore;

    const  unsigned  int DATA_REGISTER_OFFSET = 0x37200060;
    const  unsigned  int EXTERNAL_PORT_OFFSET = 0x37200060;

    const  unsigned  int KEY_OFFSET = 0x37200050;

    const int KEYPAD[4] = {0b1010100,0b1010001,0b1000101,0b10101};
     
    /**
     * Clears all hex displays
    */
    void SevenSegment::Hex_ClearAll(){ 
        reg0_hexValue=0;
        reg1_hexValue=0;
        RegisterWrite(pBase, reg0_OFFSET, reg0_hexValue);
        RegisterWrite(pBase, reg1_OFFSET, reg1_hexValue);
    }

    /**
     * Clears a specific display
    */
    void SevenSegment::Hex_ClearSpecific(int  index){
        if (index>=0 && index<=3){
            for (int k = 0; k<8; k++){
                reg0_hexValue &= ~(1 << (8*index + k));
            }
            RegisterWrite(pBase, reg0_OFFSET, reg0_hexValue);
        }
        else {
            for (int k = 0; k<8; k++){
                reg1_hexValue &= ~(1 << (8*(index-4) + k));
            }
            RegisterWrite(pBase, reg1_OFFSET, reg1_hexValue);    
        }
    }

    /**
     * Writes specific number/letter to a specific display
    */
    void SevenSegment::Hex_WriteSpecific(int index, int value){
        Hex_ClearSpecific(index);
        int num1 = Display_translator(value);
        if (index>=0 && index<=3){
            reg0_hexValue = reg0_hexValue | (num1 << (index*8));
            RegisterWrite(pBase, reg0_OFFSET, reg0_hexValue);
        }
        else {
            reg1_hexValue = reg1_hexValue | (num1 << ((index-4)*8));
            RegisterWrite(pBase, reg1_OFFSET, reg1_hexValue);
        }
    }

    /**
     * Writes a hex number to all displays
    */
    void SevenSegment::Hex_WriteNumber(int number){
        Hex_ClearAll();
        int num2;
        for (int k=0;k<6;k++){
            num2 = (number >> (4 * k)) & 0x000F;
            if (num2!=0){
                num2 = Display_translator(num2);
                if (k>=0 && k<=3){
                    reg0_hexValue = reg0_hexValue | (num2 << (k*8));
                    RegisterWrite(pBase, reg0_OFFSET, reg0_hexValue);
                }
                else {
                    reg1_hexValue = reg1_hexValue | (num2 << ((k-4)*8));
                    RegisterWrite(pBase, reg1_OFFSET, reg1_hexValue);
                }
            }            
        }
    }

    /**
     * Writes a decimal number to all displays
    */
    void SevenSegment::Hex_WriteDecNumber(int number){
        Hex_ClearAll();
        int num2;
        for (int k=0;k<6;k++){
            num2 = (number / int(pow(10,k)) % 10);
                num2 = Display_translator(num2);
                if (k>=0 && k<=3){
                    reg0_hexValue = reg0_hexValue | (num2 << (k*8));
                    RegisterWrite(pBase, reg0_OFFSET, reg0_hexValue);
                }
                else {
                    reg1_hexValue = reg1_hexValue | (num2 << ((k-4)*8));
                    RegisterWrite(pBase, reg1_OFFSET, reg1_hexValue);
                }         
        }
    }

    /**
     * "Translates" real numbers to the once comprehended by the display
    */
    int SevenSegment::Display_translator(int value){
        int num1;
        switch (value){
            case (0):          // also letter O
                num1 = 0x3F;
                break;
            case (1):
                num1 = 0x6;
                break;
            case (2):
                num1 = 0x5B;
                break;
            case (3):
                num1 = 0x4F;
                break;                            
            case (4):
                num1 = 0x66;
                break;
            case (5):          // letter S
                num1 = 0x6D;
                break;
            case (6):
                num1 = 0x7D;
                break;
            case (7):
                num1 = 0x7;
                break;
            case (8):
                num1 = 0x7F;
                break;
            case (9):
                num1 = 0x6F;
                break;
            case (10):          //also letter A
                num1 = 0x77;
                break;
            case (11):
                num1 = 0x7C;
                break;
            case (12):
                num1 = 0x39;
                break;
            case (13):
                num1 = 0x5E;
                break;
            case (14):
                num1 = 0x79;
                break;
            case (15):
                num1 = 0x71;
                break;
            case (16):          // letter K
                num1 = 0x75;
                break;
            case (17):          // letter T
                num1 = 0x37;
                break;
            case (18):          // letter N
                num1 = 0x78;
                break;
                                                                                                                                                                                          
        }
        return num1;
    }

    /**
     * Displays a note (used by SoundPlayer.cpp)
    */
    void SevenSegment::Display_Note(int note){
        switch(note){
        case (0):   // A4
            Hex_WriteSpecific(1, 10);
            Hex_WriteSpecific(0, 4);
            break;
        case (1):   // C5
            Hex_WriteSpecific( 1,  12);
            Hex_WriteSpecific( 0,  5);
            break;
        case (2):   // D5
            Hex_WriteSpecific( 1,  13);
            Hex_WriteSpecific( 0,  5);
            break;
        case (3):   // E5
            Hex_WriteSpecific( 1,  14);
            Hex_WriteSpecific( 0,  5);
            break;
        case (4):   // F5
            Hex_WriteSpecific( 1,  15);
            Hex_WriteSpecific( 0,  5);
            break;
        case (5):   // G5
            Hex_WriteSpecific( 1,  9);
            Hex_WriteSpecific( 0,  5);
            break;
        case (6):   // A5
            Hex_WriteSpecific( 1,  10);
            Hex_WriteSpecific( 0,  5);
            break;
        case (7):   // B5
            Hex_WriteSpecific( 1,  11);
            Hex_WriteSpecific( 0,  5);
            break;
        case (8):   // C6
            Hex_WriteSpecific( 1,  12);
            Hex_WriteSpecific( 0,  6);
            break;
        case (9):   // D6
            Hex_WriteSpecific( 1,  13);
            Hex_WriteSpecific( 0,  6);
            break;
        case (10):  // E6
            Hex_WriteSpecific( 1,  14);
            Hex_WriteSpecific( 0,  6);
            break;
        case (11):  // F6
            Hex_WriteSpecific( 1,  15);
            Hex_WriteSpecific( 0,  6);
            break;
        case (12):  // G6
            Hex_WriteSpecific( 1,  9);
            Hex_WriteSpecific( 0,  6);
            break;
        }
    }

    /**
     * Reads all switches
    */
    int SevenSegment::ReadAllSwitches(char *pBase){
        int num1 = RegisterRead(pBase, 0x37200040);
        return num1;
    }

    /**
     * Reads one specific switch
    */
    int SevenSegment::Read1Switch(char *pBase,int switchNum){
        int a = ReadAllSwitches(pBase);
        int b = (a >> switchNum) & 1;
        return b;
    }

    /**
     * This function reads the keypad and gives back
     * the integer indicating the pressed key using 
     * the following logic:
     *  
     *  1   5   9   13
     *  2   6   10  14
     *  3   7   11  15
     *  4   8   12  16
     * 
    */
    int SevenSegment::Keypad(){
        int v1, key, md;
        for (int k = 0; k<4; k++){
            RegisterWrite(pBase,DATA_REGISTER_OFFSET,(KEYPAD[k]<<15));
            for (int i = 0; i<4; i++){
                switch (i){
                    case (0):
                        md = 9;
                        break;
                    case (1):
                        md = 11;
                        break;
                    case (2):
                        md = 13;
                        break;
                    case (3):
                        md = 14;
                        break;
                }
                v1 = RegisterRead(pBase,DATA_REGISTER_OFFSET);
                v1 = (v1 >> md) & 1;

                if (v1 == 0){
                    key = i + 1 + 4*k;
                    return key;
                }
            }
        }
        return 0;
    }

    /**
     * "Translates" keypad input as numbers, giving symbols following numbers:
     *  "A" - 10
     *  "B" - 11
     *  "C" - 12
     *  "D" - 13
     *  "*" - 14
     *  "#" - 15
     * 
     *  No keys pressed (0) is output as 999
    */
    int SevenSegment::Keypad_translate(int key){
        switch (key){
            case (0):
                return 999;
                break;
            case (1):
                return 1;
                break;
            case (2):
                return 4;
                break;
            case (3):
                return 7;
                break;
            case (4):
                return 14;
                break;
            case (5):
                return 2;
                break;
            case (6):
                return 5;
                break;
            case (7):
                return 8;
                break;
            case (8):
                return 0;
                break;
            case (9):
                return 3;
                break;
            case (10):
                return 6;
                break;
            case (11):
                return 9;
                break;
            case (12):
                return 15;
                break;
            case (13):
                return 10;
                break;
            case (14):
                return 11;
                break;
            case (15):
                return 12;
                break;
            case (16):
                return 13;
                break;
        }
    }

    /**
     * Allows user type decimal numbers
     * Number buttons work as usual
     *  "A" is used to submit number
     *  "B" is used to delete last digit
    */
    int SevenSegment::Keypad_input(){
        int key, n1;
        int number=0;
        int check = 0;
        Hex_ClearAll();

        while (1){
            key = Keypad();
            if (key != check){
                n1 = Keypad_translate(key);
                if (n1<=9 && n1>=0){
                    number=number*10;
                    number=number+n1;
                    Hex_WriteDecNumber(number);
                }
                if (n1==10){
                    Hex_ClearAll();
                    return number;
                }
                if (n1==11){
                    number=number/10;
                    Hex_WriteDecNumber(number);
                }
                check=key;
            }
        }
    }

    /**
     * Reads all all buttons
    */
    int SevenSegment::PushButtonGet(char *pBase){
        int num1 = RegisterRead(pBase, KEY_OFFSET);
        return num1;
    } 

    SevenSegment::SevenSegment(){
        initialvalueLoadMPCore = RegisterRead(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET);
        initialvalueControlMPCore = RegisterRead(pBase, MPCORE_PRIV_TIMER_CONTROL_OFFSET);
        initialvalueInterruptMPCore = RegisterRead(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET);
        reg0_OFFSET      = 0x37200020;
        reg1_OFFSET      = 0x37200030;
        reg0_hexValue = RegisterRead(pBase,reg0_OFFSET); //First register initialisation
        reg1_hexValue = RegisterRead(pBase,reg1_OFFSET); //Second register initialisation
    }
    SevenSegment::~SevenSegment(){
        RegisterWrite(pBase, MPCORE_PRIV_TIMER_LOAD_OFFSET, initialvalueLoadMPCore);
        RegisterWrite(pBase, MPCORE_PRIV_TIMER_CONTROL_OFFSET, initialvalueControlMPCore);
        RegisterWrite(pBase, MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, initialvalueInterruptMPCore);
        Hex_ClearAll();
    }

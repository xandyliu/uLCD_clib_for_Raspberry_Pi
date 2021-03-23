//
// uLCD_4DGL is a class to drive 4D Systems uLCD 144 G2
//
// Copyright (C) <2010> Stephane ROCHON <stephane.rochon at free.fr>
// Modifed for Goldelox processor <2013> Jim Hamblen
//
// uLCD_4DGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// uLCD_4DGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with uLCD_4DGL.  If not, see <http://www.gnu.org/licenses/>.

//#include "mbed.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>

#include "uLCD_4DGL.h"

#define ARRAY_SIZE(X) sizeof(X)/sizeof(X[0])

//Serial pc(USBTX,USBRX);


//******************************************************************************************************
uLCD_4DGL :: uLCD_4DGL(unsigned rst) :
    _rst(rst)
{
    // Constructor
    _devtty = (char *)"/dev/serial0"; // Pi3: ttyS0; older Pis: ttyAMA0
    
    _cmd = serOpen(_devtty, 9600, 0); // open a serial device at a specified baud rate
    
    if((_cmd == PI_NO_HANDLE)||(_cmd == PI_SER_OPEN_FAILED)){
        printf("ERROR: PI_NO_HANDLE or PI_SER_OPEN_FAILED.\n");
        printf("Press Ctrl+C to exit.\n");
        while(1){ time_sleep(5); }
    }
#if DEBUGMODE
    printf("\n\n\n");
    printf("*********************\n");
    printf("uLCD_4DGL CONSTRUCTOR\n");
    printf("*********************\n");
#endif

    gpioWrite(_rst, 1);                 // put RESET pin to high to start TFT screen
    reset();
    cls();       // clear screen
    current_col         = 0;            // initial cursor col
    current_row         = 0;            // initial cursor row
    current_color       = WHITE;        // initial text color
    current_orientation = IS_PORTRAIT;  // initial screen orientation
    current_hf = 1;
    current_wf = 1;
    set_font(FONT_7X8);                 // initial font
//   text_mode(OPAQUE);                  // initial texr mode
}

//******************************************************************************************************
uLCD_4DGL :: ~uLCD_4DGL(){
    //printf("~uLCD_4DGL()\n");
    serClose(_cmd);
}

//******************************************************************************************************
void uLCD_4DGL :: writeBYTE(char c)   // send a BYTE command to screen
{

    serWriteByte(_cmd, (unsigned) c);
    time_sleep(0.0005);  //mbed is too fast for LCD at high baud rates in some long commands

#if DEBUGMODE
    printf("   Char sent : 0x%02X\n",c);
#endif

}

//******************************************************************************************************
void uLCD_4DGL :: writeBYTEfast(char c)   // send a BYTE command to screen
{

    //_cmd.putc(c);
    serWriteByte(_cmd, (unsigned) c);
    //wait_ms(0.0);  //mbed is too fast for LCD at high baud rates - but not in short commands

#if DEBUGMODE
    printf("   Char sent : 0x%02X\n",c);
#endif

}
//******************************************************************************************************
void uLCD_4DGL :: freeBUFFER(void)         // Clear serial buffer before writing command
{
    while (serDataAvailable(_cmd) != 0) serReadByte(_cmd);  // clear buffer garbage
}

//******************************************************************************************************
int uLCD_4DGL :: writeCOMMAND(char *command, int number)   // send several BYTES making a command and return an answer
{

#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif
    int i, resp = 0;
    freeBUFFER();
    writeBYTE(0xFF);
    for (i = 0; i < number; i++) {
        if (i<16)
            writeBYTEfast(command[i]); // send command to serial port
        else
            writeBYTE(command[i]); // send command to serial port but slower
    }
    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO/1000);              // wait for screen answer
    if (serDataAvailable(_cmd) != 0) resp = serReadByte(_cmd);           // read response if any
    switch (resp) {
        case ACK :                                     // if OK return   1
            resp =  1;
            break;
        case NAK :                                     // if NOK return -1
            resp = -1;
            break;
        default :
            resp =  0;                                 // else return   0
            break;
    }
#if DEBUGMODE
    printf("   Answer received : %d\n",resp);
#endif

    return resp;
}

//**************************************************************************
void uLCD_4DGL :: reset()    // Reset Screen
{
    time_sleep(0.005);
    gpioWrite(_rst, 0);               // put RESET pin to low
    time_sleep(0.005);                // wait a few milliseconds for command reception
    gpioWrite(_rst, 1);               // put RESET back to high
    time_sleep(3);                 // wait 3s for screen to restart

    freeBUFFER();           // clean buffer from possible garbage
}
//******************************************************************************************************
int uLCD_4DGL :: writeCOMMANDnull(char *command, int number)   // send several BYTES making a command and return an answer
{

#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif
    int i, resp = 0;
    freeBUFFER();
    writeBYTE(0x00); //command has a null prefix byte
    for (i = 0; i < number; i++) {
        if (i<16) //don't overflow LCD UART buffer
            writeBYTEfast(command[i]); // send command to serial port
        else
            writeBYTE(command[i]); // send command to serial port with delay
    }
    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO/1000);              // wait for screen answer
    if (serDataAvailable(_cmd) != 0) resp = serReadByte(_cmd);           // read response if any
    switch (resp) {
        case ACK :                                     // if OK return   1
            resp =  1;
            break;
        case NAK :                                     // if NOK return -1
            resp = -1;
            break;
        default :
            resp =  0;                                 // else return   0
            break;
    }
#if DEBUGMODE
    printf("   Answer received : %d\n",resp);
#endif

    return resp;
}

//**************************************************************************
void uLCD_4DGL :: cls()    // clear screen
{
    char command[1] = "";

    command[0] = CLS;
    writeCOMMAND(command, 1);
    current_row=0;
    current_col=0;
    current_hf = 1;
    current_wf = 1;
    set_font(FONT_7X8);                 // initial font
}

//**************************************************************************
int uLCD_4DGL :: version()    // Populate revision with SPE version. return 0 for correct return.
{

    char command[2] = "";
    command[0] = '\x00';
    command[1] = VERSION;
    return readVERSION(command, 2);
}

//**************************************************************************
void uLCD_4DGL :: baudrate(int speed)    // set screen baud rate
{   //The baud rate for pi's serial must be one of 50, 75, 110, 134, 150, 200, 300, 600, 
    //1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, or 230400
    char command[3]= "";
    writeBYTE(0x00);
    command[0] = BAUDRATE;
    command[1] = 0;
    int newbaud = BAUD_9600;
    switch (speed) {
        case  110 :
            newbaud = BAUD_110;
            break;
        case  300 :
            newbaud = BAUD_300;
            break;
        case  600 :
            newbaud = BAUD_600;
            break;
        case 1200 :
            newbaud = BAUD_1200;
            break;
        case 2400 :
            newbaud = BAUD_2400;
            break;
        case 4800 :
            newbaud = BAUD_4800;
            break;
        case 9600 :
            newbaud = BAUD_9600;
            break;
        //case 14400 : // not supported by Pi serial
        //    newbaud = BAUD_14400;
        //    break;
        case 19200 :
            newbaud = BAUD_19200;
            break;
        //case 31250 : // not supported by Pi serial
        //    newbaud = BAUD_31250;
        //    break;
        case 38400 :
            newbaud = BAUD_38400;
            break;
        // case 56000 : // not supported by Pi serial
        //     newbaud = BAUD_56000;
        //     break;
        case 57600 :
            newbaud = BAUD_57600;
            break;
        case 115200 :
            newbaud = BAUD_115200;
            break;
        /*
        case 128000 :
            newbaud = BAUD_128000;
            break;
        case 256000 :
            newbaud = BAUD_256000;
            break;
        case 300000 :
            newbaud = BAUD_300000;
            speed = 272727;
            break;
        case 375000 :
            newbaud = BAUD_375000;
            speed = 333333;
            break;
        case 500000 :
            newbaud = BAUD_500000;
            speed = 428571;
            break;
        case 600000 :
            newbaud = BAUD_600000;
            break;
        case 750000 : //rates over 600000 are not documented, but seem to work
            newbaud = BAUD_750000;
            break;
        case 1000000 :  
            newbaud = BAUD_1000000;
            break;
        case 1500000 :
            newbaud = BAUD_1500000;
            break;
        case 3000000 :
            newbaud = BAUD_3000000;
            break;
        */
        default   :
            newbaud = BAUD_9600;
            speed = 9600;
            break;
    }

    int i, resp = 0;

    freeBUFFER();
    command[1] = char(newbaud >>8);
    command[2] = char(newbaud % 256);
    time_sleep(0.001); //wait_ms(1);
    for (i = 0; i <3; i++) writeBYTEfast(command[i]);      // send command to serial port
    for (i = 0; i<10; i++) time_sleep(0.001); //wait_ms(1); 
    //dont change baud until all characters get sent out
    //_cmd.baud(speed);                                  // set mbed to same speed
    serClose(_cmd);
    _cmd = serOpen(_devtty, speed, 0);
    i=0;
    while ((serDataAvailable(_cmd) == 0) && (i<25000)) {
        time_sleep(TEMPO/1000); //wait_ms(TEMPO);           // wait for screen answer - comes 100ms after change
        i++; //timeout if ack character missed by baud change
    }
    if (serDataAvailable(_cmd) != 0) resp = serReadByte(_cmd);           // read response if any
    switch (resp) {
        case ACK :                                     // if OK return   1
            resp =  1;
            break;
        case NAK :                                     // if NOK return -1
            resp = -1;
            break;
        default :
            resp =  0;                                 // else return   0
            break;
    }
}

//******************************************************************************************************
int uLCD_4DGL :: readVERSION(char *command, int number)   // read screen info and populate data
{

    int i, temp = 0, resp = 0;
    char response[5] = "";

    freeBUFFER();

    for (i = 0; i < number; i++) writeBYTE(command[i]);    // send all chars to serial port

    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO/10); //wait_ms(TEMPO);               // wait for screen answer

    while ((serDataAvailable(_cmd) != 0) && resp < (int) (ARRAY_SIZE(response))) {
        temp = serReadByte(_cmd);
        response[resp++] = (char)temp;
    }
    switch (response[0]) {
        case ACK:                                           // if OK populate data and return 0
            revision  = (response[1]<<8) + response[2];
            resp      = 0;
            break;
        default :
            resp =  -1;                                     // else return -1
            break;
    }
    return resp;
}

//****************************************************************************************************
void uLCD_4DGL :: background_color(int color)              // set screen background color
{
    char command[3]= "";                                  // input color is in 24bits like 0xRRGGBB

    command[0] = BCKGDCOLOR;

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color

    writeCOMMAND(command, 3);
}

//****************************************************************************************************
void uLCD_4DGL :: textbackground_color(int color)              // set screen background color
{
    char command[3]= "";                                  // input color is in 24bits like 0xRRGGBB

    command[0] = TXTBCKGDCOLOR;

    int red5   = (color >> (16 + 3)) & 0x1F;              // get red on 5 bits
    int green6 = (color >> (8 + 2))  & 0x3F;              // get green on 6 bits
    int blue5  = (color >> (0 + 3))  & 0x1F;              // get blue on 5 bits

    command[1] = ((red5 << 3)   + (green6 >> 3)) & 0xFF;  // first part of 16 bits color
    command[2] = ((green6 << 5) + (blue5 >>  0)) & 0xFF;  // second part of 16 bits color

    writeCOMMAND(command, 3);
}

//****************************************************************************************************
void uLCD_4DGL :: display_control(char mode)     // set screen mode to value
{
    char command[3]= "";

    command[0] = DISPCONTROL;
    command[1] = 0;
    command[2] = mode;

    //if (mode ==  ORIENTATION) {
        switch (mode) {
            case LANDSCAPE :
                current_orientation = IS_LANDSCAPE;
                break;
            case LANDSCAPE_R :
                current_orientation = IS_LANDSCAPE;
                break;
            case PORTRAIT :
                current_orientation = IS_PORTRAIT;
                break;
            case PORTRAIT_R :
                current_orientation = IS_PORTRAIT;
                break;
        }
    //}
    writeCOMMAND(command, 3);
    set_font(current_font);
}
//****************************************************************************************************
void uLCD_4DGL :: display_power(char mode)     // set screen mode to value
{
    char command[3]= "";

    command[0] = DISPPOWER;
    command[1] = 0;
    command[2] = mode;
    writeCOMMAND(command, 3);
}
//****************************************************************************************************
void uLCD_4DGL :: set_volume(char value)     // set sound volume to value
{
    char command[2]= "";

    command[0] = SETVOLUME;
    command[1] = value;

    writeCOMMAND(command, 2);
}


//******************************************************************************************************
// can be used to get LCD model. return number of bytes received. -1 if error return.
/**
Example:
* @code
* char command[2] = {0x00, 0x07};
* char response[20] = "";
* getSTATUS(command, 2, response, 20);
* printf("%s\n", &(response[3]));
* @endcode
*/

int uLCD_4DGL :: getSTATUS(char *command, int cmd_number, char *response, int max_resp_number)   // read screen info and populate data
{

#if DEBUGMODE
    printf("\n");
    printf("New COMMAND : 0x%02X\n", command[0]);
#endif

    int i, temp = 0, resp = 0;

    freeBUFFER();

    for (i = 0; i < cmd_number; i++) writeBYTE(command[i]);    // send all chars to serial port

    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO/10);    // wait for screen answer

    while ((serDataAvailable(_cmd) != 0) && resp < max_resp_number) {
        temp = serReadByte(_cmd);
        response[resp++] = (char)temp;
    }
    switch (response[0]) {
        case ACK :
            break;
        default :
            resp =  -1;                      // else return   -1
            break;
    }

#if DEBUGMODE
    printf("   Answer received : %d\n", resp);
#endif

    return resp;
}


//
// uLCD_4DGL is a class to drive 4D Systems LCD screens
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

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>
#include "uLCD_4DGL.h"


//Media Commands

//******************************************************************************************************
// UPDATED
int uLCD_4DGL :: media_init()
{
    int resp = 0;
    char command[1] = "";
    command[0] = MINIT;
    writeCOMMAND(command, 1);
    // wait for screen answer
    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO*0.001); //while (!_cmd.readable()) wait_ms(TEMPO);
    if (serDataAvailable(_cmd) != 0) { //if (_cmd.readable()) {
        // read response
        resp = serReadByte(_cmd); //resp = _cmd.getc();           
        resp = resp << 8 + serReadByte(_cmd); //resp = resp << 8 + _cmd.getc();
    }
    return resp;
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: set_byte_address(int hi, int lo)
{
    char command[5]= "";
    command[0] = SBADDRESS;

    command[1] = (hi >> 8) & 0xFF;
    command[2] = hi & 0xFF;

    command[3] = (lo >> 8) & 0xFF;
    command[4] = lo & 0xFF;
    writeCOMMAND(command, 5);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: set_sector_address(int hi, int lo)
{

    char command[5]= "";
    command[0] = SSADDRESS;

    command[1] = (hi >> 8) & 0xFF;
    command[2] = hi & 0xFF;

    command[3] = (lo >> 8) & 0xFF;
    command[4] = lo & 0xFF;
    writeCOMMAND(command, 5);
}

//******************************************************************************************************
// UPDATED
char uLCD_4DGL :: read_byte()
{
    char resp = 0;
    char command[1] = "";
    command[0] = READBYTE;
    writeCOMMAND(command, 1);
    // wait for screen answer
    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO*0.001); //while (!_cmd.readable()) wait_ms(TEMPO);              
    if (serDataAvailable(_cmd) > 0) { //if (_cmd.readable()) {
        // read response
        resp = serReadByte(_cmd); //resp = _cmd.getc();           
        resp = serReadByte(_cmd); //resp = _cmd.getc();
    }
    return resp;
}

//******************************************************************************************************
// UPDATED
int  uLCD_4DGL :: read_word()
{
    int resp=0;
    char command[1] = "";
    command[0] = READWORD;
    writeCOMMAND(command, 1);
    // wait for screen answer
    while (serDataAvailable(_cmd) == 0) time_sleep(TEMPO*0.001); //while (!_cmd.readable()) wait_ms(TEMPO);              
    if (serDataAvailable(_cmd) > 0) { //if (_cmd.readable()) {
        // read response
        resp = serReadByte(_cmd); //resp = _cmd.getc();           
        resp = resp << 8 + serReadByte(_cmd); //resp = resp << 8 + _cmd.getc();
    }
    return resp;
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: write_byte(int value)
{
    char command[3]= "";

    command[0] = WRITEBYTE;

    command[1] = (value >> 8) & 0xFF;
    command[2] = value & 0xFF;
    writeCOMMAND(command,3);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: write_word(int value)
{
    char command[3]= "";

    command[0] = WRITEWORD;

    command[1] = (value >> 8) & 0xFF;
    command[2] = value & 0xFF;
    writeCOMMAND(command,3);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: flush_media()
{
    char command[1] = "";
    command[0] = FLUSHMEDIA;
    writeCOMMAND(command, 1);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: display_image(int x, int y)
{
    char command[6]= "";
    command[0] = DISPLAYIMAGE;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;
    writeCOMMAND(command, 5);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: display_video(int x, int y)
{
    char command[5]= "";
    command[0] = DISPLAYVIDEO;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;
    writeCOMMAND(command, 5);
}

//******************************************************************************************************
// No update needed
void uLCD_4DGL :: display_frame(int x, int y, int w)
{
    char command[7]= "";

    command[0] = DISPLAYFRAME;

    command[1] = (x >> 8) & 0xFF;
    command[2] = x & 0xFF;

    command[3] = (y >> 8) & 0xFF;
    command[4] = y & 0xFF;

    command[5] = (w >> 8) & 0xFF;
    command[6] = w & 0xFF;
    writeCOMMAND(command,7);
}


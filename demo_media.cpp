#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "uLCD_4DGL.h"



// #define TXD 14 // TXD pin is GPIO_14
// #define RXD 15 // RXD pin is GPIO_15
#define RST 18 // RST pin is GPIO_18

// Called when CTL C or STOP button hit
static void err_handler (int sig){
    //printf("err_handler\n");
	gpioTerminate(); //release GPIO locks & resources
	signal(SIGINT, SIG_DFL); //exit program
	kill(getppid(), SIGINT); //kill it off
	kill(getpid(), SIGINT);
	exit(0);
}
static void exit_handler(void) {
    //printf("exit_handler\n");
	gpioTerminate(); //release GPIO locks & resources on exit
}
int main(int argc, char *argv[])
{
    if (gpioInitialise()<0) return 1; // init I/O library
    signal (SIGQUIT, err_handler);// CTL C and STOP button
    signal (SIGINT, err_handler); // GPIO exit & cleanup
    signal (SIGTERM, err_handler);
    signal (SIGABRT, err_handler);
    atexit(exit_handler);  // exit handler cleanup 
    
    uLCD_4DGL uLCD(RST); // serial tx, serial rx, reset pin;



    /********************************************************
     *              MEDIA DEMO                               
     * *****************************************************/
    // Read documentation on GITHUB wiki to understand how to 
    // store raw images and videos on uSD for displaying on
    // LCD.

    uLCD.puts("Running media demo");
    time_sleep(2.0);


    // Each test will be followed by a 1 sec. green screen to
    // break up the individual parts.
    uLCD.background_color(GREEN);

    //
    // Display Image test
    //
    uLCD.media_init();
    uLCD.set_sector_address(0x0020, 0x4100); // You will have to change these values based where RAW data is stored (check .GC file)
    uLCD.display_image(0,0);
    time_sleep(2.0);
    uLCD.cls();
    time_sleep(1.0);


    //
    // Displays Video (GIF) 3x
    //
    uLCD.set_sector_address(0x0020, 0x4500); // Again, will have to set to your own address values
    uLCD.display_video(0,0);
    uLCD.set_sector_address(0x0020, 0x4500);
    uLCD.display_video(0,0);
    uLCD.set_sector_address(0x0020, 0x4500); 
    uLCD.display_video(0,0);
    uLCD.cls();
    time_sleep(1.0);


    //
    // Display Single Video Frame
    // 
    uLCD.set_sector_address(0x0020, 0x4500); // Again, will have to set to your own address values
    uLCD.display_frame(0,0,13); // Displays a frame of the previous video for 2 seconds
    time_sleep(2.0);
    uLCD.cls();
    time_sleep(1.0);


    //
    // Write data to uSD card, read data, and display text on LCD
    //
    char datastring[80]=""; // Empty string to store text in
    uLCD.set_sector_address(0,4096); //go to an unused area on SD card
    //Write Hello SD World to SD card
    sprintf(datastring,"%s","Hello from sector 4096!");
    uLCD.puts("Writing to SD.");
    for (int i=0; i<strlen(datastring); i++) {
        uLCD.write_byte(datastring[i]); //write a byte to SD card
    }
    uLCD.flush_media(); //flush out (write) entire remaining 512 byte sector with 0xFF fills
    //Now Read back bytes from SD card

    // Brief pause
    time_sleep(2.0);
    uLCD.cls();

    uLCD.set_sector_address(0,4096); //reset to start of sector
    uLCD.puts("Reading SD...");
    time_sleep(1.0);
    uLCD.cls();
    char readchar=' ';
    while (readchar != 0xFF) { //0xFF is padding character at end of bytes
        readchar = uLCD.read_byte();  //read a byte from SD card
        uLCD.putc(readchar); //also send byte to display
    }
    time_sleep(3.0);
    uLCD.cls();
    time_sleep(1.0);


    //
    // Demo is finished
    //
    uLCD.background_color(RED);
    uLCD.cls();
    uLCD.puts("Finished demo.");
    time_sleep(2.0);
    uLCD.reset();
    return 0;
}
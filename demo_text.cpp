#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>

#include "uLCD_4DGL.h"

#define RST 18 // RST pin is GPIO_18

// Called when CTL C or STOP button hit
static void err_handler (int sig){
	gpioTerminate(); //release GPIO locks & resources
	signal(SIGINT, SIG_DFL); //exit program
	kill(getppid(), SIGINT); //kill it off
	kill(getpid(), SIGINT);
	exit(0);
}
static void exit_handler(void) {
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

    // code starts here
    uLCD_4DGL uLCD(RST); // reset pin
    
    uLCD.background_color(BLUE);
    uLCD.cls();

    char s[10] = "Hello!";

    uLCD.text_height(2);
    uLCD.text_width(2);
    
    uLCD.color(RED);
    uLCD.text_bold(ON);
    uLCD.puts(s);
    uLCD.putc('\n');
    uLCD.text_bold(OFF);

    uLCD.color(GREEN);
    uLCD.text_italic(ON);
    uLCD.puts(s);
    uLCD.putc('\n');
    uLCD.text_italic(OFF);

    
    uLCD.color(WHITE);
    uLCD.text_inverse(ON);
    uLCD.puts(s);
    uLCD.putc('\n');
    uLCD.text_inverse(OFF);

    uLCD.color(LGREY);
    uLCD.text_underline(ON);
    uLCD.puts(s);
    uLCD.putc('\n');
    uLCD.text_underline(OFF);

    uLCD.color(0xFF00FF);
    uLCD.text_mode(TRANSPARENT);
    uLCD.puts(s);
    uLCD.putc('\n');

    uLCD.color(0x00FFFF);
    
    uLCD.puts(s);
    uLCD.putc('\n');
    

    time_sleep(5.0);

    return 0;
}

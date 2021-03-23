#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>

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

    //IO code starts here
    char *devtty = (char *)"/dev/serial0"; // on Pi3, ttyS0 also works; if not, try serial1 or ttyAMA0
   
    int uLCDhandle = serOpen(devtty, 9600, 0); // open a serial device at a specified baud rate
    
    if((uLCDhandle == PI_NO_HANDLE)||(uLCDhandle == PI_SER_OPEN_FAILED)){
        printf("ERROR: PI_NO_HANDLE or PI_SER_OPEN_FAILED.\n");
        return -1;
    }

    // clearing out initial unwanted data
    while(serDataAvailable(uLCDhandle) != 0){
        serReadByte(uLCDhandle);
    }
    //print A to Z
    for (int i = 0; i < 26; i++) {   
        serWriteByte(uLCDhandle, (int)('A'+i));
        while(serDataAvailable(uLCDhandle) == 0);
        printf("%c", serReadByte(uLCDhandle));
    }
    printf("\n");
    serClose(uLCDhandle);

    return 0;
}

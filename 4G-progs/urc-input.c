

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B38400            

#define _POSIX_SOURCE 1 /* POSIX compliant source */

main(int argc, char* argv[])
{
  char applicationPort[32];

  // run 'source ./get-app-port.sh' before running the binary.

  // Get the environment varaible for the Applicaiton Port
  char * appPort = "ttyACM1"; // comnected out just for now. getenv("APPTTY");

  if((appPort == NULL) || (strlen(appPort) == 0))
  {
    fprintf(stderr, "Environment variable $APPTTY is not present\n");
    exit(1);
/*
    // This in fact does not work in practice. system() uses '/bin/sh -c to run the command
    // but it fails to run successfully and the environment varaible is not exported.

    // First try and initialise this env variable.
    int rtn = system("source /home/get-app-port.sh");

    if(rtn == -1)
    {
      fprintf(stderr, "Fork failed and get-app-port failed to run\n");
      exit(1);
    }else{
      // Try and get the application port.
      appPort = getenv("APPTTY");

      if((appPort == NULL) || (strlen(appPort) == 0))
      {
        printf("Failed to get Application Port\n");
        exit(1);
      }else{
        applicationPort[0] = '\0';
        strcat(applicationPort, "/dev/");
        strcat(applicationPort, appPort);
      }
    }
*/

  }else{
    applicationPort[0] = '\0';
    strcat(applicationPort, "/dev/");
    strcat(applicationPort, appPort);
  }

  if(argc == 2)
  {
    // We can expect a single parameter '-D' which will run this as a daemon
    if(strcmp(argv[1], "-D")==0)
    {
      if(daemon(1,1) < 0)
      {
        perror("daemon");
        exit(-1);
      }
    }else{
      fprintf(stdout, "Unrecognised option %s\n", argv[1]);
      fprintf(stdout, "Help: -D to run as daemon\n");
      exit(-1);
    }
  }

  int fd, c, res;
  struct termios oldtio,newtio;
  char buf[128];
  /* 
    Open modem device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  fd = open(applicationPort, O_RDWR | O_NOCTTY); 
  if(fd <0) 
  {
	  perror(applicationPort); 
	  exit(-1); 
  }
        
  tcgetattr(fd, &oldtio); /* save current serial port settings */
  bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
        
  /* 
    BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
    CRTSCTS : output hardware flow control (only used if the cable has
              all necessary lines. See sect. 7 of Serial-HOWTO)
    CS8     : 8n1 (8bit,no parity,1 stopbit)
    CLOCAL  : local connection, no modem contol
    CREAD   : enable receiving characters
  */
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
         
  /*
    IGNPAR  : ignore bytes with parity errors
    ICRNL   : map CR to NL (otherwise a CR input on the other computer
              will not terminate input)
    otherwise make device raw (no other input processing)
  */
  newtio.c_iflag = IGNPAR | ICRNL;
         
  /*
    Raw output.
  */
  newtio.c_oflag = 0;
         
  /*
    ICANON  : enable non-canonical input
    disable all echo functionality, and don't send signals to calling program
  */
  newtio.c_lflag = ICANON;
         
  /* 
    initialize all control characters 
    default values can be found in /usr/include/termios.h, and are given
    in the comments, but we don't need them here
  */
  newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  newtio.c_cc[VERASE]   = 0;     /* del */
  newtio.c_cc[VKILL]    = 0;     /* @ */
  newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
  newtio.c_cc[VTIME]    = 10;     /* inter-character timer  1 second*/
  newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
  newtio.c_cc[VSWTC]    = 0;     /* '\0' */
  newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
  newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
  newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
  newtio.c_cc[VEOL]     = 0;     /* '\0' */
  newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
  newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
  newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
  newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
  newtio.c_cc[VEOL2]    = 0;     /* '\0' */
        
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);
        
  while(1)
  {            
    res = read(fd, buf, sizeof(buf));

    if((res == 1) && (buf[0] == '\n'))
    {
      continue;
    }

    if(res > 1)
    {
      // This should be the complete URC.
      // Knock off the \n at the end and terminate as for a sting.
      buf[res-1] = '\0';

      printf("URC: %s\n", buf);
      
      // Call handler script.
      char* heapStr;

      size_t sl = strlen("/usr/local/bin/urc-dispatch.sh") + strlen(buf) + 4; // Allow for null termination and space char plus two single quote chars
      heapStr = malloc(sl);
      snprintf(heapStr, sl, "%s '%s'", "/usr/local/bin/urc-dispatch.sh", buf);
      int rc = system(heapStr);
      free(heapStr);
    }
  }

  /* restore the old port settings */
  tcsetattr(fd, TCSANOW, &oldtio);
}



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B38400            
/* change this definition for the correct port */
// #define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

struct metaData
{
  size_t allocatedSize;
}MetaData;

void* MyMalloc(size_t size, struct metaData *MetaData);
void* MyRealloc(void *ptr, size_t size, struct metaData *MetaData);
void MyFree(void* ptr, struct metaData *MetaData);


// Search string for 'from' char and replace with 'to' char
void swapchars(char* ptr, size_t len, char from, char to)
{
  char* pCopy = ptr;
  size_t i = 0;
  for(; i<len; i++, pCopy++)
  {
    if(*pCopy == from)
    {
      *pCopy = to;
    }
  }
}
  
int WaitForInputFromModem(int fd, int totalWaitTime, int incrementWaitTime) // Times are in ms
{
  int ntr=0;

  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = incrementWaitTime * 1000000; // Convert to nano seconds

  int i = totalWaitTime / incrementWaitTime;
  while(i>0)
  {
    nanosleep(&req, NULL);
    int rc = ioctl(fd, TIOCINQ, &ntr);
    if(rc == 0 && ntr > 0)
    {
      // fprintf(stdout, "%d chars read after %d ms\n", ntr, totalWaitTime - (i*incrementWaitTime));
      break;
    }else if(rc < 0){
      return ntr;
    }
    --i;
  }

  return ntr;
}



  // We use the PLS8-E's default mode of handling URC. 
  // In particular the allocation of the Modem and Application instances.
  // See section 1.9 of the users' pdf manual

int main(int argc, char * argv[])
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];
  char *resultStr;
  struct metaData MetaData;

  MetaData.allocatedSize = 0;

  // If there are less than three command line args then 
  if(argc != 3)
  {
	  fprintf(stderr, "Number of command line arguments is incorrect. i.e. ttyport at-command\n");
	  exit(-1);
  }

  /* 
	  Open modem device for reading and writing and not as controlling tty
	  because we don't want to get killed if linenoise sends CTRL-C.
  */

  char ttyDevice[16];
  ttyDevice[0] = '\0';
  strcat(ttyDevice, "/dev/");
  strcat(ttyDevice, argv[1]);

  fd = open(ttyDevice, O_RDWR | O_NOCTTY); 
  if(fd < 0) 
  {
	  perror(ttyDevice); 
	  return 1; 
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
  ICANON  : enable canonical input
  disable all echo functionality, and don't send signals to calling program
  */
  newtio.c_lflag = ICANON;
         
  /* 
  initialize all control characters 
  default values can be found in /usr/include/termios.h, and are given
  in the comments
  */
  newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  newtio.c_cc[VERASE]   = 0;     /* del */
  newtio.c_cc[VKILL]    = 0;     /* @ */
  newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
  newtio.c_cc[VTIME]    = 10;    /* inter-character timer  1 second */
  newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
  // newtio.c_cc[VMIN]     = 0;     /* Read with time out. This way we never block indefinitely */
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
        
  /* 
  now clean the modem line and activate the settings for the port
  */
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);
  
  char atCmd[64];

  atCmd[0] = '\0';
  strcpy(atCmd, argv[2]);
  strcat(atCmd, "\r"); // Input AT commands for the modem have to be termiated with a \r

  // fprintf(stdout, " AT command <%s>\n", atCmd);


  ssize_t wo = write(fd, atCmd, strlen(atCmd));

  if(wo != strlen(atCmd))
  {
    fprintf(stderr, "Error: Bytes written %d of %d\n", wo, strlen(atCmd));
    exit(-1);
  }
  
  /*
    terminal settings done, now handle input
    In this example, inputting a 'z' at the beginning of a line will 
    exit the program.
  */

  // If the tty port provided as input is either not working or is incorrectly
  // specified then trying to read characters from this port will block indefinitely
  // as it is set for canonical input. To enable a timeout if no character
  // is received then we would have to be in non-canonical mode. Rather than switching
  // input mode we use a tty_ioctl to monitor for any characters ready for reading.

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);
  int ntr=0, okReceived = FALSE, errorReceived = FALSE;

  ntr = WaitForInputFromModem(fd, 2000, 10); // Times are in ms

  if(ntr==0)
  {
    // No characters availalbe to be read.
    return 1;
  }

  resultStr = (char*)MyMalloc(256, &MetaData);
  resultStr[0] = '\0';

  while((ntr>0)) // && !errorReceived)
  {
    // fprintf(stdout, "before read\n", res);
    res = read(fd, buf, sizeof(buf));
    buf[res] = '\0';

    swapchars(buf, strlen(buf), '\n', ' ');

    // Print input as it arrives.
    fprintf(stdout, "%s", (char*)buf);

    // fprintf(stdout, "%d (1) characters read <<<%s>>>\n", res, buf);

    // URCs can follow the expected termination of the input AT command
    if(strstr(buf, "OK ")!= NULL)
    {
      okReceived = TRUE;

    } else if(strstr(buf, "ERROR") != NULL){
      errorReceived = TRUE;
    } else if(strstr(buf, "NO CARRIER") != NULL){
      // I've seen this message appearing before 'OK' hence its inclusion here.
      // I can generate this message by toggling between CFUN=1 and CFUN=4 (^SYSSTART and ^SYSTART AIRPLANCE MODE)
      // but afterwards the ppp0 interface is non-responsive and requires a cold-start for the PLS8.
      // If the ppp0 is not started then toggling between CFUN=1 and CFUN=4 does no harm and I can 
      // then generate URC for testing. 
      // In normal operation the modem would not be operated in AIRPLANE MODE.
      SendToURCDispatch(buf);
    } else if(okReceived && strlen(buf)){
      
      if(strlen(buf) > 1)
      {
        // fprintf(stdout, "Reading URC %s\n", buf);
        // These can be fired off to the urc handler.
        // fprintf(stdout, "Sending <%s> to urc handler\n", buf);
        SendToURCDispatch(buf);

      }
    }

    if(res >= 1)
    {
      // fprintf(stdout, "Read <<%s>> \n", buf);
      if((strlen(resultStr) + strlen(buf) +1) > MetaData.allocatedSize)
      {
        // How much more space is required? Allocate a reasoable chunk and not just the minimum needed
        size_t increase = 1024;
        while(10*strlen(buf) > increase)
        {
          increase += increase;
        }

        resultStr = (char*)MyRealloc((void*)resultStr, MetaData.allocatedSize + increase, &MetaData);
      }

      // strcat(resultStr, buf);
      // fprintf(stdout, "Input string so far <<<%s>>>\n", resultStr);
    }

    // Up to the point that the OK or ERROR is received the read(...) should not block indefinitely
    // After the 'OK' is received there may not be any URCs sent so we have to monitor for 
    // additional characters before calling read(...) again as read(..) would block indefinitely 
    // waiting for charaters that may never be sent.
    if(okReceived || errorReceived)
    {
      ntr = WaitForInputFromModem(fd, 1000, 10);
      // fprintf(stdout, "Chars to read %d\n", ntr);
      if(ntr == 0)
      {
        // End of input from modem.
		    // fprintf(stdout, "%s\n", resultStr);
      }
    }
  } // end while

  // system("kill -s CONT `pidof urc-input`");

  fprintf(stdout, "\n");

  /* restore the old port settings */
  tcsetattr(fd, TCSANOW, &oldtio);

  MyFree(resultStr, &MetaData);

  return errorReceived?1:0;
}


//**************************************************************************
// Wrapper functions for malloc
//**************************************************************************
void* MyMalloc(size_t size, struct metaData *MetaData)
{
  MetaData->allocatedSize = size;
  void * mem = malloc(size);
  return mem;
}

void* MyRealloc(void *ptr, size_t size, struct metaData *MetaData)
{
  void * mem = realloc(ptr, size);

  if(size > MetaData->allocatedSize)
  {
    memset(mem+MetaData->allocatedSize+1, 0, size-MetaData->allocatedSize);
  }

  MetaData->allocatedSize = size;
  
  return ptr;
}  

void MyFree(void* ptr, struct metaData *MetaData)
{
  free(ptr);
  MetaData->allocatedSize = 0;
}

int SendToURCDispatch(char * const urc)
{
  char* heapStr;

  size_t sl = strlen("/usr/local/bin/urc-dispatch.sh") + strlen(urc) + 4; // Allow for null termination and space char plus two single quote chars
  heapStr = malloc(sl);
  snprintf(heapStr, sl, "%s '%s'", "/usr/local/bin/urc-dispatch.sh", urc);
  int rc = system(heapStr);
  free(heapStr);
  return rc;
}

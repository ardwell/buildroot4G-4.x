#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* According to POSIX.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>


// Pin 17 use with the following values
// Connect 3.3V to pin 17
// echo 17 > /sys/class/gpio/export
// echo in  > /sys/class/gpio/gpio17/direction
// echo both  > /sys/class/gpio/gpio17/edge
// echo 0  > /sys/class/gpio/gpio17/active_low
// echo 17 > /sys/class/gpio/unexport





char * const gpioPath="/sys/class/gpio/gpio17/value";

int main(void)
{
  int res, i;
  char buff[16];


// Try poll()
// #define USE_POLL

#ifdef USE_POLL
 struct pollfd xfds[1];

 int fdGpio = open(gpioPath, O_RDWR);  
 if(fdGpio < 0)  
  {
    fprintf(stdout, "Gpio file falied to open\n");
    exit(1);
  }

  xfds[0].fd = fdGpio;
  xfds[0].events = POLLPRI | POLLERR;
  xfds[0].revents = 0;
  while(1) // for (i=0; i<3; i++)
  {
    printf("Waiting for interrupt..\n");
    res = poll(xfds, 1, -2000); // Use a negative timeout value and poll waits forever
    if(res == -1) 
    {
      fprintf(stdout, "poll failed");
      exit(1);
    }
    fprintf(stdout, "poll rc=%d, revents=0x%x\n", res, xfds[0].revents);

    // Read value
    res = lseek(fdGpio, 0, SEEK_SET); 
    if(res < 0)
    {
      fprintf(stdout, "lseek failed\n");
    }

    res = read(fdGpio, buff, 2);       
    if(res != 2)
    {
      fprintf(stdout, ("Read incorrect number of characters\n"));
      exit(1);
    }
  
    buff[1] = '\0'; /* Overwrite the newline character with terminator */
    printf("read res=%d, val=%s\n", res, buff);
  }

  close(fdGpio);
#endif 

#define USE_SELECT 1
#ifdef USE_SELECT

  // Use select
  fd_set exceptfds;

  int fdGpio = open(gpioPath, O_RDONLY);

  if(fdGpio == -1)
  {
    fprintf(stdout, "Gpio file falied to open\n");
    exit(1);
  }

  FD_ZERO(&exceptfds);
  FD_SET(fdGpio, &exceptfds);

  while(1)
  {
    res = select(fdGpio+1, 
                 NULL,               // readfds - not needed
                 NULL,               // writefds - not needed
                 &exceptfds,
                 NULL);              // timeout (never)

    if (res > 0 && FD_ISSET(fdGpio, &exceptfds))
    {
      res = lseek(fdGpio, 0, SEEK_SET);
      if(res < 0)
      {
        fprintf(stdout, "lseek failed\n");
      }

      res = read(fdGpio, buff, 2); // Must read after select (or poll) return to clear up and prepare for the next select (or poll)
      if(res != 2)
      {
        fprintf(stdout, ("Read incorrect number of characters\n"));
        exit(1);
      }
  
      buff[1] = '\0'; /* Overwrite the newline character with terminator */
      fprintf(stdout, "read res=%d, val=%s\n", res, buff);
      // GPIO line changed
      fprintf(stdout, "Gpio 17 value changed\n");
    }
  }

  close(fdGpio);
  
#endif


  return 0;

}



#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>	/* for uint64 definition */

/* According to POSIX.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <errno.h>



// Pin 17 use with the following values
// Connect 3.3V to pin 17
// echo 17 > /sys/class/gpio/export
// echo in  > /sys/class/gpio/gpio17/direction
// echo both  > /sys/class/gpio/gpio17/edge
// echo 1  > /sys/class/gpio/gpio17/active_low  # Means that first transition is from high to low i.e. 1 ---> 0
// echo 17 > /sys/class/gpio/unexport

char buffer[64];

int initGpioPin(int pin)
{	
	char buffer[64];
		
	// Configure gpio pin
	buffer[0] = '0';
	snprintf(buffer, sizeof(buffer), "echo %d > /sys/class/gpio/export\n", pin);
	system(buffer);
	
	buffer[0] = '0';
	snprintf(buffer, sizeof(buffer), "echo in > /sys/class/gpio/gpio%d/direction\n", pin);
	system(buffer);
	
	buffer[0] = '0';
	snprintf(buffer, sizeof(buffer), "echo both > /sys/class/gpio/gpio%d/edge\n", pin);
	system(buffer);	
	
	buffer[0] = '0';
	snprintf(buffer, sizeof(buffer), "echo 1 > /sys/class/gpio/gpio%d/active_low\n", pin);
	system(buffer);	

	return 1;
}


// #define debug 

#ifdef debug
	#define DEBUG_PRINT(a, b, c) fprintf(a, b, c);
	#define DEBUG_PRINT2(a, b) fprintf(a, b);
#else
	#define DEBUG_PRINT(a, b, c)
	#define DEBUG_PRINT2(a, b) 
#endif


	uint64_t firstRange_lower=1001000000LL;
	uint64_t firstRange_upper=2999999999LL;
	uint64_t secondRange_lower=3000000000LL;
	uint64_t secondRange_upper=5999999999LL;
	uint64_t thirdRange_lower=6000000000LL;
	uint64_t thirdRange_upper=10000000000LL;


int GPIO_PIN = 30;

char * const gpioPath="/sys/class/gpio/gpio30/value";

int main(int argc, char **argv)
{
  int res, i;
  char buff[16];

// Try poll()
// #define USE_POLL

#ifdef USE_POL
 struct pollfd xfds[1];

 int fdGpio = open(gpioPath, O_RDWR);  
 if(fdGpio < 0)  
  {
    DEBUG_PRINT2(stdout, "Gpio file falied to open\n");
    exit(1);
  }

  xfds[0].fd = fdGpio;
  xfds[0].events = POLLPRI | POLLERR;
  xfds[0].revents = 0;
  while(1)
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

	if(argc==2 && 0==strcmp("-D", argv[1]))
	{
		daemon(1, 1);
	}

	// Prepare the gpio pin
	initGpioPin(GPIO_PIN);

// Test time resolution

	int rv;
	struct timespec t_res, start_time, stop_time;
	
	// rv = clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t_res);
	rv = clock_getres(CLOCK_MONOTONIC_RAW, &t_res);
	
	if(rv == 0)
	{
		DEBUG_PRINT(stdout, "Clock resolution %ld nanosecond \n", t_res.tv_nsec);
	}else{
		DEBUG_PRINT(stdout, "clock_getres failed with %d\n", errno);
	}

  // Use select
  fd_set exceptfds;

  int fdGpio = open(gpioPath, O_RDONLY);
	uint64_t t_diff;
	int last_value, new_value;
	last_value = 0;

  if(fdGpio == -1)
  {
    fprintf(stdout, "Gpio file failed to open\n");
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
        // fprintf(stdout, "lseek failed\n");
      }

      res = read(fdGpio, buff, 2); // Must read after select (or poll) return to clear up and prepare for the next select (or poll)
      if(res != 2)
      {
        fprintf(stdout, ("Read incorrect number of characters\n"));
        exit(1);
      }
  
      buff[1] = '\0'; /* Overwrite the newline character with terminator */
      // fprintf(stdout, "read res=%d, val=%s\n", res, buff);
      
      new_value = atoi(buff);
      
      if(last_value == 0 && new_value == 1)
      {
				// Get out starting time
				rv = clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
				if(rv == -1)
				{
					DEBUG_PRINT2(stdout, "Getting start time ****FAILED***\n");
				}
				last_value = new_value;
			}else if(last_value == 1 && new_value == 0)
			{
				last_value = new_value;
				// Get stop time
				rv = clock_gettime(CLOCK_MONOTONIC_RAW, &stop_time);
			  // fprintf(stdout, "Getting stop time ... \n");
				if(rv == -1)
				{
					DEBUG_PRINT2(stdout, "Getting stop time ****FAILED***\n");
				}

				// Keep time diff in nano seconds
				t_diff = 1E9 * (stop_time.tv_sec - start_time.tv_sec) + (stop_time.tv_nsec - start_time.tv_nsec);

				if(t_diff < firstRange_lower)
				{
					DEBUG_PRINT(stdout, "NOISE (< 1s) - ignore (%llu)\n", t_diff);
				}else if((t_diff > firstRange_lower) && (t_diff <firstRange_upper))
				{
					DEBUG_PRINT(stdout, "SHORT (>1s <3s) (%llu)\n", t_diff);
					int rc = system("/usr/local/bin/restartSS.sh");
				}else if((t_diff > secondRange_lower) && (t_diff < secondRange_upper))
				{
					int rc = system("/usr/local/bin/rebootFreedom.sh");
					DEBUG_PRINT(stdout, "MEDIUM (>3s <6s) (%llu)\n)", t_diff);
				}else if((t_diff > thirdRange_lower))
				{
					int rc = system("/usr/local/bin/reinstallSS.sh");
					DEBUG_PRINT(stdout, "LONG (>6s) (%llu)\n", t_diff)
				}
				
				DEBUG_PRINT(stdout, "Time interval nano seconds %llu \n", t_diff);
			}
    }
  }

  close(fdGpio);
  
#endif // USE_SELECT
  return 0;

}



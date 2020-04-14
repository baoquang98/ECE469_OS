#include "usertraps.h"
#include "misc.h"


void main (int argc, char *argv[])
{
  sem_t test_complete; // Semaphore to signal the original process that we're done
  int i;
  int counter=0;
  
  if (argc != 2) { 
    Printf("Usage: %s <test_complete_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  test_complete = dstrtol(argv[1], NULL, 10);

  // print message
  Printf("TEST6 (%d): Looping to a large number\n", getpid());
  Printf("TEST6 (%d): START!\n", getpid());

  // starts to loop
  for(i =0; i<100000; i++) {
    counter ++;
  }

  Printf("part6 (%d): Looping complete\n", getpid());
  Printf("TEST6 (%d): Done!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(test_complete) != SYNC_SUCCESS) {
    Printf("TEST6 (%d): Bad semaphore test_complete (%d)!\n", getpid(), test_complete);
    Exit();
  }

}

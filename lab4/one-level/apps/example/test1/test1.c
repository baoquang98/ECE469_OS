#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t test_complete; // Semaphore to signal the original process that we're done
  Printf("TEST1 (%d): Start hello world testing (PID: %d): Print 'Hello World' and exit.\n", getpid());

  if (argc != 2) { 
    Printf("Usage: %s <test_complete_semaphore>\n"); 
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  test_complete = dstrtol(argv[1], NULL, 10);

  // print hello world
  Printf("Hello world\n");

  // Now print a message to show that everything worked
  Printf("TEST1 (%d): Done with TEST1 in Q2\n", getpid());

  if(sem_signal(test_complete) != SYNC_SUCCESS) {
    Printf("TEST1 (%d): Bad semaphore test_complete (%d)!\n", getpid(), test_complete);
    Exit();
  }
}

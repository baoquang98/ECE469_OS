#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t test_complete; // Semaphore to signal the original process that we're done
  Printf("Hello World (%d): Start hello world testing (PID: %d): Print 'Hello World' and exit.\n", getpid());

  if (argc != 2) { 
    Printf("Usage: %s <test_complete_semaphore>\n"); 
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  test_complete = dstrtol(argv[1], NULL, 10);

  // print hello world
  Printf("Hello world\n");

  // Now print a message to show that everything worked
  Printf("Hello World (%d): Done\n", getpid());

  if(sem_signal(test_complete) != SYNC_SUCCESS) {
    Printf("Hello World (%d): Bad semaphore test_complete (%d)!\n", getpid(), test_complete);
    Exit();
  }
}

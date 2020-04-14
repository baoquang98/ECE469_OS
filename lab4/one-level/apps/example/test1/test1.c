#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  // sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  Printf("Start hello world testing (PID: %d): Print 'Hello World' and exit.\n", getpid());

  // Convert the command-line strings into integers for use as handles
  // s_procs_completed = dstrtol(argv[1], NULL, 10);

  // print hello world
  Printf("Hello world\n");

  // Now print a message to show that everything worked
  Printf("Done with TEST1 in Q2 (PID: %d)\n", getpid());
}

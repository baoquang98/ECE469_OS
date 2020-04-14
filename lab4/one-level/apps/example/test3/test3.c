#include "usertraps.h"
#include "misc.h"
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  // sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int * ptr;
  Printf("Start accessing out of range testing (PID: %d): Access memory beyond the maximum virtual address.\n", getpid());

  // Convert the command-line strings into integers for use as handles
  // s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Access memory beyond the maximum virtual address.
  ptr = (MEM_MAX_VIRTUAL_ADDRESS + 1);
  Printf("Accessing Memory Location: %d (decimal)\n", ptr);
  // attempt to access that location
  Printf("Accessing Memory Value: %d (decimal)\n", *ptr);


  // Now print a message to show that everything worked
  Printf("Done with TEST2 in Q2 (PID: %d)\n", getpid());
}

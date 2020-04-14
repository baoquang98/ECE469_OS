#include "usertraps.h"
#include "misc.h"
#include "memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int * ptr;
  Printf("Start hello world testing (PID: %d): Print 'Hello World' and exit.\n", getpid());

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Access memory beyond the maximum virtual address.
  ptr = (MEM_MAX_VIRTUAL_ADDRESS + 1);
  Printf("Accessing Memory Location: %d (decimal)\n", ptr);
  // attempt to access that location
  Printf("Accessing Memory Value: %d (decimal)\n", *ptr);

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("hello_world (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  // Now print a message to show that everything worked
  Printf("Done with TEST1 in Q2 (PID: %d)\n", getpid());
}

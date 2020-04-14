#include "usertraps.h"
#include "misc.h"
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t test_complete; // Semaphore to signal the original process that we're done
  int * ptr;
  Printf("TEST2 (%d): Start accessing out of range testing: Access memory beyond the maximum virtual address.\n", getpid());

  if (argc != 2) { 
    Printf("Usage: %s <test_complete_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  test_complete = dstrtol(argv[1], NULL, 10);

  // Access memory beyond the maximum virtual address.
  ptr = (MEM_MAX_VIRTUAL_ADDRESS + 1);
  Printf("TEST2 (%d): Accessing Memory Location: %d (decimal)\n", getpid(), ptr);

  // Now print a message to show that everything worked
  Printf("TEST2 (%d): Will be accessing the memory s.t. the process will be killed\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(test_complete) != SYNC_SUCCESS) {
    Printf("TEST2 (%d): Bad semaphore test_complete!\n", getpid(), test_complete);
    Exit();
  }
  // attempt to access that location
  Printf("TEST2 (%d): Accessing Memory Value: %d (decimal)\n", getpid(), *ptr);

}

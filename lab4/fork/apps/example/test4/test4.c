#include "usertraps.h"
#include "misc.h"

int recusive_builder(int n)
{
  if(n == 0) {return 0;}
  // recursion to grow the call stack
  if (n==1) {
    Printf("TEST4 (%d): running out of stack\n", getpid());
  }
  return recusive_builder(n - 1);
}


void main (int argc, char *argv[])
{
  sem_t test_complete; // Semaphore to signal the original process that we're done
  int x = 2500;
  int out;
  // int * ptr;
  Printf("TEST4 (%d): Start accessing out of range testing: Access memory beyond the maximum virtual address.\n", getpid());
  
  if (argc != 2) { 
    Printf("Usage: %s <test_complete_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  test_complete = dstrtol(argv[1], NULL, 10);
  // Now print a message to show that everything worked
  Printf("TEST4 (%d): Test growing stack past 1 page!\n", getpid());

  // call recursive function to grow the call stack
  out = recusive_builder(x);

  Printf("TEST4 (%d): Done!\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(test_complete) != SYNC_SUCCESS) {
    Printf("TEST4 (%d): Bad semaphore test_complete (%d)!\n", getpid(), test_complete);
    Exit();
  }
}

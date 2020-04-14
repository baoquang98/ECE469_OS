#include "usertraps.h"
#include "misc.h"

// define the user program string
#define TEST1 "test1.dlx.obj"
#define TEST2 "test2.dlx.obj"
#define TEST3 "test3.dlx.obj"
#define TEST4 "test4.dlx.obj"
#define TEST5 "test5.dlx.obj"
#define TEST6 "test6.dlx.obj"

void test_wrapper(int test_id){

  sem_t test_complete;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if ((test_complete = sem_create(0)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(test_complete, s_procs_completed_str);

  switch(test_id) {
    case 1: 
      process_create(TEST1, s_procs_completed_str, NULL);
      break;
    case 2:
      process_create(TEST2, s_procs_completed_str, NULL);
      // Signal the semaphore to tell the original process that we're done
      if(sem_signal(test_complete) != SYNC_SUCCESS) {
        Printf("TEST2 (%d): Bad semaphore test_complete (%d)!\n", getpid(), test_complete);
        Exit();
      }
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
  }
  if (sem_wait(test_complete) != SYNC_SUCCESS) {
    Printf("Bad semaphore test_complete (%d) in TEST%d\n", test_complete, test_id);
    Exit();
  }
}

void main (int argc, char *argv[])
{
  int num_hello_world = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable

  if (argc != 1) {
    Printf("Usage: no argument, this just runs the tests\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  // num_hello_world = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  // Printf("makeprocs (%d): Creating %d hello_world processes\n", getpid(), num_hello_world);

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.


  Printf("-------------------------------------------------------------------------------------\n");
  Printf("Start Testing\n");
  Printf("-------------------------------------------------------------------------------------\n");

  test_wrapper(1);
  Printf("-------------------------------------------------------------------------------------\n");
  test_wrapper(2);
  Printf("-------------------------------------------------------------------------------------\n");
  // test_wrapper(3);
  // Create Hello World processes
  // Printf("-------------------------------------------------------------------------------------\n");
  // Printf("makeprocs (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), num_hello_world);
  // for(i=0; i<num_hello_world; i++) {
  //   Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
  //   process_create(HELLO_WORLD, s_procs_completed_str, NULL);
  //   if (sem_wait(test_complete) != SYNC_SUCCESS) {
  //     Printf("Bad semaphore test_complete (%d) in %s\n", test_complete, argv[0]);
  //     Exit();
  //   }
  // }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
  Printf("Q2 end\n");
}

#include "usertraps.h"
#include "misc.h"

// define the user program string
#define TEST1 "test1.dlx.obj"
#define TEST2 "test2.dlx.obj"
#define TEST3 "test3.dlx.obj"
#define TEST4 "test4.dlx.obj"
#define TEST5 "test5.dlx.obj"
#define TEST6 "test6.dlx.obj"

void test_wrapper(int test_id, sem_t all_complete){
  int i;
  sem_t test_complete;             // Semaphore used to wait until all spawned processes have completed
  char test_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes
  int num_processes;

  switch(test_id) {
    case 1: num_processes = 1;    break;
    case 2: num_processes = 1;    break;
    case 3: num_processes = 1;    break;
    case 4: num_processes = 1;    break;
    case 5: num_processes = 100;  break;
    case 6: num_processes = 30;   break;
  }


  if ((test_complete = sem_create(num_processes - 1)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(test_complete, test_completed_str);

  switch(test_id) {
    case 1:   
      Printf("TEST1\n");
      process_create(TEST1, test_completed_str, NULL);
      break;
    case 2:
      process_create(TEST2, test_completed_str, NULL);
      break;
    case 3:
      process_create(TEST3, test_completed_str, NULL);
      break;
    case 4:
      process_create(TEST4, test_completed_str, NULL);
      break;
    case 5:
      Printf("TEST5\n");
      for (i = 0; i < num_processes; i++)
      {
        process_create(TEST1, test_completed_str, NULL);
      }
      break;
    case 6:
      for (i = 0; i < num_processes; i++)
      {
        process_create(TEST6, test_completed_str, NULL);
      }
      break;
  }

  if (sem_wait(test_complete) != SYNC_SUCCESS) {
    Printf("Bad semaphore test_complete (%d) in TEST%d\n", test_complete, test_id);
    Exit();
  }

  if(sem_signal(all_complete) != SYNC_SUCCESS) {
    Printf("All test bad (%d)!\n", getpid(), all_complete);
    Exit();
  }
  Printf("-------------------------------------------------------------------------------------\n");

}

void main (int argc, char *argv[])
{
  sem_t all_complete;             // Semaphore used to wait until all spawned processes have completed

  int num_hello_world = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable

  if (argc != 1) {
    Printf("Usage: no argument, this just runs the tests\n");
    Exit();
  }

  if ((all_complete = sem_create(6 - 1)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
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

  test_wrapper(1, all_complete);
  test_wrapper(3, all_complete);
  test_wrapper(4, all_complete);
  test_wrapper(5, all_complete);
  test_wrapper(6, all_complete);
  test_wrapper(2, all_complete);

  // test_wrapper(3);
  // Create Hello World processes
  // Printf("-------------------------------------------------------------------------------------\n");
  // Printf("makeprocs (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), num_hello_world);
  // for(i=0; i<num_hello_world; i++) {
  //   Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
  //   process_create(HELLO_WORLD, test_completed_str, NULL);
  //   if (sem_wait(test_complete) != SYNC_SUCCESS) {
  //     Printf("Bad semaphore test_complete (%d) in %s\n", test_complete, argv[0]);
  //     Exit();
  //   }
  // }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
  Printf("Q2 end\n");
}

#include "usertraps.h"
#include "misc.h"

// define the user program string
#define TEST1 "test1.dlx.obj"
#define TEST2 "test2.dlx.obj"
#define TEST3 "test3.dlx.obj"
#define TEST4 "test4.dlx.obj"
#define TEST5 "test5.dlx.obj"
#define TEST6 "test6.dlx.obj"

void test1(sem_t s_procs_completed, char * s_procs_completed_str){
  Printf("Start hello world testing: Print 'Hello World' and exit.\n");
  process_create(TEST1, s_procs_completed_str, NULL);
  Printf("Done with TEST1 in Q2\n");

  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in TEST1\n", s_procs_completed);
    Exit();
  }
}

void main (int argc, char *argv[])
{
  int num_hello_world = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 1) {
    Printf("Usage: no argument, this just runs the tests\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  // num_hello_world = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  // Printf("makeprocs (%d): Creating %d hello_world processes\n", getpid(), num_hello_world);

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);

  test1(s_procs_completed, s_procs_completed_str);

  // Create Hello World processes
  // Printf("-------------------------------------------------------------------------------------\n");
  // Printf("makeprocs (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), num_hello_world);
  // for(i=0; i<num_hello_world; i++) {
  //   Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
  //   process_create(HELLO_WORLD, s_procs_completed_str, NULL);
  //   if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
  //     Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
  //     Exit();
  //   }
  // }

  // Printf("-------------------------------------------------------------------------------------\n");
  // Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}

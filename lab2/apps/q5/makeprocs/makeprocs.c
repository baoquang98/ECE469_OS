#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  int h2o;               // Used to store number of H2O to create
  int so4;               // Used to store number of SO4 to create
  int h2o_split_count;               // Used to store number of reaction 1
  int so4_split_count;               // Used to store number of reaction 2
  int h2so4_produce_count;               // Used to store number of reaction 3
  int numprocs;
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  sem_t h2o_inject_sem;        // Semaphore used to wait for h20 injection
  sem_t so4_inject_sem;        // Semaphore used to wait for so4 injection
  sem_t h2_sem;        // Semaphore used to wait for reaction 1
  sem_t o2_sem;        // Semaphore used to wait for reaction 1
  sem_t so2_sem;        // Semaphore used to wait for reaction 2
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char h2o_inject_sem_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char so4_inject_sem_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char h2_sem_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char o2_sem_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char so2_sem_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char h2o_str[10];               
  char so4_str[10];               
  char h2o_split_count_str[10];               
  char so4_split_count_str[10];               
  char h2so4_produce_count_str[10];              
	
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of H20 molecules> <number of SO4 molecules>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  h2o = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  so4 = dstrtol(argv[2], NULL, 10);
  numprocs = 5; //2 injection for H20 and SO4, 3 consumptions which maps to 3 reactions
  Printf("Creating 5 processes\n");

  //Number of reaction needed as a stop condition for the child processes
  h2o_split_count = h2o/2;
  so4_split_count = so4;
  (2*h2o_split_count < so4_split_count) ? (h2so4_produce_count = 2*h2o_split_count) : (h2so4_produce_count = so4_split_count);

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((h2o_inject_sem = sem_create(0)) == SYNC_FAIL) { //2 h2o needed for reaction
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((so4_inject_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((h2_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((o2_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((so2_sem = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }


  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(h2o_inject_sem, h2o_inject_sem_str);
  ditoa(so4_inject_sem, so4_inject_sem_str);
  ditoa(h2_sem, h2_sem_str);
  ditoa(o2_sem, o2_sem_str);
  ditoa(so2_sem, so2_sem_str);
  ditoa(h2o_split_count, h2o_split_count_str);
  ditoa(so4_split_count, so4_split_count_str);
  ditoa(h2so4_produce_count, h2so4_produce_count_str);
  ditoa(h2o, h2o_str);
  ditoa(so4, so4_str);
  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  process_create(H2O_PRODUCE_FILE, s_procs_completed_str, h2o_inject_sem_str, h2o_str, NULL);
  process_create(SO4_PRODUCE_FILE, s_procs_completed_str, so4_inject_sem_str, so4_str, NULL);
  process_create(H2O_SPLIT_FILE, s_procs_completed_str, h2o_inject_sem_str, h2_sem_str, o2_sem_str, h2o_split_count_str, NULL);
  process_create(SO4_SPLIT_FILE, s_procs_completed_str, so4_inject_sem_str, so2_sem_str, o2_sem_str, so4_split_count_str, NULL);
  process_create(H2SO4_PRODUCE_FILE, s_procs_completed_str, h2_sem_str, o2_sem_str, so2_sem_str, h2so4_produce_count_str, NULL);
  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("All other processes completed, exiting main process.\n");
}

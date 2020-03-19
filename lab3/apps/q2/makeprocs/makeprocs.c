#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  int s2;               // Used to store number of s2 to get
  int co;               // Used to store number of SO4 to get
  int s;
  int o2;
  int s2_split_count;               // Used to store number of reaction 1
  int co_split_count;               // Used to store number of reaction 2
  int so4_produce_count;               // Used to store number of reaction 3
  
  int numprocs;
  int i;

  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  
  mbox_t s2_mbox;        // mailbox used to wait for s2 injection
  mbox_t co_mbox;        // mailbox used to wait for co injection
  mbox_t s_mbox;        // mailbox used to wait for reaction 1
  mbox_t o2_mbox;        // mailbox used to wait for reaction 1
  
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  
  char s2_mbox_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char co_mbox_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char s_mbox_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
  char o2_mbox_str[10]; 	// Used as command-line argument to pass page_mapped handle to new processes
	
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of S2 molecules> <number of CO molecules>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  s2 = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  co = dstrtol(argv[2], NULL, 10);
  
  //Number of reaction needed as a stop condition for the child processes
  s2_split_count = s2;
  s = 2 * s2;
  co_split_count = co/4;
  o2 = 2 * co_split_count;
  
  (s < o2/2) ? (so4_produce_count = s) : (so4_produce_count = o2/2);
  
  numprocs = s2 + co + s2_split_count + co_split_count + so4_produce_count; //2 injection for S2 and CO, 3 consumptions which maps to 3 reactions
  Printf("Creating %d processes\n", numprocs);


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
  // injection boxes
  if ((s2_mbox = mbox_create()) == MBOX_FAIL) { 
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((co_mbox = mbox_create()) == MBOX_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  // reaction boxes
  if ((s_mbox = mbox_create()) == MBOX_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((o2_mbox = mbox_create()) == MBOX_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  mbox_open(s2_mbox);
  mbox_open(co_mbox);
  mbox_open(s_mbox);
  mbox_open(o2_mbox);

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(s2_mbox, s2_mbox_str);
  ditoa(co_mbox, co_mbox_str);
  ditoa(s_mbox, s_mbox_str);
  ditoa(o2_mbox, o2_mbox_str);
  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  for (i = 0; i < s2; i++) {
  	process_create(S2_PRODUCE_FILE, 0, 1, s_procs_completed_str, s2_mbox_str, NULL);
  }
  
  for (i = 0; i < co; i++) {
  	process_create(CO_PRODUCE_FILE, 0, 1, s_procs_completed_str, co_mbox_str, NULL);
  }
  for (i = 0; i < s2_split_count; i++) {
  	process_create(S2_SPLIT_FILE, 0, 1, s_procs_completed_str, s2_mbox_str, s_mbox_str, NULL);
  }
  for (i = 0; i < co_split_count; i++) {
  	process_create(CO_SPLIT_FILE, 0, 1, s_procs_completed_str, co_mbox_str, o2_mbox_str, NULL);
  }
  for (i = 0; i < so4_produce_count; i++) {
  	process_create(SO4_PRODUCE_FILE, 0, 1, s_procs_completed_str, s_mbox_str, o2_mbox_str, NULL);
  }
  
  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  mbox_close(s2_mbox);
  mbox_close(co_mbox);
  mbox_close(s_mbox);
  mbox_close(o2_mbox);
  /*
  Printf("%d H20 molecues left over. ", h2o - 2*h2o_split_count);
  Printf("%d H2 molecues left over. ", 2*h2o_split_count - h2so4_produce_count);
  Printf("%d O2 molecues left over. ",h2o_split_count + so4_split_count - h2so4_produce_count);
  Printf("%d SO2 molecues left over. ",so4_split_count - h2so4_produce_count);
  Printf("%d H2SO4 molecues left over.\n", h2so4_produce_count);
  */
  Printf("All other processes completed, exiting main process.\n");
}

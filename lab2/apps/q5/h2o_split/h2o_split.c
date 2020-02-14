#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_t h2o_inject_sem; // Semaphore to signal the original process that we're done
  sem_t h2_sem; // Semaphore to signal the original process that we're done
  sem_t o2_sem; // Semaphore to signal the original process that we're done
  int h2o_split_count;
  int count = 0;
  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf("<handle_to_page_mapped_semaphore> <handle_to_page_mapped_semaphore> <handle_to_page_mapped_semaphore> <handle_to_page_mapped_semaphore> <handle_to_h20_split_number>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  h2o_inject_sem = dstrtol(argv[2], NULL, 10);
  h2_sem = dstrtol(argv[3], NULL, 10);
  o2_sem = dstrtol(argv[4], NULL, 10);
  h2o_split_count = dstrtol(argv[5], NULL, 10);

  // Now print a message to show that everything worked
  for (count = 0; count < h2o_split_count; count++) {
  	sem_wait(h2o_inject_sem);
  	sem_wait(h2o_inject_sem);
  	sem_signal(h2_sem);
  	sem_signal(h2_sem);
  	sem_signal(o2_sem);
	Printf("2H2O -> 2H2 + O2, pid: %d\n", getpid());
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

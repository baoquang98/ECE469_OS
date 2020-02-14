#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_t h2o_inject_sem; // Semaphore to signal the original process that we're done
  int h2o;
  int count = 0;
  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf("<handle_to_page_mapped_semaphore> <handle_to_page_mapped_semaphore> <handle_to_h20_number>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  h2o_inject_sem = dstrtol(argv[2], NULL, 10);
  h2o = dstrtol(argv[3], NULL, 10);
  // Now print a message to show that everything worked
  for (count = 0; count < h2o; count++) {
  	sem_signal(h2o_inject_sem);
	Printf("H2O injected into Raedon, pid: %d\n", getpid());
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

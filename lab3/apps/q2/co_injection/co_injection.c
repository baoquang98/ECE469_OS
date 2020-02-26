#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t mbox_co; //mailbox for s2
  char mes[] = "co"; 
  
  if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf("<handle_to_page_mapped_semaphore> <handle_to_page_mapped_mbox>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  mbox_co = dstrtol(argv[2], NULL, 10);

  // Now print a message to show that everything worked
  mbox_open(mbox_co);
  mbox_send(mbox_co, sizeof(mes), (void *) mes);
  Printf("CO injected, pid: %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

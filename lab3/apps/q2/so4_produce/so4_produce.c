#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t mbox_s; //mailbox for s2
  mbox_t mbox_o2; //mailbox for s2
  char buffer_s[sizeof("2s") + 1];
  char buffer_o2[sizeof("o2") + 1];
  int mes_recv;
  int i;
  
  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf("<handle_to_page_mapped_semaphore> <handle_to_page_mapped_mbox> <handle_to_page_mapped_mbox>\n"); 
    Exit();
  } 
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  mbox_s = dstrtol(argv[2], NULL, 10);
  mbox_o2 = dstrtol(argv[3], NULL, 10);
  // Now print a message to show that everything worked
  mbox_open(mbox_s);
  for (i = 0; i < 1; i++) {
  	mes_recv = mbox_recv(mbox_s, sizeof(buffer_s), (void *) buffer_s);
	/*if (mes_recv != sizeof(buffer_s)) {
		Printf("Error receiving S molecule number %d in process %d\n", i+1, getpid());
		Exit();
	}*/
  }
  mbox_close(mbox_s);
  mbox_open(mbox_o2);
  for (i = 0; i < 2; i++) {
  	mes_recv = mbox_recv(mbox_o2, sizeof(buffer_o2), (void *) buffer_o2);
  	/*if (mes_recv != sizeof(buffer_o2)) {
		Printf("Error receiving O2 molecule number %d in process %d\n", i+1, getpid());
		Exit();
	}*/
  }
  mbox_close(mbox_o2);
  
  Printf("S + 2O2 -> SO4, pid: %d\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

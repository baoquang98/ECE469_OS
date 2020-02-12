#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  circ_buffer *buffer;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  lock_t producer_lock; 
  cond_t cond_full;
  cond_t cond_empty;

  char text[] = "HelloWorld";
  int count = 0;
  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle_to_page_mapped_lock> <handle_to_page_mapped_cond_var> <handle_to_page_mapped_cond_var>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  producer_lock = dstrtol(argv[3], NULL, 10);
  cond_empty = dstrtol(argv[4], NULL, 10);
  cond_full = dstrtol(argv[5], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((buffer = (circ_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
 
  // Now print a message to show that everything worked
  Printf("producer: My PID is %d\n", getpid());
  for (count = 0; count < strlen("HelloWorld"); count++) { 
  	lock_acquire(producer_lock);
  	if ((buffer->head + 1) % BUFFERSIZE == buffer->tail) { //If buffer is full
		cond_wait(cond_full);
	}
	Printf("producer %d inserted: %c\n", getpid(), text[count]);
	buffer->head = (buffer->head + 1) % BUFFERSIZE;
	buffer->buff[buffer->head] = text[count];
	cond_signal(cond_empty);
	lock_release(producer_lock);
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("producer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  circ_buffer *buffer;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  lock_t consumer_lock; 
  int count = 0;
  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle_to_page_mapped_lock>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  consumer_lock = dstrtol(argv[3], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((buffer = (circ_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
 
  // Now print a message to show that everything worked
  Printf("consumer: My PID is %d\n", getpid());
  while(count < strlen("HelloWorld")) {
  	lock_acquire(consumer_lock);
  	if (buffer->head != buffer->tail) { //If buffer is not empty, remove a char
		Printf("Consumer %d removed: %c\n", getpid(), buffer->buff[buffer->tail]);
		buffer->tail = (buffer->tail + 1) % BUFFERSIZE;
  		count++;
	}
	lock_release(consumer_lock);
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("consumer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

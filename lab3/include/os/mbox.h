#ifndef __MBOX_OS__
#define __MBOX_OS__

#define MBOX_NUM_MBOXES 16           // Maximum number of mailboxes allowed in the system
#define MBOX_NUM_BUFFERS 50          // Maximum number of message buffers allowed in the system
#define MBOX_MAX_BUFFERS_PER_MBOX 10 // Maximum number of buffer slots available to any given mailbox
#define MBOX_MAX_MESSAGE_LENGTH 100   // Buffer size of 100 for each message

#define MBOX_FAIL -1
#define MBOX_SUCCESS 1

#include "process.h"
//---------------------------------------------
// Define your mailbox structures here
//--------------------------------------------

typedef struct mbox_message {
	char buffer[MBOX_MAX_MESSAGE_LENGTH];
	int size;
	int inuse;
} mbox_message;

typedef struct mbox {
	int inuse;	// if the space is allocated/reserved for 
				// the mailbox/ or that the mailbox is created
	Queue message_buffer;
	int lock_allocated;	// used in case locks are over allocated
	lock_t lock;
	cond_t cond_full;
	cond_t cond_empty;
	int pid[PROCESS_MAX_PROCS];	// processes currently using this mail box
	int total_messages;
} mbox;

typedef int mbox_t; // This is the "type" of mailbox handles

//-------------------------------------------
// Prototypes for Mbox functions you have to write
//-------------------------------------------

void MboxModuleInit();
mbox_t MboxCreate();
int MboxOpen(mbox_t m);
int MboxClose(mbox_t m);
int MboxSend(mbox_t m, int length, void *message);
int MboxRecv(mbox_t m, int maxlength, void *message);
int MboxCloseAllByPid(int pid);

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif

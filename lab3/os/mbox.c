#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

mbox mbox_list[MBOX_NUM_MBOXES];
mbox_message mbox_messages_list[MBOX_NUM_BUFFERS];
//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
	int i;
	int j;
	for (i = 0; i < MBOX_NUM_MBOXES; i++) {
		mbox_list[i].inuse = 0;
		mbox_list[i].lock_allocated = 0;
		// mbox_list[i].total_messages = 0; should be done in mbox_create
		AQueueInit(&(mbox_list[i].message_buffer));
		for (j = 0; j < PROCESS_MAX_PROCS; j++) {
			mbox_list[i].pid[j] = 0;
		}
	}
	//init and clear all inuse flag
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
//similar to sem_create procedure? pick the first mbox thats not inuse
//init all the fields iinside the mbox ur picking
 	//Disable interupt here
	
	int handle = 0;
	//Search for the first not in use mailbox
	for(handle = 0; handle < MBOX_NUM_MBOXES; handle++) {
		if (!mbox_list[handle].inuse) {
			break;
		}
	}
	if (handle >= MBOX_NUM_MBOXES) {
		return MBOX_FAIL;
	}
	mbox_list[handle].inuse = 1;
	if (mbox_list[handle].lock_allocated == 0) { 
		// only create the lock if there is no 
		// associated lock with the mail box.
		// we only use this because there is no way to destroy the lock once created.
		mbox_list[handle].lock = LockCreate();
		mbox_list[handle].lock_allocated = 1;
	}
	mbox_list[handle].total_messages = 0;
	mbox_list[handle].cond_full = CondCreate(mbox_list[handle].lock);
	mbox_list[handle].cond_empty = CondCreate(mbox_list[handle].lock);
  	//enable interupt
  return handle;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
//set the pid flag to be 1
	if(!mbox_list[handle].inuse) {
		printf("Mbox is not inuse, what are u doin process %d\n", GetCurrentPid());
		return MBOX_FAIL;
	}

	//set the pid flag to be 1
	if (SYNC_FAIL == LockHandleAcquire(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
	mbox_list[handle].pid[GetCurrentPid()] = 1;	// openning
	if (SYNC_FAIL == LockHandleRelease(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
 //2 conditions: if not last pid, set flag in pid list to 0 else deletes everything in messges queue, free lock, set inuse to 0
	int i = 0;
	int count = 0;
	Link *l;
	// atomic on lock to close
	if (SYNC_FAIL == LockHandleAcquire(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
	if (mbox_list[handle].pid[GetCurrentPid()]==0) {
		return MBOX_FAIL;
	}

	for (i = 0; i < PROCESS_MAX_PROCS; i++) {
		if (mbox_list[handle].pid[i]) 
			count++;
	}
	if (count == 1){ // If last process and if it is the current process
		//Free stuff
		while(!AQueueEmpty(&(mbox_list[handle].message_buffer))) {
			l = AQueueFirst(&(mbox_list[handle].message_buffer));
			AQueueRemove(&l);
		}
		mbox_list[handle].inuse = 0;
		mbox_list[handle].pid[GetCurrentPid()] = 0;
    }

	if (SYNC_FAIL == LockHandleRelease(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
 //acquire lock, check ur pid in the list of pid thats suppose to use the lock, check if the mbox is full, if full, wait on condition, copy the message to the buffer , put the messges in the queue, increment the total_message count, release lock
	//Need much error checking
	int i;
	Link * l;
	if (!mbox_list[handle].pid[GetCurrentPid()]) {
		return MBOX_FAIL;
	}
	if (mbox_list[handle].total_messages >= MBOX_MAX_BUFFERS_PER_MBOX) {	// wait till the mail box is not full
		CondHandleWait(mbox_list[handle].cond_full);
	}

	if (SYNC_FAIL == LockHandleAcquire(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}



	for (i = 0; i < MBOX_NUM_BUFFERS; i++) {
		if (!mbox_messages_list[i].inuse) {
			break;
		}
	}
	mbox_messages_list[i].inuse = 1;
	bcopy(message, mbox_messages_list[i].buffer, length);
	mbox_messages_list[i].size = length;
	mbox_list[handle].total_messages++;

	l = AQueueAllocLink(&(mbox_messages_list[i]));
	AQueueInsertLast(&(mbox_list[handle].message_buffer),l);

	if (SYNC_FAIL == LockHandleRelease(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
	CondHandleSignal(mbox_list[handle].cond_empty);

 return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
	//similar to MboxSend but get the first messages, return the number of bytes
	//Need much error checking
	mbox_message * mes;
	Link * l;
	if (maxlength > MBOX_MAX_MESSAGE_LENGTH){
		return MBOX_FAIL;
	}
	if (!mbox_list[handle].pid[GetCurrentPid()]) {
		return MBOX_FAIL;
	}
	if (mbox_list[handle].total_messages == 0) {
		CondHandleWait(mbox_list[handle].cond_empty);
	}
	if (SYNC_FAIL==LockHandleAcquire(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}

	l = AQueueFirst(&(mbox_list[handle].message_buffer));
	mes = (mbox_message *) l->object;	// read the message
	// by this point we should already have the size of the message
	if (maxlength < mes->size) {
		return MBOX_FAIL;
	}

	bcopy(mes->buffer, message, mes->size);
	mes->inuse = 0;
	AQueueRemove(&l);
	mbox_list[handle].total_messages--;

	if (SYNC_FAIL == LockHandleRelease(mbox_list[handle].lock)){
		return MBOX_FAIL;
	}
	CondHandleSignal(mbox_list[handle].cond_full);
  
  return mes->size;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
	int i;
	Link * l;
	int handle = 0;
	int count;
	for (handle = 0; handle < MBOX_NUM_MBOXES; handle++) {
		if (mbox_list[handle].inuse) {
			if (SYNC_FAIL == LockHandleAcquire(mbox_list[handle].lock)){
				return MBOX_FAIL;
			}
			count = 0;
			for (i = 0; i < PROCESS_MAX_PROCS; i++) {
				if (mbox_list[handle].pid[i]) 
					count++;
			}
			if (count == 1 && mbox_list[handle].pid[pid]==1){ // If last process and if it is the current process
				//Free stuff
				while(!AQueueEmpty(&(mbox_list[handle].message_buffer))) {
					l = AQueueFirst(&(mbox_list[handle].message_buffer));
					AQueueRemove(&l);
				}
				mbox_list[handle].inuse = 0;
				mbox_list[handle].pid[pid] = 0;
			}
			if (SYNC_FAIL == LockHandleRelease(mbox_list[handle].lock)){
				return MBOX_FAIL;
			}
		}
		/////////
  	}

  return MBOX_SUCCESS;
}

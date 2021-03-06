//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
static uint32 freemap[MEM_MAX_SIZE / MEM_PAGESIZE / 32];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;
static int reference_counters[MEM_MAX_SIZE >> MEM_L1FIELD_FIRST_BITNUM];

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}


//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int i;
  // set up the freemap for pages
  int maxpage = MemoryGetSize() / MEM_PAGESIZE;   // get the maximum number of pages available for allocation
  uint32 num_os_pages = (lastosaddress & 0x1FFFFC) / MEM_PAGESIZE;  // get the number of pages used by the operating system
  uint32 index;
  uint32 bit_position;

  dbprintf('m', "Entering memory module initialization\n");

  pagestart = num_os_pages + 1;               // the starting page after the os
  freemapmax = (maxpage+31) / 32;             // the maximum number of uint32 of freemap

  // Initialize all
  for(i = 0; i < freemapmax; i++) {
    freemap[i] = 0;
  }

  // Go from the page start to the maxpage
  nfreepages = 0;
  for(i = pagestart; i < maxpage; i++) {
    nfreepages += 1;
    // get page and bit position
    index = i / 32;
    bit_position = i % 32;
    // set the val
    freemap[index] = (freemap[index] & invert(1 << bit_position)) | (1 << bit_position);
  }
  dbprintf('m', "Done with memory module initialization\n");
}

  
//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  uint32 page = MEM_ADDR2PAGE(addr);    // the page index

  if (pcb->pagetable[page] & MEM_PTE_VALID) {
    return ((pcb->pagetable[page] & MEM_MASK_PTE2PAGE) | MEM_ADDR2OFFS(addr));
  }
  return MEM_FAIL;
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  // addresses to use
  uint32 u_stack_ptr = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER];
  uint32 fault_addr = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  // corresponding pages for the addresses
  int pg_fault_addr = MEM_ADDR2PAGE(fault_addr);
  int genPage;

  u_stack_ptr &= 0x1FF000;    // mask

  dbprintf('m', "Start: Memory-Page Fault Handler (%d)\n", GetPidFromAddress(pcb));

  if(fault_addr < u_stack_ptr) {
    // It is indeed a seg fault
    printf("Exiting process %d: MemoryPageFaultHandler seg fault\n", GetPidFromAddress(pcb));
    dbprintf ('m', "Memory-Page Fault Handler (%d): seg fault addr=0x%x\n", GetPidFromAddress(pcb), fault_addr);
    ProcessKill();
    return MEM_FAIL;
  } else {
    // Not a seg fault, so we are gonna allocate a new page to use
    genPage = MemoryAllocPage();
    if(genPage == MEM_FAIL) {
      printf("FATAL: Not Enough Free Pages %d\n", GetPidFromAddress(pcb));
      ProcessKill();
    }
    // Use the setup pte function
    pcb->pagetable[pg_fault_addr] = MemorySetupPte(genPage);
    // Used to show a debug message that a new page has been allocated from the memorypagefault handler for part5
    dbprintf('z', "MemoryPageFaultHandler Process (%d): allocating new page (%d)\n", GetPidFromAddress(pcb), genPage);
    pcb->npages += 1;
    return MEM_SUCCESS;
  }
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------

int MemoryAllocPage(void) {
  int index = 0;
  uint32 bit_position = 0;
  uint32 page_physical_index;

  // dbprintf('m', "Start to allocate pages\n");
  // Check if we still have free pages
  if(nfreepages == 0) {
    // dbprintf('m', "MemoryAllocPage: no available pages\n");
    return MEM_FAIL;
  }

  // loop through the freemaps until you find one with available slots
  while(freemap[index] == 0) {
    index += 1;
    if(index >= freemapmax) {
      // index starts from 0
      // dbprintf('m', "MemoryAllocPage: no available pages\n");
      return MEM_FAIL;
    }
  }
  // get the 32 bit segment
  page_physical_index = freemap[index];
  // Find the first available page
  while ((page_physical_index & (1 << bit_position)) == 0) {
    bit_position++;
  }
  
  // set the bit 0
  freemap[index]  &= invert(1 << bit_position);
  // get the index of the page
  page_physical_index = (index * 32) + bit_position; 
  // dbprintf('m', "MemoryAllocPage: allocated memory from map=%d, page=%d\n", index, page_physical_index);
  // Decrement nfreepages
  nfreepages -= 1;

  return page_physical_index; // page index on memory space
}


uint32 MemorySetupPte (uint32 page) {
  return ((page * MEM_PAGESIZE) | MEM_PTE_VALID);
}

void MemorySharePTE (uint32 pte) {
  // convert PTE to page (the real fork process handles setting the read only bit)
  int page = ((pte & MEM_MASK_PTE2PAGE) / MEM_PAGESIZE);
  // increment the reference counter
  reference_counters[page] += 1;
  // debug message
  dbprintf('m', "MemorySharePTE: page=%d count=%d\n", page, reference_counters[page]);
}


void MemoryFreePageTableEntry(uint32 pte) {
  // Converts pagetable entry to page and then sends to memory free page function
  MemoryFreePage((pte & MEM_MASK_PTE2PAGE) / MEM_PAGESIZE);
}

void* malloc(PCB* pcb, int memsize){
  return NULL;
}
int mfree(PCB* pcb, void* ptr){
  return 1;
}

void MemoryFreePage(uint32 page) {
  // get page and bit position
  uint32 index = page / 32;
  uint32 bit_position = page % 32;
  // set the bit to 1
  freemap[index] = (freemap[index] & invert(1 << bit_position)) | (1 << bit_position);
  // increment the number of free pages
  nfreepages += 1;
  dbprintf ('m', "Freed page 0x%x, %d remaining.\n", page, nfreepages);
}


void MemoryRopHandler(PCB * pcb) {
  // addresses to use
  uint32 fault_address = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  // corresponding pages for the addresses
  int pg_fault_address = MEM_ADDR2PAGE(fault_address);
  int parent_page = MEM_ADDR2PAGE(pcb->pagetable[pg_fault_address] & MEM_MASK_PTE2PAGE);
  int new_page;

  dbprintf('m', "MemoryRopHandler: Let's start.\n");

  if(reference_counters[parent_page] > 1) {
    // there are more users, which calls for new pages
    dbprintf('m', "Now it's time to copy page %d to page %d\n", parent_page, pg_fault_address);
    // generate and setup a page
    new_page = MemoryAllocPage();
    pcb->pagetable[pg_fault_address] = MemorySetupPte (new_page);
    // do the copying
    bcopy((char *)(fault_address), (char *)(new_page * MEM_PAGESIZE), MEM_PAGESIZE);
    // decrement reference counter since one has its own
    reference_counters[parent_page] -= 1;
  } else {
    dbprintf('m', "MemoryRoHandler: there is exactly one process using this page, invert read-only.\n");
    // Reference counter is only one, so reset the readonly
    pcb->pagetable[pg_fault_address] &= invert(MEM_PTE_READONLY);
  }
  dbprintf('m', "MemoryRoHandler: End.\n");
}

#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
	int pid = 0;			      // used to distinguish from parent/child
	int b_Juliana = 1127;	  // test variable

	//Print pid from original process
	Printf("Fork Testing (PID: %d): This is the original process\n", getpid());
	Printf("Fork Testing (PID: %d): this process holds a variable j of value %d\n", b_Juliana);
	pid = fork();

	if(pid == 0) {
		//child process
		Printf("Fork Testing (PID: %d): This is the child process\n", getpid());
		Printf("Fork Testing (PID: %d): j was %d.\n", getpid(), b_Juliana);
		b_Juliana = 1998;
		Printf("Fork Testing (PID: %d): j now is %d.\n", getpid(), b_Juliana);
	} else {
		//parent 
		Printf("Fork Testing (PID: %d): Hello i'm the respectable parent. My id is %d :)\n", getpid());
		Printf("Fork Testing (PID: %d): j was %d\n", getpid(), b_Juliana);
		b_Juliana = 902;
		Printf("Fork Testing (PID: %d): j now is %d\n", getpid(), b_Juliana);
	}

	Printf("Fork Testing (PID: %d): Done.\n", getpid());
}
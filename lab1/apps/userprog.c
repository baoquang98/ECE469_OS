#include "usertraps.h"

void main (int x)
{
  Printf("Hello World!\n");
  Printf("Pid: %d\n", Getpid());
  while(1); // Use CTRL-C to exit the simulator
}

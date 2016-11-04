/* 
 * arghalt.c 
 */
#include "syscall.h"

int 
main(int argc, char **argv) 
{ 
  int i;

  for (i=0; i<argc; i++) 
    Create(argv[i]); /* Uses BOGUS Create to print from kernel */

  Halt(); 
    /* not reached */ 
}

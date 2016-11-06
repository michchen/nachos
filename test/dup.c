/* kid.c
 *
 * Child in the parent_child system.
 *
 */

#include "syscall.h"

int
main()
{
  int i;
  int j;
  prints("Duping starting\n", ConsoleOutput);
  i = Open("fileio.out");
  j = Dup(ConsoleOutput);
  Close(ConsoleOutput);
  prints("Duping finished\n", j);
  
  Halt();
    /* not reached */
}


/* Print a null-terminated string "s" on open file descriptor "file". */

prints(s,file)
char *s;
OpenFileId file;

{
  int count = 0;
  char *p;

  p = s;
  while (*p++ != '\0') count++;
  Write(s, count, file);  

}
printd(n,file)
int n;
OpenFileId file;

{

  int i;
  char c;

  if (n < 0) {
    Write("-",1,file);
    n = -n;
  }
  if ((i = n/10) != 0)
    printd(i,file);
  c = (char) (n % 10) + '0';
  Write(&c,1,file);
}
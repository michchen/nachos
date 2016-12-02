/* duptest.c
 *
 * Dup a partially read file onto ConsoleInput.
 *
 */

#include "syscall.h"

int
main()
{

  int numbytes;
  char buffer[20], onechar;
  OpenFileId sharedfile, dupid;

  sharedfile = Open("data");
  prints("Output Open returned descriptor ", ConsoleOutput);
  printd((int)sharedfile, ConsoleOutput);
  prints("\n", ConsoleOutput);
  
  numbytes = Read(buffer, 20, sharedfile);
  prints("Read ", ConsoleOutput);
  printd(numbytes, ConsoleOutput);
  prints(" bytes\n", ConsoleOutput);

  prints("Data from the read was: <", ConsoleOutput);
  Write(buffer, numbytes, ConsoleOutput);
  prints(">\n", ConsoleOutput);

  Close(ConsoleInput);
  dupid = Dup(sharedfile);
  Close(sharedfile);

  /* Now the sharedfile is on ConsoleInput. Read a byte. */ 

  numbytes = Read(&onechar, 1, ConsoleInput);
  prints("Read ", ConsoleOutput);
  printd(numbytes, ConsoleOutput);
  prints(" bytes\n", ConsoleOutput);

  prints("Data from the read was: <", ConsoleOutput);
  Write(&onechar, numbytes, ConsoleOutput);
  prints(">\n", ConsoleOutput);

  Halt();
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


/* Print an integer "n" on open file descriptor "file". */

printd(n,file)
int n;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];
  
  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }
  
  if (n == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = n / divisor; n = n % divisor;
    if (d == 0) {
      if (!zflag) buffer[pos++] =  (char) (d % 10) + '0';
    } else {
      zflag = 0;
      buffer[pos++] =  (char) (d % 10) + '0';
    }
    divisor = divisor/10;
  }
  Write(buffer,pos,file);
}

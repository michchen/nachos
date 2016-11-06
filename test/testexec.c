#include "syscall.h"

int
main(){
	char *args[3];
	prints("about to enter the exec\n", ConsoleOutput);
	args[0] = "test/halt";
	args[1] = "Hello there";
  args[2] = (char *)0;

	Exec("test/testScript", args);
}


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
    /* cast as int and do printd of argv[0] */


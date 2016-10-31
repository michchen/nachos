#include "syscall.h"

int
main(){
	prints("about to enter the exec\n", ConsoleOutput);
	Exec("../test/halt");
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
/* cp.c
copies data from one file to another
*/
#include "syscall.h"

int 
main(int argc, char **argv) 
{ 
  int origFile;
  int newFile;
  char ch;
  int i;

  if (argc < 3) {
  	prints("Not enough args\n", ConsoleOutput);
  	Exit(-1);
  }

  for (i=0; i<argc; i++) {
    prints("Arg[",ConsoleOutput);
    printd(i,ConsoleOutput);
    prints("]=<",ConsoleOutput);
    prints(argv[i],ConsoleOutput);
    prints(">\n",ConsoleOutput);
  } 

  origFile = Open(argv[1]);
  newFile = Open(argv[2]);

  if (newFile == -1) {
  	Create(argv[2]);
  	newFile = Open(argv[2]);
  }

  if(origFile == -1 || newFile == -1) {
  	prints("Error finding file!\n", ConsoleOutput);
  	Exit(-1);
  }
  else {
  	while (Read(&ch, 1, origFile) == 1){
      Write(&ch, 1, newFile);
    }

  }




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
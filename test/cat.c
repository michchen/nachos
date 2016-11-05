/* Cat.c
writes from file to stdout
*/
#include "syscall.h"

int 
main(int argc, char **argv) 
{ 
  char ch;
  int fd;
  int i;
  int dupfd;
  if (argc == 1) {
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

  if (argc == 4) {
  	prints("trying to do redirected output", ConsoleOutput);
  	if (argv[2][1] == '\0'){
  		prints("looks like it's only one char long\n", ConsoleOutput);
  		fd = Open(argv[1]);
  		dupfd = Open(argv[3]);
  		if (dupfd == -1) {
  			Create(argv[3]);
  			dupfd = Open(argv[3]);
  		}
		if(fd == -1 || dupfd == -1) {
			prints("Error finding file!\n", ConsoleOutput);
		  	Exit(-1);
		}
		else {
			Close(ConsoleOutput);
			Dup(dupfd);
			Close(dupfd);
		  	while (Read(&ch, 1, fd) == 1){
		    	Write(&ch, 1, ConsoleOutput);
		  }
		}
  	}
  }
  else {
	  fd = Open(argv[1]);
	  if(fd == -1) {
	  	prints("Error finding file!\n", ConsoleOutput);
	  	Exit(-1);
	  }
	  else {
	  	while (Read(&ch, 1, fd) == 1){
	      Write(&ch, 1, ConsoleOutput);
	    }

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

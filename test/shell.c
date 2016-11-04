#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;
    char *args[1];

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	
	do {
	
	    Read(&buffer[i], 1, input); 

	} while( buffer[i++] != '\n' );


	buffer[--i] = '\0';

	prints(buffer, ConsoleOutput);
	prints("\n", ConsoleOutput);

	args[0] = (char *)0;

	if( i > 0 ) {
	  newProc = Fork();
          if (newProc == 0) {
          	prints("doing kid stuff");
          	Exec(buffer, args);
          }
	  else Join(newProc);
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


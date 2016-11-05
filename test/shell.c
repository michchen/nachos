#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    /*
    char prompt[2], ch, buffer[60];
    int i;
    char *args[128];
    int curArg;


    prompt[0] = '-';
    prompt[1] = '-';


    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	curArg = 0;

	args[0] = ' ';

	do {
		do {
			i++;
	    	Read(&buffer[i], 1, input); 
	    	
	    	args[curArg] += buffer[i];
	    } while (buffer[i] != ' ' && buffer[i] != '\n');
	    curArg++;
	    args[curArg] = ' ';
	    prints(args[curArg]);
	} while( buffer[i] != '\n' );
  */
    char prompt[3], ch, buffer[60];
    int i,error;
    char *args[1]; 

    prompt[0] = '-';
    prompt[1] = '-';
    prompt[2] = '>';
    /*While loop starts the console and lasts forever*/
    while( 1 )
    {
      /*Write a greeting message*/
      Write(prompt, 3, output);

      /*the length of the command input*/

      i = 0;
      /*THIS IS THE KEY BOARD LISTENER FOR A COMMAND TO BE INPUTED WITH ENTER*/
      do {
          Read(&buffer[i], 1, input); 

      } while(buffer[i++] != '\n' );

      /*add null terminating string*/
      buffer[--i] = '\0';

      prints(buffer, ConsoleOutput);
      prints("\n", ConsoleOutput);

      args[0] = (char *)0;

      if( i > 0 ) {
        newProc = Fork();
        if (newProc == 0) {
          prints("Trying to do kid stuff\n");
          error = Exec(buffer, args);
          if(error == -1){
            prints("There is no such scripted or function or class or anything called -> ",ConsoleOutput);
            prints(buffer,ConsoleOutput);
            prints("!\n",ConsoleOutput);
            Exit(-1);
          }
        }
        else {
          prints("The Parent is Here\n");
          Join(newProc);
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


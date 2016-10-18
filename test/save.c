#include "syscall.h"

int
main()
{
	OpenFileId fd;
    Create("hello World");
    Create("Fuck you");
    fd = Open("Fuck you");
    Close(fd);
    Halt();
}

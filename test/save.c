#include "syscall.h"

int
main()
{
	OpenFileId fd;
    Create("Fuck you");
    fd = Open("Fuck you");
    Write("hello there", 11, fd);
    Halt();
}

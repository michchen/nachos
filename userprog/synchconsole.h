#ifndef SYNCHCONSCOLE_H
#define SYNCHCONSCOLE_H

#include "console.h"
#include "synch.h"
#include "syscall.h"

class SynchConsole{
public:
	SynchConsole(char *file);

	~SynchConsole();

	char * ReadFile(char *buffer, int size);

	void WriteFile(char *buffer, int size);

	void ReadDone();

	void WriteDone();

private:
	Semaphore *readAvail;
	Semaphore *writeDone;
	Console *console;
	Lock *lock;
};

#endif //SYNCHCONSCOLE_H
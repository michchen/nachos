#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"
#include "syscall.h"

class SynchConsole{
public:
	SynchConsole();

	~SynchConsole();

	int Read(char buffer, int size);

	void Write(char buffer, int size);

	void ReadDone();

	void WriteDone();

private:
	Semaphore *readAvail;
	Semaphore *writeDone;
	Console *console;
	Lock *lock;
};

#endif //SYNCHCONSOLE_H
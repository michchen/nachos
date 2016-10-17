#ifndef SYNCHCONSCOLE_H
#define SYNCHCONSCOLE_H

#include "console.h"
#include "synch.h"

class SynchConsole{
public:
	SynchConsole(char *file);

	~SynchConsole();

	void ReadFile();

	void WriteFile();

	void ReadDone();

	void WriteDone();

private:
	Semaphore *readAvail;
	Semaphore *writeDone;
	Console *console;
	Lock *lock;
};

#endif //SYNCHCONSCOLE_H
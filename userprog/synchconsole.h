#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H
#ifdef CHANGED

#include "console.h"
#include "synch.h"
#include "syscall.h"

class SynchConsole{
public:
	SynchConsole();

	~SynchConsole();

	int Read(char *buffer, int size);

	void Write(char buffer, int size);

	void ReadDone();

	void WriteDone();

};

#endif //SYNCHCONSOLE_H
#endif
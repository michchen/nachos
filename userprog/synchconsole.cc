#include "synchconsole.h"


static void ReadAvail(int arg) { 

	SynchConsole* synch = (SynchConsole *)arg;
	synch->ReadDone();
}
static void WriteFinish(int arg) { 
	SynchConsole* synch = (SynchConsole *)arg;
	synch->WriteDone();
}


SynchConsole::SynchConsole(char *file) {
	readAvail = new(std::nothrow) Semaphore("read lock", 0);
	writeDone = new(std::nothrow) Semaphore("write lock", 0);
	console = new(std::nothrow) Console(file, file, ReadAvail, WriteFinish, 0);
}

SynchConsole::~SynchConsole() {
	delete readAvail;
	delete writeDone;
	delete console;
}

void
SynchConsole::ReadFile(){
	printf("%s\n", "reading not yet implemented");
}

void
SynchConsole::WriteFile() {
	printf("%s\n", "writing not yet implemented");
}

void
SynchConsole::ReadDone() {
	readAvail->V(); 
}

void
SynchConsole::WriteDone() {
	writeDone->V(); 
}
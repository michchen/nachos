#include "synchconsole.h"


static void ReadAvail(int arg) { 

	SynchConsole* synch = (SynchConsole *)arg;
	synch->ReadDone();
}
static void WriteFinish(int arg) { 
	SynchConsole* synch = (SynchConsole *)arg;
	synch->WriteDone();
}


SynchConsole::SynchConsole() {
	readAvail = new(std::nothrow) Semaphore("read lock", 0);
	writeDone = new(std::nothrow) Semaphore("write lock", 0);
	console = new(std::nothrow) Console(NULL, NULL, ReadAvail, WriteFinish, 0);
	lock = new(std::nothrow) Lock("synch console lock");
}

SynchConsole::~SynchConsole() {
	delete readAvail;
	delete writeDone;
	delete console;
	delete lock;
}

int
SynchConsole::Read(char buffer, int size){
	char ch;
	int curIndex = 0;
	printf("%s\n", "reading not yet implemented");
	lock->Acquire();
	readAvail->P();
	ch = console->GetChar();
	buffer = ch;

	lock->Release();
	return curIndex;


}

void
SynchConsole::Write(char buffer, int size) {
	int curIndex;
	lock->Acquire();
	fprintf(stderr, "%s\n", "is the problem before");

	console->PutChar(buffer);
	fprintf(stderr, "%s\n", "put is successful");

	writeDone->P();
	fprintf(stderr, "%s\n", "lock has been P'd");
	lock->Release();
	return;
}

void
SynchConsole::ReadDone() {
	readAvail->V(); 
}

void
SynchConsole::WriteDone() {
	writeDone->V(); 
}
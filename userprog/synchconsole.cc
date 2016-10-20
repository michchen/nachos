#ifdef CHANGED
#include "synchconsole.h"

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;
static Lock *lock;

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
SynchConsole::Read(char *buffer, int size){
	char ch;
	int curIndex = 0;
	lock->Acquire();
	while (curIndex < size){
		readAvail->P();
		ch = console->GetChar();

		*buffer = ch;
		curIndex++;
		buffer++;
	}
	lock->Release();
	return curIndex;


}

void
SynchConsole::Write(char buffer, int size) {
	int curIndex;
	lock->Acquire();

	console->PutChar(buffer);

	writeDone->P();
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

#endif
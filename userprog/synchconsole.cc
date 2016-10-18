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
	lock = new(std::nothrow) Lock("synch console lock");
}

SynchConsole::~SynchConsole() {
	delete readAvail;
	delete writeDone;
	delete console;
	delete lock;
}

char *
SynchConsole::ReadFile(char *buffer, int size){
	char ch;
	int curIndex = 0;
	printf("%s\n", "reading not yet implemented");
	lock->Acquire();
	readAvail->P();
	for (curIndex = 0; curIndex < size; curIndex++){
		ch = console->GetChar();
		buffer[curIndex] = ch;
		if(ch == '\0') {
			lock->Release();
			return buffer;
		}
	}

	lock->Release();
	return buffer;


}

void
SynchConsole::WriteFile(char *buffer, int size) {
	int curIndex;
	printf("%s\n", "writing not yet implemented");
	lock->Acquire();
	for(curIndex = 0; curIndex < size; curIndex++) {
		console->PutChar(buffer[curIndex]);
	}
	writeDone->P();
	lock->Release();
}

void
SynchConsole::ReadDone() {
	readAvail->V(); 
}

void
SynchConsole::WriteDone() {
	writeDone->V(); 
}
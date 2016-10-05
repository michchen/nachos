#include "prodconMonitor.h"
#include <string.h>

ProdconMonitor::ProdconMonitor(char *string)
{
	lock = new(std::nothrow) Lock("prodcon lock");
	full = new(std::nothrow) Condition("full");
	empty = new(std::nothrow) Condition("empty");
	emptySpace = 512;
	prodString = string;
	nextletter = 0;
	nextcon = 0;
	bufspot = 0;

}

ProdconMonitor::~ProdconMonitor()
{
	delete lock;
	delete full;
	delete empty;
}

void ProdconMonitor::Produce()
{
	lock->Acquire();
	while (emptySpace == 0) full->Wait(lock);
	//printf("%s\n", "in the producer");
	if(nextletter >= strlen(prodString)) {
		buffer[bufspot] = '!';
		nextletter++;
	}
	else {
		buffer[bufspot] = prodString[nextletter];
		nextletter++;
	}
	//printf("%s\n", "is this what I think it is");
	bufspot++;
	bufspot = bufspot % 512;
	emptySpace--;
	//printf("%s\n", "what the fuck");
	//Let all threads waiting know there is something in buff
	empty->Signal(lock);
	lock->Release();
}

char ProdconMonitor::GetStr() {
	return prodString[nextletter];
}

char ProdconMonitor::GetOutput() {
	return output;
}

void ProdconMonitor::Consume()
{
	lock->Acquire();
	while(emptySpace == 512) empty->Wait(lock);
	//printf("%s\n", "consuming?");
	output = buffer[nextcon];
	nextcon++;
	nextcon = nextcon % 512;
	emptySpace++;

	printf("%c\n", output);

	full->Signal(lock);
	lock->Release();
}
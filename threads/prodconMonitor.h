#ifndef PRODCONMONITOR_H
#define PRODCONMONITOR_H

#include "system.h"
#include "synch.h"

class ProdconMonitor {
	public:
		ProdconMonitor(char *string);
		~ProdconMonitor();

		void Produce();
		void Consume();
		char GetStr();
		char GetOutput();

	private:
		char buffer[512];
		int emptySpace;
		Condition *full;
		Condition *empty;
		Lock *lock;
		char *prodString;
		int nextletter;
		int nextcon;
		int bufspot;
		char output;

};

#endif 
//PRODCONMONITOR_H

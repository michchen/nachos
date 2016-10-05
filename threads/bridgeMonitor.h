#ifndef BRIDGEMONITOR_H
#define BRIDGEMONITOR_H

#include "system.h"
#include "synch.h"

class BridgeMonitor {
	public:
		BridgeMonitor();
		~BridgeMonitor();

		void ArriveBridge(int dir);
		void ExitBridge(int dir);
		int CrossBridge(int dir);
	private:
		int onBridge;
		int direction;
		Condition *north;
		Condition *south;
		Lock *lock;
		int northLine;
		int southLine;
};

#endif
//BRIDGEMONITOR_H
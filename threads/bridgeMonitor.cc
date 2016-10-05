#include "bridgeMonitor.h"

BridgeMonitor::BridgeMonitor(){
	onBridge = 0;
	direction = 0;
	north = new(std::nothrow) Condition("north");		// 0 is north
	south = new(std::nothrow) Condition("south");		// 1 is south
	lock = new(std::nothrow) Lock("bridge lock");
	northLine = 0;
	southLine = 0;
}

BridgeMonitor::~BridgeMonitor() {
	delete north;
	delete south;
}

void BridgeMonitor::ArriveBridge(int dir) 
{
	lock->Acquire();

	if (northLine == 0 && southLine == 0 && onBridge == 0){
		direction = dir;
	}
	
	if (dir == 0) {
		northLine++;
	}
	else {
		southLine++;
	}

	while ((direction != dir || onBridge == 3)) {
		if (dir == 0) {
			//northLine++;
			north->Wait(lock);
		}
		else {
			//southLine++;
			south->Wait(lock);
		}
	}

	

	printf("%s %d\n", "Car arriving from: ", dir );
	
	onBridge++;
	printf("%s %d\n","Number of cars on the bridge: ", onBridge );
	if (direction == 0) {
		northLine--;
		north->Broadcast(lock);
		
	}
	else{
		southLine--;
		south->Broadcast(lock);
		
	}
	lock->Release();
}

void BridgeMonitor::ExitBridge(int dir) 
{
	lock->Acquire();
	ASSERT(dir == direction);
	onBridge--;
	if(direction == 0) {
		if (onBridge == 0 && northLine == 0) {
			direction = 1;
			south->Broadcast(lock);
			printf("%s\n", "No more cars heading North, switching directions");
		}
		else {
			north->Broadcast(lock);
		}
	}
	else {
		if (onBridge == 0 && southLine == 0) {
			direction = 0;
			north->Broadcast(lock);
			printf("%s\n", "No more cars heading South, switching directions");
		}
		else {
			south->Broadcast(lock);
		}

	}
	printf("%s\n", "Car leaving");
	printf("%s %d\n", "Current direction", direction);
	lock->Release();
}

int BridgeMonitor::CrossBridge(int dir) 
{
	printf("%s %d\n", "Car current crossing from", dir);
	return dir;
}
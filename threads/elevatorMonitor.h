#ifndef ELEVATORMONITOR_H
#define ELEVATORMONITOR_H

#include "system.h"
#include "synch.h"
#include <string.h>
#include <vector>

struct floorInput {
		int atFloor;
		int toFloor;
} ;
class Floor{
	public:
		Floor(int floor);
		~Floor();

		Condition *upDirection;
		Condition *downDirection;

		int getFloor();
	private:
		int floor_number;
};

class ElevatorMonitor {
	public:
		ElevatorMonitor(int floors);
		~ElevatorMonitor();

		void changeFloors(int *currentfloor,int curdirection);
		void requestElevator(floorInput *input);
		void elevator();

	private:
		//List *allFloors;
		std::vector<Floor*> allFloors;
		Lock *lock;
		Lock *lock1;
		Lock *lock2;
		Lock *lock3;
		Lock *lock4;
		Lock *lock5;
		Condition *destinationReached;
		Condition *startElevator;
		Condition *peopleInElev;
		int direction;
		int current_floor;
		int max_floors;
		int min_floors;
		int peopleNum;
		int peoplewait;
};
#endif 
//ELEVATORMONITOR_H

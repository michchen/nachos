
#include "elevatorMonitor.h"

Floor::Floor(int floor)
{
	upDirection = new(std::nothrow) Condition("up");
	downDirection = new(std::nothrow) Condition("down");
	floor_number = floor;
}

Floor::~Floor()
{
	delete upDirection;
	delete downDirection;
}

int Floor::getFloor()
{
	return floor_number;
}

ElevatorMonitor::ElevatorMonitor(int floors)
{ 
	lock = new(std::nothrow) Lock("Elevator lock");
	lock1 = new(std::nothrow) Lock("Elevator lock1");
	lock2 = new(std::nothrow) Lock("Elevator lock2");
	lock3 = new(std::nothrow) Lock("Elevator lock3");
	lock4 = new(std::nothrow) Lock("Elevator lock4");
	lock5 = new(std::nothrow) Lock("Elevator lock5");

	for(int i = 0; i < floors; i++){

		Floor *floorCondition = new(std::nothrow) Floor(i+1);

		allFloors.push_back(floorCondition);

	}
	destinationReached = new(std::nothrow) Condition("destinationReached");
	startElevator = new(std::nothrow) Condition("startElevator");
	peopleInElev = new(std::nothrow) Condition("peopleInElev");
	max_floors = 0;
	min_floors = floors;
	direction = 1;
	current_floor = 1;
	peopleNum = 0;
	peoplewait = current_floor;
}

ElevatorMonitor::~ElevatorMonitor()
{
	delete lock;
	delete destinationReached;
}

void ElevatorMonitor::changeFloors(int *currentfloor,int curdirection)
{
	for(int i=0; i < 100; i++){
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}
	lock5->Acquire();
	if(curdirection == 1){
		*currentfloor += 1;
		if(*currentfloor == max_floors){
			direction = 0;
		}
	}
	else{
		*currentfloor -= 1;
		if(*currentfloor == min_floors){
			direction = 0;
		}
	}
	lock5->Release();
}

void ElevatorMonitor::requestElevator(floorInput *input)
{

	int atFloor = input->atFloor;
	int toFloor = input->toFloor;

	printf("*** Person on floor %d requesting the elevator to floor %d ***\n",atFloor,toFloor);

	Floor *currentFloorCond;
	
	currentFloorCond = allFloors.at(current_floor-1);

	ASSERT(currentFloorCond != NULL);
	int getDir = (atFloor > toFloor) ? 0 : 1;
	
	if(getDir){
		max_floors = (max_floors <= toFloor ? toFloor : max_floors);
	}
	else{
		min_floors = (min_floors >= toFloor ? toFloor : min_floors);
		max_floors = (max_floors <= atFloor ? atFloor : max_floors);
	}
	peopleNum++;

	while(current_floor != atFloor || getDir != direction){
		lock5->Acquire();
		if(peoplewait != current_floor){
			changeFloors(&current_floor,direction);
		}
		//printf("Current_floor: %d. PersonAtFloor: %d \n",current_floor,atFloor);
//		printf("%d getDir, %d direction\n", getDir,direction);

		startElevator->Signal(lock5);
		lock5->Release();
		//lock->Release();
		lock2->Acquire();
		peopleInElev->Wait(lock2);
		lock2->Release();
	}
	ASSERT(atFloor == current_floor);
	printf("=== Person at floor %d wants to get in elevator in floor %d === \n",atFloor,current_floor);

	while(getDir != direction){
		printf("$$$ Person at floor %d must wait in line $$$ \n",current_floor);
		peoplewait++;
		if(getDir){
			currentFloorCond->upDirection->Wait(lock);
		}
		else{
			currentFloorCond->downDirection->Wait(lock);
		}
		peoplewait--;

	}

	while(current_floor != toFloor){
		lock->Acquire();
		//printf("Waiting for Destination\n");
		startElevator->Signal(lock);
		lock->Release();
		lock1->Acquire();
		destinationReached->Wait(lock1);
		lock1->Release();
	}

	ASSERT(current_floor == toFloor);
	printf("^^^ Person who was at floor %d and wanted to go to floor %d reached floor %d and got off ^^^\n",atFloor,toFloor,current_floor);
	peopleNum--;

	lock->Acquire();
	startElevator->Signal(lock);
	lock->Release();


}

void ElevatorMonitor::elevator(){
	//lock->Acquire();
	while(1){
		ASSERT(peopleNum >= 0);
	
		if(peopleNum > 0){

			lock1->Acquire();
			destinationReached->Broadcast(lock1);
			lock1->Release();
			lock2->Acquire();
 			peopleInElev->Broadcast(lock2);
 			lock2->Release();
			Floor *currentFloorObj = allFloors.at(current_floor-1);
			for(int i=0; i < 100; i++){
				interrupt->SetLevel(IntOff);
				interrupt->SetLevel(IntOn);
			}
			
			lock5->Acquire();
			if(direction == 1){
				current_floor += 1;
				if(current_floor == 5){
					direction = 0;
				}
			}
			else{
				current_floor -= 1;
				if(current_floor == 1){
					direction = 1;
				}
			}	
			printf("Moved Floors to %d\n",current_floor);
			peoplewait = current_floor;

			lock5->Release();
			lock->Acquire();
			startElevator->Wait(lock);
			lock->Release();
			//printf("Notify Line Queues\n");
			if(direction)
			{
				if(current_floor == max_floors){

					lock->Acquire();
					currentFloorObj->downDirection->Broadcast(lock); 
					lock->Release();
				}
				else
				{
	
					lock->Acquire();
					currentFloorObj->upDirection->Broadcast(lock);
					lock->Release();
				}
			}
			else
			{

				if(current_floor == min_floors){

					lock->Acquire();
					currentFloorObj->upDirection->Broadcast(lock); 
					lock->Release();
				}
				else
				{

					lock->Acquire();
					currentFloorObj->downDirection->Broadcast(lock);
					lock->Release();
				}
			}
		}
		else{
			printf("Elevator Waiting\n");
			lock->Acquire();
			startElevator->Wait(lock);
			lock->Release();
		}
	}
}
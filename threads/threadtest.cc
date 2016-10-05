// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "elevatorMonitor.h"
#include "prodconMonitor.h"
#include "bridgeMonitor.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

BridgeMonitor *bridge;
ProdconMonitor *prodcon;
ElevatorMonitor* trackElevator;

Lock *lock = new(std::nothrow) Lock("prodcon lock");
Condition *full = new(std::nothrow) Condition("full");
Condition *empty = new(std::nothrow) Condition("empty");
char buffer[512];
int emptySpace = 512;
int nextcon = 0;
int bufspot = 0;

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        //printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}
//-----------------------------------------------------------------------
/* 
    Producer and Consumer Threads for Question 2

The producer places characters from the string ”Hello world” into the buffer one character at a time;
it must wait if the buffer is full. The consumer pulls characters out of the buffer one at a time and
prints them to the screen; it must wait if the buffer is empty. Test your solution with a multi-character
buffer and with multiple producers and consumers. Of course, with multiple producers or consumers,
the output display will be gibberish.

*/
//-----------------------------------------------------------------------
// void producerThread(int which)
// {
// 	DEBUG('t', 'Creating Producer Thread\n');
// }

// void consumerThread()
// {
//     DEBUG('t', 'Creating Consumer Thread\n');
// }

// void PCTest(char *string)
// {
    
//     DEBUG('t', 'Preforming producer and consumer testing\n');
//     Thread *t = new(std::nothrow) Thread("forked thread");
    
//     t->Fork(producerThread);
//     t->Fork(consumerThread);

// }

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ProdconProcess(int arg)
{
	int i, delay;

	delay = 1+(int) (100000.0 * rand()/(RAND_MAX+1.0));


	char *prodString = (char *)"Hello World";
	

	for (i=0; i<delay; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}


	while (*prodString != '\0'){
		lock->Acquire();
		while (emptySpace == 0) full->Wait(lock);
		DEBUG('t', "A Producer is inserting a letter into the buffer\n");

		buffer[bufspot] = *prodString++;
		
		bufspot++;
		bufspot = bufspot % 512;
		emptySpace--;
		//Let all threads waiting know there is something in buff
		empty->Broadcast(lock);
		lock->Release();

		currentThread->Yield();
	}
	printf("%s\n", "A Producer has finished");

}

void
Prodconsume(int arg)
{
	int i, delay;

	delay = 1+(int) (100000.0 * rand()/(RAND_MAX+1.0));

	char output;

	for (i=0; i<delay; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);

	}



	while(nextcon < bufspot){
		lock->Acquire();
		while(emptySpace == 512) empty->Wait(lock);
		
		output = buffer[nextcon];
		buffer[nextcon] = '\0';
		nextcon++;
		nextcon = nextcon % 512;
		emptySpace++;
		DEBUG('t', "A Consumer is consuming a letter\n" );
		printf("%c\n", output);

		full->Broadcast(lock);
		lock->Release();
		currentThread->Yield();
	}
	printf("%s\n", "A Consumer has finished");
	
}

void
headNorth(int arg){
	int i, delay, hold;

	delay = 1+(int) (100000.0 * rand()/(RAND_MAX+1.0));

	hold = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));

	for (i=0; i<delay; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}

	bridge->ArriveBridge(0);
	currentThread->Yield();

	bridge->CrossBridge(0);
	currentThread->Yield();

	for (i=0; i<hold; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}

	bridge->ExitBridge(0);
}

void
headSouth(int arg){
	int i, delay, hold;

	delay = 1+(int) (100000.0 * rand()/(RAND_MAX+1.0));

	hold = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));

	for (i=0; i<delay; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}

	bridge->ArriveBridge(1);
	currentThread->Yield();

	bridge->CrossBridge(1);
	currentThread->Yield();

	for (i=0; i<hold; i++) {
		interrupt->SetLevel(IntOff);
		interrupt->SetLevel(IntOn);
	}

	bridge->ExitBridge(1);
}

void ElevatorProcess(int args){
    if(args != 1){
        floorInput input;
        input.atFloor = rand() % 5 + 1;;
        input.toFloor = rand() % 5 + 1;
        int delay = 1+(int) (1000000.0*rand()/(RAND_MAX+1.0));

        for (int i=0; i<delay; i++) {
            interrupt->SetLevel(IntOff);
            interrupt->SetLevel(IntOn);
        }
        trackElevator->requestElevator(&input);
        currentThread->Yield();
    }
    else{
        trackElevator->elevator();
        currentThread->Yield(); 
    }

}

void
ThreadTest(int option)
{
    if(option == 1)
    {
		trackElevator = new(std::nothrow) ElevatorMonitor(5);
        Thread *t;
        DEBUG('t', "Entering Elevator Test");
        for(int i = 1 ; i < 5 ; i++){
           t = new(std::nothrow) Thread("forked thread");
           t->Fork(ElevatorProcess,i);
        }
        ElevatorProcess(0);

    }

    if(option == 2)
    {
    	DEBUG('t', "Entering prodcon\n");
    	
    	prodcon = new ProdconMonitor((char *)"Hello World");

    	Thread *producers[5];
    	Thread *consumers[3];
    	
    	for (int i = 0; i < 5; i++) 
    	{
    		producers[i] = new(std::nothrow) Thread("producer");
    		producers[i]->Fork(ProdconProcess, i);
    		
    	}

    	for (int k = 0; k < 3; k++)
    	{
    		consumers[k] = new(std::nothrow) Thread("consumer");
    		consumers[k]->Fork(Prodconsume, k);
    		
    	}

    }

    if (option == 3)
    {
    	DEBUG('t', "Entering bridge\n");
    	bridge = new BridgeMonitor();

    	Thread *northbound[10];
    	Thread *southbound[10];

    	for (int i = 0; i < 10; i++)
    	{
    		northbound[i] = new(std::nothrow) Thread("northbound");
    		northbound[i]->Fork(headNorth, i);
    		printf("%s\n","Forking the north" );
    	}

    	for (int k = 0; k < 10; k++)
    	{
    		southbound[k] = new(std::nothrow) Thread("southbound");
    		southbound[k]->Fork(headSouth, k);
    		printf("%s\n", "Forking the south");
    	}
    }
    else{
        DEBUG('t', "Entering SimpleTest");
        
        Thread *t = new(std::nothrow) Thread("forked thread");

        t->Fork(SimpleThread, 1);
        SimpleThread(0);
    }
   


}




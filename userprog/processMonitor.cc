#include "processMonitor.h"

ProcessMonitor::ProcessMonitor(){
	activeThreads = new ThreadBlocks *[MAX_THREAD_COUNT]{};
	waitingThreads = new List();
	monitorLock = new(std::nothrow) Lock("process monitor Lock");
	monitorSemaphore = new(std::nothrow) Semaphore("process Semaphore");
	totalThreads = 0;
	exitStatus = -5;
}
void ProcessMonitor::setExitStatus(int threadID, int exitStatus){
	if(activeThreads[threadID] != NULL){
		ThreadBlocks *temp = activeThreads[threadID];
		temp->exitStatus = exitStatus;
	}
	else{
		DEBUG('a', 'Thread Does not exist!');
	}
}
ProcessMonitor::~ProcessMonitor(){
	delete activeThreads;
	delete monitorLock;
	delete monitorSemaphore;
}

int ProcessMonitor::assignId(){
	for(int i = 0; i < MAX_THREAD_COUNT; i++){
		if(activeThreads[i] == NULL)
			return i;
	}
	DEBUG('a', "Too many active threads unable to assign id");
	return -1;
}

int ProcessMonitor::addThread(Thread *thread){
	int spaceID = assignId();
	if(spaceID == -1){
		waitingThreads->Append(thread);
		return -1;
	}
	else{
		thread->setThreadId(spaceID);
		ThreadBlocks *newBlock;
		newBlock->threadId = thread->getThreadID();
		newBlock->parentId = currentThread->getThreadID();
		newBlock->done = false;
		newBlock->lock = new(std::nothrow) Lock("new thread fork lock");
		newBlock->semaphore = new(std::nothrow) Semaphore("new semaphore fork lock");
		activeThreads[spaceID] = newBlock;
		totalThreads++;
		return spaceID;
	}
}

void ProcessMonitor::removeThread(Thread *thread){
	if(thread != NULL){
		totalThreads--;
		int threadID = thread->getThreadID();
		delete activeThreads[threadID];
		activeThreads[threadID] = NULL;
		delete thread;
	}
	else{
		DEBUG('a', "Thread is null. Unable to delete");
	}
}

void ProcessMonitor::lock(){
	DEBUG('a', "Locking Monitor Lock");
}

void ProcessMonitor::unlock(){
	DEBUG('a', "Unlocking Monitor Lock");
}
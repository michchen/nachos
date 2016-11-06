#include "processMonitor.h"

#ifdef CHANGED
ProcessMonitor::ProcessMonitor(){
	activeThreads = new ThreadBlocks *[MAX_THREAD_COUNT]();
	waitingThreads = new List();
	monitorLock = new(std::nothrow) Lock("process monitor Lock");
	monitorSemaphore = new(std::nothrow) Semaphore("process Semaphore",0);
	totalThreads = 0;
}
int ProcessMonitor::setExitStatus(int threadID, int exitStatus){
	DEBUG('e',"ThreadID in setExitStatus in Thread %d and exitStatus is %d\n", threadID, exitStatus);
	if(threadID != -1 && activeThreads[threadID] != NULL){
		activeThreads[threadID]->exitStatus = exitStatus;
		activeThreads[threadID]->done = true;
		DEBUG('e', "Thread exists and setting to exit!\n");
		return 1;
	}
	else{
		DEBUG('e', "Thread Does not exist! (setExitStatus)\n");
		return -1;
	}
}
int ProcessMonitor::getExitStatus(int threadID){
	DEBUG('e',"ThreadID in getExitStatus %d\n", threadID);
	if(threadID != -1 && activeThreads[threadID] != NULL){
		return activeThreads[threadID]->exitStatus;
	}
	else{
		DEBUG('e', "Thread Does not exist! (getExitStatus)\n");
		return -1;
	}
}
ProcessMonitor::~ProcessMonitor(){
	delete activeThreads;
	delete monitorLock;
	delete monitorSemaphore;
}

int ProcessMonitor::assignID(){
	for(int i = 0; i < MAX_THREAD_COUNT; i++){
		if(activeThreads[i] == NULL)
			return i;
	}
	DEBUG('e', "Too many active threads unable to assign id\n");
	return -1;
}

int ProcessMonitor::addThread(Thread *thread,Thread *parent){
	int spaceID = ProcessMonitor::assignID();
	//printf("SpaceID in addThread %d\n", spaceID);
	if(spaceID == -1){
		waitingThreads->Append(thread);
		return -1;
	}
	else{
		thread->setThreadId(spaceID);
		ThreadBlocks *newBlock = new ThreadBlocks;
		newBlock->threadId = thread->getThreadId();
		newBlock->parentId = parent->getThreadId();
		newBlock->done = false;
		newBlock->lock = new(std::nothrow) Lock("new thread fork lock");
		newBlock->semaphore = new(std::nothrow) Semaphore("new semaphore fork lock",0);
		activeThreads[spaceID] = newBlock;
		totalThreads++;
		return spaceID;
	}
}

void ProcessMonitor::removeThread(int threadID){
	if(threadID > -1){
		totalThreads--;
		activeThreads[threadID] = NULL;
	}
	else{
		DEBUG('e', "This is the root thread. Unable to delete\n");
	}
}

bool ProcessMonitor::containsThread(int threadID){
	DEBUG('e',"ThreadID in containsThread %d\n", threadID);
	if(threadID > MAX_THREAD_COUNT || threadID < 0)
		return false;
	if(activeThreads[threadID] != NULL)
		return true;
	return false;
}
void ProcessMonitor::wakeParent(int threadID){
	DEBUG('e',"waking Parent of thread %d\n", threadID);
	ASSERT(threadID >= 0 && threadID < MAX_THREAD_COUNT);
	if(activeThreads[threadID] != NULL){
		if(activeThreads[threadID]->threadId != activeThreads[threadID]->parentId){
			threadID = activeThreads[threadID]->parentId;
			//activeThreads[threadID]->semaphore->V();
		}
		DEBUG('e',"wake thread -> %d\n", threadID);
		activeThreads[threadID]->semaphore->V();
	}
}
void ProcessMonitor::sleepParent(int threadID){
	DEBUG('e',"sleeping Parent of thread %d\n", threadID);
	ASSERT(threadID >= 0 && threadID < MAX_THREAD_COUNT);
	if(activeThreads[threadID] != NULL){
		if(activeThreads[threadID]->threadId != activeThreads[threadID]->parentId){
			threadID = activeThreads[threadID]->parentId;
			//activeThreads[threadID]->semaphore->V();
		}
		DEBUG('e',"sleep thread -> %d\n", threadID);
		activeThreads[threadID]->semaphore->P();
	}
}
void ProcessMonitor::lock(){
	DEBUG('e', "Locking Monitor Lock\n");
	monitorLock->Acquire();
}

void ProcessMonitor::unlock(){
	DEBUG('e', "Unlocking Monitor Lock\n");
	monitorLock->Release();
}
void ProcessMonitor::lockThreadBlock(int threadID){
	if(activeThreads[threadID-1] != NULL){
		activeThreads[threadID-1]->lock->Acquire();	
	}

}
void ProcessMonitor::unlockThreadBlock(int threadID){
	if(activeThreads[threadID-1] != NULL){
		activeThreads[threadID-1]->lock->Release();
	}

}
void ProcessMonitor::cleanUpDeadThreads(int threadID){
	for(int i = 0; i < MAX_THREAD_COUNT; i++){
		ThreadBlocks *temp = activeThreads[i];
		if(temp != NULL && temp->parentId == threadID && temp->done){
			DEBUG('e', "Removing Dead Threads\n");
			removeThread(i);
		}
	}
}

#endif
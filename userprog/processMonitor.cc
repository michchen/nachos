#include "processMonitor.h"

ProcessMonitor::ProcessMonitor(){
	activeThreads = new ThreadBlocks *[MAX_THREAD_COUNT]();
	waitingThreads = new List();
	monitorLock = new(std::nothrow) Lock("process monitor Lock");
	monitorSemaphore = new(std::nothrow) Semaphore("process Semaphore",0);
	totalThreads = 0;
}
int ProcessMonitor::setExitStatus(int threadID, int exitStatus){
	if(threadID != -1 && activeThreads[threadID-1] != NULL){
		activeThreads[threadID-1]->exitStatus = exitStatus;
		activeThreads[threadID-1]->done = true;
		return 1;
	}
	else{
		DEBUG('a', "Thread Does not exist!");
		return -1;
	}
}
int ProcessMonitor::getExitStatus(int threadID){
	//printf("ThreadID in getExitStatus %d\n", threadID);
	if(threadID != -1 && activeThreads[threadID-1] != NULL){
		return activeThreads[threadID-1]->exitStatus;
	}
	else{
		DEBUG('a', "Thread Does not exist!");
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
			return i+1;
	}
	DEBUG('a', "Too many active threads unable to assign id");
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
		activeThreads[spaceID-1] = newBlock;
		totalThreads++;
		return spaceID;
	}
}

void ProcessMonitor::removeThread(int threadID){
	if(threadID != -1){
		totalThreads--;
		activeThreads[threadID-1] = NULL;
	}
	else{
		DEBUG('a', "This is the root thread. Unable to delete");
	}
}

bool ProcessMonitor::containsThread(int threadID){
	//printf("ThreadID in containsThread %d\n", threadID);
	if(threadID > MAX_THREAD_COUNT || threadID < 0)
		return false;
	if(activeThreads[threadID-1] != NULL)
		return true;
	return false;
}
void ProcessMonitor::wakeParent(int threadID){
	ASSERT(threadID >= -1 && threadID < MAX_THREAD_COUNT);
	if(activeThreads[threadID-1] != NULL){
		activeThreads[threadID-1]->semaphore->V();
	}
}
void ProcessMonitor::sleepParent(int threadID){
	ASSERT(threadID >= -1 && threadID < MAX_THREAD_COUNT);
	if(activeThreads[threadID-1] != NULL){
		activeThreads[threadID-1]->semaphore->P();
	}
}
void ProcessMonitor::lock(){
	DEBUG('a', "Locking Monitor Lock\n");
	monitorLock->Acquire();
}

void ProcessMonitor::unlock(){
	DEBUG('a', "Unlocking Monitor Lock\n");
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
			removeThread(i+1);
		}
	}
}
#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include "synch.h"
#include "syscall.h"

#define MAX_THREAD_COUNT 100

struct ThreadBlocks{
	Thread *thread;
	int threadId;
	int parentId;
	int exitStatus;
	bool done;
	Lock *lock;
	Semaphore *semaphore;
};
struct OpenFileBlocks{
	OpenFileId fileID;
};
class ProcessMonitor{
	public:
		ProcessMonitor();
		~ProcessMonitor();
		int addThread(Thread *thread,Thread *parent);
		void lock();
		void unlock();
		void lockThreadBlock(int threadID);
		void unlockThreadBlock(int threadID);
		void removeThread(int threadID);
		int getTotalThreads(){return totalThreads;};
		int setExitStatus(int threadID, int exitStatus);
		int assignID();
		bool containsThread(int threadID);
		void wakeParent(int threadID);
		void sleepParent(int threadID);
		int getExitStatus(int threadID);
		Thread *getThread(int threadID){ return activeThreads[threadID]->thread;};
		bool isDeadThread(int threadID){ return activeThreads[threadID]->done;};
		void cleanUpDeadThreads(int threadID);
		Lock *monitorLock;
		Semaphore *monitorSemaphore;

	private:
		 ThreadBlocks **activeThreads;
		 OpenFileBlocks **OpenFiles;
		 int totalThreads;
		 List *waitingThreads;
};

#endif //PROCESS_MONITOR_H

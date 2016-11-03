#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include "synch.h"

#define MAX_THREAD_COUNT 100

struct ThreadBlocks{
	int threadId;
	int parentId;
	int exitStatus;
	bool done;
	Lock *lock;
	Semaphore *semaphore;
};
class ProcessMonitor{
	public:
		ProcessMonitor();
		~ProcessMonitor();
		int addThread(Thread *thread,Thread *parent);
		void lock();
		void unlock();
		void removeThread(Thread *thread);
		int getTotalThreads(){return totalThreads;};
		int setExitStatus(int threadID, int exitStatus);
		int assignID();
		bool containsThread(int threadID);
		void wakeParent(int threadID);
		void sleepParent(int threadID);
		int getExitStatus(int threadID);
		Lock *monitorLock;
		Semaphore *monitorSemaphore;

	private:
		 ThreadBlocks **activeThreads;
		 int totalThreads;
		 List *waitingThreads;
};

#endif //PROCESS_MONITOR_H

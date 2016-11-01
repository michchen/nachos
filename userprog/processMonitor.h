
#ifdef PROCESS_MONITOR
#define PROCESS_MONITOR

#include "synch.h"

#define MAX_THREAD_COUNT 100;

struct ThreadBlocks{
	int threadId;
	int parentId;
	bool done;
	Lock *lock;
	Semaphore *semaphore;
};

class ProcessMonitor{
	public:
		processMonitor();
		~processMonitor();
		int addThread(Thread *thread);
		void removeThread(Thread *thread);
		int getTotalThreads(){return totalThreads;};

		Lock *monitorLock;
		Semaphore *monitorSemaphore;

	private:
		ThreadBlocks **activeThreads;
		int totalThreads;
		List *waitingThreads;
		List *exitThreads;
};

#endif 
#ifdef PROCESS_MONITOR
#define PROCESS_MONITOR
#ifdef CHANGED

#include "synch.h"

#define MAX_THREAD_COUNT 100;
struct ThreadBlocks{
	int threadId;
	int parentId;
	int exitStatus
	bool done;
	Lock *lock;
	Semaphore *semaphore;
};
class ProcessMonitor{
	public:
		processMonitor();
		~processMonitor();
		int addThread(Thread *thread);
		void lock();
		void unlock();
		void removeThread(Thread *thread);
		int getTotalThreads(){return totalThreads;};
		void setExitStatus(int threadID, int exitStatus);
		Lock *monitorLock;
		Semaphore *monitorSemaphore;

	private:
		static int assignID();
		static ThreadBlocks **activeThreads;
		static int totalThreads;
		static List *waitingThreads;
};

#endif //PROCESS_MONITOR
#endif
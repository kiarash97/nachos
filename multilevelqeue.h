#ifndef THREADS_multilevelSCHEDULER_H_
#define THREADS_multilevelSCHEDULER_H_

#include "scheduler.h"
#include "system.h"
#include "list.h"

class multilevelScheduler:public Scheduler {
public:
	multilevelScheduler();
	virtual ~multilevelScheduler();
    void ReadyToRunPQ(Thread* thread);
    void ReadyToRunSJF(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
	void Run(Thread* nextThread);
private:
    List *multilevelPQ;
    List *multilevelSJF;
};

#endif /* THREADS_SJFSCHEDULER_H_ */

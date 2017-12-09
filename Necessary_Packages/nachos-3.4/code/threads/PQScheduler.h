#ifndef THREADS_PQSCHEDULER_H_
#define THREADS_PQSCHEDULER_H_

#include "scheduler.h"

class PQScheduler:public Scheduler
{
public:

	PQScheduler();
	virtual ~PQScheduler();
	void ReadyToRun(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
private:
	List *PQreadyList;
};

#endif /* THREADS_PQSCHEDULER_H_ */
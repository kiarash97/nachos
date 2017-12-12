/*
 * PQScheduler.h
 *
 *  Created on: Dec 9, 2017
 *      Author: kiarash
 */

#ifndef THREADS_PQSCHEDULER_H_
#define THREADS_PQSCHEDULER_H_

#include "scheduler.h"
#include "system.h"

class PQScheduler:public Scheduler
{
public:

	PQScheduler();
	virtual ~PQScheduler();
	void ReadyToRun(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
	void Run(Thread* nextThread);
private:
	List *PQreadyList;
};

#endif /* THREADS_PQSCHEDULER_H_ */

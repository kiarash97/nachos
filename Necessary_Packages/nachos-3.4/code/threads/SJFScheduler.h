/*
 * SJFScheduler.h
 *
 *  Created on: Dec 9, 2017
 *      Author: kiarash
 */

#ifndef THREADS_SJFSCHEDULER_H_
#define THREADS_SJFSCHEDULER_H_

#include "scheduler.h"
#include "system.h"

class SJFScheduler:public Scheduler {
public:
	SJFScheduler();
	virtual ~SJFScheduler();
	void ReadyToRun(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
	void Run(Thread* nextThread);
private:
	List *SJFreadyList;
};

#endif /* THREADS_SJFSCHEDULER_H_ */

/*
 * MLTQScheduler.h
 *
 *  Created on: Dec 12, 2017
 *      Author: kiarash
 */

#ifndef THREADS_MLTQSCHEDULER_H_
#define THREADS_MLTQSCHEDULER_H_

#include "scheduler.h"
#include "system.h"

class MLTQScheduler:public Scheduler {
public:
	MLTQScheduler();
	virtual ~MLTQScheduler();
	void ReadyToRun(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
	void Run(Thread* nextThread);
private:
    List *multilevelPQ;
    List *multilevelSJF;
};

#endif /* THREADS_MLTQSCHEDULER_H_ */

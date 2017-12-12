#ifndef THREADS_multilevelqeue_H_
#define THREADS_multilevelqeue_H_

#include "scheduler.h"
#include "system.h"
#include "list.h"

class multilevelqeue:public Scheduler {
public:
	multilevelqeue();
	virtual ~multilevelqeue();
    void ReadyToRun(Thread* thread);
    //void ReadyToRunSJF(Thread* thread);	// Thread can be dispatched.
	Thread* FindNextToRun();		// Dequeue first thread on the ready
	void Run(Thread* nextThread);
private:
    List *multilevelPQ;
    List *multilevelSJF;
};

#endif /* THREADS_SJFSCHEDULER_H_ */

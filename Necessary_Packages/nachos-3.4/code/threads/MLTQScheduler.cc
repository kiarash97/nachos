/*
 * MLTQScheduler.cc
 *
 *  Created on: Dec 12, 2017
 *      Author: kiarash
 */

#include "MLTQScheduler.h"
#include <sys/time.h>

MLTQScheduler::MLTQScheduler() {
	// make two list for SJF and PQ
	// ******  SJF list has more priority than PQ *******
	multilevelSJF = new List;
	multilevelPQ = new List;
}

MLTQScheduler::~MLTQScheduler() {
	// TODO Auto-generated destructor stub
	delete multilevelSJF;
	delete multilevelPQ;
}

Thread* MLTQScheduler::FindNextToRun(){
    if (!(multilevelSJF->IsEmpty())){
        return (Thread*)multilevelSJF->SortedRemove(NULL);
    }
    else
        return (Thread*)multilevelPQ->SortedRemove(NULL);
}

void MLTQScheduler::ReadyToRun(Thread* thread){
	DEBUG('t', "Putting thread %s on ready list.\n", thread->getName());
	thread->setStatus(READY);
	if (thread->priority==-1)//if thread in SJF queue
		multilevelSJF->SortedInsert((void*) thread,thread->rettimejobdone());
	else //if thread in PQ
		multilevelPQ->SortedInsert((void*) thread,thread->priority);
}

void MLTQScheduler::Run(Thread* nextThread){
	Thread *oldThread = currentThread;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned long long time_in_micros = 1000000ll * tv.tv_sec + tv.tv_usec;//
																//get time in micro second
	oldThread->finishTime=time_in_micros;//set finish time for old thread
	oldThread->settimejobdone(oldThread->finishTime-oldThread->startTime);//set the time needed
																	//for thread to done

	#ifdef USER_PROGRAM			// ignore until running user programs
	    if (currentThread->space != NULL) {	// if this thread is a user program,
	        currentThread->SaveUserState(); // save the user's CPU registers
		currentThread->space->SaveState();
	    }
	#endif

	oldThread->CheckOverflow();		    // check if the old thread
						// had an undetected stack overflow

	currentThread = nextThread;		    // switch to the next thread
	currentThread->setStatus(RUNNING);

	// nextThread is now running
	DEBUG('t', "Switching from thread \"%s\" to thread \"%s\"\n",
	  oldThread->getName(), nextThread->getName());

	// This is a machine-dependent assembly language routine defined
	// in switch.s.  You may have to think
	// a bit to figure out what happens after this, both from the point
	// of view of the thread and from the perspective of the "outside world".
	SWITCH(oldThread, nextThread);
	DEBUG('t', "Now in thread \"%s\"\n", currentThread->getName());
	struct timeval tvv;
	gettimeofday(&tvv,NULL);
	unsigned long long time_in_micross = 1000000ll * tvv.tv_sec + tvv.tv_usec;//get time in micro second
	currentThread->startTime=time_in_micross;//set the start time for current thread
	// If the old thread gave up the processor because it was finishing,
	// we need to delete its carcass.  Note we cannot delete the thread
	// before now (for example, in Thread::Finish()), because up to this
	// point, we were still running on the old thread's stack!
	if (threadToBeDestroyed != NULL) {
		delete threadToBeDestroyed;
		threadToBeDestroyed = NULL;
	}

	#ifdef USER_PROGRAM
	    if (currentThread->space != NULL) {		// if there is an address space
	        currentThread->RestoreUserState();     // to restore, do it.
		currentThread->space->RestoreState();
	    }
	#endif
}


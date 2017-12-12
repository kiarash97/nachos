/*
 * PQScheduler.cpp
 *
 *  Created on: Dec 9, 2017
 *      Author: kiarash
 */

#include "PQScheduler.h"

PQScheduler::PQScheduler() {
	// TODO Auto-generated constructor stub
	PQreadyList = new List;
}

PQScheduler::~PQScheduler() {
	// TODO Auto-generated destructor stub
	delete PQreadyList;
}

Thread* PQScheduler::FindNextToRun(){
	return (Thread*)PQreadyList->SortedRemove(NULL);
}

void PQScheduler::ReadyToRun(Thread* thread){
	DEBUG('t', "Putting thread %s on ready list.\n", thread->getName());
	thread->setStatus(READY);
	PQreadyList->SortedInsert((void*) thread,thread->priority); //inserted by priority
}

void PQScheduler::Run (Thread *nextThread)
{
    Thread *oldThread = currentThread;

#ifdef USER_PROGRAM			// ignore until running user programs
    if (currentThread->space != NULL) {	// if this thread is a user program,
        currentThread->SaveUserState(); // save the user's CPU registers
	currentThread->space->SaveState();
    }
#endif

    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    currentThread = nextThread;		    // switch to the next thread
    currentThread->setStatus(RUNNING);      // nextThread is now running

    DEBUG('t', "Switching from thread \"%s\" to thread \"%s\"\n",
	  oldThread->getName(), nextThread->getName());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);

    DEBUG('t', "Now in thread \"%s\"\n", currentThread->getName());

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


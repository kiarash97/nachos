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
	return (Thread*)PQreadyList->Remove();
}

void PQScheduler::ReadyToRun(Thread* thread){
	DEBUG('t', "Putting thread %s on ready list.\n", thread->getName());
	thread->setStatus(READY);
	PQreadyList->SortedInsert((void*) thread,thread->priority);
}
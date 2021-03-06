// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "../threads/system.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void HandleFork();
void HandleJoin();
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which==SyscallException)
    {
        switch(type){
            case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n\n");
               	interrupt->Halt();
                break;

            case SC_Fork:
                HandleFork();
                break;

            case SC_Exit:
                printf ("thread %d finished\n \n",currentThread->threadID);
                currentThread->Finish();
                machine->IncrementPCReg();
                break;
            case SC_Join:
                HandleJoin();
                break;
        }
    }
    else
    {
	    printf("Unexpected user mode exception %d %d\n", which, type);
	    ASSERT(FALSE);
    }
}

void TestFunction(int bullShit)
{
    printf("i'm forked thread %d \n\n",currentThread->threadID);
    #ifdef USER_PROGRAM
    if (currentThread->space != NULL) {		// if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
	    currentThread->space->RestoreState();
    }
    #endif
    machine->WriteRegister(2,0);
    machine->IncrementPCReg();
    machine->Run();
}

void HandleFork()
{
    currentThread->SaveUserState();
    AddrSpace* newAddressSpace = new AddrSpace(currentThread->space);

    Thread* newThread = new Thread(currentThread,"forked");
    newThread->space = newAddressSpace;

    machine->WriteRegister(2,newThread->threadID);
    machine->IncrementPCReg();

    newThread->Fork(TestFunction,0);
}


bool NotFinished(int childID)
{
    List* RunningList = scheduler->getReadyList();
    ListElement* element = RunningList->first;
    Thread* thread;
    while (element != NULL)
    {
        thread = (Thread*) element->item;
        element = element->next;
        if (thread->threadID == childID)
        return true;
    }
    return false;
}

void HandleJoin()
{
    
    int childID = machine->ReadRegister(4);
    while (NotFinished(childID)) currentThread->Yield();
    machine->IncrementPCReg();
}

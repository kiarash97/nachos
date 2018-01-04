In this phase we implement fork & join & exit system calls. 


First we need to correct the address space for each thread (physical and virtual pages were the same) we add a FreePhysicalPages stack to Machine.cc & Machine.h and whenever a thread want to have some physical page it Pops from at and when finish its job it pushes it back to stack.


We add a threadID and parentID for each thread and an id generator to Machine.cc & Machine.h.


Now we can start to explain the implementation of fork system call. For this we add a swich case that each system call is a case in it. To implement fork we have a forkHandler function that save the state of the current thread and create a new thread with the exact address space of the current thread, and when we are in the child thread it return 0 and when we are in parent thread it return the threadID of the child(system calls return things by writing them in register number2) Then we increment the pc register to avoid of running the same system call for ever and ever again! now we run the machine by Machine->Run().


Now we want to explain the implementation of the join system call. We have a joinHandler function for this one. This system call has a parameter that is the threadID of the thread that it want to wait for (system call get an argument by reading from register number4). Now that we now which thread we want to wait for we get the readyList from scheduler and search in it for the threadID we want to wait for, if it was still in the readyList we yield the current thread else we know the thread we were waiting for was finished so we continue in the current thread. (we have to increment the pc register to for the same reason)


For exit system call we have a exitHandler. This system call has a parameter too but we don't use it much (just for printing).
In the exitHandler we just finish the current thread and increment the pc register like before.


For testing the result we write two main() (one is comment) in the halt.c, One for fork and one for join system call.

In the first one (testing fork) we just call fork system call and it returns us an integer value that if it was equal to zero that means we are in the child thread else means we are in the parent thread. Result was that, thread0 created and finished then thread1 created and finished.


In the second one (testing join) first we call a fork system call then, if we were in the child process we do sth and exit ,else we call join for the childID. Result for that was, thread0 created then thread1 (the child) CREATED and FINISHED at the end thread0 finished.


The results were satisfying and we are happy ! :)

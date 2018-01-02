/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

// #include "syscall.h"
// int main()
// {
//     //print ("hi i'm %d",currentThread->threadID);
//     Fork();
//     Fork();
//     Exit(6);
//     /* not reached */
// }


/* joinTest.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that fork The Process.
 *  Then join parent Process to Child So Parent Freezes until Child ends
 * 
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int main()
{
    int pid = Fork();
    if (pid == 0)
    {
        int a = 2;
        int b = 3;
        int c = a+b;
        Exit(c);
    }
    else
    {
        Join(pid);
        Exit(100);
    }
    /* not reached */
}


/* forkTest.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that fork The Process.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

// #include "syscall.h"

// int
// main()
// {
//     int pid = Fork();
//     if (pid == 0)
//     {
//         Exit(pid);
//     }
//     else
//     {
//         Exit(pid);
//     }
//     /* not reached */
// }
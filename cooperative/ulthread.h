/******************************************************
 * Author: Victor Santos
 * Last edited: 05/19/2014  MM/DD/YYYY
 * email: victor.ifsc@gmail.com
 *
 * This file is a header file where are implemented the prototype functions
 * of the linux thread scheduler being developped on this folder.
 *
 * ULState is a enum of the thread state:
 *      RUNNING: Thread running at the moment;
 *      READY: Thread on the scheduler list named "ready", i.e threads ready to run;
 *      WAITING: Thread waiting for some event to finish, i.e sensor result, read operation, or something else;
 *
 * ULThread is a structure of a thread itself.
 *      tid: Thread id, the number which identifies the thread as soons it is created;
 *      context: Thread context, makes use of the ucontext_t structure in ucontext.h. Here
 *               the Thread context is manipulated using getcontext() and makecontext();
 *      state: Thread state: READY, RUNNING or WAITING;
 *      next: pointer to the next thread in the linked list (a.k.a scheduler)
 *      prev: pointer to the previous thread in the link list.
 *
 * ULList is a handcrafted linked list where there are some implemented method to modify its content.
 * It is like the core of the scheduler system.
 *      head: pointer to the head of the list (first element)
 *      tail: pointer to the tail of the list (last element)
 *      size: number of the list elements
 *
 * ULScheduler is the thread scheduler, basically it puts a thread to run and modify the linked list to reinsert 
 * the done thread into the list tail.
 *      running: the running task at the momment
 *      ready: list of ready tasks
 *      last_id: take care of the id number when a new thread is created
 *
 */


#ifndef __ulthread_h
#define __ulthread_h

#define MAX_THREADS 50

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

/* task state definition */
typedef enum ULState {
    RUNNING,
    READY,
    WAITING,
    FINISHING
} ULState;

typedef enum ULPriority {
    THREAD_PRIORITY_TIME_CRITICAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_LOWEST
} ULPriority;

//resolve o problema de dependencia circular
//forward declaration
struct ULThread;

/* linked list structure */
typedef struct ULList {
    struct ULThread * head;
    struct ULThread * tail;
    int size;
} ULList;

/* thread structure */
typedef struct ULThread {
    int tid;
    ucontext_t * context;  //dentro de ucontext.h
    int state;
    int ret_val;
    int priority;
    ULList waiting;
    struct ULThread  * next;
    struct ULThread  * prev;
} ULThread;

/* scheduler structure */
typedef struct ULScheduler {
    ULThread * running;
    ULList ready;
    int last_tid;
} ULScheduler;

/*list manipulators*/
ULThread * list_get(ULList * l, int tid);
ULThread * list_removeTail(ULList * l);
ULThread * list_removeHead(ULList * l);
ULThread * list_remove_tid(ULList * l, int tid);
void list_insert(ULList * l, ULThread *t);
void list_append(ULList * l, ULThread * t); 
void plist_insert(ULList * l, ULThread * t);
const char * get_threadState(enum ULState state);
const char * get_threadPriority(enum ULPriority priority);

//void reschedule(void);

/* public methods to manipulate the threads
 * these methods are described in link below:
 * 
 * http://www.lisha.ufsc.br/teaching/os/exercise/ulthreads.html
 *
 */

/* Function descriptions
 * 
 *   int init(void)
 *   Initializes the user-level thread manager, registering the calling process as the first thread. This function must be called before any other in the package and returns "0" in case of success and "-1" in case of failure.
 *
 *   int create(void (* entry)(int), int arg)
 *   Creates a thread with entry point set to entry, which is a pointer to a plain function taking an integer as argument. The function returns an identifier for the created thread in case of success and "-1" in case of failure.
 *
 *   int join(int tid, int * status)
 *   Waits for the thread identified by tid to exit, returning it's return value in status. In case of success, this function returns "0", otherwise, "-1".
 *
 *   void exit(int status)
 *   Immediately terminates the execution of the calling thread, allowing for a return value to be eventually handed over to join.
 *
 *   void yield(void)
 *   Causes the calling thread to release the CPU, thus activating the scheduler.
 *
 *   int getid(void)
 *   Returns the identifier of the calling thread.
 *
 */

int init(void);
int create(void(*entry)(int), int arg, int priority);
int join(int tid, int * ret_val);
void finish(int ret_val);
void yield(void);
int getid(void);

#endif

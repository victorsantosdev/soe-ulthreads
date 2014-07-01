/*************************************************************
 * ULThread.c
 *
 * Author: Victor Santos
 * Latest Modified: 05/19/2014i MM/DD/YYYY
 * email: victor.ifsc@gmail.com
 *
 * This file is where is implemented all the function of the thread creation, schedulling and all another functions
 * related to thread and scheduler manipulations.
 *
 ****************************************************************/

#include "ulthread.h"
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define QUANTUM 5000 /*us = 5ms*/
/*********************
 * Globals
 * *****************/

ULScheduler sch;
ULThread * threads[MAX_THREADS];
volatile long long tick = 0;
struct itimerval time={0};
sigset_t _sigs;

/****************************************/
/* Functions related to scheduler timer */
/****************************************/
void signal_handler(){
    tick++; //every 5ms 1 is added on tick
    printf("signal_handler\n");
    reschedule();
}

void waitfor(int nMiliseconds) {
    long long start = tick;
    while( (tick - start) < (nMiliseconds/5))  yield();  
}

void pauseScheduler(void) {
    sigprocmask(SIG_BLOCK, &_sigs, 0);
    //kill(0,SIGVTALRM);
}

void resumeScheduler(void) {
    sigprocmask(SIG_UNBLOCK, &_sigs, 0);
}

void initTimer(void) {
    //Create a list with signals we want block.
    sigemptyset(&_sigs);
    sigaddset(&_sigs, SIGVTALRM);
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_usec = QUANTUM;
    time.it_value.tv_usec = QUANTUM;
    time.it_value.tv_sec = 0;
    setitimer(ITIMER_VIRTUAL, &time, NULL);
}
/*******************************
 * List manipulation functions
 * ***************************/

/*******************************************
 * ULThread * list_get(ULList * l, int tid)
 *
 * Get the thread object specified by its tid number
 * args: ULList * list, int task_id
 * return: ULThread of desired tid
 *
 ******************************************/
ULThread * list_get(ULList * l, int tid) {
    ULThread * ret = 0;
    if (l->size > 0) {
        ret  = l->head;
        while (ret != 0) {
            if (ret->tid == tid) {
                break;
            } else {
                ret = ret->next;
            }
        }
    }
    return ret;
}


/**********************************************
 * ULThread * list_removeTail(ULList * l)
 *
 * Remove the thread in the list tail and returns the removed thread object
 * args: ULList * list
 * return: ULThread removed
 *
 * ******************************************/
ULThread * list_removeTail(ULList * l) {
    ULThread * ret = 0;
    if (l->size > 0) {
        if (l->size == 1) {
            ret=l->tail;
            ret->prev=0;
            ret->next=0;
            l->head=0;
            l->tail=0;
            l->size=0;

        } else {
            ret = l->tail;
            l->tail = l->tail->prev;
            l->tail->next = 0;
            ret->prev = 0;
            ret->next=0;
            l->size--;
        }
    }
    return ret;
}

/***********************************************
 * ULThread * list_removeHead(ULList * l)
 *
 * Remove the thread in the list head and return the removed thread object
 * args: ULList * list
 * return: ULThread removed
 *
 * *********************************************/
ULThread * list_removeHead(ULList * l) {

    ULThread * ret = 0;
    if (l->size > 0) {
        if (l->size == 1) {
            ret=l->head;
            ret->prev=0;
            ret->next=0;
            l->head=0;
            l->tail=0;
            l->size=0;

        } else {
            l->head = l->head->next;
            ret = l->head->prev;
            l->head->prev = 0;
            ret->next=0;
            l->size--;
        }
    }
    return ret;
}

/***********************************************
 * ULThread * list_remove_tid(ULList * l, int tid)
 *
 * Remove the thread with specified tid in ULList * list passed and return the removed thread object
 * args: ULList * list, int task_tid
 * return: ULThread removed
 *
 * **********************************************/
ULThread * list_remove_tid(ULList * l, int tid) {
    ULThread * ret = 0;
    if (l->size > 0) {
        ret = l->head;
        while (ret != 0) {
            if (ret->tid == tid) {
                break;
            } else {
                ret = ret->next;
            }
        }
        if (ret != 0) {
            /*Tests if it is HEAD*/
            if (ret->prev == 0) ret = list_removeHead(l);
            /*Tests if it is TAIL*/
            else if (ret->next == 0) ret = list_removeTail(l);
            /*No HEAD nor TAIL*/
            else {
                ret->next->prev = ret->prev;
                ret->prev->next = ret->next;
                ret->prev = 0;
                ret->next = 0;
                l->size--;
            }
        }
    }
    return ret;
}


/*************************************************
 * void plist_insert(ULList * l, ULThread *t)
 *
 * Insert thread by priority
 * args: ULList * list, ULThread * thread
 * 
 * *********************************************/
void plist_insert(ULList * l, ULThread *t) {

    ULThread * ret = 0;
    ULList * tempList = 0;
    //if empty list, the thread will be simply added
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        if (t->priority == THREAD_PRIORITY_LOWEST) {
            list_append(l,t);
        } 
        else {
            ret = l->tail;
            while (ret != 0) {  //sweeps tail to head
                if (ret->priority <= t->priority) {
                    break;
                } else {
                    ret = ret->prev;
                }
            }
            if (ret != 0) {
                /*Tests if it is TAIL*/
                if (ret->next == 0) list_append(l,t);
                else {
                    t->next = ret->next;
                    ret->next->prev = t;
                    ret->next = t;
                    t->prev = ret;
                    l->size++;
                }
            } else {
                //sweeped all the list, and all the elements (or the only one) have/has low priority than t
                list_insert(l,t);
            }
        }
    }
}
/*************************************************
 * void list_insert(ULList * l, ULThread *t)
 *
 * Insert thread on list head
 * args: ULList * list, ULThread * thread
 * 
 * *********************************************/
void list_insert(ULList * l, ULThread *t) {
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        t->prev = 0;
        t->next = l->head;
        l->head->prev = t;
        l->head = t;
        l->size++;
    }
}

/**********************************************
 * void list_append(ULList * l, ULThread * t)
 *
 * Insert thread on list tail
 * args: ULList * list, ULThread * thread
 *
 * *******************************************/
void list_append(ULList * l, ULThread * t) {
    if (l->size == 0) {
        l->head = t;
        l->tail = t;
        l->size++;
    } else {
        t->next = 0; //bug fix:14/06
        t->prev = l->tail;
        l->tail->next = t;
        l->tail = t;
        l->size++;
    }
} 

/***************************************************
 * Thread manipulation functions
 * ************************************************/

int create(void(*entry)(int), int arg, int priority) {

    if(sch.last_tid > MAX_THREADS) 
        return -1;

    ULThread * thread = (ULThread *)malloc(sizeof(ULThread));
    thread->tid = sch.last_tid++;
    threads[thread->tid] = thread;
    thread->context = (ucontext_t *)malloc(sizeof(ucontext_t));
    thread->state = READY;
    thread->waiting.head = 0;
    thread->waiting.tail = 0;
    thread->waiting.size = 0;
    thread->prev = 0;
    thread->next = 0;
    thread->priority = priority;

    //SIGSTKSZ esta declarado em /usr/include/sys/ucontext.h
    thread->context->uc_link          = 0;
    thread->context->uc_stack.ss_sp   = (char *)malloc(sizeof(SIGSTKSZ));
    thread->context->uc_stack.ss_size = SIGSTKSZ;

    getcontext(thread->context);
    makecontext(thread->context, (void (*)(void)) entry, 1, arg);

    //list_append(&sch.ready, thread);
    plist_insert(&sch.ready, thread); //insert with priority support

    return thread->tid;
}

void yield(void) {
    ULThread * old_thread;

    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        pauseScheduler();    
        old_thread = sch.running;
        old_thread->state = READY;
        sch.running= list_removeHead(&sch.ready);
        sch.running->state = RUNNING;
        //list_append(&sch.ready, old_thread);
        plist_insert(&sch.ready, old_thread);
        printf("yield\n");
        resumeScheduler();
        swapcontext(old_thread->context, sch.running->context);
    }
}

/*returns the tID of the running thread*/
int getid(void) {
    return sch.running->tid;
}


/*espera a thread especificada com tid join acabar para prosseguir seu andamento */
int join(int tid, int * ret_val) {

    ULThread * old_thread;
    if(threads[tid]) {
        if(threads[tid]->state != FINISHING) {
            if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
                pauseScheduler();
                old_thread = sch.running;
                old_thread->state = WAITING;
                sch.running= list_removeHead(&sch.ready);
                sch.running->state = RUNNING;
                //pega a thread running e insere na lista de waiting da thread A
                list_append(&threads[tid]->waiting, old_thread);
                printf("join\n");
                resumeScheduler();
                swapcontext(old_thread->context, sch.running->context);
            }
        }
        //atualiza o estado da thread para FINISHING
        *ret_val = threads[tid]->ret_val;
        return 0;

    } else {
        return -1;
    }
}

void finish(int ret_val) {
    ULThread * tmp; //variavel de auxilio
    ULThread * thr = sch.running;
    thr->ret_val = ret_val;
    thr->state = FINISHING;
    while(thr->waiting.size) {
        pauseScheduler();
        tmp = list_removeHead(&thr->waiting);
        tmp->state = READY;
        //acordando as tarefas que estao esperando liberacao
        //list_append(&sch.ready, tmp);
        plist_insert(&sch.ready, tmp);
        resumeScheduler();
    }
    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        pauseScheduler();
        sch.running= list_removeHead(&sch.ready);
        sch.running->state = RUNNING;
        printf("finish ready.size > 0\n");
        resumeScheduler();
        swapcontext(thr->context, sch.running->context);
    }
}

/*translates int state to a string-readable state*/
const char * get_threadState(enum ULState state) {
    switch(state) {
        case RUNNING: return "RUNNING";
        case READY: return "READY";
        case WAITING: return "WAITING";
        case FINISHING: return "FINISHING";
        default: return "ERROR";                
    }
}

/*translates int priority to a string-readable priority*/
const char * get_threadPriority(enum ULPriority priority) {
    switch(priority) {
        case THREAD_PRIORITY_TIME_CRITICAL: return "THREAD_PRIORITY_TIME_CRITICAL";
        case THREAD_PRIORITY_HIGHEST: return "THREAD_PRIORITY_HIGHEST";
        case THREAD_PRIORITY_ABOVE_NORMAL: return "THREAD_PRIORITY_ABOVE_NORMAL";
        case THREAD_PRIORITY_NORMAL: return "THREAD_PRIORITY_NORMAL";
        case THREAD_PRIORITY_BELOW_NORMAL: return "THREAD_PRIORITY_BELOW_NORMAL";
        case THREAD_PRIORITY_LOWEST: return "THREAD_PRIORITY_LOWEST";
        default: return "ERROR";                
    }
}

void print_list(ULList * l) {

    ULThread * handler;
    handler = l->head;
    while (handler != 0) {
        printf("thread ID: %d\tpriority: %s\tstate: %s\n", handler->tid, get_threadPriority(handler->priority), get_threadState(handler->state));
        handler=handler->next;
    }
}

void reschedule(void) {
    ULThread * old_thread;
    ULThread * temp;

    printf("reschedule\n");
    //printf("lista de ready atual:\n ");
    //print_list(&sch.ready);
    
    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        if (sch.running->priority >= sch.ready.head->priority) {
            printf("prioridade da running maior ou igual do que a head da lista\n");
            pauseScheduler();
            old_thread = sch.running;
            old_thread->state = READY;
            //plist_insert(&sch.ready, old_thread);
            sch.running= list_removeHead(&sch.ready);
            sch.running->state = RUNNING;
            plist_insert(&sch.ready, old_thread);
            printf("lista de ready atual:\n ");
            print_list(&sch.ready);
            printf("running ID: %d\n", sch.running->tid);
            resumeScheduler();
            swapcontext(old_thread->context, sch.running->context);
        } else printf("prioridade da running menor do que a head da lista\n");
    }
    resumeScheduler();

}

void debug() {
    printf("prioridade do head da lista: %d\n", sch.ready.head->priority);
}

/* init of the main thread */
int init(void) {

    if(sch.last_tid > MAX_THREADS) 
        return -1;
    //initialize scheduler
    sch.running = 0;
    sch.last_tid = 0;
    sch.ready.head =0;
    sch.ready.tail = 0;
    sch.ready.size = 0;

    //forjar primeira thread
    sch.running = (ULThread *)malloc(sizeof(ULThread));
    sch.running->tid = sch.last_tid++;
    threads[sch.running->tid] = sch.running;
    sch.running->context = (ucontext_t *)malloc(sizeof(ucontext_t));
    sch.running->waiting.head = 0;
    sch.running->waiting.tail = 0;
    sch.running->waiting.size = 0;
    sch.running->state = RUNNING;
    sch.running->prev = 0;
    sch.running->next = 0;

    getcontext(sch.running->context);
    signal(SIGVTALRM, signal_handler);
    initTimer();
    return 0;
}



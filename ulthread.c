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

/*********************
 * Globals
 * *****************/

ULScheduler sch;
ULThread * threads[MAX_THREADS];

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
        t->prev = l->tail;
        l->tail->next = t;
        l->tail = t;
        l->size++;
    }
} 

/***************************************************
 * Thread manipulation functions
 * ************************************************/

int create(void(*entry)(int), int arg) {

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


    //SIGSTKSZ esta declarado em /usr/sys/ucontext.h
    thread->context->uc_link          = 0;
    thread->context->uc_stack.ss_sp   = (char *)malloc(sizeof(SIGSTKSZ));
    thread->context->uc_stack.ss_size = SIGSTKSZ;

    getcontext(thread->context);
    makecontext(thread->context, (void (*)(void)) entry, 1, arg);

    list_append(&sch.ready, thread);

    return thread->tid;
}

void yield(void) {
    ULThread * old_thread;

    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        old_thread = sch.running;
        old_thread->state = READY;
        sch.running= list_removeHead(&sch.ready);
        sch.running->state = RUNNING;
        list_append(&sch.ready, old_thread);
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
                old_thread = sch.running;
                old_thread->state = WAITING;
                sch.running= list_removeHead(&sch.ready);
                sch.running->state = RUNNING;
                //pega a thread running e insere na lista de waiting da thread A
                list_append(&threads[tid]->waiting, old_thread);
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
        tmp = list_removeHead(&thr->waiting);
        tmp->state = READY;
        //acordando as tarefas que estao esperando liberacao
        list_append(&sch.ready, tmp);
    }
    if (sch.ready.size > 0) { //uma tarefa running e uma+ ready
        sch.running= list_removeHead(&sch.ready);
        sch.running->state = RUNNING;
        swapcontext(thr->context, sch.running->context);
    }
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

    return 0;
}



/*************************************************************
 * Linked List in C, with methods
 *
 * Author: Victor Santos
 * Latest Build: 05/13/2014
 * email: victor.ifsc@gmail.com
 *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

typedef struct ULThread {
    int tid;
    struct ULThread  * next;
    struct ULThread  * prev;
    ucontext_t * ucontext;  //dentro de ucontext.h
} ULThread;

typedef struct ULList {
    ULThread * head;
    ULThread * tail;
    int size;
} ULList;

/*Returns the object of specified tID*/
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

/*Remove list TAIL, but return the removed ULThread object*/
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

/*Remove list HEAD, but return the remove ULThread object*/
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

/*Remove specified tID in list, and returns the removed ULThread object*/
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

/*Insert on list HEAD*/
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

/*Insert on list TAIL*/
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


int main () {

    ULList * lista = malloc(sizeof(ULList));
    ULThread * handler = malloc(sizeof(ULThread));

    printf("Raw Linked List: Hello World\n");    

    ULThread * _a = malloc(sizeof(ULThread));
    _a->tid = 10;
    _a->next = 0;
    _a->prev = 0;

    ULThread * _b = malloc(sizeof(ULThread));
    _b->tid = 20;
    _b->next = 0;
    _b->prev = 0;

    ULThread * _c = malloc(sizeof(ULThread));
    _c->tid = 30;
    _c->next = 0;
    _c->prev = 0;

    ULThread * _d = malloc(sizeof(ULThread));
    _d->tid = 40;
    _d->next = 0;
    _d->prev = 0;

    ULThread * _e = malloc(sizeof(ULThread));
    _e->tid = 50;
    _e->next = 0;
    _e->prev = 0;

    printf("Adicionando Threads na lista...\n");
    list_append(lista, _a);
    printf("Thread TID:%d adicionada\n",lista->tail->tid);
    list_append(lista, _b);
    printf("Thread TID:%d adicionada\n",lista->tail->tid);
    list_append(lista, _c);
    printf("Thread TID:%d adicionada\n",lista->tail->tid);
    printf("Removendo HEAD da lista...\n");
    handler = list_removeHead(lista);
    printf("Novo HEAD da lista: %d\n",lista->head->tid);
    printf("Reinserindo o HEAD removido no fim da lista...\n");
    list_append(lista, handler);
    printf("Novo TAIL da lista: %d\n",lista->tail->tid);
    printf("Removendo HEAD da lista novamente...\n");
    handler = list_removeHead(lista);
    printf("Novo HEAD da lista: %d\n",lista->head->tid);
    printf("Reinserindo o HEAD removido no fim da lista...\n");
    list_append(lista, handler);
    printf("Novo TAIL da lista: %d\n",lista->tail->tid);
    printf("Adicionando nova thread no HEAD da lista...\n");
    list_insert(lista, _d);
    printf("Novo HEAD da lista: %d\n",lista->head->tid);
    printf("Removendo TAIL da lista...\n");
    handler = list_removeTail(lista);
    printf("Novo TAIL da lista: %d\n",lista->tail->tid);
    printf("Reinserindo o TAIL removido para o HEAD da lista...\n");
    list_insert(lista, handler);
    printf("Novo HEAD da lista: %d\n",lista->head->tid);
    printf("Buscando tID 30 na lista...\n");
    handler = list_get(lista, 30);
    printf("Handler com tID %d, encontrado na lista\n", handler->tid);
    list_append(lista, _e);
    printf("Novo TAIL da lista: %d\n",lista->tail->tid);

    printf("tIDs cadastrados na lista.. \n");
    handler=lista->head;
    while(handler != 0) {
        printf("tID: %d\n", handler->tid);
        handler = handler->next;
    }
    printf("Removendo o handler com tID 10 da lista...\n");
    handler = list_remove_tid(lista, 10);
    printf("Handler com tID %d, removido da lista\n", handler->tid);
    printf("Reinserindo o tID removido no fim da lista...\n");
    list_append(lista, handler);
    printf("Novo TAIL da lista: %d\n",lista->tail->tid);

    printf("tIDs cadastrados na lista.. \n");
    handler=lista->head;
    while(handler != 0) {
        printf("tID: %d\n", handler->tid);
        handler = handler->next;
    }

    return 0;

}

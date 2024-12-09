#include "ThreadQueue.h"
#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t QMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cCond = PTHREAD_COND_INITIALIZER;
ThreadMap *threadSet = NULL;

int setSize = 0;
void insertMap(thread_t tid, Thread *tcb) {

    if (__getThread(tid) != NULL) {

        return;
    }

    pthread_mutex_lock(&QMutex);
    setSize++;
    ThreadMap *nData = (ThreadMap *)malloc(sizeof(ThreadMap));
    nData->tcb = tcb;
    nData->tid = tid;
    nData->pNext = NULL;
    if (threadSet == NULL) {
        threadSet = nData;
        pthread_mutex_unlock(&QMutex);
        return;
    }
    ThreadMap *iter = threadSet;
    while (iter->pNext != NULL) {
        iter = iter->pNext;
    }
    iter->pNext = nData;
    pthread_mutex_unlock(&QMutex);
    return;
}

int QueueEmpty(Thread *head) {
    pthread_mutex_lock(&QMutex);
    if (head == NULL) {
        pthread_mutex_unlock(&QMutex);
        return 1;
    }
    pthread_mutex_unlock(&QMutex);
    return 0;
}
void __thread_to_ready2(Thread **head, Thread **tail, Thread *tcb) {

    AddThread(head, tail, tcb);
    insertMap(tcb->tid, tcb);
    pthread_cond_signal(&cCond);
}

void AddThread(Thread **head, Thread **tail, Thread *tcb) {
    pthread_mutex_lock(&QMutex);
    tcb->pNext = NULL;
    tcb->pPrev = NULL;

    if (*head == NULL) {
        *head = tcb;
        *tail = tcb;
        pthread_mutex_unlock(&QMutex);
        return;
    }

    (*tail)->pNext = tcb;
    tcb->pPrev = (*tail);
    *tail = (*tail)->pNext;
    (*tail)->pNext = NULL;
    pthread_mutex_unlock(&QMutex);
}

void PopThread(Thread **head) {
    pthread_mutex_lock(&QMutex);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&QMutex);
        return;
    }

    *head = (*head)->pNext;
    if (*head != NULL) {

        (*head)->pPrev = NULL;
    }
    pthread_mutex_unlock(&QMutex);
}
Thread *FrontThread(Thread **head) {
    pthread_mutex_lock(&QMutex);
    if (*head == NULL) {
        pthread_mutex_unlock(&QMutex);
        return NULL;
    }
    pthread_mutex_unlock(&QMutex);
    return (Thread *)*head;
}
void RemoveThread(Thread **head, Thread **tcb) {
    pthread_mutex_lock(&QMutex);

    if (head == NULL || *head == NULL || *tcb == NULL) {
        (*tcb)->pNext = NULL;
        (*tcb)->pPrev = NULL;
        pthread_mutex_unlock(&QMutex);
        return;
    }

    if (*head == *tcb) {
        *head = (*tcb)->pNext;
        if ((*tcb)->pNext != NULL) {
            (*tcb)->pNext->pPrev = NULL;
        }
    } else {

        if ((*tcb)->pPrev != NULL) {
            (*tcb)->pPrev->pNext = (*tcb)->pNext;
        }
        if ((*tcb)->pNext != NULL) {
            (*tcb)->pNext->pPrev = (*tcb)->pPrev;
        }
    }

    (*tcb)->pNext = NULL;
    (*tcb)->pPrev = NULL;
    pthread_mutex_unlock(&QMutex);
}
Thread *__getThread(thread_t tid) {
    pthread_mutex_lock(&QMutex);
    ThreadMap *iter = threadSet;
    while (iter != NULL) {
        if ((iter)->tid == tid) {
            pthread_mutex_unlock(&QMutex);
            return iter->tcb;
        }
        iter = (iter)->pNext;
    }
    pthread_mutex_unlock(&QMutex);
    return NULL;
}

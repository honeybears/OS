#ifndef __TREADHQUEU_H__
#define __THREADQUEUE_H__
#include "Thread.h"

typedef struct _ThreadMap ThreadMap;

typedef struct _ThreadMap {
    thread_t tid;
    Thread *tcb;
    ThreadMap *pNext;
} ThreadMap;
extern int setSize;
extern pthread_mutex_t QMutex;
extern pthread_mutex_t cMutex;
extern pthread_cond_t cCond;
// Control TCB Queue
extern ThreadMap *threadSet;

void AddThread(Thread **head, Thread **tail, Thread *tcb);
Thread *FrontThread(Thread **head);
void PopThread(Thread **head);
Thread *__getThread(thread_t tid);
int QueueEmpty(Thread *head);
void insertMap(thread_t tid, Thread *tcb);
void RemoveThread(Thread **head, Thread **tcb);
void __thread_to_ready2(Thread **head, Thread **tail, Thread *tcb);

#endif

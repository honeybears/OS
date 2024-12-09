#include "Scheduler.h"
#include "Init.h"
#include "Thread.h"
#include "ThreadQueue.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
pthread_mutex_t SMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t SCond = PTHREAD_COND_INITIALIZER;

void __ContextSwitch(Thread *pCurThread, Thread *pNewThread);

void __thread_to_run(Thread *tcb);

void __thread_to_ready(int signo);

Thread **curtcb;

int RunScheduler(void) {
    Thread *tcb = NULL;
    Thread *new_tcb;
    while (QueueEmpty(ReadyQHead))
        sleep(TIMESLICE);

    tcb = FrontThread(&ReadyQHead);

    PopThread(&ReadyQHead);
    curtcb = &tcb;

    for (int i = 0;; i++) {
        pthread_mutex_lock(&SMutex);
        if (*curtcb == NULL) {
            break;
            pthread_mutex_unlock(&SMutex);
        }

        if (i == 0)
            __thread_to_run(*curtcb);
        sleep(TIMESLICE);

        new_tcb = FrontThread(&ReadyQHead);
        PopThread(&ReadyQHead);
        if ((*curtcb)->status == THREAD_STATUS_RUN && new_tcb != *curtcb) {

            AddThread(&ReadyQHead, &ReadyQTail, *curtcb);
        }

        __ContextSwitch(*curtcb, new_tcb);

        if (new_tcb != (*curtcb) && new_tcb != NULL) {
            *curtcb = new_tcb;
        }
        if (new_tcb == (*curtcb) || new_tcb == NULL) {
            *curtcb = tcb;
        }
        pthread_mutex_unlock(&SMutex);
    }
    return 0;
}
void __ContextSwitch(Thread *pCurThread, Thread *pNewThread) {

    if (pCurThread == pNewThread) {
        __thread_to_run(pCurThread);
        return;
    }

    if (pCurThread != NULL) {

        pthread_kill(pCurThread->tid, SIGUSR1);
    }

    if (pNewThread != NULL)
        __thread_to_run(pNewThread);
}
void __thread_to_run(Thread *tcb) {

    if (tcb->status == THREAD_STATUS_READY) {
        pthread_mutex_lock(&tcb->readyMutex);
        tcb->status = THREAD_STATUS_RUN;
        tcb->bRunnable = TRUE;
        pthread_cond_signal(&tcb->readyCond);
        pthread_mutex_unlock(&tcb->readyMutex);
    }
}
// thread_to_ready by Scheduler.c
void __thread_to_ready(int signo) {

    Thread *tcb;

    tcb = __getThread(thread_self());

    tcb->bRunnable = FALSE;
    if (tcb->status != THREAD_STATUS_BLOCKED) {

        tcb->status = THREAD_STATUS_READY;

        while (tcb->bRunnable != TRUE) {
            pthread_cond_wait(&tcb->readyCond, &tcb->readyMutex);
        }
    }
}

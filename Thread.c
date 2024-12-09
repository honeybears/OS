#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include "ThreadQueue.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

Thread *ReadyQHead = NULL;
Thread *ReadyQTail = NULL;

Thread *WaitQHead = NULL;
Thread *WaitQTail = NULL;

extern pthread_mutex_t SMutex;

void __thread_to_zombie(Thread *tcb);
void __thread_to_join(Thread *tcb);

void *WrapperFunc(void *args) {
    WrapperArg *wArg = (WrapperArg *)args;

    wArg->pThread->tid = thread_self();

    __thread_to_ready2(&ReadyQHead, &ReadyQTail, wArg->pThread);

    pthread_mutex_lock(&wArg->pThread->readyMutex);
    while (wArg->pThread->bRunnable != TRUE) {
        pthread_cond_wait(&wArg->pThread->readyCond,
                          &wArg->pThread->readyMutex);
    }
    void *retval = wArg->funcPtr(wArg->funcArg);

    pthread_mutex_unlock(&wArg->pThread->readyMutex);
    return retval;
}

int thread_create(thread_t *thread, thread_attr_t *attr,
                  void *(*start_routine)(void *), void *arg) {
    pthread_mutex_lock(&cMutex);
    Thread *tcb = (Thread *)malloc(sizeof(Thread));
    tcb->status = THREAD_STATUS_READY;
    pthread_mutex_t tMutex;
    pthread_mutex_init(&tMutex, NULL);
    tcb->readyMutex = tMutex;

    pthread_mutex_t zMutex;
    pthread_mutex_init(&zMutex, NULL);
    tcb->zombieMutex = zMutex;
    pthread_cond_t rCond;
    pthread_cond_init(&rCond, NULL);
    pthread_cond_t zCond;
    pthread_cond_init(&zCond, NULL);
    tcb->readyCond = rCond;
    tcb->zombieCond = zCond;
    tcb->pNext = NULL;
    tcb->pPrev = NULL;
    tcb->bRunnable = FALSE;
    tcb->bZombie = FALSE;
    tcb->pExitCode = NULL;
    tcb->parentTid = thread_self();

    WrapperArg *args = (WrapperArg *)malloc(sizeof(WrapperArg));
    args->funcArg = arg;
    args->funcPtr = start_routine;
    args->pThread = tcb;

    pthread_create(thread, attr, WrapperFunc, args);

    pthread_cond_wait(&cCond, &cMutex);
    pthread_mutex_unlock(&cMutex);

    return 0;
}

int thread_join(thread_t thread, void **retval) {

    Thread *ctcb = __getThread(thread);
    Thread *ptcb = __getThread(ctcb->parentTid);

    if (ctcb->bZombie == TRUE) {

        *retval = ctcb->pExitCode;

        RemoveThread(&ReadyQHead, &ctcb);

        free(ctcb);
        return 0;
    }
    pthread_mutex_lock(&ctcb->zombieMutex);
    ptcb->status = THREAD_STATUS_BLOCKED;

    AddThread(&WaitQHead, &WaitQTail, ptcb);
    RemoveThread(&ReadyQHead, &ptcb);

    while (ctcb->bZombie != TRUE) {
        pthread_cond_wait(&ctcb->zombieCond, &ctcb->zombieMutex);
    }
    ptcb->bRunnable = FALSE;
    ptcb->status = THREAD_STATUS_READY;

    RemoveThread(&WaitQHead, &ptcb);
    AddThread(&ReadyQHead, &ReadyQTail, ptcb);

    while (ptcb->bRunnable != TRUE) {
        pthread_cond_wait(&ptcb->readyCond, &ptcb->readyMutex);
    }
    pthread_mutex_unlock(&ctcb->zombieMutex);

    thread_resume(ptcb->tid);

    *retval = ctcb->pExitCode;

    RemoveThread(&ReadyQHead, &ctcb);

    free(ctcb);
    return 0;
}

void __thread_to_zombie(Thread *tcb) {
    pthread_mutex_lock(&tcb->zombieMutex);

    tcb->bZombie = TRUE;
    tcb->status = THREAD_STATUS_ZOMBIE;

    pthread_cond_signal(&tcb->zombieCond);

    pthread_mutex_unlock(&tcb->zombieMutex);
}

int thread_suspend(thread_t tid) {

    Thread *tcb = __getThread(tid);

    pthread_mutex_lock(&tcb->readyMutex);

    tcb->bRunnable = FALSE;
    tcb->status = THREAD_STATUS_BLOCKED;

    RemoveThread(&ReadyQHead, &tcb);

    AddThread(&WaitQHead, &WaitQTail, tcb);

    pthread_mutex_unlock(&tcb->readyMutex);
    return 0;
}

int thread_resume(thread_t tid) {

    Thread *tcb = __getThread(tid);

    tcb->status = THREAD_STATUS_READY;

    RemoveThread(&WaitQHead, &tcb);

    AddThread(&ReadyQHead, &ReadyQTail, tcb);

    return 0;
}

int thread_exit(void *retval) {
    Thread *tcb = __getThread(thread_self());
    tcb->pExitCode = retval;
    __thread_to_zombie(tcb);
    return 0;
}

thread_t thread_self() { return pthread_self(); }

#include "TestCase3.h"

void *Tc3ThreadProc(void *param) {
    thread_t tid = 0;
    int *retVal;
    tid = thread_self();
    while (1) {
        sleep(2);
        printf("Tc3ThreadProc: my thread id:(%d), arg : %d\n", (int)tid,
               *((int *)param));
    }
    retVal = (int *)param;
    thread_exit(retVal);
    return retVal;
}

/*
 * - TestCase3 tests suspending, resuming and deleting Threads.
 * - Testing API scopes: thread_suspend, thread_resume
 */

void TestCase3(void) {
    thread_t tid[TOTAL_THREAD_NUM];
    int i = 0;
    int j = 0;
    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;
    signal(SIGCHLD, SIG_IGN);

    thread_create(&tid[0], NULL, (void *)Tc3ThreadProc, (void *)&i1);
    thread_create(&tid[1], NULL, (void *)Tc3ThreadProc, (void *)&i2);
    thread_create(&tid[2], NULL, (void *)Tc3ThreadProc, (void *)&i3);
    thread_create(&tid[3], NULL, (void *)Tc3ThreadProc, (void *)&i4);
    thread_create(&tid[4], NULL, (void *)Tc3ThreadProc, (void *)&i5);

    /* Suspend all thread */
    for (i = 0; i < TOTAL_THREAD_NUM; i++) {
        sleep(2);

        thread_suspend(tid[i]);

        Thread *temp = WaitQHead;

        for (j = 0; j < i; j++)
            temp = temp->pNext;

        if (temp->status != 2) {
            printf("TestCase3: Thread is not suspended\n");
            assert(0);
        }
    }

    /* Resume thread sequentially */
    for (i = 0; i < TOTAL_THREAD_NUM; i++) {
        sleep(2);

        thread_resume(tid[i]);
    }

    for (i = 0; i < TOTAL_THREAD_NUM; i++) {
        int *retVal;
        thread_join(tid[i], (void **)&retVal);

        printf("Thread [ %d ] is finish Return : [ %d ] \n", (int)tid[i],
               *retVal);
    }

    return;
}

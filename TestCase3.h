#ifndef TEST_CASE3_H
#define TEST_CASE3_H

#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_THREAD_NUM (5)

void *Tc3ThreadProc(void *param);
void TestCase3(void);

#endif

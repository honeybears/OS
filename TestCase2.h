#ifndef TEST_CASE2_H
#define TEST_CASE2_H

#include "Init.h"
#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_THREAD_NUM (5)

void *Tc2ThreadProc(void *parm);
void TestCase2(void);

#endif

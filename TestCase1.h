#ifndef TEST_CASE_1_H
#define TEST_CASE_1_H

#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_THREAD_NUM (5)
void *Tc1ThreadProc(void *param);
void TestCase1(void);

#endif

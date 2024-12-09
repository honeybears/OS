#include "Init.h"
#include "Thread.h"
#include "ThreadQueue.h"
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

// mutex from init

void __thread_to_ready(int signo);

void Init() { signal(SIGUSR1, __thread_to_ready); }

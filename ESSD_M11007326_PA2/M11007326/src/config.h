#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <sched.h>

#define PART 3

// Hardware dependency parameter
#define CORE_NUM 4
#define THREAD_NUM 4

// Workload parameter
#define MASK_SIZE 31 
#define MATRIX_SIZE 1000 
#define CONVOLUTION_TIMES 3

// Protecte shared resource setting
#define MUTEX 0
#define SPINLOCK 1

#define _ProtectType SPINLOCK

// Synchronize method setting
#define BARRIER 0
#define SEMAPHORE 1

#define _SynType BARRIER


#endif


#include "system.h"

/*~~~~~~~~~~~~Global Resuource~~~~~~~~~~~*/
/*           Your code(Part1~3)          */
float sharedSum = 0;
pthread_mutex_t* ioMutex = new pthread_mutex_t;
pthread_mutex_t* count_mutex = new pthread_mutex_t;
pthread_barrier_t *barr = new pthread_barrier_t;
pthread_spinlock_t *lock = new pthread_spinlock_t;

/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

System::System (char* input_file)
{
    printInfo ();
    initMatrix ();

    /*~~~~~~~~~~~~~Your code(Part1~3)~~~~~~~~~~~~~*/
    // Initial the resources (barrier, semaphore) //
    pthread_barrier_init ( barr, NULL, THREAD_NUM );
	pthread_spin_init ( lock, PTHREAD_PROCESS_PRIVATE );
//	sem_init ( sem, pshared, value );
    // ...
    /*~~~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~~~~*/

    check       = new Check;
    threadSet   = new Thread[THREAD_NUM];

    check->init (CONVOLUTION_TIMES, 
                 MASK_SIZE, 
                 MATRIX_SIZE, 
                 THREAD_NUM, 
                 multiResult);

    for (int threadID = 0; threadID < THREAD_NUM; threadID++)
    {
        threadSet [threadID].init (threadID,
                                   mask, 
                                   matrix, 
                                   multiResult);

        threadSet [threadID].setCore (threadID);
        threadSet [threadID].setStartCalculatePoint (threadID * MATRIX_SIZE / THREAD_NUM);
        threadSet [threadID].setEndCalculatePoint ((threadID + 1) * MATRIX_SIZE / THREAD_NUM);
    }

    std::cout << "\n=================== Generate Matrix Data ==================" << std::endl;
    setStartTime ();

        check->dataGenerator (matrix, mask);

    setEndTime ();
    std::cout << "Generate Date Spend time : " << _timeUse << std::endl;

}

void
System::createAnswer ()
{
    check->createAnswer (PART);
}

void
System::initMatrix ()
{
    if((MASK_SIZE % 2) == 0) {
        std::cout << "!! Mask size not odd number !!" << std::endl;
        assert(false);
        
    } else {
        mask        = new float* [MASK_SIZE];
        matrix      = new float* [MATRIX_SIZE];
        multiResult = new float* [MATRIX_SIZE];

        for (int i = 0; i < MASK_SIZE; i++)
                mask [i]    = new float [MASK_SIZE];
        for (int i = 0; i < MATRIX_SIZE; i++) {
            matrix [i]      = new float [MATRIX_SIZE];
            multiResult [i] = new float [MATRIX_SIZE];
        }
    }
}

/**
 * Using the pthread function to perform PA exection.
 * 
 * pthread_create( pthread_t* thread, NULL, void* function (void*), void* args );
 * 
 * pthread_join( pthread_t* thread, NULL );
 * 
 */
void
System::multiCoreConvolution ()
{
    std::cout << "\n============== Start Multi-Thread Convolution =============" << std::endl;
    setStartTime ();

        for (int threadID = 0; threadID < THREAD_NUM; threadID++)
            pthread_create (&threadSet [threadID]._thread,
                            NULL, 
                            (THREADFUNCPTR)&Thread::convolution, 
                            &threadSet [threadID]);

        for (int threadID = 0; threadID < THREAD_NUM; threadID++)
            pthread_join (threadSet[threadID]._thread, NULL);   

    setEndTime ();
    std::cout << "Multi Thread Spend time : " << _timeUse << std::endl;

    check->checkCorrectness (PART);    
}

void
System::printInfo ()
{
    std::cout << "\n======================= System Info =======================" << std::endl; 
    std::cout << "numThread: " << THREAD_NUM << std::endl;
    std::cout << "maskSize: " << MASK_SIZE << " x " << MASK_SIZE << std::endl;
    std::cout << "matrixSize: " << MATRIX_SIZE << " x " << MATRIX_SIZE << std::endl;
    std::cout << "Protect Shared Resource: ";
#if _ProtectType == MUTEX
    std::cout << "Mutex" << std::endl;
#else
    std::cout << "Spinlock" << std::endl;
#endif

    std::cout << "Synchronize: ";
#if _SynType == BARRIER
    std::cout << "Barrier" << std::endl;
#else
    std::cout << "Semaphore" << std::endl;
#endif

}
   
void
System::setEndTime ()
{
	gettimeofday (&end, NULL);
	_timeUse = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000.0;
}

void
System::setStartTime ()
{
	gettimeofday (&start, NULL);
}




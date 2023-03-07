#include "system.h"

System::System (char* input_file)
{
    loadInput(input_file);

    for (int i = 0; i < numThread; i++)
    {
#if (PART == 1)
        /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
        // For part1, we assign the matrix0 into all threads
        threadSet[i].init(multiResult[0], matrix[0], mask[0]);
        // Set up the caculation range of each thread matrix
        threadSet[i].setStartCalculatePoint(i * threadSet[i].matrixSize()/numThread);
		threadSet[i].setEndCalculatePoint((i + 1) * threadSet[i].matrixSize()/ numThread);
        // ...
        // ...
        /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#else
        threadSet[i].init(multiResult[i], matrix[i], mask[i]);
            
#endif

#if (PART == 3)
	    /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
        // Set the scheduling policy for thread.
        
		threadSet[i].setSchedulingPolicy(SCHEDULING);
        // ...
        // ...
	    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#endif
    }

    /* CPU list Initialize */
    cpuSet = new CPU[CORE_NUM];
    for (int i = 0; i < CORE_NUM; i++)
        cpuSet[i].init (i, numThread);
    
    /* Set up checker for checking correctness */
    check = new Check;
    check->init(CORE_NUM, MASK_SIZE, numThread, multiResult);
    std::cout << "\n===========Generate Matrix Data===========" << std::endl;
    setStartTime();
    for (int i = 0; i < numThread; i++)
    {
        check->setThreadWithIndex (i, &threadSet[i]._thread);
        check->setMatrixSizeWithIndex( i, threadSet[i].matrixSize() );
        check->dataGenerator(i, matrix, mask);
        threadSet[i].setCheck( check );
    }
    setEndTime();
    std::cout << "Generate Date Spend time : " << _timeUse << std::endl;
    
#if (PART == 3)
    if (SCHEDULING == SCHED_FIFO) {
        check->setCheckState(PARTITION_FIFO);
    } else if (SCHEDULING == SCHED_RR) {
        check->setCheckState(PARTITION_RR);
    } else {
        std::cout << "!! Not supported scheduler !!" << std::endl;
        assert(false);
    }
#endif

}

System::~System()
{
    for (int thread_id = 0; thread_id < numThread; thread_id++)
    {
        for (int i = 0; i < MASK_SIZE; i++)
        {
            delete[] mask[thread_id][i];
        }
        delete[] mask[thread_id];
        for (int i = 0; i < threadSet[thread_id].matrixSize(); i++)
        {
            delete[] matrix[thread_id][i];
            delete[] multiResult[thread_id][i];
        }
        delete[] matrix[thread_id];
        delete[] multiResult[thread_id];
    }
    delete[] mask;
    delete[] matrix;
    delete[] multiResult;

    delete cpuSet;
}

void
System::creatAnswer ()
{
    check->creatAnswer(PART);
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
System::globalMultiCoreConv ()
{
    std::cout << "\n===========Start Global Multi-Thread Convolution===========" << std::endl;
    check->setCheckState(GLOBAL);
    setStartTime();

    /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Create thread and join
    
	for(int i=0;i<numThread;i++){
    	pthread_create (&threadSet[i]._thread, NULL, threadSet[i].convolution, &threadSet[i]);
	}
    for(int i=0;i<numThread;i++){
    	pthread_join (threadSet[i]._thread, NULL);
	}

    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    setEndTime();
    std::cout << "Global Multi Thread Spend time : " << _timeUse << std::endl;
    cleanMultiResult();
}

void
System::partitionMultiCoreConv ()
{
#if (PART == 1)
    std::cout << "\n===========Start Partition Multi-Thread Convolution===========" << std::endl;
    check->setCheckState(PARTITION);
    for(int i=0;i<numThread;i++){
    	threadSet[i].setCore(i%CORE_NUM);
	}
#else
	for(int i=0;i<CORE_NUM;i++)
	{
		pthread_mutex_lock(&count_Mutex );
		cpuSet[i].printCPUInformation();
		pthread_mutex_unlock(&count_Mutex );
	}
#endif
    setStartTime();
     /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Create thread and join
//    for(int i=0;i<numThread;i++){
//    	threadSet[i].method = 1;
//	}
	for(int i=0;i<numThread;i++){
    	pthread_create (&threadSet[i]._thread, NULL, threadSet[i].convolution, &threadSet[i]);
	}
    for(int i=0;i<numThread;i++){
    	pthread_join (threadSet[i]._thread, NULL);
	}
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    
    setEndTime();
    std::cout << "Partition Multi Thread Spend time : " << _timeUse << std::endl;
    cleanMultiResult();
}

void
System::partitionFirstFit ()
{
    std::cout << "\n===========Partition First-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_FF);
#endif
	for (int i = 0; i < CORE_NUM; i++)
		cpuSet[i].emptyCPU();

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement parititon first-fit and print result.
    for(int i=0;i<numThread;i++){
    	int q=0;
    	
    	while(cpuSet[q].utilization()+threadSet[i].utilization()>1){
    		q = q+1;
	    	if(q>CORE_NUM){
	    		threadSet[i].setCore(-1);
				std::cout<<"Thread-"<<threadSet[i].ID() << " not schedulable"<<std::endl;
	    		continue;
			}
		}
		if(q<CORE_NUM){
//			std::cout << q<<"_utilization : " << cpuSet[q].utilization() << std::endl;
	    	cpuSet[q].push_thread(threadSet[i].ID(),threadSet[i].utilization());
//	    	std::cout << q<<"_afterutilization : " << cpuSet[q].utilization() << std::endl;
			threadSet[i].setCore(q);
		}
    	
	}
    // ...
    // ...
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    partitionMultiCoreConv();
    cleanMultiResult();
}
    
void
System::partitionBestFit ()
{
    std::cout << "\n===========Partition Best-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_BF);
#endif

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement partition best-fit and print result.
    for (int i = 0; i < CORE_NUM; i++)
		cpuSet[i].emptyCPU(); // Reset the CPU set
    for(int i=0;i<numThread;i++){
    	int q=-1;
    	float maxcutilization = -1;
    	for(int j=0;j<CORE_NUM;j++){
//    		std::cout << j<<"_utilization : " << cpuSet[j].utilization() << std::endl;
			if(cpuSet[j].utilization()+threadSet[i].utilization()<=1){
				if(maxcutilization<cpuSet[j].utilization()){    			
    				maxcutilization = cpuSet[j].utilization();
    				q=j;
				}
			}
			else{
				if(j==3&&q==-1){
					std::cout<<"Thread-"<< i << " not schedulable"<<std::endl;
					threadSet[i].setCore(-1);
//					q=-1;
				}
				continue;
			}
		}
		if(q!=-1){
//			std::cout << q<<"_utilization : " << cpuSet[q].utilization() << std::endl;
    		cpuSet[q].push_thread(threadSet[i].ID(),threadSet[i].utilization());
//    		std::cout << q<<"_afterutilization : " << cpuSet[q].utilization() << std::endl;
			threadSet[i].setCore(q);
    	}
    	
	}
    // ...
    // ...
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    partitionMultiCoreConv();
    cleanMultiResult();
}

void
System::partitionWorstFit ()
{
    std::cout << "\n===========Partition Worst-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_WF);
#endif

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement partition worst-fit and print result.
    for (int i = 0; i < CORE_NUM; i++)
		cpuSet[i].emptyCPU(); // Reset the CPU set
    for(int i=0;i<numThread;i++){
    	int q=-1;
    	float mincutilization = 1;
    	for(int j=0;j<CORE_NUM;j++){
//    		std::cout << j<<"_utilization : " << cpuSet[j].utilization() << std::endl;
			if(cpuSet[j].utilization()+threadSet[i].utilization()<=1){
//				std::cout << mincutilization << "owo" <<cpuSet[j].utilization() << std::endl;
				if(mincutilization>cpuSet[j].utilization()){    			
    				mincutilization = cpuSet[j].utilization();
    				q=j;
//    				std::cout<<q<< std::endl;
				}
			}
			else{
				if(j==3&&q==-1){
					std::cout<<"Thread-"<< i << " not schedulable"<<std::endl;
					threadSet[i].setCore(-1);
//					q=-1;
				}
				continue;
			}
		}
		if(q!=-1){
//			std::cout << q<<"_utilization : " << cpuSet[q].utilization() << std::endl;
    		cpuSet[q].push_thread(threadSet[i].ID(),threadSet[i].utilization());
//    		std::cout << q<<"_afterutilization : " << cpuSet[q].utilization() << std::endl;
			threadSet[i].setCore(q);
    	}
    	
	}
    // ...
    // ...
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    
    partitionMultiCoreConv();
    cleanMultiResult();
}

void
System::cleanMultiResult ()
{
    for (int thread_id = 0; thread_id < numThread; thread_id++)
    {
        int matrix_size = threadSet[thread_id].matrixSize();
        for (int i = 0; i < matrix_size; i++)
            memset(multiResult[thread_id][i], 0, sizeof(float)*matrix_size);
    }  
}

void 
System::loadInput (char* input_file)
{
    std::ifstream infile(input_file);
    std::string line;

    int read_matrix_size = 0;
    float total_matrix_size = 0;

    if(infile.is_open()) {
        getline(infile, line);
        numThread = atoi(line.c_str());
    
            std::cout << "Input File Name : " << input_file << std::endl;
            std::cout << "numThread : " << numThread << std::endl;

        threadSet = new Thread[numThread];
            multiResult = new float**[numThread];
            matrix = new float**[numThread];
            mask = new float**[numThread];
        
        for (int i = 0; i < numThread; i++)
        {
            getline(infile, line);
            read_matrix_size = atoi(line.c_str());
                std::cout << i << ".Matrix size : " << read_matrix_size << std::endl;
            total_matrix_size += read_matrix_size;
            threadSet[i].setThreadID(i);
            threadSet[i].setMatrixSize(read_matrix_size);
                setUpMatrix(i);

        }
//        std::cout << "Workload Utilization : " << total_matrix_size / UTILIZATION_DIVIDER << std::endl;
        
        infile.close();
    } else {
//        std::cout << "!! Input file not found !!" << std::endl;
        assert(false);
    }
}

void
System::setEndTime ()
{
	gettimeofday(&end, NULL);
	_timeUse = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000.0;
}

void
System::setStartTime ()
{
	gettimeofday(&start, NULL);
}

void
System::setUpMatrix (int thread_id)
{
    int matrix_size = threadSet[thread_id].matrixSize();

    multiResult[thread_id] = new float*[matrix_size];
    for (int i = 0; i < matrix_size; i++)
        multiResult[thread_id][i] = new float[matrix_size];

    matrix[thread_id] = new float*[matrix_size];
    for (int i = 0; i < matrix_size; i++)
        matrix[thread_id][i] = new float[matrix_size];

    if((MASK_SIZE % 2) != 0)
    {
        mask[thread_id] = new float*[MASK_SIZE];
        for (int i = 0; i < MASK_SIZE; i++)
            mask[thread_id][i] = new float[MASK_SIZE];
    } else {
        std::cout << "!! Mask size not odd number !!" << std::endl;
        assert(false);
    }
    
}



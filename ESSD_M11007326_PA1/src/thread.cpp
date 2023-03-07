#include "thread.h"

Thread::~Thread()
{
  for (int i = 0; i < MASK_SIZE; i++)
    delete[] mask[i];
  
  for (int i = 0; i < _matrixSize; i++)
  {
    delete[] matrix[i];
    delete[] multiResult[i];	
  }
  delete[] mask;
  delete[] matrix;
  delete[] multiResult;

  delete check;
}

void
Thread::init (float **multi_result, float **input_matrix, float **conv_mask)
{
  _utilization = float( _matrixSize / float(UTILIZATION_DIVIDER) );

    startCalculatePoint = 0;
    endCalculatePoint = _matrixSize;

    multiResult = multi_result;
    matrix = input_matrix;
    mask = conv_mask;
}
void*
Thread::convolution(void* args)
{
  
  Thread *obj = (Thread*) args;
  
  int now =-1;
  int pre =-1;
  
  pthread_mutex_lock( &count_Mutex );
  obj->PID = syscall (SYS_gettid);

  /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  	// Set up the affinity mask
  obj->setUpCPUAffinityMask(obj->core);
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
  obj->cur_core = sched_getcpu();
  obj->printThreadInfo();
#if (PART == 3)
  obj->setUpScheduler();
  if(obj->cur_core == 0 && obj->_ID==0){
      current_PID = obj ->PID;
      std::cout << "Core0 start PID-" << current_PID << std::endl;
  }
#endif
  now = sched_getcpu();
  pre = sched_getcpu();
  pthread_mutex_unlock( &count_Mutex );
  	
  /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  // Edit the function into partial multiplication.
  // Hint : Thread::startCalculatePoint & Thread::endCalculatePoint
  
  int shift = (MASK_SIZE-1)/2;
  for (int i = obj->startCalculatePoint; i < obj->endCalculatePoint; i++)
  {
    for (int j = 0; j < obj->_matrixSize; j++)
    {
      for (int k = -shift; k <= shift; k++)
      {
        for (int l = -shift; l <= shift; l++)
        {
          if( i + k < 0 ||  i + k >= obj->_matrixSize || j + l < 0 ||  j + l >= obj->_matrixSize)
            continue;
          obj->multiResult[i][j] += obj->matrix[i+k][j+l] * obj->mask[k+shift][l+shift];
        }
      }
      /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
      // Observe the thread migration
#if (PART == 1)
      pthread_mutex_lock(&count_Mutex);
      now = sched_getcpu();
      if(now!= pre){
	        now = sched_getcpu();
		    std::cout << "The thread " << obj->_ID << " PID : " << obj->PID << " is moved from CPU" <<pre <<" to CPU" << now << std::endl;
		    pre = now;
	    }
      pthread_mutex_unlock(&count_Mutex);
#endif
      /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    }
#if (PART == 3)
    /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
    /* Obaserve the execute thread on core-0 */
    pthread_mutex_lock(&count_Mutex);
//    std::cout << "owo_" << now << "now_" << now_pid << " switch to" <<obj ->PID<< std::endl;
    
    if(now==0&&current_PID != syscall (SYS_gettid)){
    	std::cout <<"Core0 context switch form PID-"<< current_PID << " to PID-" << obj ->PID << std::endl;
    	current_PID = syscall (SYS_gettid);
	}
	pthread_mutex_unlock(&count_Mutex);
    // ...
    // ...
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#endif
  }
  /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/


	obj->check->checkCorrectness();
	pthread_mutex_unlock( &count_Mutex );

  return 0;
}

void
Thread::setUpCPUAffinityMask (int core_num)
{
 	/*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  // Pined the thread to core.
  	if(core_num == -1){
  		return;
	  }
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(core_num,&set);
	if(sched_setaffinity(0,sizeof(set),&set)==-1)
		std::cout << "WRONG ";
	cur_core = core_num;
 	
// 	setThreadCore=-1;
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}

void
Thread::setUpScheduler()
{
	/*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
  // Set up the scheduler for current thread
    struct sched_param param;
    int maxpri;
    maxpri = sched_get_priority_max(_schedulingPolicy);
    // std::cout << "setUpScheduler" <<std::endl;
    if(maxpri == -1){
    	std::cout << "!! Failed sched_get_priority_max_ !!" << std::endl;
	}
	param.sched_priority = maxpri;
	PID = syscall (SYS_gettid);
	int ret = sched_setscheduler (PID, _schedulingPolicy, &param);
    if (ret ==-1) {
        std::cout << "!! Failed sched_setscheduler_ !!" << std::endl;
    }
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}

void
Thread::printThreadInfo()
{
    std::cout << "Thread ID : " << _ID ;
    std::cout << "\tPID : " << PID;
    std::cout << "\tCore : " << cur_core;
//    std::cout << "\tPolicy : " << _schedulingPolicy;
#if (PART != 1)
    std::cout << "\tUtilization : " << _utilization;
    std::cout << "\tMatrixSize : " << _matrixSize;	
#endif
    std::cout << std::endl;
}

void
Thread::setCheck (Check* _check)
{
  check = _check;
}

void
Thread::setCore (int _core)
{
  core = _core;
}

void 
Thread::setMatrixSize (int matrixSize)
{
  _matrixSize = matrixSize;
}

void 
Thread::setEndCalculatePoint (int endPoint)
{
  endCalculatePoint = endPoint;
}

void 
Thread::setStartCalculatePoint (int startPoint)
{
  startCalculatePoint = startPoint;
}

void 
Thread::setSchedulingPolicy (int schedulingPolicy)
{
  _schedulingPolicy = schedulingPolicy;
}

void
Thread::setThreadID (int id)
{
  _ID = id;
}



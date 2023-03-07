#ifndef _CHECK_H_
#define _CHECK_H_

#include <cassert>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>

class Check{

  public:

    /* Constructure */
    Check(int = 0, int = 0, int = 0, int = 0, float** = NULL);
    void init(int, int, int, int, float**);

    /* Setup */
    void dataGenerator (float**, float**);
    
    /* Checking */
    void checkCorrectness(int);
    void createAnswer(int);

  
  private:
    /* Each part checking method */
    void compareResult ();

    /* Other */
    void singleConvolution ();

    void setEndTime ();
    void setStartTime ();
    double period () { return _timeUse; };

  private:
    int convTimes;
    int maskSize;
    int matrixSize;
    int numThread;

    bool correctnessResult;

    float **Answer;
    float **matrix;
    float **mask;
    float **multiResult;

    struct timeval start;
    struct timeval end;
    double _timeUse;

};

#endif
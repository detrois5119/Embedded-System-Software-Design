#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>

#include "config.h"
#include "../libs/check.h"
#include "thread.h"

typedef void* (*THREADFUNCPTR)(void*);

class System
{
  public:
    /* Constructrue */
    System (char*);               // Construct a new System:: System object and initial parameter.

    /* Baseline */
    void createAnswer ();         // Caculate answer inside the checker.
    void initMatrix ();           // initial matrix dimension

    /* Part 1~3 */
    void multiCoreConvolution ();

    /* Other Function */
    void printInfo ();
    void setEndTime ();
    void setStartTime ();
    double period () { return _timeUse; };

  private:
    Check* check;
    Thread* threadSet;            // list of thread

    struct timeval start;
    struct timeval end;
    double _timeUse;

    float **mask;                 // convolution mask
    float **matrix;               // data matirx
    float **multiResult;          // result matrix

};

#endif

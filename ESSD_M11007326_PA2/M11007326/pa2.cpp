#include "src/system.h"

/*****************************************************
Compiling: make part#.out 

e.g.       make part1.out 
           make part2.out 
           make part3.out 

Execute	: ./part#.out

e.g.      ./part1.out 
          ./part2.out
          ./part3.out 
******************************************************/


int main(int argc, char** argv)
{
  System sys( argv[1] );
  sys.createAnswer();  // Create answer by single thread convolution

  sys.multiCoreConvolution(); // Partition multi-thread convolution

  return 0;
}
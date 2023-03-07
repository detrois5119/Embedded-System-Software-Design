CC := g++
SRC := $(wildcard ./src/[!c]*.cpp *.cpp)
OBJ := $(patsubst %.cpp, %.o, $(SRC))
CFLAGS := -pthread -g -std=c++11
SHELL := /bin/bash
LIBS = libs/check.o

part1.out: clean 1 $(OBJ)
	@echo Building $@
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

part2.out: clean 2 $(OBJ)
	@echo Building $@
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

part3.out: clean 3 $(OBJ)
	@echo Building $@
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	@echo Building $@
	@$(CC) $(CFLAGS) -c -o $@ $<

%:
	@sed -i "/#define PART/c\#define PART $@" ./src/config.h
	@if [ $@ == 3 ]; \
	then \
	    sed -i "/#define _ProtectType/c\#define _ProtectType SPINLOCK" ./src/config.h; \
	    sed -i "/#define _SynType/c\#define _SynType BARRIER" ./src/config.h; \
	else \
	    sed -i "/#define _ProtectType/c\#define _ProtectType MUTEX" ./src/config.h; \
	    sed -i "/#define _SynType/c\#define _SynType BARRIER" ./src/config.h; \
	fi


clean:
	@rm -f *.o ./src/[!c]*.o

CC=gcc
CXX=g++
COPT= -std=c99 -O0 -g
CXXOPT= -std=c++11 -O0 -g

INCLUDES= -I../common/
NVCC=nvcc
NVCOPT= -arch=sm_20  -O0 -g -Wno-deprecated-gpu-targets $(INCLUDES)

TARGET=checkthread
OBJC=../common/sample-common.cu.o \
	checkthread.cu.o	\

.PHONY: all clean

all: $(OBJC)
	$(NVCC) $(NVCOPT) -o $(TARGET) $(OBJC)

%.c.o: %.c
	$(CC) $(COPT) -o $@ -c $<

%.cpp.o: %.cpp
	$(CXX) $(CXXOPT) -o $@ -c $<

%.cu.o : %.cu
	$(NVCC) $(NVCOPT) -o $@ -c $<

clean:
	rm $(TARGET) *.o

CC=gcc
CXX=g++
COPT= -std=c99 -O0 -g
CXXOPT= -std=c++11 -O0 -g

NVCC=nvcc
NVCOPT= -arch=sm_20  -O0 -g -Wall

TARGET=sumarray
OBJC=sumarray.cu.o
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

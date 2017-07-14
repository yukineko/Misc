CROSS_GCC=/usr/arm-linux-gnueabihf/libsf
#CROSS_GCC=/usr/lib/gcc-cross/arm-linux-gnueabihf/4.8
CC=arm-linux-gnueabihf-gcc-4.8
CPP=arm-linux-gnueabihf-g++-4.8
LIBS=
COPT= -O2 -L$(CROSS_GCC) -mfpu=neon
DUMPOPT= -S 
OBJS=sample.c.o

TARGET=sample
.PHONY: all clean dump

all: $(OBJS)
	$(CC) $(COPT) -o $(TARGET) $(OBJS) $(LIBS)

%.c.o : %.c
	$(CC) $(COPT) -c $< -o $@

dump:
	$(CC) $(COPT) $(DUMPOPT)  sample.c
clean:
	rm sample *.o

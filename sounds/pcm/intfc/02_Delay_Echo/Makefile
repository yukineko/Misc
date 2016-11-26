OBJS=echo_delay.c.o \
	../common/wavheader.c.o \
	../common/wavpcm.c.o \
	../common/util.c.o \

CFLAGS=-Wall -O0 -g
TARGET=echo_delay

%.c.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


OBJS=through.c.o \
	../common/wavheader.c.o \
	../common/wavpcm.c.o \

CFLAGS=-Wall -O0 -g
TARGET=through

%.c.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


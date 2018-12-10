TARGET=base
TARGETS=$(TARGET).asm.o test1.asm.o
#TARGETS=$(TARGET).asm.o
FLAGS=-felf64
ASM=nasm

%.asm.o : %.asm
	$(ASM) $(FLAGS) $< -o $@


all: $(TARGETS)
	ld -o $(TARGET) $^

clean:
	rm *.o

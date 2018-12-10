TARGET=print_rax
TARGETS=print_rax.asm.o
FLAGS=-felf64
ASM=nasm

%.asm.o : %.asm
	$(ASM) $(FLAGS) $< -o $@


all: $(TARGETS)
	ld -o $(TARGET) $(TARGETS)

clean:
	rm *.o

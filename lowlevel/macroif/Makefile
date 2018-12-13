TARGET=macroif
SRC=macroif.asm
TARGETS=$(SRC:%.asm=%.asm.o)
FLAGS=-felf64
ASM=nasm

%.asm.o : %.asm
	$(ASM) $(FLAGS) $< -o $@


all: $(TARGETS)
	ld -o $(TARGET) $(TARGETS)

expand: 
	$(ASM) -E $(SRC)
clean:
	rm *.o

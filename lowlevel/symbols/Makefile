TARGET=symbols
TARGETS=symbols.asm.o
FLAGS=-felf64
ASM=nasm
LD=ld
%.asm.o : %.asm
	$(ASM) $(FLAGS) $< -o $@


all: $(TARGETS)
	$(LD) -o $(TARGET) $(TARGETS)

clean:
	rm *.o

dump:
	objdump -tr -m intel $(TARGETS)

asm:
	objdump -D -M intel-mnemonic $(TARGETS)

relocs:
	readelf --relocs $(TARGETS)

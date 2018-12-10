TARGET=executable
TARGETS=$(TARGET).asm.o symbols.asm.o
FLAGS=-felf64
ASM=nasm
LD=ld
#LDOPT=--strip-all
%.asm.o : %.asm
	$(ASM) $(FLAGS) $< -o $@


all: $(TARGETS)
	$(LD) $(LDOPT) $(TARGETS) -o $(TARGET) 

clean:
	rm *.o

dump:
	objdump -tr -m intel $(TARGET)

asm:
	objdump -D -M intel-mnemonic $(TARGETS)

relocs:
	readelf --relocs $(TARGETS)

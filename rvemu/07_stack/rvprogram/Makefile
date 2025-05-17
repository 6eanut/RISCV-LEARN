CC=riscv64-unknown-elf-gcc
READELF=riscv64-unknown-elf-readelf
QEMU=qemu-riscv64
OBJDUMP=riscv64-unknown-elf-objdump

SRC=rvprogram.c
OBJ=rvprogram

$(OBJ):$(SRC)
	$(CC) $< -o $@ -static

all:$(OBJ)
	$(QEMU) ./$(OBJ)
	$(READELF) -a $(OBJ) > readelf.txt
	$(OBJDUMP) -S $(OBJ) > code.txt

clean:
	rm $(OBJ)

.PHONY: clean all
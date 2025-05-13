CC=riscv64-linux-gnu-gcc
READELF=riscv64-linux-gnu-readelf
QEMU=qemu-riscv64
OBJDUMP=riscv64-linux-gnu-objdump

QFLAGS=-L /usr/riscv64-linux-gnu


SRC=rvprogram.c
OBJ=rvprogram

$(OBJ):$(SRC)
	$(CC) $< -o $@

all:$(OBJ)

clean:
	rm $(OBJ)

readelf:all
	$(READELF) -a $(OBJ) > readelf.txt

run:all
	$(QEMU) $(QFLAGS) ./$(OBJ)

code:all
	$(OBJDUMP) -S $(OBJ) > code.txt


.PHONY: clean all readelf code
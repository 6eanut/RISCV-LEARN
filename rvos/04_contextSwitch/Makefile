OUT_PATH = out
C_PATH = c_code/src
S_PATH = s_code

CC = riscv64-unknown-elf-gcc 
CFLAGS = -nostdlib -fno-builtin -g -Wall -march=rv32g -mabi=ilp32
LDFLAGS = -T ${OUT_PATH}/os.ld.generated 

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 1 -machine virt -bios none

GDB = gdb-multiarch

OBJDUMP = riscv64-unknown-elf-objdump

NM = riscv64-unknown-elf-nm

LSFLAGS = -E -P -x c

OBJS_C := $(patsubst $(C_PATH)/%.c, $(OUT_PATH)/%.o, $(wildcard $(C_PATH)/*.c))
OBJS_S := $(patsubst $(S_PATH)/%.S, $(OUT_PATH)/%.o, $(wildcard $(S_PATH)/*.S))
OBJS = $(OBJS_S) $(OBJS_C)  
ELF = out/os.elf

.DEFAULT_GOAL := all

${OUT_PATH}:
	mkdir -p $@

all : ${OUT_PATH} ${ELF}

${OUT_PATH}/%.o:${C_PATH}/%.c
	${CC} ${CFLAGS} -c -o $@ $<

${OUT_PATH}/%.o:${S_PATH}/%.S
	${CC} ${CFLAGS} -c -o $@ $<

${ELF}:${OBJS}
	${CC} ${LSFLAGS} os.ld > ${OUT_PATH}/os.ld.generated
	${CC} ${CFLAGS} ${LDFLAGS} -o ${ELF} ${OBJS}

run : all
	${QEMU} ${QFLAGS} -kernel ${ELF}

debug : all
	${QEMU} ${QFLAGS} -kernel ${ELF} -s -S &
	${GDB} ${ELF} -q -x ./gdbinit

code : all
	${OBJDUMP} -S ${ELF} 

# list symbols from object files
nm : all
	${NM} ${ELF}

code_rodata : all
	${OBJDUMP} -S -j .rodata ${ELF} 

code_data : all
	${OBJDUMP} -S -j .data ${ELF} 

code_bss : all
	${OBJDUMP} -S -j .bss ${ELF} 

clean:
	rm -rf ${OUT_PATH} ${ELF}
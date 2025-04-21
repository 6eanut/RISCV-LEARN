CC = riscv64-unknown-elf-gcc 
CFLAGS = -nostdlib -fno-builtin -g -Wall -march=rv32g -mabi=ilp32 
LDFLAGS = -Ttext=0x80000000

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 1 -machine virt -bios none

OUT_PATH = out
C_PATH = c_code
S_PATH = s_code


OBJS_C := $(addprefix ${OUTPUT_PATH}/, $(patsubst %.c, %.o, ${C_PATH}/src/%.c))
OBJS_S := $(addprefix ${OUTPUT_PATH}/, $(patsubst %.S, %.o, ${S_PATH}/%.S))
OBJS = $(OBJS_C)  $(OBJS_S)
ELF = out/os.elf

${OUT_PATH}/%.o:${C_PATH}/%.c
	${CC} ${CFLAGS} -o $@ $<

${OUT_PATH}/%.o:${S_PATH}/%.S
	${CC} ${CFLAGS} -o $@ $<

${ELF}:${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${ELF} ${OBJS}

run:all
	@${QEMU} ${QFLAGS} -kernel ${ELF}
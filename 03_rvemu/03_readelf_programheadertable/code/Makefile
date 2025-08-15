CC=clang
CFLAGS=-O3 -Wall -Werror -Wimplicit-fallthrough

SRCS=$(wildcard src/*.c) 
HDRS=$(wildcard include/*.h)
OBJS=$(patsubst src/%.c, obj/%.o, $(SRCS))

rvemu:$(OBJS)
	$(CC) $(CFLAGS) -lm -o $@ $^

$(OBJS):obj/%.o:src/%.c $(HDRS)
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf rvemu obj/

run:rvemu
	./rvemu ../rvprogram/rvprogram

.PHONY: clean run
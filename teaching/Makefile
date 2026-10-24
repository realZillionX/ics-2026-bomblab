CC ?= gcc
CFLAGS ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic -fno-omit-frame-pointer -fno-stack-protector -fno-pie -no-pie

PROGRAMS = 01_abi 02_loop_array 03_jump_table 04_recursion 05_linked_list 06_indirect_call 07_stack_copy

.PHONY: all asm clean

all: $(PROGRAMS)

01_abi: 01_abi.c
02_loop_array: 02_loop_array.c
03_jump_table: 03_jump_table.c
04_recursion: 04_recursion.c
05_linked_list: 05_linked_list.c
06_indirect_call: 06_indirect_call.c
07_stack_copy: 07_stack_copy.c

$(PROGRAMS):
	$(CC) $(CFLAGS) $@.c -o $@

asm: $(PROGRAMS)
	@for program in $(PROGRAMS); do \
		objdump -d --no-show-raw-insn $$program > $$program.s; \
	done

clean:
	rm -f $(PROGRAMS) $(PROGRAMS:%=%.s)

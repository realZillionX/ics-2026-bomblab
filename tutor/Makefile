CC ?= gcc
CFLAGS ?= -std=c11 -O2 -g0 -Wall -Wextra -Wpedantic -Werror -fno-omit-frame-pointer -fno-pie -no-pie
CPPFLAGS += -I..
SHARED_SOURCES = ../challenge.c ../cli.c
HEADERS = ../challenge.h ../cli.h ../defuser.h ../phase_support.h

.PHONY: all test clean
all: bomb defuser phase0.disasm.txt phase0-input.txt phase0-answer.txt

bomb: main.c phase0.c ../phase_support.c $(SHARED_SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c phase0.c ../phase_support.c $(SHARED_SOURCES) -o $@

defuser: main.c phase0_solution.c $(SHARED_SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -DTUTOR_SOLVER main.c phase0_solution.c $(SHARED_SOURCES) -o $@

phase0.disasm.txt: bomb
	objdump -d --disassemble=phase_00 bomb > $@

phase0-input.txt:
	printf '0 0\n' > $@

phase0-answer.txt: defuser
	./defuser --seed 42 > $@

test: all
	python3 ../public_test.py --phase 0

clean:
	rm -f bomb defuser phase0.disasm.txt phase0-input.txt phase0-answer.txt

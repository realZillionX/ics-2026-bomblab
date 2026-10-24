CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = defuser_main.c defuser.c tutor/phase0_solution.c challenge.c cli.c
CPPFLAGS += -I.

.PHONY: all tutor test test-tutor clean
all: defuser

# 按 config.txt 中的学号下载个人 bomb（已存在时不重复下载）
bomb:
	python3 fetch_bomb.py

defuser: $(SOURCES) defuser.h challenge.h cli.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

tutor:
	$(MAKE) -C tutor

test: defuser bomb
	python3 public_test.py

test-tutor:
	$(MAKE) -C tutor test

clean:
	rm -f defuser
	$(MAKE) -C tutor clean

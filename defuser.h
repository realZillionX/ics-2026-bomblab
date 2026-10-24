#ifndef PARAMETRIC_DEFUSER_H
#define PARAMETRIC_DEFUSER_H

#include <stddef.h>
#include <stdint.h>

#define ANSWER_CAP 256

int solve_phase_0(uint64_t challenge, char *out, size_t cap);
int solve_phase_1(uint64_t challenge, char *out, size_t cap);
int solve_phase_2(uint64_t challenge, char *out, size_t cap);
int solve_phase_3(uint64_t challenge, char *out, size_t cap);
int solve_phase_4(uint64_t challenge, char *out, size_t cap);
int solve_phase_5(uint64_t challenge, char *out, size_t cap);
int solve_phase_6(uint64_t challenge, char *out, size_t cap);
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap);

#endif

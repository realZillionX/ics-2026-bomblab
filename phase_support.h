#ifndef PARAMETRIC_PHASE_SUPPORT_H
#define PARAMETRIC_PHASE_SUPPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool valid_u32_fields(const char *input, size_t count);
bool phase_00(const char *input, uint64_t challenge);
bool phase_01(const char *input, uint64_t challenge);
bool phase_02(const char *input, uint64_t challenge);
bool phase_03(const char *input, uint64_t challenge);
bool phase_04(const char *input, uint64_t challenge);
bool phase_05(const char *input, uint64_t challenge);
bool phase_06(const char *input, uint64_t challenge);

#endif

#include "phase_support.h"

#include <inttypes.h>
#include <stdio.h>

__attribute__((noinline)) uint32_t tutorial_mix(uint32_t value) {
    return value * UINT32_C(3) + UINT32_C(7);
}

__attribute__((noinline)) bool phase_00(const char *input, uint64_t challenge) {
    uint32_t values[2];
    if (!valid_u32_fields(input, 2)) return false;
    if (sscanf(input, "%" SCNu32 " %" SCNu32, &values[0], &values[1]) != 2) return false;
    uint32_t first = (uint32_t)((challenge >> 5) & UINT64_C(255)) ^ UINT32_C(42);
    if (values[0] != first) return false;
    return values[1] == tutorial_mix(first);
}

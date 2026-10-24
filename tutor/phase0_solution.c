#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

int solve_phase_0(uint64_t challenge, char *out, size_t cap) {
    uint32_t first = (uint32_t)((challenge >> 5) & UINT64_C(255)) ^ UINT32_C(42);
    uint32_t second = first * UINT32_C(3) + UINT32_C(7);
    int size = snprintf(out, cap, "%" PRIu32 " %" PRIu32, first, second);
    return size < 0 || (size_t)size >= cap;
}

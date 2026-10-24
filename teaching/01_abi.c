#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((noinline)) uint32_t add_scaled(uint32_t value, uint32_t bias) {
    return value + UINT32_C(3) * bias + UINT32_C(7);
}

__attribute__((noinline)) uint32_t combine(uint64_t challenge) {
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    return add_scaled(low, high);
}

int main(void) {
    printf("%" PRIu32 "\n", combine(UINT64_C(0x1122334455667788)));
    return 0;
}

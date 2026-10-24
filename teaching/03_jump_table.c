#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((noinline)) uint32_t dispatch(unsigned operation, uint32_t value) {
    switch (operation & 7U) {
        case 0: return value + 10U;
        case 1: return value ^ UINT32_C(0x55);
        case 2: return value * 3U;
        case 3: return value - 7U;
        case 4: return (value << 2) | (value >> 30);
        case 5: return value + UINT32_C(0x1234);
        case 6: return value ^ (value >> 3);
        case 7: return value * 5U + 1U;
        default: return 0;
    }
}

int main(void) {
    printf("%" PRIu32 "\n", dispatch(3, UINT32_C(100)));
    return 0;
}

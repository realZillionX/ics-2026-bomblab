#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((noinline)) unsigned empty_loop(unsigned count) {
    volatile unsigned observed = 0;
    for (unsigned index = 0; index < count; ++index) {
        observed += index & 0U;
    }
    return observed;
}

__attribute__((noinline)) uint32_t array_sum(const uint32_t *values, size_t count) {
    uint32_t total = 0;
    for (size_t index = 0; index < count; ++index) total += values[index];
    return total;
}

int main(void) {
    const uint32_t values[] = {3, 5, 7, 11};
    printf("loop=%u sum=%" PRIu32 "\n", empty_loop(4), array_sum(values, 4));
    return 0;
}

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t (*Operation)(uint32_t);

__attribute__((noinline)) uint32_t add_one(uint32_t value) { return value + 1U; }
__attribute__((noinline)) uint32_t double_value(uint32_t value) { return value * 2U; }
__attribute__((noinline)) uint32_t xor_key(uint32_t value) { return value ^ UINT32_C(0xa5); }

static Operation const operations[] = {add_one, double_value, xor_key};

__attribute__((noinline)) uint32_t apply(unsigned first, unsigned second, uint32_t value) {
    value = operations[first % 3U](value);
    return operations[second % 3U](value);
}

int main(void) {
    printf("%" PRIu32 "\n", apply(1, 2, 10));
    return 0;
}

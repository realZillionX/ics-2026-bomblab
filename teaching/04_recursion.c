#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((noinline, optimize("no-optimize-sibling-calls")))
uint32_t search_path(uint32_t target, uint32_t low, uint32_t high) {
    uint32_t middle = low + (high - low) / 2U;
    if (target == middle) return middle;
    if (target < middle) return search_path(target, low, middle - 1U) * 3U + 1U;
    return search_path(target, middle + 1U, high) * 3U + 2U;
}

int main(void) {
    printf("%" PRIu32 "\n", search_path(37, 0, 63));
    return 0;
}

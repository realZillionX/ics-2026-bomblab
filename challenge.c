#include "challenge.h"

uint64_t challenge_from_seed(uint64_t seed, unsigned phase) {
    uint64_t x = seed + UINT64_C(0x9e3779b97f4a7c15) * ((uint64_t)phase + 1);
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static uint64_t context_mix(uint64_t value) {
    value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31);
}

uint64_t transcript_context(const char *const answers[6]) {
    uint64_t context = UINT64_C(0x243f6a8885a308d3);
    for (unsigned phase = 1; phase <= 6; ++phase) {
        context = context_mix(context ^ phase);
        const unsigned char *cursor = (const unsigned char *)answers[phase - 1];
        unsigned field = 0;
        while (*cursor) {
            while (*cursor && (*cursor < '0' || *cursor > '9')) ++cursor;
            if (!*cursor) break;
            uint64_t value = 0;
            while (*cursor >= '0' && *cursor <= '9') {
                value = value * 10 + (*cursor - '0');
                ++cursor;
            }
            context = context_mix(context ^ value ^ (((uint64_t)phase << 32) | field));
            ++field;
        }
    }
    return context;
}

#include "phase_support.h"

#include <ctype.h>

bool valid_u32_fields(const char *input, size_t count) {
    const unsigned char *cursor = (const unsigned char *)input;
    for (size_t i = 0; i < count; ++i) {
        while (isspace(*cursor)) ++cursor;
        if (*cursor < '0' || *cursor > '9') return false;
        uint32_t value = 0;
        do {
            uint32_t digit = *cursor - '0';
            if (value > (UINT32_MAX - digit) / 10) return false;
            value = value * 10 + digit;
            ++cursor;
        } while (*cursor >= '0' && *cursor <= '9');
        if (*cursor && !isspace(*cursor)) return false;
    }
    while (isspace(*cursor)) ++cursor;
    return *cursor == '\0';
}

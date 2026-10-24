#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct Frame {
    char scratch[16];
    uint32_t marker;
    char answer[24];
};

__attribute__((noinline)) uint32_t copy_demo(const char *source, size_t length) {
    struct Frame frame = {0};
    for (size_t index = 0; index < length; ++index)
        ((char *)&frame.scratch[0])[index] = source[index];
    return frame.marker;
}

int main(void) {
    static const char payload[] = "AAAAAAAAAAAAAAAADEMO";
    printf("scratch=%zu marker=%zu answer=%zu marker_value=%" PRIu32 "\n",
           offsetof(struct Frame, scratch), offsetof(struct Frame, marker),
           offsetof(struct Frame, answer), copy_demo(payload, sizeof(payload) - 1));
    return 0;
}

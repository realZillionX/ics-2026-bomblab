#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

struct Node {
    uint32_t value;
    uint32_t tag;
    const struct Node *next;
};

static const struct Node tail = {17, 900, NULL};
static const struct Node middle = {11, 700, &tail};
static const struct Node head = {5, 500, &middle};

__attribute__((noinline)) uint32_t walk(const struct Node *node, uint32_t *last_tag) {
    uint32_t total = 0;
    while (node != NULL) {
        total += node->value;
        *last_tag = node->tag;
        node = node->next;
    }
    return total;
}

int main(void) {
    uint32_t tag = 0;
    uint32_t total = walk(&head, &tag);
    printf("sum=%" PRIu32 " tag=%" PRIu32 "\n", total, tag);
    return 0;
}

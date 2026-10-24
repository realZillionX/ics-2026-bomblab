#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t w0 = (uint32_t)challenge;
    uint32_t w1 = (uint32_t)(challenge >> 16);
    uint32_t w2 = (uint32_t)(challenge >> 32);
    uint32_t s1 = (w2 + UINT32_C(2) * w1) ^ UINT32_C(0xd15c);
    uint32_t s0 = UINT32_C(2) * w0 + (w2 ^ UINT32_C(0x0aac));
    values[0] = (UINT32_C(5) * s0 + s1) ^ UINT32_C(0xe4a1);
    return write_answer(out, cap, values, 1);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    values[0] = (((uint32_t)(challenge >> 41) & UINT32_C(0xff)) * UINT32_C(2)) + UINT32_C(4);
    for (uint32_t i = 1; i < 6; ++i) {
        unsigned shift = 18U + 4U * (i - 1U);
        uint32_t step = ((uint32_t)(challenge >> shift) & UINT32_C(0x3f)) ^ i;
        values[i] = UINT32_C(2) * values[i - 1] + step;
    }
    return write_answer(out, cap, values, 6);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t op = (uint32_t)challenge & UINT32_C(7);
    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t res = 0;
    switch (op) {
        case 0: res = UINT32_C(5) * x + UINT32_C(0xa588); break;
        case 1: res = (UINT32_C(4) * x) ^ UINT32_C(0xbd13); break;
        case 2: res = UINT32_C(5) * x + UINT32_C(0xccce); break;
        case 3: res = (x ^ UINT32_C(0x2801)) + (x >> 1); break;
        case 4: res = (x ^ UINT32_C(0xb783)) + (x >> 1); break;
        case 5: res = UINT32_C(5) * x + UINT32_C(0x04d3); break;
        case 6: res = UINT32_C(5) * x + UINT32_C(0x3820c); break;
        case 7: res = (UINT32_C(4) * x) ^ UINT32_C(0x33f5); break;
    }
    values[0] = op;
    values[1] = res;
    return write_answer(out, cap, values, 2);
}

static uint32_t p4_walk_solve(uint32_t target, uint32_t low, uint32_t high, uint32_t key) {
    uint32_t mid = low + ((high - low) >> 1);
    if (target == mid) {
        return (mid + UINT32_C(0xd0)) ^ key;
    }
    uint32_t mask = mid + key;
    if (target < mid) {
        uint32_t sub = p4_walk_solve(target, low, mid - UINT32_C(1), key);
        return ((UINT32_C(3) * sub) ^ mask) ^ UINT32_C(0x17);
    } else {
        uint32_t sub = p4_walk_solve(target, mid + UINT32_C(1), high, key);
        return ((UINT32_C(3) * sub) ^ mask) ^ UINT32_C(0x1b);
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t target = (uint32_t)challenge & UINT32_C(0x7f);
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    values[0] = p4_walk_solve(target, 0U, 127U, key);
    return write_answer(out, cap, values, 1);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    static const struct {
        uint32_t tag;
        uint32_t val;
    } chain[12] = {
        {UINT32_C(0x0125), UINT32_C(0x7964)},
        {UINT32_C(0x0816), UINT32_C(0x1daf)},
        {UINT32_C(0x0a10), UINT32_C(0xe5c2)},
        {UINT32_C(0x0001), UINT32_C(0x7ab6)},
        {UINT32_C(0x093b), UINT32_C(0x3598)},
        {UINT32_C(0x050a), UINT32_C(0xb590)},
        {UINT32_C(0x0309), UINT32_C(0x8c86)},
        {UINT32_C(0x0b5f), UINT32_C(0x1004)},
        {UINT32_C(0x021f), UINT32_C(0xe3ea)},
        {UINT32_C(0x0482), UINT32_C(0x7663)},
        {UINT32_C(0x0714), UINT32_C(0xa6b3)},
        {UINT32_C(0x065a), UINT32_C(0x2998)},
    };
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t head_idx = (uint32_t)challenge & UINT32_C(7);
    uint32_t steps = (uint32_t)(challenge >> 3) & UINT32_C(3);
    uint32_t acc = ((uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32)) ^ UINT32_C(0x62dc);
    uint32_t last_tag = 0;
    for (uint32_t s = 0; s <= steps; ++s) {
        uint32_t idx = head_idx + s;
        last_tag = chain[idx].tag;
        acc = ((acc + chain[idx].val) * UINT32_C(5)) ^ last_tag;
    }
    values[0] = acc;
    values[1] = last_tag;
    return write_answer(out, cap, values, 2);
}

static uint32_t p6_apply_op(uint32_t idx, uint32_t x) {
    switch (idx & 7U) {
        case 0:
            return ((x << 5) | (x >> 27)) ^ UINT32_C(0xb6a4);
        case 1:
        case 5:
            return (x ^ UINT32_C(0x8877)) + UINT32_C(2) * x;
        case 2:
            return (x + UINT32_C(0x4df1)) ^ (x >> 2);
        case 3:
        case 7:
            return ((x >> 1) ^ x) + UINT32_C(0x945c);
        case 4:
            return (x >> 3) | (x << 29);
        case 6:
        default:
            return ((x << 7) + UINT32_C(0xb4fc)) ^ (UINT32_C(7) * x);
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    uint32_t idx1 = (uint32_t)challenge & UINT32_C(7);
    uint32_t idx2 = (uint32_t)(challenge >> 3) & UINT32_C(7);
    x = p6_apply_op(idx1, x) ^ UINT32_C(0x57c4);
    x = p6_apply_op(idx2, x) + UINT32_C(0x2716);
    values[0] = x;
    return write_answer(out, cap, values, 1);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t v0 = (uint32_t)context & UINT32_C(3);
    uint32_t c_fold = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t ctx_fold = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t mix_in = c_fold ^ ctx_fold;
    uint32_t mixed = ((mix_in >> 5) ^ UINT32_C(0xa10e)) + (mix_in ^ UINT32_C(0x6a66));
    uint32_t mask = (v0 * UINT32_C(0x6a66)) ^ (uint32_t)(context >> 16);
    uint32_t v1 = mixed ^ mask;
    uint32_t v2 = v1 ^ (ctx_fold + v0 + UINT32_C(0xa10e) + (c_fold >> 5));
    int size = snprintf(out, cap, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN %" PRIu32 " %" PRIu32 " %" PRIu32,
                        v0, v1, v2);
    return size < 0 || (size_t)size >= cap;
}

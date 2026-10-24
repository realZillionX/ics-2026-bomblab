#include "cli.h"
#include "challenge.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int number(const char *text, uint64_t *out) {
    if (text[0] < '0' || text[0] > '9') return 0;
    int base = text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16 : 10;
    char *end = NULL;
    errno = 0;
    uintmax_t value = strtoumax(text, &end, base);
    if (errno || end == text || *end || value > UINT64_MAX) return 0;
    *out = (uint64_t)value;
    return 1;
}

int parse_options(int argc, char **argv, struct options *options) {
    uint64_t seed = 1, raw = 0, phase = ALL_PHASES;
    unsigned seen = 0;
    for (int index = 1; index < argc; ++index) {
        const char *arg = argv[index];
        if (strcmp(arg, "--help") == 0 && argc == 2) {
            puts("Usage: [--seed UINT64] [--phase 0..6]");
            puts("       --phase 0..6 --challenge UINT64");
            puts("       --secret [--seed UINT64 | --challenge UINT64]");
            puts("Default: phases 1 through 6, seed 1. Phase 0 is the optional tutorial.");
            return 1;
        }
        if (strcmp(arg, "--secret") == 0) {
            if (seen & 8) {
                fputs("Invalid, duplicate, or missing option. Use --help.\n", stderr);
                return -1;
            }
            seen |= 8;
            continue;
        }
        unsigned flag = strcmp(arg, "--seed") == 0 ? 1 :
                        strcmp(arg, "--challenge") == 0 ? 2 :
                        strcmp(arg, "--phase") == 0 ? 4 : 0;
        uint64_t value = 0;
        if (!flag || (seen & flag) || ++index == argc || !number(argv[index], &value)) {
            fputs("Invalid, duplicate, or missing option. Use --help.\n", stderr);
            return -1;
        }
        seen |= flag;
        if (flag == 1) seed = value;
        if (flag == 2) raw = value;
        if (flag == 4) phase = value;
    }
    if ((seen & 3) == 3 || ((seen & 4) && phase > 6) ||
        ((seen & 2) && !(seen & 4) && !(seen & 8)) ||
        ((seen & 8) && (seen & 4))) {
        fputs("Choose phase 0..6; --challenge requires a phase unless --secret is used, and excludes --seed.\n", stderr);
        return -1;
    }
    options->phase = (unsigned)phase;
    options->seed = seed;
    options->challenge = raw;
    options->direct_challenge = (seen & 2) != 0;
    options->secret_solver = (seen & 8) != 0;
    return 0;
}

uint64_t phase_challenge(const struct options *options, unsigned phase) {
    return options->direct_challenge ? options->challenge : challenge_from_seed(options->seed, phase);
}

#ifndef PARAMETRIC_CLI_H
#define PARAMETRIC_CLI_H

#include <stdint.h>

#define ALL_PHASES 7

struct options {
    unsigned phase;
    uint64_t seed;
    uint64_t challenge;
    int direct_challenge;
    int secret_solver;
};

/* Returns 0 on success, 1 after help, and -1 on an invalid command. */
int parse_options(int argc, char **argv, struct options *options);
uint64_t phase_challenge(const struct options *options, unsigned phase);

#endif

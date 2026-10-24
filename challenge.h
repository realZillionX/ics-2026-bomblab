#ifndef PARAMETRIC_CHALLENGE_H
#define PARAMETRIC_CHALLENGE_H

#include <stdint.h>
#include <stddef.h>

#define CHALLENGE_PROTOCOL "splitmix64-phase-v1"
uint64_t challenge_from_seed(uint64_t seed, unsigned phase);
uint64_t transcript_context(const char *const answers[6]);

#endif

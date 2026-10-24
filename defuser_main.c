#include "cli.h"
#include "challenge.h"
#include "defuser.h"

#include <stdio.h>
#include <string.h>

static int solve_selected(const struct options *options, char outputs[6][ANSWER_CAP], int emit) {
    int (*const solvers[])(uint64_t, char *, size_t) = {
        solve_phase_0, solve_phase_1, solve_phase_2, solve_phase_3,
        solve_phase_4, solve_phase_5, solve_phase_6
    };
    char tutorial[ANSWER_CAP];
    unsigned start = options->phase == ALL_PHASES ? 1 : options->phase;
    unsigned end = options->phase == ALL_PHASES ? 6 : options->phase;
    for (unsigned phase = start; phase <= end; ++phase) {
        char *out = phase == 0 ? tutorial : outputs[phase - 1];
        memset(out, 0xff, ANSWER_CAP);
        int result = solvers[phase](phase_challenge(options, phase), out, ANSWER_CAP);
        if (result != 0) {
            fprintf(stderr, "Phase %u solver is incomplete or failed.\n", phase);
            return 3;
        }
        if (!memchr(out, '\0', ANSWER_CAP) || !out[0] || strpbrk(out, "\r\n")) {
            fputs("Solver must produce one nonempty NUL-terminated line.\n", stderr);
            return 3;
        }
        if (emit) puts(out);
    }
    return 0;
}

int main(int argc, char **argv) {
    struct options options;
    int parsed = parse_options(argc, argv, &options);
    if (parsed != 0) return parsed < 0 ? 2 : 0;
    char outputs[6][ANSWER_CAP];
    if (options.secret_solver) {
        struct options all = options;
        all.phase = ALL_PHASES;
        if (solve_selected(&all, outputs, 0) != 0) return 3;
        const char *const lines[6] = {
            outputs[0], outputs[1], outputs[2], outputs[3], outputs[4], outputs[5]
        };
        char secret[ANSWER_CAP];
        memset(secret, 0xff, sizeof(secret));
        int result = solve_secret(phase_challenge(&options, 7), transcript_context(lines),
                                  secret, sizeof(secret));
        if (result != 0) {
            fputs("Secret solver is incomplete or failed.\n", stderr);
            return 3;
        }
        if (!memchr(secret, '\0', sizeof(secret)) || !secret[0] || strpbrk(secret, "\r\n")) {
            fputs("Secret solver must produce one nonempty NUL-terminated line.\n", stderr);
            return 3;
        }
        puts(secret);
        return 0;
    }
    if (solve_selected(&options, outputs, 1) != 0) return 3;
    return 0;
}

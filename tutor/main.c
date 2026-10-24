#include "challenge.h"
#include "cli.h"
#include "defuser.h"
#include "phase_support.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts("Usage: [--seed UINT64] [--phase 0]");
        puts("       --phase 0 --challenge UINT64");
        puts("Standalone tutor program; default seed 1.");
        return 0;
    }
    struct options options;
    int parsed = parse_options(argc, argv, &options);
    if (parsed != 0) return parsed < 0 ? 2 : 0;
    if (options.secret_solver || (options.phase != ALL_PHASES && options.phase != 0)) {
        fputs("The tutor program only supports --phase 0. Use --help.\n", stderr);
        return 2;
    }
    uint64_t challenge = phase_challenge(&options, 0);
#ifdef TUTOR_SOLVER
    char output[ANSWER_CAP];
    memset(output, 0xff, sizeof(output));
    if (solve_phase_0(challenge, output, sizeof(output)) != 0 ||
        !memchr(output, '\0', sizeof(output)) || !output[0] || strpbrk(output, "\r\n")) {
        fputs("Tutorial solver failed.\n", stderr);
        return 3;
    }
    puts(output);
#else
    printf("Phase 0 challenge: 0x%016" PRIx64 "\n", challenge);
    puts("Input (unsigned decimal integers):");
    fflush(stdout);
    char input[ANSWER_CAP];
    size_t used = 0;
    int byte;
    while ((byte = getchar()) != EOF && byte != '\n') {
        if (byte == 0 || used + 1 >= sizeof(input)) {
            puts("BOOM!!!");
            return 1;
        }
        input[used++] = (char)byte;
    }
    input[used] = '\0';
    if (ferror(stdin) || !phase_00(input, challenge)) {
        puts("BOOM!!!");
        return 1;
    }
    puts("Phase 0 defused.");
#endif
    return 0;
}

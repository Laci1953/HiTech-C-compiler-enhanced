#include <stdio.h>

#define bool char
#define true  1
#define false 0

extern char key_f;

int heapmain(int argc, char **argv) 
{
    --argc, ++argv; /* would be cleaner as a for loop */

    while (0 < argc && argv[0][0] == '-') {
        switch (argv[0][1]) {
            case 'F':
            case 'f':
                key_f = true;
                break; /* use inline frame initialisation */

            default:
                pr_error("Illegal switch");
                break;
        }
        ++argv, --argc;
    }

    if (argc > 0) {
        if (!freopen(*argv, "r", stdin)) {
            pr_error("Can't open %s", *argv);
        }

        if (argc > 1 && !freopen(*(argv + 1), "w", stdout)) {
            pr_error("Can't create %s", *(argv + 1));
        }
    }
}

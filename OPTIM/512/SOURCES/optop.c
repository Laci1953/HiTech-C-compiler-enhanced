/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include <stdio.h>
#include <string.h>

#include "optim1.h"

void topmain(int argc, char **argv) 
{
    if (!InitOverlays())
	pr_error("Can't open overlays");

    Init512Banks();

    --argc, ++argv; /* would be cleaner as a for loop */

    while (0 < argc && argv[0][0] == '-') 
    {
        switch (argv[0][1]) 
	{
#ifdef STATISTICS
            case 'N':
            case 'n':
                key_n = true;
                break; /* Enables statistics printing */
#endif
            case 'L':
            case 'l':
                key_l = true;
                break; /* Enables printing of additional information */

            case 'R':
            case 'r':
                key_r = true;
                break; /* Disables register load optimisation */

            case 'F':
            case 'f':
                key_f = true;
                break; /* use inline frame initialisation */

            case 's':
                key_s = true;
                break; /* Unbuffered stdout */
            default:
                pr_error("Illegal switch %c", argv[0][1]);
                break;
        }

        ++argv, --argc;
    }

    if (argc > 0) 
    {
        if (!freopen(*argv, "r", stdin)) 
            pr_error("Can't open %s", *argv);

        if (argc > 1 && !freopen(*(argv + 1), "w", stdout))
            pr_error("Can't create %s", *(argv + 1));
    }

    if (key_s) 
        setbuf(stdout, 0);    /* Unbuffered stdout (Turns off buffering) */
}

/**************************************************************************
 8	pr_error	sub-0941	ok++
 **************************************************************************/
void pr_error(fmt, p2, p3) char * fmt;
{
    pr_message(fmt, p2, p3);
    fclose(stdout);
    quit(1);
}

/**************************************************************************
 10	pr_message	sub_0994	ok++
 **************************************************************************/
void pr_message(fmt, p2, p3) char * fmt;
{
    fprintf(stderr, fmt, p2, p3);
    fputc('\n', stderr);
}

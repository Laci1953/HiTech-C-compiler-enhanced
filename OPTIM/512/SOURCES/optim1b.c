/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include <stdio.h>
#include <string.h>

#include "optim1.h"

/**************************************************************************
 18	syntheticLabel	sub_0ca2	ok++		Used in: 
 **************************************************************************/
inst_t *syntheticLabel(register inst_t *pi) {

#ifdef DEBUG
    pr_warning("syntheticLabel");
#endif

    pi                = allocInst(pi);
    pi->iPSym         = allocBlankSym();
    pi->iSymId        = ++symbolId;
    pi->iPSym->tPInst = pi;
    pi->type          = T_SYMBOL;
    return pi;
}

/**************************************************************************
19  optimise    sub-0ce4    ok++ (PMO)
Apart from bug fix change, the code is the same
**************************************************************************/
void O_optimise(void) {
#ifdef STATISTICS
    int   l1;
#endif
    int   l2;
    int   iteration;
    char *l4;
    bool  l5;
    char *l6;

#ifdef DEBUG
    pr_warning("optimise");
#endif

    l6 = l4   = sbrk(0); /* PMO bug fix set l6 */
    iteration = 0;
    while (!feof(stdin)) { /* not eof */
        freeInstList    = NULL;
        freeOperandLst = NULL;
        resetHeap();
        /* loadFunction(); */
	CallOverlay0(loadFunction, 32+5);

#ifdef PRINT_LISTS
 PrintPi();  
#endif

        freeHashtab();
        /* sub_0ed1(); */
	CallOverlay0(sub_0ed1, 32+6);
        /* chkIXYUsage(); */
	CallOverlay0(chkIXYUsage, 32+6);
        /* sub_122f(); */
	CallOverlay0(sub_122f, 32+6);

        l2 = 0;
        do {
            if (iteration < ++l2) {
                iteration = l2;
            }

            if (key_l) {
                printf("***** Pass %d\n", l2);
                /* sub_404d(); */ CallOverlay0(sub_404d, 32+5);
            }
            l5 = false;
            do {
                hasChanged = false; /* m5: */
                /* sub_15ad(); */
		CallOverlay0(sub_15ad, 32+6);
                l5 |= hasChanged;
            } while (hasChanged);
            do {
                hasChanged = false; /* m6: */
                sub_1ec1();
            } while (hasChanged);

        } while ((l5 |= hasChanged));

        /* sub_404d(); */ CallOverlay0(sub_404d, 32+5);

        if (l6 < (char *)sbrk(0)) {
            l6 = sbrk(0);
        }
    } /* end while */

#ifdef STATISTICS

    if (key_n) 
    {
    	fprintf(stderr, "%dK, %d iterations\n", ((int)(l6 - l4) + 0x3ff) / 0x400, iteration);

    	for (l1 = 0; l1 < NOPTIM; l1++)
            if (optimiseCnters[l1] != 0) 
            	fprintf(stderr, "%d %s\n", optimiseCnters[l1], opt_msg[l1]);
    }

#endif

    if (fclose(stdout) == -1) {
        pr_warning("Error closing output file");
        ++num_warn;
    }

    ReturnOverlay0(0);
}

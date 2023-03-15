/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "optim1.h"

void topmain(int argc, char **argv);

/**************************************************************************
 7  main    sub_07b1    ok++ (PMO)
 *
 *  In the switch statement the location of the code blocks for 's' and
 *  default are swapped by the optimiser, otherwise the code is identical
 **************************************************************************/
int main(int argc, char **argv) 
{
    topmain(argc, argv);

    /* optimise(); */
    CallOverlay0(optimise, 32+4);

    return (num_warn != 0);
}


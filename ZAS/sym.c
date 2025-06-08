/*
 *
 * The sym.c file is part of the restored ZAS.COM program
 * from the Hi-Tech C compiler v3.09
 *
 * Not a commercial goal of this laborious work is to popularize among
 * potential fans of 8-bit computers the old HI-TECH C compiler V3.09
 * (HI-TECH Software) and extend its life, outside of the CP/M environment
 * (Digital Research, Inc), for full operation in a  Unix-like operating
 * system UZI-180 without using the CP/M emulator.
 *
 * The HI-TECH C compiler V3.09 is provided free of charge for any use,
 * private or commercial, strictly as-is. No warranty or product support
 * is offered or implied including merchantability, fitness for a particular
 * purpose, or non-infringement. In no event will HI-TECH Software or its
 * corporate affiliates be liable for any direct or indirect damages.
 *
 * You may use this software for whatever you like, providing you acknowledge
 * that the copyright to this software remains with HI-TECH Software and its
 * corporate affiliates.
 *
 * All copyrights to the algorithms used, binary code, trademarks, etc.
 * belong to the legal owner - Microchip Technology Inc. and its subsidiaries.
 * Commercial use and distribution of recreated source codes without permission
 * from the copyright holderis strictly prohibited.
 *
 * Early work on the decompilation was done by Andrey Nikitin
 * Completion of the work and porting to work under modern compilers done by Mark Ogden
 * 19-May-2022
 */
#include "zas.h"

sym_t *curPsect;              /* a298 */
sym_t *absPsect;              /* a29c */

sym_t* 		pSymTab;	/* pointer to sym table */
uint16_t	SymTabSize;	/* size of sym table */
uint16_t	SymTabCount;	/* counter of slots in the table */

void*	myalloc(size_t size);

/* alloc in upper RAM */
void* upalloc(size_t size) 
{
    register char *st;

    if ((st = myalloc(size)) == 0)
	fatalErr("Out of upper memory");

    return st;
}

/**************************************************************************
 103	getSym	+++
 **************************************************************************/
sym_t *getSym(char *name, int flags) 
{
    register sym_t *psIter;
    int16_t n;
    int16_t nameLen;

    psIter  = pSymTab;
    n = SymTabCount;

    // first, search sym table
    while (n)
    {
	if ( (psIter->sFlags & S_PSECT) == flags )
	{
	    GetString(tmpBuf, psIter->sName);

	    if (strcmp(tmpBuf, name) == 0)
	    	break;
	}

	n--;
	psIter++;
    }

    if (n)
        return psIter;	//found, return-it

    /* add new symbol */

    if (++SymTabCount > SymTabSize)
        fatalErr("Too many symbols");

    nameLen      = (int16_t)strlen(name);
    psIter->sName = upalloc(nameLen + 1);
    PutString(name, psIter->sName);

    if ((flags & S_PSECT))
        psIter->sFlags = flags;
    else 
    {
        psIter->sFlags = S_UNDEF;
        psIter->sEsym  = psIter;
    }

    return psIter;
}

/**************************************************************************
 105	enterAbsPsect	+++
 **************************************************************************/
void enterAbsPsect() 
{
    register sym_t *ps;

    if ((absPsect = getSym("", S_PSECT)) == 0)
        fatalErr("Can't enter abs psect");

    ps = absPsect;
    ps->sFlags |= S_ABSPSECT | S_GLOBAL;
    ps->sEsym = 0;
    curPsect  = absPsect;
}

/**************************************************************************
 106	resetVals	+++
 **************************************************************************/
void resetVals() 
{
    register sym_t *ps;
    uint16_t n;

    ps = pSymTab;

    for (n = 0; n < SymTabCount; n++, ps++)
    {
            if (ps->sFlags & S_PSECT)
                ps->sVal = 0L;
    }

    curPsect = absPsect;
}

/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

extern header SymList;

void AddToList(register char* hdr, char* new);
void prError(char*, ...);

/**************************************************
 * 107: 516C PMO +++
 **************************************************/
void sub_516c(register sym_t *st) 
{
    int tmp;
    char* p;

    if (st && !(/* st->m18 */(tmp = GetWord((char*)st, OFF_m18)) & 0x80))
    {
	p = /* st->nVname */ GetString((char*)st, OFF_nVName);

        if (/* st->m18 */ tmp & 1)
            prError("identifier redefined: %s", /* st->nVName */ p);

        /* st->m18 |= 1; */
	PutWord((char*)st, OFF_m18, tmp = tmp | 1);

        if (crfFp && /* st->nVName */ p && !(/* st->m18 */ tmp & 0x80)) 
            fprintf(crfFp, "#%s %d\n", /* st->nVName */ p, lineNo);
    }
}

/**************************************************
 * 108: 51CF PMO +++
 **************************************************/
void sub_51cf(register sym_t *st) 
{
    if (st)
        /* st->m18 |= 2; */
	PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 2);
}

/**************************************************
 * 116: 56A4 PMO +++
 **************************************************/
sym_t *sub_56a4(void) 
{
    register sym_t *st;

    st = nodeAlloc(0);

    AddToList(&SymList, (char*)st);

    /* st->m18 |= 0x83; */
    PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 0x83);

    /* st->m8    = word_a291; */

    /* word_a291 = st; */

    return st;
}

/**************************************************
 * 126: 5AA4 PMO +++
 **************************************************/
bool sub_5aa4(register s8_t *st) 
{
    return st->c7 == SNODE && (((st->i4 & 1) && st->c7 == SNODE) || st->dataType < (unsigned char)DT_VOID);
}

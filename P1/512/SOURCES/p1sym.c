/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

void prError(char*, ...);

int16_t nodeCnt = 0;

/**************************************************
 * 111: 5384 PMO +++
 **************************************************/
sym_t *nodeAlloc(char *s) 
{
    register sym_t *pn;
    char* tmp;
    int size;

    pn = (sym_t *)myalloc(sizeof(sym_t));

    /* pn->m21     = depth; */
    PutByte((char*)pn, OFF_m21, depth);

    /* pn->nRefCnt = 1; */
    PutByte((char*)pn, OFF_nRefCnt, 1);

    /* pn->nodeId  = ++nodeCnt; */
    PutWord((char*)pn, OFF_nodeId, ++nodeCnt); 

    if (s)
	size = strlen(s) + 1;
    else
	size = 1;

    tmp = myalloc(size);

    WriteMem(s ? s : "", tmp, size);

    PutWord((char*)pn, OFF_nVName, tmp);

    return pn;
}

/**************************************************
 * 118: 573B PMO +++
 **************************************************/
void sub_573b(sym_t *st_par, FILE *fp) 
{
    sym_t buf;
    register sym_t* st = &buf;

    if (st_par) 
    {
	ReadMem((char*)&buf, (char*)st_par, sizeof(struct _sym));

        if (st->m18 & 0x80)
            fprintf(fp, "F%d", st->nodeId);
        else
            fprintf(fp, "_%s", /* st->nVName */ GetString((char*)st_par, OFF_nVName));
    }
}

/**************************************************
 * 125: 5A76 PMO +++
 **************************************************/
// st may be in UpperRAM
bool sub_5a76(register s8_t *st, bool vst, uint8_t p2) 
{
    char c7;
    int i4;
    char dataType;

    if (vst)
    {
	c7 = GetByte((char*)st, OFF_ATTR_c7);
	i4 = GetWord((char*)st, OFF_ATTR_i4);
	dataType = GetByte((char*)st, OFF_ATTR_dataType);
    }
    else
    {
	c7 = st->c7;
	i4 = st->i4;
	dataType = st->dataType;
    }

    return /* st->dataType */ dataType == p2 && /* st->i4 */ i4 == 0 && /* st->c7 */ c7 == (char)SNODE;
}

/**************************************************
 * 128: 5B08 PMO +++
 **************************************************/
bool sub_5b08(register s8_t *st) 
{
    return (st->c7 == SNODE && st->i4 == 0 && st->dataType < (unsigned char)DT_FLOAT);
}


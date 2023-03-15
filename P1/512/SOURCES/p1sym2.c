/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

bool sub_5b08(register s8_t *st);
bool sub_5a76(register s8_t *st, bool vst, uint8_t p2);

/**************************************************
 * 124: 5A4A PMO +++
 **************************************************/
bool sub_5a4a(register s8_t *st) 
{
    return st->dataType == DT_VOID && st->i4 == 1;
}

/**************************************************
 * 130: 5B69 PMO +++
 **************************************************/
bool isValidDimType(register s8_t *st) 
{
    return sub_5b08(st) || sub_5a76(st, false, DT_ENUM);
}

/**************************************************
 * 129: 5B38 PMO +++
 **************************************************/
bool sub_5b38(register s8_t *st) 
{
    return st->c7 == SNODE && st->i4 == 0 &&
           (st->dataType == DT_FLOAT || st->dataType == DT_DOUBLE);
}

/**************************************************
 * 131: 5B99 PMO +++
 * uint8_t parameter
 **************************************************/
void sub_5b99(register s8_t *st) 
{
    if (st->c7 == ANODE)
        st->c7 = 0;
    else
        st->i4 >>= 1;

    if (sub_5a76(st, false, 0x16))
    {
	if (IsValid((char*)st->i_nextInfo))
		ReadMem((char*)st, (char*)(st->i_nextInfo), sizeof(struct _s8));
	else
        	*st = *(st->i_nextInfo);
    }
}

/**************************************************
 * 127: 5AD5 PMO +++
 **************************************************/
bool sub_5ad5(register s8_t *st) 
{
    return st->c7 == SNODE && st->i4 == 0 &&  st->dataType <= (unsigned char)DT_ENUM;
}

/**************************************************
 * 126: 5AA4 PMO +++
 **************************************************/
bool sub_5aa4(register s8_t *st) 
{
    return st->c7 == SNODE && (((st->i4 & 1) && st->c7 == SNODE) || st->dataType < (unsigned char)DT_VOID);
}

/**************************************************
 * 117: 56CD PMO +++
 **************************************************/
sym_t *findMember(sym_t *p1, char *p2) 
{
    register sym_t *st;
    char p2buf[32];

    strcpy(p2buf, p2);

    st = /* p1->nMemberList */ (sym_t*)GetWord((char*)p1, OFF_nMemberList);

    for (;;) 
    {
        if (p1 == st) 
	{
            prError("%s is not a member of the struct/union %s", p2buf, /* p1->nVName */ GetString((char*)p1, OFF_nVName));
            return NULL;
        }

        if (strcmp(/* st->nVName */ GetString((char*)st, OFF_nVName), p2buf) == 0)
            return st;

        /* st = st->nMemberList; */
	st = (sym_t*)GetWord((char*)st, OFF_nMemberList);
    }
}

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

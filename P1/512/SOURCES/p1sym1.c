/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

extern header hashtab[];
extern header SymList;

sym_t *nodeAlloc(char *s);
void sub_0493(register sym_t *st);
void prError(char*, ...);
void prWarning(char*, ...);

void InitHashEntry(int index, char* sym);
void AddToList(register header* hdr, char* new);
void RemoveFromList(register char* sym);
char* GetNextSym(register header* hdr, char* crt);
char* GetFirstSym(register header* hdr);
void prFuncBrace(uint8_t tok);

int ComputeHashEntry(char *buf)
{
    uint16_t crc;
    char* s;

    for (crc = 0, s = buf; *s; s++)
       crc += crc + *(uint8_t *)s;

    return crc % HASHTABSIZE;
}

/* sym_t **lookup(char *buf) { */
sym_t *lookup(register char *buf) 
{
    uint8_t type;
    sym_t *cp;
    char* hdr;

    hdr = &hashtab[ComputeHashEntry(buf)];

    for (cp = (sym_t*)GetFirstSym(hdr);
	 cp;
	 cp = (sym_t*)GetNextSym(hdr, (char*)cp)) 
    {
        if (buf && strcmp(/* cp->nVName */ GetString((char*)cp, OFF_nVName), buf) == 0) 
	{
            if (((byte_8f85 == ((type = /* cp->m20 */ GetByte((char*)cp, OFF_m20)) == D_STRUCT || type == D_UNION)) &&
                 byte_8f86 == (type == D_MEMBER)) ||
                type == 0)
                break;
        }
    }

    return cp;
}

/**************************************************
 * 105: 4E90 PMO +++
 **************************************************/
sym_t *sub_4e90(register char *buf) 
{
/*----------------------------------
    sym_t **ps = lookup(buf);
    if (*ps == 0)
        *ps = nodeAlloc(buf);
    if (crfFp && buf)
        fprintf(crfFp, "%s %d\n", buf, lineNo);
    return *ps;
-------------------------------------- */
    sym_t *ps = lookup(buf);

    if (!ps)
    {
        ps = nodeAlloc(buf);
	AddToList(&hashtab[ComputeHashEntry(buf)], (char*)ps);
    }

    if (crfFp && buf)
        fprintf(crfFp, "%s %d\n", buf, lineNo);

    return ps;
}

/**************************************************
 * 106: 4EED PMO +++
 **************************************************/
// p3 may be in Upper RAM, but may be also NULL
void O_sub_4eed(register sym_t *st, uint8_t p2, s8_t *p3, bool v, sym_t *p4) 
{
    sym_t *tmp1, *tmp2;
    char *var4;
    int16_t var6;
    int index;
    char m20;
    args_t* p3iargs, *stiargs;
    expr_t* p3iexpr, *stiexpr;
    char c7;
    s8_t buf8;
    char Name[32];

    if (m20 = /* st->m20 */ GetByte((char*)st, OFF_m20)) 
    {
        if (depth == /* st->m21 */ GetByte((char*)st, OFF_m21) &&
            (p2 != D_MEMBER || (/* st->m20 */ m20 == D_MEMBER && 
				/* st->nMemberList */ (sym_t*)GetWord((char*)st, OFF_nMemberList) == p4))) 
	{
            var4 = 0;

	    if (p3)
	    {
	    	if (v)
	    	{
		    p3iargs = (args_t*)GetWord((char*)p3, OFF_ATTR_pArgs);
		    p3iexpr = (expr_t*)GetWord((char*)p3, OFF_ATTR_pExpr);
		    c7 = GetByte((char*)p3, OFF_ATTR_c7);
	        }
	    	else
	    	{
		    p3iargs = p3->i_args;
		    p3iexpr = p3->i_expr;
		    c7 = p3->c7;
	    	}
	    }
	    else
		c7 = 0;	// keep p3 access impossible

	    stiargs = (args_t*)GetWord((char*)st, OFF_s8_t_pArgs);
	    stiexpr = (expr_t*)GetWord((char*)st, OFF_s8_t_pExpr);

            if (p2 != /* st->m20 */ m20)
                var4 = "storage class";
            else if (/* st->m18 */ GetWord((char*)st, OFF_m18) & 0x10) 
	    {
                if (p3 && 
		    ! /* sub_591d(p3, v, &st->attr, true) */
		    (bool)CallOverlay4(sub_591d, p3, v, &st->attr, true, 32+4))
                    var4 = "type";

                if (/* p3->c7 */ c7 == ANODE) 
		{
                    if (/* p3->i_args */ p3iargs && ! /* st->attr.i_args */ stiargs)
                        var4 = "arguments";
                    else if (/* p3->i_args */ p3iargs) 
		    {
                        if (/* p3->i_args->cnt */ p3iargs->cnt != /* st->attr.i_args->cnt */ stiargs->cnt)
                            var4 = "no. of arguments";
                        else 
			{
                            var6 = /* p3->i_args->cnt */ p3iargs->cnt;
                            while (var6--) 
			    {
                                if (! /* sub_591d(&p3iargs->s8array[var6], false, &stiargs->s8array[var6], false) */
				    (bool)CallOverlay4(sub_591d, &p3iargs->s8array[var6], false, &stiargs->s8array[var6], false, 32+4))		
 			        {
                                    var4 = "arguments";
                                    break;
                                }
                            }
                        }
                    }
                }
            } /* 4fea */

            if (var4)
                prError("%s: %s redeclared", /* st->nVName */ GetString((char*)st, OFF_nVName), var4);
            else if (/* p3->c7 */ c7 == ENODE &&
		     /* p3->i_expr */ p3iexpr &&
		     /* p3->i_expr */ p3iexpr != /* st->attr.i_expr */ stiexpr) 
	    {
                /* sub_2569(st->attr.i_expr); */
		CallOverlay1(sub_2569, /* st->attr.i_expr */ stiexpr, 32+5);
                /* st->attr.i_expr = p3->i_expr; */
		PutWord((char*)st, OFF_s8_t_pExpr, (short)p3iexpr);
            } 
	    else if (/* p3->c7 */ c7 == ANODE) 
	    {
                if (! /* st->attr.i_args */ stiargs)
                    /* st->attr.i_args = p3->i_args; */
		    PutWord((char*)st, OFF_s8_t_pArgs, (short)p3iargs); 
		else if (/* p3->i_args */ p3iargs &&
			 /* p3->i_args */ p3iargs != /* st->attr.i_args */ stiargs)		
		{
		    CallOverlay1(sub_583a, stiargs, 32+4);
                    /* st->attr.i_args = p3->i_args; */
		    PutWord((char*)st, OFF_s8_t_pArgs, (short)p3iargs); 
                }
            } /* 50d1 */
            /* return st; */
	    ReturnOverlay5(st);
        } /* 50d7 */
/*-----------------------------------------------
        ppSym        = lookup(st->nVName);
        *ppSym       = nodeAlloc(st->nVName);
        (*ppSym)->m8 = st;
        st           = *ppSym;
------------------------------------------------*/
	strcpy(Name, GetString((char*)st, OFF_nVName));

	index = ComputeHashEntry(Name);

	tmp1 = lookup(Name); 

	tmp2 = nodeAlloc(Name);

	if (!tmp1)
		InitHashEntry(index, (char*)tmp2);
	else
       		AddToList(&hashtab[index], (char*)tmp2);

	st = tmp2;
    } /* 5116 */

    PutByte((char*)st, OFF_m20, p2);

    switch (/* st->m20 = p2 */ p2) 
    {
    case DT_USHORT:
    case DT_INT:
    case DT_UINT:
        /* st->attr.i_labelId = newTmpLabel(); */
	PutWord((char*)st, OFF_s8_t_labelId, (short)newTmpLabel());
        /* return st; */
	ReturnOverlay5(st);
    case D_CONST:
        /* st->m18 |= 0x10; */
	PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 0x10);
        /* FALLTHRU */
    case DT_LONG:
        /* st->nMemberList = p4; */
	PutWord((char*)st, OFF_nMemberList, (short)p4);
        break;
    case DT_ULONG:
        /* return st; */
	ReturnOverlay5(st);
    case T_TYPEDEF:
        break;
    default:
        /* st->m18 |= 0x10; */
	PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 0x10);
        break;
    }

    /* st->attr = *p3; */
    if (p3)
    {
    	if (v)
    	{
	    ReadMem((char*)&buf8, (char*)p3, sizeof(struct _s8));
	    WriteMem((char*)&buf8, (char*)st, sizeof(struct _s8));
        }
        else
    	    WriteMem((char*)p3, (char*)st, sizeof(struct _s8));
    }

    /* return st; */
    ReturnOverlay5(st);
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

/**************************************************
 * 109: 51E7 PMO +++
 * uin8_t param
 **************************************************/
void O_sub_51e7(void) 
{
    int16_t var2;
    int16_t var4;
    args_t *var6;
    register sym_t *st;

    var6 = /* curFuncNode->a_args; */ (args_t*)GetWord((char*)curFuncNode, OFF_s8_t_pArgs); 
    st   = p25_a28f;

    while (st) 
    {
        /* st->m18 &= ~0x28; */
	PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) & ~0x28);

        if (var6)
            /* st->m18 |= 0x200; */
	    PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 0x200);

        if (/* st->a_c7 */ GetByte((char*)st, OFF_s8_t_c7) == (char)1) 
	{
            /* sub_2569(st->a_expr); */
	    CallOverlay1(sub_2569, /* st->a_expr */ GetWord((char*)st, OFF_s8_t_pExpr), 32+5);
            /* st->a_expr = 0; */
	    PutWord((char*)st, OFF_s8_t_pExpr, 0);
            /* st->a_c7   = 0; */
	    PutByte((char*)st, OFF_s8_t_c7, 0);
            /* sub_5be1(&st->attr); */ CallOverlay2(sub_5be1, &st->attr, true, 32+6);
        } /* 523d */

        sub_516c(st);
        sub_0493(st);

        /* st = st->nMemberList; */
	st = (sym_t*)GetWord((char*)st, OFF_nMemberList);
    }

    if (!var6)
        /* return; */ ReturnOverlay0(0);

    var4 = 0;
    var2 = var6->cnt;

    if (var2 == 1 && sub_5a76(var6->s8array, false, DT_VOID) && !p25_a28f)
        /* return; */ ReturnOverlay0(0);

    st = p25_a28f;

    while (st && var2--) 
    {
        if (var6->s8array[var4].dataType == DT_VARGS) 
	{
            st   = NULL;
            var2 = 0;
            break;
        } 
	else if (! /*sub_591d(&var6->s8array[var4], false, &st->attr, true) */
		(bool)CallOverlay4(sub_591d, &var6->s8array[var4], false, &st->attr, true, 32+4))
            break;

        /* st = st->nMemberList; */
	st = (sym_t*)GetWord((char*)st, OFF_nMemberList);
        var4++;
    }

    if (st || (var2 && var6->s8array[var4].dataType != DT_VARGS))
        prError("argument list conflicts with prototype");

    var2 = 1;
    ReturnOverlay0(0);
}

/**************************************************
 * 115: 54C0 PMO +++
 **************************************************/
void O_checkScopeExit(void) 
{
    sym_t *var4;
    uint8_t var5;
    char *var7;
    sym_t *st;
    sym_t buf;
    register sym_t *st_tmp = &buf;
    int n;
    char* hdr;

    for (n = 0; n < HASHTABSIZE; n++)
    {
        var4 = (sym_t*)GetFirstSym((char*)(hdr = &hashtab[n]));

        while ((st = var4)) 
	{
	    ReadMem((char*)&buf, (char*)st, sizeof(struct _sym));

            if (st_tmp->m21 == depth) 
	    {
                var7 = 0;
                var5 = st_tmp->m20;

                if ((st_tmp->m18 & 3) == 2) 
		{
                    switch (var5) 
		    {
                    case D_LABEL:
                        var7 = "label";
                        break;
                    case D_STRUCT:
                    case D_UNION:
                    case T_EXTERN:
                        break;
                    default:
                        var7 = "variable";
                        break;
                    }

                    if (var7)
                        prError("undefined %s: %s", var7, /* st->nVName */ GetString((char*)st, OFF_nVName));

                } 
		else if ((depth || var5 == T_STATIC) && !(st_tmp->m18 & 2)) 
		{ /* 5555  */
                    switch (var5) 
		    {
                    case D_LABEL:
                        var7 = "label";
                        break;
                    case D_STRUCT:
                        var7 = "structure";
                        break;
                    case D_UNION:
                        var7 = "union";
                        break;
                    case D_MEMBER:
                        var7 = "member";
                        break;
                    case D_ENUM:
                        var7 = "enum";
                        break;
                    case D_CONST:
                        var7 = "constant";
                        break;
                    case T_TYPEDEF:
                        var7 = "typedef";
                        break;
                    case D_6:
                        var7 = 0;
                        break;
                    default:
                        if (var5) 
			{
                            if (st_tmp->m18 & 1)
                                var7 = "variable definition";
                            else
                                var7 = "variable declaration";
                        }
                        break;
                    }
                    if (var7)
                        prWarning("unused %s: %s", var7, /* st->nVName */ GetString((char*)st, OFF_nVName));

                } /* 55d2 */

                var4 = (sym_t*)GetNextSym(hdr, (char*)var4);
                /* reduceNodeRef(st); */
		CallOverlay1(reduceNodeRef, st, 32+4);
            } else
                var4 = (sym_t*)GetNextSym(hdr, (char*)var4); 
        }
    }

    /* var4 = &word_a291; */
    var4 = (sym_t*)GetFirstSym(hdr = &SymList);

    while ((st = var4)) 
    {
        if (GetByte((char*)st, OFF_m21) == depth) 
	{
            var4 = (sym_t*)GetNextSym(hdr, (char*)var4);
            /* reduceNodeRef(st); */
	    CallOverlay1(reduceNodeRef, st, 32+4);
        } 
	else
            var4 = (sym_t*)GetNextSym(hdr, (char*)var4);
    }

    ReturnOverlay0(0);
}

/**************************************************
 * 120: 578D PMO +++
 * trivial optimiser differences
 **************************************************/
args_t *O_sub_578d(register args_t *p) 
{
    args_t *var2;
    s8_t *var4;
    int16_t var6;

    if (!p)
        /* return p; */ ReturnOverlay1(p);

    var2 = xalloc(sizeof(args_t) + (p->cnt - 1) * sizeof(s8_t));
    var6 = var2->cnt = p->cnt;

    while (var6--) 
    {
        var4  = &var2->s8array[var6];
        *var4 = p->s8array[var6];

        if (var4->dataType == DT_POINTER)
            /* var4->i_nextSym->nRefCnt++; */
	    PutByte((char*)(var4->i_nextSym), OFF_nRefCnt, GetByte((char*)(var4->i_nextSym), OFF_nRefCnt) + 1);
    }

    /* return var2; */ ReturnOverlay1(var2);
}

/**************************************************
 * 121: 583A PMO +++
 **************************************************/
void O_sub_583a(register args_t *st) 
{
    s8_t *var2;

    if (st) 
    {
        for (var2 = st->s8array; st->cnt--; var2++) 
	{
            if (var2->c7 == ANODE && var2->i_args)
                /* sub_583a(var2->i_args); */ CallOverlay1(sub_583a, var2->i_args, 32+4);

            if (var2->dataType == DT_POINTER)
                /* reduceNodeRef(var2->i_nextSym); */ CallOverlay1(reduceNodeRef, var2->i_nextSym, 32+4);
        }

        free(st);
    }

    ReturnOverlay1(0);
}

/**************************************************
 * 122: 58BD PMO +++
 **************************************************/
// st, p2 may be in upper RAM
void O_sub_58bd(s8_t *st, bool vst, register s8_t *p2, bool vp2) 
{
    s8_t buf;
    char c7;
    expr_t* iexpr;
    args_t* iargs;

    if (vp2)
    {
	if (vst)
	{
	    ReadMem((char*)&buf, (char*)st, sizeof(struct _s8));
	    WriteMem((char*)&buf, (char*)p2, sizeof(struct _s8));
	}
	else
	    WriteMem((char*)st, (char*)p2, sizeof(struct _s8));
    }
    else
    {
	if (vst)
	{
	    ReadMem((char*)&buf, (char*)st, sizeof(struct _s8));
	    *p2 = buf;	    
	}
	else
    	    *p2 = *st;
    }	
	
    if (vp2)
    {
	c7 = GetByte((char*)p2, OFF_ATTR_c7);
	iexpr = (expr_t*)GetWord((char*)p2, OFF_ATTR_pExpr);
	iargs = (args_t*)GetWord((char*)p2, OFF_ATTR_pArgs);
    }
    else
    {
	c7 = p2->c7;
	iexpr = p2->i_expr;
	iargs = p2->i_args;
    }

    if (/* p2->c7 */ c7 == (char)ENODE)
    {
	if (vp2)
	    PutWord((char*)p2, OFF_ATTR_pExpr, CallOverlay1(sub_21c7, iexpr, 32+5));
	else
            p2->i_expr = CallOverlay1(sub_21c7, iexpr, 32+5);
    }
    else if (/* p2->c7 */ c7 == (char)ANODE)
    {
	if (vp2)
	    PutWord((char*)p2, OFF_ATTR_pArgs, /* sub_578d(iargs) */ CallOverlay1(sub_578d, iargs, 32+4));
	else
            p2->i_args = /* sub_578d(iargs) */ CallOverlay1(sub_578d, iargs, 32+4);
    }

    ReturnOverlay4(0);
}

/**************************************************
 * 123: 591D PMO +++
 * some optimiser differences including movement
 * of some basic blocks
 **************************************************/
// st, p2 may be in Upper RAM
bool O_sub_591d(register s8_t *st, bool vst, s8_t *p2, bool vp2) 
{
    int16_t var2;
    char stc7, p2c7, stdatatype, p2datatype;    
    int sti4, p2i4;
    sym_t *stinextsym, *p2inextsym;
    s8_t *stinextinfo, *p2inextinfo;
    args_t *stiargs, *p2iargs;
    s8_t buf8;

    if (st == p2)
        /* return true; */ ReturnOverlay4(true);

    if (vst)
    {
	ReadMem((char*)&buf8, (char*)st, sizeof(struct _s8));
	stc7 = buf8.c7;
	stdatatype = buf8.dataType;
	sti4 = buf8.i4;
	stinextsym = buf8.i_nextSym;
	stinextinfo = buf8.i_nextInfo;
	stiargs = buf8.i_args;
    }
    else
    {
	stc7 = st->c7;
	stdatatype = st->dataType;
	sti4 = st->i4;
	stinextsym = st->i_nextSym;
	stinextinfo = st->i_nextInfo;
	stiargs = st->i_args;
    }

    if (vp2)
    {
	ReadMem((char*)&buf8, (char*)p2, sizeof(struct _s8));
	p2c7 = buf8.c7;
	p2datatype = buf8.dataType;
	p2i4 = buf8.i4;
	p2inextsym = buf8.i_nextSym;
	p2inextinfo = buf8.i_nextInfo;
	p2iargs = buf8.i_args;
    }
    else
    {
	p2c7 = p2->c7;
	p2datatype = p2->dataType;
	p2i4 = p2->i4;
	p2inextsym = p2->i_nextSym;
	p2inextinfo = p2->i_nextInfo;
	p2iargs = p2->i_args;
    }

    if (stc7 != p2c7 ||
	stdatatype != p2datatype ||
	sti4 != p2i4)
        /* return false; */ ReturnOverlay4(false);

    switch (stdatatype) 
    {
    case DT_ENUM:
    case DT_STRUCT:
    case DT_UNION:
        /* return stinextsym == p2inextsym; */ ReturnOverlay4(stinextsym == p2inextsym);
    case DT_POINTER:
        /* return sub_591d(stinextinfo, IsValid((char*)stinextinfo), p2inextinfo, IsValid((char*)p2inextinfo)); */
	ReturnOverlay4((bool)CallOverlay4(sub_591d, stinextinfo, IsValid((char*)stinextinfo), p2inextinfo, IsValid((char*)p2inextinfo), 32+4));
    }

    if (stc7 != (char)ANODE || !stiargs || !p2iargs)
        /* return true; */ ReturnOverlay4(true);

    if (stiargs->cnt != p2iargs->cnt)
        /* return false; */ ReturnOverlay4(false);

    var2 = stiargs->cnt;

    do
    { 
        if (var2-- == 0)
            /* return true; */ ReturnOverlay4(true);
    }
    while (/* sub_591d(&stiargs->s8array[var2], false, &p2iargs->s8array[var2], false) */
	    (bool)CallOverlay4(sub_591d, &stiargs->s8array[var2], false, &p2iargs->s8array[var2], false, 32+4));

    /* return false; */ ReturnOverlay4(false);
}

/**************************************************
 * 112: 540C PMO +++
 * optimiser has better code for --pn->nRefCnt
 **************************************************/
void O_reduceNodeRef(sym_t *pn_par) 
{
    sym_t buf;
    register sym_t *pn = &buf;
    char tmp;

/*  if (--pn->nRefCnt == 0) { */

    PutByte((char*)pn_par, OFF_nRefCnt, tmp = (GetByte((char*)pn_par, OFF_nRefCnt) - 1));
    ReadMem((char*)&buf, (char*)pn_par, sizeof(struct _sym));

    if (tmp == 0) 
    {
        if (pn->m20 != 0 &&
	    pn->m20 != D_LABEL &&
	    pn->m20 != D_STRUCT &&
	    pn->m20 != D_UNION &&
            pn->m20 != D_ENUM) 
	{
            if (pn->a_c7 == ANODE)
                /* sub_583a(pn->a_args); */ CallOverlay1(sub_583a, pn->a_args, 32+4);
            else if (pn->a_c7 == ENODE)
                /* sub_2569(pn->a_expr); */
		CallOverlay1(sub_2569, pn->a_expr, 32+5);
            if (pn->a_dataType == DT_POINTER)
                /* reduceNodeRef(pn->a_nextSym); */
		CallOverlay1(reduceNodeRef, pn->a_nextSym, 32+4);
        }

/* ------------------------------
        if (pn->nVName != blank)
            free(pn->nVName);
-------------------------------- */

	free512(GetWord((char*)pn_par, OFF_nVName));

        /* pn->nMemberList = s25FreeList; */

	RemoveFromList((char*)pn_par);

	free512(pn_par);
    }

    ReturnOverlay1(0);
}

/**************************************************
 * 113: 549C PMO +++
 * use of uint8_t param
 **************************************************/
void enterScope(void) 
{
    prFuncBrace(T_LBRACE);
    ++depth;
}

/**************************************************
 * 114: 54AC PMO +++
 * use of uint8_t param
 **************************************************/
void exitScope(void) 
{
    CallOverlay0(checkScopeExit, 32+4);
    --depth;
    prFuncBrace(T_RBRACE);
}

/**************************************************
 * 119: 5785 PMO +++
 **************************************************/
int16_t newTmpLabel(void) 
{
    return ++tmpLabelId;
}


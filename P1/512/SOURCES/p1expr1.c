/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

extern s2_t *p2List; /* 8bc7 */
extern int16_t strId;            /* 8bd7 */
extern bool byte_8f86;        /* 8f86 */

extern expr_t **s13SP;   /* 9cf1 */
extern s2_t s2_9cf3[20]; /* 9cf3 */
extern expr_t s13_9d1b; /* 9d1b */
extern expr_t s13_9d28; /* 9d28 */

extern expr_t *s13FreeList; /* 9d35 */
extern uint8_t byte_9d37;   /* 9d37 */
extern expr_t *s13Stk[20];  /* 9d38 */

bool sub_0aed(register expr_t *st);
bool sub_0b93(register expr_t *st);
bool sub_10a8(void);
expr_t *allocFConst(char *fltStr);
expr_t *sub_1b94(register expr_t *st);
expr_t *sub_1bf7(register expr_t *st, s8_t *p2);
expr_t *sub_1ccc(expr_t *p1, uint8_t p2);
expr_t *sub_1d02(register expr_t *st);
uint8_t sub_1d5a(register s8_t *st, s8_t *p2);
expr_t *sub_1df0(register expr_t *st);
expr_t *sub_1e37(register expr_t *st);
expr_t *sub_1e58(register expr_t *st);
expr_t *sub_1ebd(register expr_t *st);
bool sub_1ef1(register expr_t *st);
expr_t *sub_1f5d(register expr_t *st, s8_t *p2, int16_t p3);
expr_t *s13Alloc(uint8_t tok);
expr_t *sub_225a(uint8_t p1, register expr_t *st, expr_t *p3);
expr_t *sub_23b4(uint8_t tok, register expr_t *st, expr_t *p3);
expr_t *allocSConst(void);
expr_t *popExpr(void);
void sub_2529(uint8_t p1);
uint8_t sub_255d(void);
bool sub_5a76(register s8_t *st, bool vst, uint8_t p2);
void prError(char*, ...);
void prWarning(char*, ...);
bool isValidDimType(register s8_t *st);
void sub_516c(register sym_t *st);
void sub_51cf(register sym_t *st);
void expectErr(char *p);
sym_t *findMember(sym_t *p1, char *p2);
bool sub_5ad5(register s8_t *st);
bool sub_5b08(register s8_t *st);
bool sub_5a4a(register s8_t *st );
bool sub_5aa4(register s8_t *st);
void *xalloc(size_t size);
bool sub_5b38(register s8_t *st);
void sub_5b99(register s8_t *st);

/**************************************************
 * 17: 07F5 PMO +++
 **************************************************/
expr_t * O_sub_07f5(char p1) 
{
    char l1;
    register expr_t *st;

    l1        = byte_9d37;
    byte_9d37 = p1;
    st        = /* sub_0bfc(); */ CallOverlay0(sub_0bfc, 32+5);
    byte_9d37 = l1;
    /* return st; */ ReturnOverlay1(st);
}

/**************************************************
 * 18: 0817 PMO +++
 * Minor differences due to addition of missing
 * parameters and use of uint8_t parameter
 * also one missed optimisation
 * of pop bc, push hl to ex (sp),hl
 **************************************************/
expr_t *sub_0817(register s8_t *st) 
{
    int16_t var2;
    int16_t var4;
    expr_t *var6;
    uint8_t tok;
    char inextinfoc7;
    bool v = false;
    bool vnext;
    args_t* stiargs;
    expr_t *arr[128];

    /* fix as if st is NULL the else clause shouldn't be used */
    if (st) 
    {
	if (vnext = IsValid((char*)st->i_nextInfo))
	    inextinfoc7 = GetByte((char*)(st->i_nextInfo), OFF_ATTR_c7);
	else
	    inextinfoc7 = st->i_nextInfo->c7;

        if (st->c7 != ANODE && st->i4 == 1 && st->dataType == DT_POINTER &&
            /* st->i_nextInfo->c7 */ inextinfoc7 == ANODE)
	{
            st = st->i_nextInfo;
	    v = vnext;
	}
        else if (st->c7 != ANODE)
            prError("function or function pointer required");
    }

    if (
	st && 
	( ( v ? GetByte((char*)st, OFF_ATTR_c7) : st->c7 ) == (char)ANODE ) &&
 	( v ? ( stiargs = (args_t*)GetWord((char*)st, OFF_ATTR_pArgs) ) : ( stiargs = st->i_args ) )
       ) 
    {
        var4 = stiargs->cnt;
        st   = stiargs->s8array;
    } 
    else 
    {
        st   = NULL;
        var4 = 0;
    }

    var2     = 0;
    ungetTok = tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

    for (;;) 
    {
        if (st && st->dataType == DT_VARGS) 
	{
            var4 = 0;
            st   = 0;
        }

        if (tok != T_RPAREN) 
	{
            if (st && sub_5a76(st, false, DT_VOID)) 
	    {
                prError("function does not take arguments");
                var4 = 0;
                st   = NULL;
            }

            var6 = /* sub_1441(T_60, sub_07f5(3), 0); */ 
		   CallOverlay3(sub_1441, T_60, /* sub_07f5(3) */ CallOverlay1(sub_07f5, 3, 32+5), 0, 32+5);

            if (var6) 
	    {
                if (st && var4-- == 0) 
		{
                    prError("too many arguments");
                    st   = 0;
                    var4 = 0;
                }

                if (st)
                    var6 = sub_1f5d(var6, st++, 1);
                else
                    var6 = sub_1d02(var6);

                arr[var2++] = var6;
                tok         = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

                if (tok == T_COMMA)
                    continue;

                ungetTok = tok;
            }
        }
        break;
    }

    if ((var4 != 1 || var2 != 0 || !sub_5a76(st, false, DT_VOID)) && var4 && st->dataType != DT_VARGS)
        prError("too few arguments");

    if (var2 == 0)
        return sub_23b4(T_120, NULL, NULL); /* dummy 2nd & 3rd args added */

    var4 = 0;

    while (var4 + 1 != var2) 
    {
        arr[var4 + 1] = sub_23b4(T_COMMA, arr[var4], arr[var4 + 1]);
        ++var4;
    }

    return arr[var4];
}

/**************************************************
 * 19: 0A83 PMO +++
 * Minor differences due to addition of missing
 * parameters and use of uint8_t parameter
 **************************************************/
expr_t * O_sub_0a83(uint8_t n) 
{
    register expr_t *st;

    byte_9d37 = n;

    if ((st = /* sub_1441(T_60, sub_0bfc(), 0) */ /* PMO added dummy arg3 */
		CallOverlay3(sub_1441, T_60, CallOverlay0(sub_0bfc, 32+5), 0, 32+5))) 
    { 
        if (!sub_0aed(st))
            prError("constant expression required");
        else if (byte_9d37 == 2) 
	{
            if (isValidDimType(&st->attr))
                st = sub_1ccc(st, DT_CONST);
            else
                prError("illegal type for array dimension");
        }
    }

    byte_9d37 = 0;
 
    /* return st; */ ReturnOverlay1(st);
}

/**************************************************
 * 20: 0AED PMO +++
 * Locations of code to set return  values 1/0 swapped
 * rest of code adjusted accordingly
 **************************************************/
bool sub_0aed(register expr_t *st) 
{
    uint8_t type, flags;

    if (st == 0)
        return true;

    type = st->tType;

    if (type == T_ID)
        return sub_5a76(&st->attr, false, DT_ENUM);

    if (type == T_SIZEOF)
        return true;

    if (type == D_ADDRESSOF && sub_0b93(st->t_next))
        return true;

    flags = opTable[(type - 60)].uc4;

    if (!(flags & 0x10))
        return false;

    if (flags & 1)
        return true;

    return sub_0aed(st->t_next) && (!(flags & 2) || sub_0aed(st->t_alt));
}

/**************************************************
 * 21: 0B93 PMO +++
 **************************************************/
bool sub_0b93(register expr_t *st) 
{
    uint8_t type;
    char tmp;

    type = st->tType;

    if (type == T_ID)
    {
	tmp = GetByte((char*)(st->t_pSym), OFF_m20);
        return /* st->t_pSym->m20 */ tmp == (char)T_EXTERN ||
               /* st->t_pSym->m20 */ tmp == (char)T_STATIC;
    }

    if (type == T_DOT)
        return sub_0b93(st->t_next);

    return type == T_69 && sub_0aed(st->t_next);
}

/**************************************************
 * 22: 0BFC PMO +++
 * many basic blocks moved around, logic still valid
 * some optimisation differences but equivalent code
 **************************************************/
expr_t * O_sub_0bfc(void) 
{
    s8_t var8;
    expr_t *varA FORCEINIT;
    s2_t *varC;
    expr_t **varE;
    uint8_t tok;
    uint8_t tok2;
    uint8_t var11;
    uint8_t var12;
    bool var13;
    uint8_t var14;
    uint8_t var15;
    int16_t var17;
//    int16_t var19; /* not used */
    register sym_t *st;

    varE = s13SP;
    varC = p2List;
    sub_2529(T_60);
    var11 = 0;
//    var19 = 0;

    for (;;) 
    {
        var13 = (byte_8f86 = p2List->type1 == T_DOT || p2List->type1 == T_POINTER);
        tok               = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);
        byte_8f86         = false;

        if (tok < T_60 || tok >= T_128) 
	{
            ungetTok = tok;
            tok      = T_60;
        }

        if (opTable[tok - T_60].uc4 & 1) 
	{
            if (var11)
                goto error;

            switch (tok) 
	    {
            case T_ID: /* c9a */
                st = yylval.ySym;
                if (!var13) 
		{
                    if (/* st->m20 */ GetByte((char*)st, OFF_m20) == 0) 
		    {
                        if (/* peekCh() */ (uint8_t)CallOverlay0(peekCh, 32+4) == '(') 
			{
                            var8.c7       = 2;
                            var8.dataType = DT_INT;
                            var8.i_sym    = 0;
                            var8.i4       = 0;
                            st            = /* sub_4eed(st, T_EXTERN, &var8, false, 0); */
					    (sym_t*)CallOverlay5(sub_4eed, st, T_EXTERN, &var8, false, 0, 32+4);
                            /* st->m18 |= 0x42; */
			    PutWord((char*)st, OFF_m18, GetWord((char*)st, OFF_m18) | 0x42);
                            /* st->m21 = 0 */ 
			    PutByte((char*)st, OFF_m21, 0);
                            sub_0493(st);
                        } 
			else 
			{
                            prError("undefined identifier: %s", /* st->nVName */ GetString((char*)st, OFF_nVName));
                            /* st->m20           = T_STATIC; */
			    PutByte((char*)st, OFF_m20, T_STATIC);
                            /* st->attr.dataType = DT_INT; */
			    PutByte((char*)st, OFF_s8_t_a_dataType, DT_INT);
                            /* st->m18           = 0x11; */
			    PutWord((char*)st, OFF_m18, 0x11);
                        }
                    }
		    else 
		    {
                        var17 = /* st->m18 */ GetWord((char*)st, OFF_m18);

                        if (!(var17 & 0x10))
                            prError("not a variable identifier: %s", /* st->nVName */ GetString((char*)st, OFF_nVName));
                        else if (/* st->m20 */ GetByte((char*)st, OFF_m20) == T_EXTERN && !(var17 & 0x100))
                            sub_0493(st);
                    }
                    sub_51cf(st);
                } /* d57 */
                varA = /* allocId(st); */ CallOverlay1(allocId, st, 32+5);
                break;
            case T_ICONST: /* d75 */
                varA = /* sub_1b4b(yylval.yNum, DT_INT); */ CallOverlay2L(sub_1b4b, yylval.yNum, DT_INT, 32+5);
                break;
            case T_LCONST: /* d90 */
                varA = /* sub_1b4b(yylval.yNum, DT_LONG); */ CallOverlay2L(sub_1b4b, yylval.yNum, DT_LONG, 32+5);
                break;
            case T_FCONST: /* da5 */
                varA = allocFConst(yylval.yStr);
                break;
            case T_SCONST: /* dae */
                varA       = allocSConst();
                varA->t_i2 = strChCnt;
                sub_053f(varA, yylval.yStr);
                free(yylval.yStr);
                break;
            case S_TYPE:
                goto error;
            }
            /* d63 */
            pushS13(varA);
            var11 = true;
            continue;
        } /* dfa */
        switch (tok) 
	{
        case T_LPAREN:
            ungetTok = tok2 = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

            if (tok2 == S_TYPE || (tok2 == T_ID && 
				   /* yylval.ySym->m20 */ GetByte((char*)(yylval.ySym), OFF_m20) == T_TYPEDEF)) 
	    {
                if (var11)
                    goto error;

                /* sub_5dd1(0, &var8); */ CallOverlay2(sub_5dd1, 0, &var8, 32+6);
                sub_516c(st = /* sub_69ca(T_79, &var8, 0, 0) */ CallOverlay4(sub_69ca, T_79, &var8, 0, 0, 32+6));
                sub_51cf(st);
                tok2 = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

                if (tok2 != T_RPAREN)
                    goto error;

                /* var8 = st->attr; */ 
		ReadMem((char*)&var8, (char*)st, sizeof(struct _s8));
                pushS13(/* allocSType(&var8) */ CallOverlay1(allocSType, &var8, 32+5));

                if (p2List->type1 == T_SIZEOF) 
		{
                    p2List->type2 = T_EQ;
                    var11         = true;
                    continue;
                }
	        else
                    tok = T_79;
            }
	    else if (var11) 
	    {
                sub_2529(T_61);
                pushS13(sub_0817(&(*s13SP)->attr));
            }
            break;
        case T_60:
            break;
        case T_LBRACK:
            if (!var11)
                goto error;

            tok   = T_64;
            var11 = false;
            break;
        case T_RPAREN:
        case T_RBRACK:
            if (!var11)
                goto error;

            break;
        case T_INC:
        case T_DEC:
            if (var11)
                tok++;

            break;
        default:
            if (opTable[tok - T_60].uc4 & 4 && !var11)
                tok -= 11;

            var14 = (opTable[tok - T_60].uc4 & 2) != 0;

            if (var14 != var11)
                goto error;

            var11 = false;
            break;
        }

        /* f23 */
        var15 = opTable[tok - T_60].c3;

        if ((byte_9d37 >= (unsigned char)3 && tok == T_COMMA) ||
            (byte_9d37 == 1 && tok == T_COLON && p2List->type1 != T_QUEST))
            var15 = 5;
        /* f8e */
        do 
	{
            var12 = 0;

            if (p2List->type2 < var15 ||
                (p2List->type2 == var15 && (opTable[tok - T_60].uc4 & 8))) 
	    {
                switch (tok) 
		{
                case T_75:
                case T_77:
                    var15 = 0x1f;
                    break;
                case T_LPAREN:
                case T_64:
                    var15 = 4;
                    break;
                }
                sub_2529(tok);
                p2List->type2 = var15;
            }
	    else 
	    {
                if (p2List->type1 == T_60) 
		{ /* 1058 */
                    if (tok != T_60)
                        ungetTok = tok;
                    varA = popExpr();
                    if (varA && s13SP == varE)
                        goto done;
                    else
                        goto error;
                } 
		else if (p2List->type1 == T_LPAREN) 
		{
                    if (tok != T_RPAREN) 
		    {
                        expectErr(")");
                        ungetTok = tok;
                    }
                } 
		else if (p2List->type1 == T_64) 
		{
                    if (tok != T_RBRACK) 
		    {
                        expectErr("]");
                        ungetTok = tok;
                    }
                } 
		else
                    var12 = 1;
                /* 1037 */
                if (sub_10a8())
                    goto error;
            }
        } while (var12);
    }
error:
    prError("expression syntax"); 
    /* skipStmt(tok); */ CallOverlay1(skipStmt, tok, 32+4);

    while (s13SP != varE)
        /* sub_2569(popExpr()); */ CallOverlay1(sub_2569, popExpr(), 32+5);

    varA = NULL;

done:
    s13SP  = varE;
    p2List = varC;

    /* return varA; */ ReturnOverlay0(varA);
}

/**************************************************
 * 23: 10A8 PMO +++
 * Minor differences, use of uint8_ parameter and
 * addition of missing parameters
 **************************************************/
bool sub_10a8(void) 
{
    expr_t *l1;
    expr_t *l2;
    uint8_t tok;
    register expr_t *st FORCEINIT;

    if ((tok = sub_255d()) == T_LPAREN)
        return false;

    l1 = NULL;
    if (tok != T_120 &&
        (((opTable[tok - T_60].uc4 & 2) && (l1 = popExpr()) == NULL) || (st = popExpr()) == NULL))
        return true;

    switch (tok) 
    {
    case T_64:
        sub_2529(T_69);
        p2List->type2 = T_EQ;
        tok           = T_PLUS;
        if (isValidDimType(&l1->attr))
            l1 = sub_1ccc(l1, DT_CONST);
        else
            prError("illegal type for index expression");
        break;
    case T_INC:
    case T_75:
    case T_DEC:
    case T_77:
        l1  = &s13_9d28;
        tok = tok == T_INC ? T_PLUSEQ : tok == T_DEC ? T_MINUSEQ : tok == T_75 ? T_102 : T_104;
        break;
    case T_POINTER:
        tok = T_DOT;
        st  = /* sub_1441(T_69, st, 0); */ CallOverlay3(sub_1441, T_69, st, 0, 32+5); /* added dummy 3rd arg */
        break;
    case T_SIZEOF:
        if (st->tType == T_SCONST) 
	{
            l2 = /* sub_1b4b((long)st->t_i2 + 1, DT_UINT); */ CallOverlay2L(sub_1b4b, (long)st->t_i2 + 1, DT_UINT, 32+5);
            /* sub_2569(st); */ CallOverlay1(sub_2569, st, 32+5);
            pushS13(l2);
            return false;
        }

        if (st->tType != S_TYPE && st->tType != T_ID) 
	{
            l2             = /* allocSType(&st->attr) */ CallOverlay1(allocSType, &st->attr, 32+5);
            l2->attr.i_sym = NULL;
            l2->attr.c7    = 0;

            if (st->attr.c7 == ENODE) 
	    {
                l2  = /* sub_1441(T_SIZEOF, l2, 0); */ CallOverlay3(sub_1441, T_SIZEOF, l2, 0, 32+5);
                l1  = sub_1ccc(/* sub_21c7(st->attr.i_expr) */ CallOverlay1(sub_21c7, st->attr.i_expr, 32+5) , DT_UINT);
                tok = T_STAR;
            }

            /* sub_2569(st); */ CallOverlay1(sub_2569, st, 32+5);
            st = l2;
        } 
	else if (st->tType == S_TYPE && st->attr.c7 == ENODE) 
	{
            l1  = sub_1ccc(/* sub_21c7(st->attr.i_expr) */ CallOverlay1(sub_21c7, st->attr.i_expr, 32+5), DT_UINT);
            st  = /* sub_1441(T_SIZEOF, st, 0); */ CallOverlay3(sub_1441, T_SIZEOF, st, 0, 32+5);
            tok = T_STAR;
        }
        break;
    case T_COMMA:
        if (p2List[0].type1 != T_LPAREN || p2List[1].type1 != T_61)
            tok = T_114;
        break;
    case T_79:
        pushS13(sub_1bf7(/* sub_1441(T_60, l1, 0) */ CallOverlay3(sub_1441, T_60, l1, 0, 32+5), &st->attr));
        /* sub_2569(st); */ CallOverlay1(sub_2569, st, 32+5);
        return 0;
    }

    if ((tok == T_QUEST) != (l1 && l1->tType == T_COLON))
        return true;

    pushS13(/* sub_1441(tok, st, l1) */ CallOverlay3(sub_1441, tok, st, l1, 32+5));
    return false;
}

/**************************************************
 * 24: 1340 PMO +++
 * Optimised uint8_t param
 **************************************************/
expr_t *sub_1340(register expr_t *st, expr_t *p2) 
{
    sym_t *var2;
    sym_t *var4;

    if (!sub_5a76(&st->attr, false, DT_STRUCT) && !sub_5a76(&st->attr, false, DT_UNION))
        prError("struct/union required");
    else if (p2->tType != T_ID)
        prError("struct/union member expected");
    else if ((var4 = st->a_nextSym) == 0)
        ;
    else if (!(/* var4->m18 */ GetWord((char*)var4, OFF_m18) & 1))
        prError("undefined struct/union: %s", /* var4->nVName */ GetString((char*)var4, OFF_nVName));
    else if ((var2 = findMember(var4, /* p2->t_pSym->nVName */ GetString((char*)(p2->t_pSym), OFF_nVName)))) 
    {
        sub_51cf(var2);
        st       = s13Alloc(T_126);
        st->t_i0 = /* var2->m14 */ GetWord((char*)var2, OFF_m14);
        /* st->attr = var2->attr; */
	ReadMem((char*)&st->attr, (char*)var2, sizeof(struct _s8));
        /* sub_2569(p2); */ CallOverlay1(sub_2569, p2, 32+5);
        return st;
    }

    return p2;
}

/**************************************************
 * 25: 1441 PMO +++
 * Minor optimiser differences, but equivalent code
 * Also uint8_t parameter differences
 **************************************************/
expr_t * O_sub_1441(uint8_t p1, register expr_t *lhs, expr_t *rhs) 
{
    s8_t tmpExpr;
    expr_t *savedLhs;
    expr_t *minusLhs FORCEINIT;
    bool hasRhs;
    bool minusLhsValid;
    int16_t opFlags;
    char *opStr;
    uint8_t var13;

    hasRhs = (opTable[p1 - 60].uc4 & 2) != 0;
    opStr  = opTable[p1 - 60].s0;

    if (!lhs || (hasRhs && rhs == 0)) 
    {
        /* sub_2569(lhs); */ CallOverlay1(sub_2569, lhs, 32+5);

        if (hasRhs)
            /* sub_2569(rhs); */ CallOverlay1(sub_2569, rhs, 32+5);

        /* return NULL; */ ReturnOverlay3(NULL);
    }

    minusLhsValid = false;
    opFlags       = opTable[p1 - 60].i5;

    if (p1 == D_ADDRESSOF && lhs->tType == T_ID && 
	(/* lhs->t_pSym->m18 */ GetWord((char*)(lhs->t_pSym), OFF_m18) & 4))
        prError("can't take address of register variable");

    if (!(opFlags & 0x100))
        lhs = sub_1e37(lhs);

    if (hasRhs && !(opFlags & 0x800))
        rhs = sub_1e37(rhs);

    if (p1 == T_61) 
    {
        if ((lhs->attr.i4 & 1) && lhs->attr.c7 == SNODE)
            lhs = /* sub_1441(T_69, lhs, 0); */ CallOverlay3(sub_1441, T_69, lhs, 0, 32+5); /* dummy 3rd arg */
    } 
    else
        lhs = sub_1df0(lhs);

    if (hasRhs)
        rhs = sub_1df0(rhs);

    if ((opFlags & 0x2000) && sub_1ef1(lhs) == 0) 
    {
        if (p1 == D_ADDRESSOF) 
	{
            if (lhs->tType == D_ADDRESSOF && lhs->t_next->attr.c7 == ENODE)
                /* return lhs; */ ReturnOverlay3(lhs);
            else
                prError("can't take this address");
        } 
	else
            prError("only lvalues may be assigned to or modified");
    }

    if ((opFlags & 0x4000) && (!(lhs->attr.i4 & 1) || lhs->attr.c7 != SNODE))
        prError("pointer required");

    if (!(opFlags & 3)) 
    {
        if (sub_5a76(&lhs->attr, false, DT_2))
            lhs = sub_1ccc(lhs, DT_INT);

        if (hasRhs && sub_5a76(&rhs->attr, false, DT_2))
            rhs = sub_1ccc(rhs, DT_INT);
    }

    switch (p1) 
    {
    case T_60:
        /* return lhs; */ ReturnOverlay3(lhs);
    case T_DOT:
        rhs = sub_1340(lhs, rhs);
        break;
    case T_121:
        /* tmpExpr    = curFuncNode->attr; */
	ReadMem((char*)&tmpExpr, (char*)curFuncNode, sizeof(struct _s8));
        tmpExpr.c7 = SNODE;

        if (sub_5a76(&tmpExpr, false, DT_VOID))
            prError("void function cannot return value");
        else
            lhs = sub_1f5d(lhs, &tmpExpr, 1);
        break;
    }

    if ((opFlags & 3)) 
    {
        if ((opFlags & 2))
            lhs = sub_1b94(lhs);
        if ((opFlags & 1))
            rhs = sub_1b94(rhs);
    } 
    else if ((opFlags & 4) && (lhs->attr.i4 & 1) && lhs->attr.c7 == SNODE &&
               sub_5b08(&rhs->attr)) /* 16e1 */
        rhs = sub_1ccc(/* sub_1441(T_STAR, rhs, sub_1ebd(lhs)) */ 
			CallOverlay3(sub_1441, T_STAR, rhs, sub_1ebd(lhs), 32+5),
                       		     (rhs->attr.dataType & DT_UNSIGNED) ? DT_UCONST : DT_CONST);
    else if (p1 == T_PLUS && (rhs->attr.i4 & 1) && rhs->attr.c7 == SNODE &&
             sub_5b08(&lhs->attr)) 
    { /* 1740 */
        savedLhs = lhs;
        lhs      = rhs;
        rhs      = sub_1ccc(/* sub_1441(T_STAR, savedLhs, sub_1ebd(lhs)) */ 
			    CallOverlay3(sub_1441, T_STAR, savedLhs, sub_1ebd(lhs), 32+5),
                       			 (rhs->attr.dataType & DT_UNSIGNED) ? DT_UCONST : DT_CONST);
    } 
    else if ((opFlags & 8) && (lhs->attr.i4 & 1) && lhs->attr.c7 == SNODE &&
               (!hasRhs || ((rhs->attr.i4 & 1) && rhs->attr.c7 == SNODE))) 
    { /* 17ab */
        if (!(opFlags & 0x8000) || (!sub_5a4a(&lhs->attr) && !sub_5a4a(&rhs->attr))) 
	{
            if (hasRhs && 
		! /* sub_591d(&lhs->attr, false, &rhs->attr, false) */
		(bool)CallOverlay4(sub_591d, &lhs->attr, false, &rhs->attr, false, 32+4))
                prWarning("operands of %.3s not same pointer type", opStr);
            else if (p1 == T_MINUS) 
	    {
                minusLhs      = lhs;
                minusLhsValid = true;
                lhs           = sub_1ccc(lhs, DT_CONST);
                rhs           = sub_1ccc(rhs, DT_CONST);
            }
        }
    } else if ((opFlags & 0x30) && sub_5ad5(&lhs->attr) &&
               (!hasRhs || sub_5ad5(&rhs->attr))) 
    { /* 187a */
        if (opFlags & 0x40) 
	{
            var13 = sub_1d5a(&lhs->attr, &rhs->attr);
            lhs   = sub_1ccc(lhs, var13);
            rhs   = sub_1ccc(rhs, var13);
        } /* 18fa */

        if (!(opFlags & 0x10) && (!sub_5b08(&lhs->attr) || (hasRhs && !sub_5b08(&rhs->attr))))
            prError("integral type required");

    } 
    else if (opFlags & 0x80) 
    { /* 1937 */
        if (sub_5a76(&lhs->attr, false, DT_VOID) || (hasRhs && sub_5a76(&rhs->attr, false, DT_VOID))) 
	{
            if (p1 == T_124 && lhs->tType == T_61 && sub_5a76(&rhs->attr, false, DT_VOID)) 
	    {
                lhs->attr = rhs->attr;
                /* sub_2569(rhs); */ CallOverlay1(sub_2569, rhs, 32+5);
                /* return lhs; */ ReturnOverlay3(lhs);
            }

            prError("illegal use of void expression");
        } 
	else if (!(opFlags & 0x200) &&
                   (!sub_5aa4(&lhs->attr) || (hasRhs && !sub_5aa4(&rhs->attr)))) /* 19cc */
            prError("simple type required for %.3s", opStr);
        else if (opFlags & 0x1000) 
	{ /* 1a11 */
            if ((opFlags & 0x8000)) 
	    {
                if (/* sub_2105(lhs) */ CallOverlay1(sub_2105, lhs, 32+5) && (rhs->attr.i4 & 1) && rhs->attr.c7 == SNODE)
                    lhs = sub_1bf7(lhs, &rhs->attr);
                else if (/* sub_2105(rhs) */ CallOverlay1(sub_2105, rhs, 32+5) && (lhs->attr.i4 & 1) && lhs->attr.c7 == SNODE)
                    rhs = sub_1bf7(rhs, &lhs->attr);
            } /* 1a95 */

            if (! /* sub_591d(&lhs->attr, false, &rhs->attr, false) */
		(bool)CallOverlay4(sub_591d, &lhs->attr, false, &rhs->attr, false, 32+4))
                prWarning("operands of %.3s not same type", opStr);
        }
    } 
    else if (!(opFlags & 0x200)) /* 1ac3 */
        prError("type conflict");

    /* 1ad1 */
    if (opFlags & 0x400)
        rhs = sub_1f5d(rhs, &lhs->attr, (opFlags & 4) == 0);

    savedLhs = sub_225a(p1, lhs, rhs);

    if (minusLhsValid)
        savedLhs = /* sub_1441(T_DIV, savedLhs, sub_1ebd(minusLhs)); */ 
		   CallOverlay3(sub_1441, T_DIV, savedLhs, sub_1ebd(minusLhs), 32+5);

    /* return savedLhs; */ ReturnOverlay3(savedLhs);
}


/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

extern int8_t depth;       /* a288 */
extern uint8_t byte_a289;  /* a289 */
extern bool unreachable;   /* a28a */
extern int16_t word_a28b;  /* a28b */
extern sym_t *curFuncNode; /* a28d */
extern sym_t *p25_a28f;    /* ad8f */

void sub_05b5(expr_t *p1);
void prError(char*, ...);

/**************************************************
 * 81: 3ADF +++
 * minor differences due to adding missing arg and use
 * of uint8_t paramater
 * one use of add a,255 vs sub a,1 i.e. equivalent
 **************************************************/
void O_sub_3adf(void) 
{
    uint8_t tok;
    uint8_t scType;
    s8_t attr;
    uint8_t varb;
    uint8_t scFlags;
    register sym_t *st;
    sym_t symbuf;
    sym_t* ps = &symbuf;

    scFlags = /* sub_5dd1(&scType, &attr); */ (uint8_t)CallOverlay2(sub_5dd1, &scType, &attr, 32+6);

    if ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_SEMI)
        /* return; */ ReturnOverlay0(0);

    ungetTok = tok;
    varb     = true;

    for (;;) 
    {
        p25_a28f = 0;
        st       = /* sub_69ca(scType, &attr, scFlags & ~1, 0); */ 
		   CallOverlay4(sub_69ca, scType, &attr, scFlags & ~1, 0, 32+6);
        tok      = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

	if (st)
	    ReadMem((char*)&symbuf, (char*)st, sizeof(struct _sym));
 
        if (st && (/* st->m18 */ ps->m18 & 0x10) && 
		   /* st->a_c7 */ ps->a_c7 == ANODE) 
	{
            if (varb && tok != T_COMMA && tok != T_SEMI && scType != T_TYPEDEF) 
	    {
                byte_a289 = /* st->a_c7 */ ps->a_c7 == ANODE && 
			    /* st->a_i4 */ ps->a_i4 == 0 &&
                            (/* st->a_dataType */ ps->a_dataType == DT_VOID || 
			     /* st->a_dataType */ ps->a_dataType == DT_INT);
                ungetTok = tok;
                sub_516c(st);
                sub_0493(st);
                curFuncNode = st;
                sub_409b();
                /* return; */ ReturnOverlay0(0);
            }

            if (p25_a28f && !(/* p25_a28f->m18 */ GetWord((char*)p25_a28f, OFF_m18) & 8))
                expectErr("function body");

            ++depth;
            /* checkScopeExit(); */ CallOverlay0(checkScopeExit, 32+4);
            --depth;
            sub_0493(st);
        } 
	else if (tok == T_EQ) 
	{
            if (scType == T_TYPEDEF)
                prError("illegal initialisation");

            sub_516c(st);
            sub_0493(st);
            /* sub_3c7e(st); */ CallOverlay1(sub_3c7e, st, 32+4);
            tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);
        } 
	else if (st) 
	{
            if ((scFlags & 1) || /* st->m20 */ ps->m20 != T_EXTERN) 
	    {
                sub_516c(st);
                sub_0493(st);
            } 
	    else
                sub_01ec(st);
        }

        if (tok == T_ID || tok == T_STAR) 
	{
            expectErr(",");
            ungetTok = tok;
        } 
	else if (tok != T_COMMA)
            break;

        varb = false;
    }

    if (tok != T_SEMI) 
    {
        expectErr(";");

        while (tok > T_RBRACE)
            tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);
    }

    ReturnOverlay0(0);
}

/**************************************************
 * 82: 3C7E PMO +++
 * use of uint8_t param
 **************************************************/
void O_sub_3c7e(sym_t *p1) 
{
    int16_t var2;
    register sym_t *st;
    expr_t* ex;

    if (p1) 
    {
        printf("[i ");
        sub_573b(p1, stdout);
        putchar('\n');
        st = p1;

        if ((var2 = sub_3d24(st, 1)) < 0) 
	{
            prError("initialisation syntax");
            /* skipToSemi(); */ CallOverlay0(skipToSemi, 32+4);
	}
	else if (/* st->a_c7 */ GetByte((char*)st, OFF_s8_t_c7) == ENODE && 
		 /* st->a_expr */ (ex = (expr_t*)GetWord((char*)st, OFF_s8_t_pExpr)) && 
		 /* sub_2105(st->a_expr) */ (bool)CallOverlay1(sub_2105, /* st->a_expr */ ex, 32+5) ) 
	{
            /* sub_2569(st->a_expr); */ CallOverlay1(sub_2569, /* st->a_expr */ ex, 32+5);
            /* st->a_expr = allocIConst(var2); */
	    PutWord((char*)st, OFF_s8_t_pExpr, CallOverlay1L(allocIConst, var2, 32+5));
        }

        printf("]\n");
    } 
    else
        /* skipToSemi(); */ CallOverlay0(skipToSemi, 32+4);

    ReturnOverlay1(0);
}

/**************************************************
 * 83: 3D24 +++
 * minor optimiser differences including moving basic
 * blocks. Use of uint8_t parameter
 **************************************************/
/* manual string optimisation */
char Ustr[] = ":U ..\n";
char Dstr[] = "..\n";

int16_t sub_3d24(sym_t *st, uint8_t p2) 
{
    int16_t var2;
    uint8_t tok;
    char *var5;
    bool haveLbrace;
    sym_t *var8;
    sym_t *vara;
    bool varb;
    expr_t *vard;
    bool vare;
    sym_t symbuf;
    register sym_t* ps = &symbuf;

    ReadMem((char*)&symbuf, (char*)st, sizeof(struct _sym));

    var2 = -1;

    if (p2 && /* st->a_c7 */ ps->a_c7 == ENODE && /* st->a_expr */ ps->a_expr) 
    {
        printf(":U ..\n");

        if ((haveLbrace = ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_LBRACE)))
            tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

        if (tok == T_SCONST && 
	    /* st->attr.i4 */ ps->attr.i4 == 0 && 
	    (/* st->attr.dataType */ ps->attr.dataType & ~1) == 4) 
	{
            var2 = 0;
            var5 = yylval.yStr;

            while (var2 < strChCnt) 
	    {
                printf("-> %d `c\n", *var5++);
                ++var2;
            }

            free(yylval.yStr);

            if (/* sub_2105(st->a_expr) */ (bool)CallOverlay1(sub_2105, /* st->a_expr */ ps->a_expr, 32+5)) 
	    {
                printf("-> %d `c\n", 0);
                ++var2;
            }

            if (haveLbrace)
                tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

        } 
	else if (!haveLbrace) /* 3e4a */
            expectErr("{");
        else 
	{
            ungetTok = tok;

            if (/* st->a_i4 */ ps->a_i4 == 0 && /* st->a_dataType */ ps->a_dataType == DT_POINTER)
                st = /* st->a_nextSym */ ps->a_nextSym;
            else
                p2 = 0;

            var2 = 0;
            for (;;) 
	    {
                if (sub_3d24(st, p2) < 0)
                    break;

                var2++;

                if ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_RBRACE || tok != T_COMMA || 
		    (tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_RBRACE)
                    break;

                ungetTok = tok;
            }
        }

        /* 3e22 */
        if (haveLbrace && tok != T_RBRACE) 
	{
            expectErr("}");
            var2 = -1;
        }

        printf(Dstr);
    } 
    else if ((p2 == 0 || 
	      /* st->a_c7 */ ps->a_c7 != ENODE) && 
	      /* st->a_i4 */ ps->a_i4 == 0 &&
              /* st->a_dataType */ ps->a_dataType == DT_STRUCT) 
    { /* 3ec6 */
        if (p2)
            printf(Ustr);

        printf(Ustr);

        if ((var8 = /* st->a_nextSym */ ps->a_nextSym)) 
	{
            varb = (tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_LBRACE;

            if (!varb)
                ungetTok = tok;

            vara = /* var8->nMemberList */ (sym_t*)GetWord((char*)var8, OFF_nMemberList);

            do 
	    {
                if ((sub_3d24(vara, 1) < 0))
                    break;

                if ((vara = vara->nMemberList) == var8)
                    break;

                if ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) != T_COMMA) 
		{
                    ungetTok = tok;
                    break;
                }
            } 
	    while (!varb || (ungetTok = tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) != T_RBRACE);

            /* 3f7c: */
            if (varb) 
	    {
                if ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_COMMA)
                    tok = /* yylex(); */ (uint8_t)CallOverlay0(yylex, 32+4);

                if (tok != T_RBRACE)
                    expectErr("}");
                else
                    var2 = 1;
            } 
	    else
                var2 = 1;
        } 

	/* 3fcd */
        printf(Dstr);

        if (p2)
            printf(Dstr);

    } 
    else if ((p2 && 
	       /* st->attr.c7 */ ps->attr.c7 == ENODE) || 
	       /* st->attr.c7 */ ps->attr.c7 == ANODE ||
               (!(/* st->a_i4 */ ps->a_i4 & 1) && 
		  /* st->attr.dataType */ ps->attr.dataType >= (unsigned char)T_AUTO))
        prError("illegal initialisation");
    else 
    {
        vare = (tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) == T_LBRACE;

        if (!vare)
            ungetTok = tok;

        vard          = /* allocSType(&st->attr); */ 
			CallOverlay1(allocSType, /* &st->attr */ &symbuf.attr, 32+5);
        vard->attr.c7 = SNODE;

        if ((vard = /* sub_1441(T_125, vard, sub_0a83(3)) */ 
		    CallOverlay3(sub_1441, T_125, vard, CallOverlay1(sub_0a83, 3, 32+5), 32+5)))
            sub_05b5(vard->t_alt);

        if (vare && /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4) != T_RBRACE)
            expectErr("}");

        /* sub_2569(vard); */ CallOverlay1(sub_2569, vard, 32+5);
        var2 = 1;
    }

    return var2;
}

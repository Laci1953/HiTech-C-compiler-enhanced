/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#include "optim1.h"

#define _Z       0      /* 0000 0000 */
#define _U       1      /* 0000 0001 */
#define _L       2      /* 0000 0010 */
#define _D       4      /* 0000 0100 */
#define _H       8      /* 0000 1000 */
#define _S      16      /* 0001 0000 */

#define ISALPHA(c)     ( ccClass[c] & (_U|_L) )
#define ISUPPER(c)     ( ccClass[c] & _U )
#define ISDIGIT(c)     ( ccClass[c] & _D )
#define ISXDIGIT(c)    ( ccClass[c] & _H )
#define ISSPACE(c)     ( ccClass[c] & _S )
#define ISALNUM(c)     ( ccClass[c] & (_U|_L|_D) )

extern char* conditions[];
extern char* regs[];
extern operator_t operators[];
extern int ccSwap[];
extern char* psectNames[];
extern struct _s1 regHiLoMap[];
extern struct _s2 regHiLoValMap[];
extern operand_t lo1,lo2,lo3,lo4;
extern operand_t *plo1, *plo2, *plo3, *plo4;

/*
 *      Common code sequences
 */
#define PEEKCH() (charsLeft > 0 ? *ptr_inbuf : '\n')
#define GETCH()  (--charsLeft >= 0 ? *ptr_inbuf++ : get_line())

/**************************************************************************
 23     sub_122f        ok++    Used in: optimise
 **************************************************************************/
void sub_122f() {
register     inst_t *pi;
    operand_t *po;
    sym_t* ps;

#ifdef DEBUG
    pr_warning("sub_122f");
#endif

    for (pi = root->pNext; pi; pi = pi->pNext) /* set initial values for symbols */
        if (pi->type == T_SYMBOL)
            if (pi->iPSym->label[0] == '_') { /* check for public name */
                pi->aux = INT_MAX;
            } else {
                pi->aux = 0;
            }

    for (pi = root->pNext; pi; pi = pi->pNext) { /* update reference counts */
        if (pi->type == T_JP || pi->type == T_DJNZ) {
            if ((po = pi->iLhs) &&
                 /* po->type */ (uint8_t)GetByte((char*)po, OFF_type) == (uint8_t)T_CONST &&
                 /* po->oPSym */ (ps = (sym_t*)GetWord((char*)po, OFF_oPSym)) &&
                 /* po->oPSym->p.pInst */ ps->p.pInst ) {

                /* po->oPSym->p.pInst->aux++; */
                ps->p.pInst->aux++;
            }
        }
    }
    for (pi = switchVectors; pi; pi = pi->pNext) { /* do the same for the jump tables */
        if (pi->type == T_DEFW) {
            if (pi->iLhs &&
                 (po = pi->iLhs) &&
                 /* po->type */  (uint8_t)GetByte((char*)po, OFF_type) == (uint8_t)T_CONST &&
                 /* po->oPSym */ (ps = (sym_t*)GetWord((char*)po, OFF_oPSym)) &&
                 /* po->oPSym->p.pInst */ ps->p.pInst) {

                /* po->oPSym->p.pInst->aux++; */
                ps->p.pInst->aux++;
            }
        }
    }
}

/**************************************************************************
 24     sub_1369        ok++ (PMO)
 **************************************************************************/
bool sub_1369(register operand_t const *pi_arg) {
/* pi_arg is checked !!! */
#ifdef DEBUG
    pr_warning("sub_1369");
#endif

    if (pi_arg && IsValid((char*)pi_arg))
    {
        ReadMem((char*)&lo1, (char*)pi_arg, sizeof(operand_t));

        return (plo1->type == (uint8_t)T_CONST) ||
                (plo1->type == (uint8_t)T_INDEXED) ||
                 (plo1->type == (uint8_t)T_REGREF && plo1->aux == (uint8_t)REG_HL);
    }
    else
        return false;
}

/**************************************************************************
 25     sub_1397        ok++ (PMO)
 **************************************************************************/
/* note there are occasions when pi is accessed after this is called so
 * freeInst has to preserve at least pi->pNext
 */
void removeInstr(register inst_t *pi) {
    sym_t *s;

#ifdef DEBUG
    pr_warning("removeInstr pi=%04x", pi);
#endif

    if (pi->type == T_JP &&
        /* pi->iLhs->type */ GetByte((char*)(pi->iLhs), OFF_type) == (uint8_t)T_CONST &&
        /* pi->iLhs->oPSym */ (s = (sym_t*)GetWord((char*)(pi->iLhs), OFF_oPSym)) ) {
        removeLabelRef(/* pi->iLhs->oPSym */ s );
    }

    pi->pAlt->pNext = pi->pNext;
    pi->pNext->pAlt = pi->pAlt;
    if (pi->type != T_SYMBOL) {
        freeOperand(pi->iLhs);
        freeOperand(pi->iRhs);
    }
    freeInst(pi);
}

/**************************************************************************
 26     sub_140b        ok++
 **************************************************************************/
inst_t *getNxtRealInst(register inst_t *pi) {

#ifdef DEBUG
    pr_warning("getNxtRealInst");
#endif

    while (pi->type == T_SYMBOL) {
        pi = pi->pNext;
    }
    return pi;
}

/**************************************************************************
 27     sub_142f        ok++
 **************************************************************************/
bool operandsSame(register operand_t const *po1_arg, operand_t const *po2_arg) {
    operand_t *po1, *po2;

#ifdef DEBUG
    pr_warning("operandsSame");
#endif

    if (!po1_arg && !po2_arg) {
        return true;
    }

    if (!po1_arg || !po2_arg) {
        return false;
    }

    if (!IsValid((char*)po1_arg))
        po1 = po1_arg;
    else
    {
        ReadMem((char*)&lo1, (char*)po1_arg, sizeof(operand_t));
        po1 = plo1;
    }

    if (!IsValid((char*)po2_arg))
        po2 = po2_arg;
    else
    {
        ReadMem((char*)&lo2, (char*)po2_arg, sizeof(operand_t));
        po2 = plo2;
    }

    if (po1->type != po2->type || po1->aux != po2->aux || po1->type == (uint8_t)T_INVALID) {
        return false;
    }

    return po1->oPSym == po2->oPSym && po1->oVal == po2->oVal;
}

/**************************************************************************
 28     instrSame       sub_14ac        ok++
 **************************************************************************/
bool instrSame(register inst_t const *pi1, inst_t const *pi2) {

#ifdef DEBUG
    pr_warning("instrSame");
#endif

    if (pi1->type == T_INVALID || pi1->type == T_SYMBOL || pi2->type == T_INVALID || pi2->type == T_SYMBOL) {
        return false;
    }

    if (pi1->type != pi2->type || pi1->aux != pi2->aux) {
        return false;
    }

    return operandsSame(pi1->iLhs, pi2->iLhs) && operandsSame(pi1->iRhs, pi2->iRhs);
}

/**************************************************************************
 29     sub_153d        ok++
 **************************************************************************/
void removeLabelRef(register sym_t *ps) {
    inst_t *pi;

#ifdef DEBUG
    pr_warning("removeLabelRef");
#endif

    if (!(pi = ps->p.pInst)) {
        return;
    }
    if (pi->aux == 0) {
        pr_error("Refc == 0");
    }
    if (--pi->aux != 0) {
        return;
    }
    removeInstr(pi);
    ps->p.pInst = NULL;
    freeSymbol(ps);
    logOptimise(O_UNREF_LAB); /* 6fc5 opt_msg[4] = "Unref'ed labels" */
}

/**************************************************************************
 30     sub_15ad        ok++    (PMO) Used in optimize
 *
 * code optimised over original as noted below, otherwise identical
 * 1) for loop iteration expression moved
 * 2) code to increment an optiminse counter & set hasChanged = true shared
 * 3) sub_1d94() == 0, test removed as code falls through in either case
 **************************************************************************/
void sub_15ad() {
    inst_t *pi;
    sym_t *s;
    uint8_t t;

#ifdef DEBUG
    pr_warning("sub_15ad");
#endif

    for (gPi = root; gPi; gPi = gPi->pNext)
    {
        if (IsValid((char*)gPi->iLhs))
        {
            s = (sym_t*)GetWord((char*)(gPi->iLhs), OFF_oPSym);
            t = GetByte((char*)(gPi->iLhs), OFF_type);
        }
        else
        {
            s = NULL;
            t = 0x7F;
        }

        if (s)
            pi = s->p.pInst;
        else
            pi = NULL;

        if (!sub_1795() && gPi->type == T_JP && !sub_1aec() && !sub_1b86())
        {
            if (/* gPi->iLhs->type */ t == (uint8_t)T_CONST &&
                pi &&
                /* gPi->iLhs->oPSym->p.pInst */ pi == gPi->pNext)
            {
                removeInstr(gPi);
                logOptimise(O_JMP_TO_PLUS1); /* 6fc9 opt_msg[6] = "Jumps to .+1" */
            }
            else
            {
                if (gPi->aux == 0)
                { /* 1648 / 164B */
                    while (gPi->pNext->type != T_INVALID && gPi->pNext->type != T_SYMBOL)
                    {
                        removeInstr(gPi->pNext);
                        logOptimise(O_UNREACH_LAB); /* 6fc7 opt_msg[5] = "Unreachable code" */
                    }

                    if (/* gPi->iLhs->type */ t != (uint8_t)T_REGREF &&
                        /* gPi->iLhs->oPSym->p.pInst */ pi &&
                        sub_1071(gPi))
                        continue;
                }

                if (/* gPi->iLhs->type */ t == T_CONST &&
                   (gPi->pNext->type == T_CALL || gPi->pNext->type == T_JP) &&
                    gPi->pNext->aux == 0 &&
                    pi &&
                    /* gPi->iLhs->oPSym->p.pInst */ pi == gPi->pNext->pNext)
                {
                    gPi->pNext->aux = ccSwap[gPi->aux]; /* swap condition code */
                    removeInstr(gPi);
                    logOptimise(O_SKIP_OVER_JMP); /* 6fcb opt_msg[7] = "Skips over jumps" */
                }
                else if (sub_1c67() || !sub_1d94())
                    ;
            }
        }
    }
}


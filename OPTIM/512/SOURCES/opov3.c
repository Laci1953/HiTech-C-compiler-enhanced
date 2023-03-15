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
 20	chkIXYUsage	sub_0e67	ok++	Used in: optimise
 **************************************************************************/
void O_chkIXYUsage() {
    register inst_t *pi;
    uint8_t typeL;
    bool vL, vR;

#ifdef DEBUG
    pr_warning("chkIXYUsage");
#endif

    usesIXorIY = false;

    for (pi = root; pi; pi = pi->pNext) 
    {
        vL = pi->iLhs && IsValid((char*)pi->iLhs);
        vR = pi->iRhs && IsValid((char*)pi->iRhs);

        if (pi->type != T_SYMBOL &&
            ((vL && (typeL = GetByte((char*)(pi->iLhs), OFF_type)) == (uint8_t)T_INDEXED) || 
	     (vR && GetByte((char*)(pi->iRhs), OFF_type) == (uint8_t)T_INDEXED) ||
              (vL && typeL == (uint8_t)T_REG && GetByte((char*)(pi->iLhs), OFF_aux) >= (uint8_t)REG_IX))) 
	{
            usesIXorIY = true;
            /* return; */ ReturnOverlay0(0);
        }
    }

    ReturnOverlay0(0);
}

/**************************************************************************
 21	sub_0ed1			ok++	used in optimise
 **************************************************************************/
void O_sub_0ed1() {
register     inst_t    *pi2;
    operand_t *po;
    bool       newLabel; /* Flag of the sub_0ca2 call */
    inst_t *pi1;
    uint8_t ty;
    bool valid_po;

#ifdef DEBUG
    pr_warning("sub_0ed1");
#endif

    for (pi1 = word_6ffc; pi1; pi1 = pi1->pAlt) 
    {
        if (pi1->type == T_CONST) 
	{
            newLabel = false;
            logOptimise(O_TMP_LAB); /* 6fc3 opt_msg[3] = "Temporary labels" */
            for (pi2 = pi1->pAlt; pi2; pi2 = pi2->pAlt) {
                if (pi2->type == T_CONST && pi2->aux == pi1->aux) 
                    break;

		po = pi2->iLhs;

		valid_po = IsValid((char*)po);

		if (valid_po)
		    ty = GetByte((char*)po, OFF_type);
		else
		    ty = 0x7F;

                if (valid_po && 
		     /* po->type */ ty == (uint8_t)T_FWD && 
		     /* po->oVal */ (int)GetWord((char*)po, OFF_oVal) == pi1->aux) 
		{
                    if (!newLabel) 
		    {
                        syntheticLabel(pi1);
                        newLabel = true;
                    }

                    /* po->type      = T_CONST; */
		    PutByte((char*)po, OFF_type, (uint8_t)T_CONST);

                    /* po->oPOperand = pi1->pNext->iLhs; */
		    PutWord((char*)po, OFF_oPOperand, (short)pi1->pNext->iLhs);

                    /* po->oVal      = 0; */
		    PutWord((char*)po, OFF_oVal, (short)0);
                }
            }
            if (pi1->aux < REG_BC) 
	    { /* m7: */
                if (!newLabel) 
                    syntheticLabel(pi1);

                for (pi2 = pi1->pNext; pi2; pi2 = pi2->pNext) 
		{

		    po = pi2->iLhs;

		    valid_po = IsValid((char*)po);

		    if (valid_po)
		        ty = GetByte((char*)po, OFF_type);
		    else
		        ty = 0x7F;
		    
                    if (valid_po && 
			/* po->type */ ty == (uint8_t)T_BWD && 
			/* po->oVal */ (int)GetWord((char*)po, OFF_oVal) == pi1->aux) 
		    {

                        /* po->type      = T_CONST; */
		        PutByte((char*)po, OFF_type, (uint8_t)T_CONST);

                        /* po->oPOperand = pi1->pNext->iLhs; */
		        PutWord((char*)po, OFF_oPOperand, (short)pi1->pNext->iLhs);

                        /* po->oVal      = 0; */
		        PutWord((char*)po, OFF_oVal, (short)0);

                        break;
                    }
                }
            }

            pi1 = pi1->pNext; /* m12: */
            removeInstr(pi1->pAlt);
        }
    }

    ReturnOverlay0(0);
}

/**************************************************************************
 23     sub_122f        ok++    Used in: optimise
 **************************************************************************/
void O_sub_122f() {
register     inst_t *pi;
    operand_t *po;
    sym_t* ps;

#ifdef DEBUG
    pr_warning("sub_122f");
#endif

    for (pi = root->pNext; pi; pi = pi->pNext) /* set initial values for symbols */
    {
        if (pi->type == T_SYMBOL)
        {
            if (/* pi->iPSym->label[0] */ *GetLabel(pi->iPSym) == (char)'_') { /* check for public name */
                pi->aux = INT_MAX;
            } else {
                pi->aux = 0;
            }
        }
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

    ReturnOverlay0(0);
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
//    freeSymbol(ps);
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
void O_sub_15ad() {
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

    ReturnOverlay0(0);
}

/**************************************************************************
 62 sub_4625    ok+
 *
 *  Generates correct code, but in a sequence different from original
 **************************************************************************/
bool sub_4625(register inst_t *pi_arg) {
    operand_t local_pi;
    operand_t *pi = &local_pi;
    uint8_t t,a;

#ifdef DEBUG
    pr_warning("sub_4625");
#endif

/* -------------------------------------------------------------------------- */
    if (IsValid((char*)pi_arg->iLhs))
    {
        ReadMem((char*)&local_pi, (char*)pi_arg->iLhs, 0, sizeof(operand_t));
        t = pi->type;
        a = pi->aux;
    }
    else
    {
        t = pi_arg->iLhs->type;
        a = pi_arg->iLhs->aux;
    }
/* -------------------------------------------------------------------------- */

    switch (pi_arg->type) {
        case T_LD:
        case T_STK:
            return true;
        case T_INCDEC:
            return /* pi_arg->iLhs->type */ t == (uint8_t) T_REG && /*pi_arg->iLhs->aux*/ a >= (uint8_t) REG_BC;
        case T_EX:
            return /* pi_arg->iLhs->aux */ a != (uint8_t) REG_AF;
    }
    return false;
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
        ReadMem((char*)&lo1, (char*)po1_arg, 0, sizeof(operand_t));
        po1 = plo1;
    }

    if (!IsValid((char*)po2_arg))
        po2 = po2_arg;
    else
    {
        ReadMem((char*)&lo2, (char*)po2_arg, 0, sizeof(operand_t));
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
 55 sub_4000    ok++ (PMO)
 **************************************************************************/
bool sub_4000(register inst_t const *pi) {
   sym_t *p;

#ifdef DEBUG
    pr_warning("sub_4000");
#endif

    return pi->type == T_JP &&
           (p = (sym_t*)GetWord((char*)(pi->iLhs), OFF_oPSym) ) &&
           strcmp(/* p->label */ GetLabel(p), "cret") == 0;
}


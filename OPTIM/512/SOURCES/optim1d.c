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
 24     sub_1369        ok++ (PMO)
 **************************************************************************/
bool sub_1369(register operand_t const *pi_arg) {
/* pi_arg is checked !!! */
#ifdef DEBUG
    pr_warning("sub_1369");
#endif

    if (pi_arg && IsValid((char*)pi_arg))
    {
        ReadMem((char*)&lo1, (char*)pi_arg, 0, sizeof(operand_t));

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



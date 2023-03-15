/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "optim1.h"

/**************************************************************************
 22	sub_1071			ok++	used in sub_15ad
 **************************************************************************/
bool sub_1071(register inst_t *pi) {
    inst_t *pi_1;
    inst_t *pi_2;
    inst_t *pi_3;
    sym_t * s;

#ifdef DEBUG
    pr_warning("sub_1071");
#endif

    if (pi->type != T_JP || pi->aux != 0 ||
	 /* pi->iLhs->type */ GetByte((char*)(pi->iLhs), OFF_type) != (uint8_t)T_CONST) {
        return false;
    }

    /* pi_1 = pi->iLhs->oPSym->p.pInst; */
    s = (sym_t*)GetWord((char*)(pi->iLhs), OFF_oPSym);

    if (!s)
	pr_error("oPSym is NULL");

    pi_1 = s->p.pInst;

    while (pi_1->type == T_SYMBOL) {
        pi_1 = pi_1->pAlt;
    }

    if (pi_1 == pi) {
        removeInstr(pi);
        logOptimise(O_JMP_TO_PLUS1); /* 6fc9 opt_msg[6] = "Jumps to .+1" */
        return false;
    }
    if (pi_1->type != T_JP || pi_1->aux != 0) {
        return false;
    }

    for (pi_3 = pi_1->pNext; pi_3->type && (pi_3->type != T_JP || pi_3->aux != 0); pi_3 = pi_3->pNext)
        ;

    if (pi_3 == pi_1->pNext || pi_3->type == T_INVALID || pi_3 == pi) {
        return false;
    }

    pi->pNext->pAlt  = pi_3;
    pi_1->pNext->pAlt = pi;

    pi_3->pNext->pAlt = pi_1;

    pi_2              = pi_1->pNext;
    pi_1->pNext       = pi_3->pNext;
    pi_3->pNext       = pi->pNext;
    pi->pNext        = pi_2;
    logOptimise(O_CODE_MOTIONS); /* 6fdd opt_msg[16] = "Code motions" */
    removeInstr(pi);
    return logOptimise(O_JMP_TO_PLUS1); /* 6fc9 opt_msg[6] = "Jumps to .+1" */
}

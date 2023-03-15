/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

/*
 * File part21.c created 14.08.2021, last modified 11.11.2021.
 *
 * The part21.c file is part of the restored optimization program
 * from the Hi-Tech C compiler v3.09 package.
 *
 *	Andrey Nikitin & Mark Ogden 11.11.2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "optim1.h"

/**************************************************************************
 31 sub_1795    ok++
 **************************************************************************/
bool sub_1795() {
register     inst_t *pi;
    int stackAdjust;
    int val;

#ifdef DEBUG
    pr_warning("sub_1795");
#endif

    if (gPi->type != T_LD) {
        return false;
    }

    ReadMem((char*)&lo1, (char*)gPi->iLhs, sizeof(operand_t));

    if (/* gPi->iLhs->type */ plo1->type != (uint8_t)T_REG ||
	/* gPi->iLhs->aux */ plo1->aux != (uint8_t)REG_SP) {
        return false;
    }

    if ((pi = gPi->pAlt)->type != T_CADD || (pi = pi->pAlt)->type != T_LD) {
        return false;
    }

    ReadMem((char*)&lo1, (char*)pi->iLhs, sizeof(operand_t));
    ReadMem((char*)&lo2, (char*)pi->iRhs, sizeof(operand_t));

    if (/* pi->iLhs->type */ plo1->type != (uint8_t)T_REG ||
	/* pi->iLhs->aux */ plo1->aux != (uint8_t)REG_HL ||
	/* pi->iRhs->type */ plo2->type != (uint8_t)T_CONST) {
        return false;
    }

    if (/* pi->iRhs->oPOperand */ plo2->oPOperand) {
        pr_error("Funny stack adjustment");
    }

    stackAdjust = /* pi->iRhs->oVal */ plo2->oVal;
    pi          = gPi->pNext;

    if (pi->type == T_SIMPLE && pi->aux == I_EXX) {
        pi = pi->pNext;
    }

    for (; pi->type != T_CALL && pi->type != T_JP && pi->type != T_STK && (pi->type != T_EX ||
	 /* pi->iLhs->aux */ GetByte((char*)(pi->iLhs)+OFF_aux) != (uint8_t)REG_SP);
            pi = pi->pNext)
        ;

    if (stackAdjust > 0 && usesIXorIY && pi->aux == 0 && sub_4000(pi)) {
        removeInstr(gPi->pAlt->pAlt);
        removeInstr(gPi->pAlt);
        gPi = gPi->pAlt;
        removeInstr(gPi->pNext);
        if (gPi->pNext->type == T_SIMPLE && gPi->pNext->aux == I_EXX && gPi->type == T_SIMPLE &&
                gPi->aux == I_EXX) { /* exx */
            removeInstr(gPi->pNext);
            removeInstr(gPi);
        }
        return logOptimise(O_STK_ADJUST); /* 6fc1 opt_msg[2] = "Stack adjustments" m7: */
    }

    pi = gPi->pAlt->pAlt;

    if (stackAdjust < 0) {
        stackAdjust = -stackAdjust;
    }

    if (pi->pAlt->type == T_SIMPLE && pi->pAlt->aux == I_EXX) {
        stackAdjust -= 2;
    }

    if (stackAdjust > 8 || /* pi->iRhs->oVal */ (val = GetWord((char*)(pi->iRhs)+OFF_oVal)) < 0) {
        return false;
    }

    logOptimise(O_STK_ADJUST); /* 6fc1 opt_msg[2] = "Stack adjustments" */

    stackAdjust = /* pi->iRhs->oVal */ val;
    pi          = pi->pAlt;
    removeInstr(pi->pNext->pNext);
    removeInstr(pi->pNext);
    removeInstr(gPi);
    gPi = pi;

    while (stackAdjust != 0) {
        gPi             = allocInst(gPi);
        gPi->iLhs       = allocOperand();

        /* gPi->iLhs->type = T_REG; */
	PutByte((char*)(gPi->iLhs)+OFF_type, (uint8_t)T_REG);

        if (1 < stackAdjust) {

            /* gPi->iLhs->aux = REG_BC; */
	    PutByte((char*)(gPi->iLhs)+OFF_aux, (uint8_t)REG_BC);

            gPi->type      = T_STK;
            stackAdjust -= 2;
            gPi->aux = I_POP;
        } else {

            /* gPi->iLhs->aux = REG_SP; */
	    PutByte((char*)(gPi->iLhs)+OFF_aux, (uint8_t)REG_SP);

            gPi->type      = T_INCDEC; /* Decrement, Increment */
            --stackAdjust;
            gPi->aux = SI_INC;
        }
    }
    if (gPi->pNext->type == T_SIMPLE && gPi->pNext->aux == I_EXX && pi->type == T_SIMPLE && pi->aux == I_EXX) {
        removeInstr(gPi->pNext);
        removeInstr(pi);
    }
    return true;
}

/**************************************************************************
 32 sub_1aec    ok++
 **************************************************************************/
bool sub_1aec() {
    register inst_t *pi;
    bool valid;

#ifdef DEBUG
    pr_warning("sub_1aec");
#endif

    valid = IsValid((char*)gPi->iLhs);

    if (valid)
    {
        ReadMem((char*)&lo1, (char*)gPi->iLhs, sizeof(operand_t));

        if (/* gPi->iLhs->type */ plo1->type != (uint8_t)T_REGREF) 
        {
	    gPs = plo1->oPSym;

            if (gPs && gPs->p.pInst) 
	    {
                pi = getNxtRealInst(gPs->p.pInst);
                pi = pi->pAlt;
                if (gPs->p.pInst != pi) 
	        {
                    /* gPi->iLhs->oPOperand = pi->iLhs; */
		    PutWord((char*)(gPi->iLhs)+OFF_oPOperand, (short)pi->iLhs);

                    removeLabelRef(gPs);
                    ++pi->aux;                     /* safe const change */
                    return logOptimise(O_RED_LAB); /* 6fbd opt_msg[0] = "Redundant labels" */
                }
            }
        }
    }

    return false;
}

bool sub_1b86() {
register     inst_t *pi;
    sym_t *ps;
    bool valid;

#ifdef DEBUG
    pr_warning("sub_1b86");
#endif

    valid = IsValid((char*)gPi->iLhs);

    if (valid)
    {
        if (gPi->type == T_JP || gPi->type == T_CALL) 
        {
            if ((gPs = /* gPi->iLhs->oPSym */ (sym_t*)GetWord((char*)(gPi->iLhs)+OFF_oPSym)) &&
		 (pi = gPs->p.pInst)) 
            {
                pi = getNxtRealInst(pi);

                if (pi->type == T_JP && (pi->aux == 0 || pi->aux == gPi->aux) &&
	   	    /* pi->iLhs->oPSym */ (ps = (sym_t*)GetWord((char*)(pi->iLhs)+OFF_oPSym)) != gPs) 
                {
                    removeLabelRef(gPs);
                    gPs              = /* pi->iLhs->oPSym */ ps;

                    /* gPi->iLhs->oPSym = gPs; */
		    PutWord((char*)(gPi->iLhs)+OFF_oPSym, (short)gPs);

                    if (gPs->p.pInst) 
                        ++gPs->p.pInst->aux;

                    return logOptimise(O_JMP_TO_JMP); /* 6fbf opt_msg[1] = "Jumps to jumps" */
                }
            }
        }
    }

    return false;
}

/**************************************************************************
 34 sub_1c67    ok++
 **************************************************************************/
bool sub_1c67() {
    register inst_t *pi;

#ifdef DEBUG
    pr_warning("sub_1c67");
#endif

    if (gPi->aux == 0) {
        for (pi = gPi->pNext; pi; pi = pi->pNext) {
            if (instrSame(pi, gPi)) {
                seq1 = gPi;
                seq2 = pi;
                /* match chains of instructions */
                while (instrSame(seq2->pAlt, seq1->pAlt)) {
                    seq1 = seq1->pAlt;
                    seq2 = seq2->pAlt;
                }
                if (seq1 != gPi) { /* we matched some common code */
                    seq1              = syntheticLabel(seq1->pAlt);
                    seq2              = allocInst(seq2->pAlt);
                    seq2->iLhs        = allocOperand();
                    seq2->type        = T_JP;
                    /* seq2->iLhs->type  = T_CONST; */
		    PutByte((char*)(seq2->iLhs)+OFF_type, (uint8_t)T_CONST);
                    /* seq2->iLhs->oPSym = seq1->iPSym; */
		    PutWord((char*)(seq2->iLhs)+OFF_oPSym, (short)seq1->iPSym);
                    ++seq1->aux;
                    removeInstr(pi);
                    return logOptimise(O_CMN_CODE_SEQ); /* 6fcd opt_msg[8] = "Common code seq's" */
                }
            }
        }
    }
    return false;
}

/**************************************************************************
 35 sub_1d94    ok++
 **************************************************************************/
bool sub_1d94() {
register     inst_t *pi;
    sym_t * ps;
    operand_t *po;

#ifdef DEBUG
    pr_warning("sub_1d94");
#endif

    po = gPi->iLhs;

    if (po)
	ps = (sym_t*)GetWord((char*)po+OFF_oPSym);
    else
	ps = NULL;

    if (gPi->aux != 0 && 
	ps &&
        (pi = /* gPi->iLhs->oPSym->p.pInst */ ps->p.pInst) ) 
    {
        for (seq1 = gPi; sub_4625(pi->pNext) && instrSame(seq1->pNext, pi->pNext); pi = pi->pNext) 
	{
            HEAP(pi->pNext);
            HEAP(seq1->pNext);
            seq1 = seq1->pNext;
        }

        if (seq1 != gPi) 
	{
            if (pi->pNext->type == T_SYMBOL) 
	    {
                pi = pi->pNext;
            } 
	    else 
	    {
                pi = syntheticLabel(pi);
            }

            seq1                  = allocInst(seq1);
            seq1->iLhs            = allocOperand();

            /* seq1->iLhs->type      = T_CONST; */
	    PutByte((char*)(seq1->iLhs)+OFF_type, (uint8_t)T_CONST);

            /* seq1->iLhs->oPOperand = pi->iLhs; */
	    PutWord((char*)(seq1->iLhs)+OFF_oPOperand, (short)pi->iLhs);

            seq1->type            = gPi->type;
            seq1->aux             = gPi->aux;
            ++pi->aux;
            removeInstr(gPi);
            return logOptimise(O_CMN_CODE_SEQ); /* 6fcd opt_msg[8] = "Common code seq's" */
        }
    }

    return false;
}


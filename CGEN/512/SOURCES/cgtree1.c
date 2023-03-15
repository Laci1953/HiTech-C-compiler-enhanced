/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "cgen512.h"

/*********************************************************
 * sub_3DC9 OK++ PMO		Used in: sub_6D1, sub_D66
 *********************************************************/
void O_sub_3DC9(register node_t *sa) 
{
    uint16_t loc;

    /* sa = sub_600E(sa); */
    sa = (node_t*)CallOverlay1(sub_600E, sa, 32+4);

    if (sa->a_c0 == DOLLAR && sa->info.np[0]->a_c0 == CONST) 
    {
        prWarning("constant conditional branch");

        if (sa->info.np[0]->info.l != 0) 
	{
            sa->a_c0 = DOLLAR_U;
            freeNode(sa->info.np[0]);
            sa->info.np[0] = sa->info.np[1];
        } 
	else 
	{
            sub_475C(sa);
            /* return; */ ReturnOverlay1(0);
        }
    }

    byte_B014 = 0;

    if (sa->a_c0 == INCR)
        sa->a_c0 = ASADD;
    else if (sa->a_c0 == DECR)
        sa->a_c0 = ASSUB;

    sa = sub_3CDF(sa);

    if (byte_B014 != 0)
        /* sa = sub_600E(sa); */
	sa = (node_t*)CallOverlay1(sub_600E, sa, 32+4);

    /* sub_2BD0(sa); */
    CallOverlay1(sub_2BD0, sa, 32+5);

    for (loc = 0; loc < byte_B014; loc++)
        /* sub_2BD0(array_AFFD[loc]); */
	CallOverlay1(sub_2BD0, array_AFFD[loc], 32+5);

    ReturnOverlay1(0);
}

/*********************************************************
 * sub_3EAA OK++ PMO			Used in: sub_600E
 *********************************************************/
node_t * O_sub_3EAA(register node_t *sa) 
{
    node_t *l1a;
    int l2;

    if (l2 = dopetab[sa->a_c0] & 0xC)
        /* sa->info.np[0] = sub_3EAA(sa->info.np[0]); */
	sa->info.np[0] = (node_t*)CallOverlay1(sub_3EAA, sa->info.np[0], 32+5);

    if (l2 == 8)
        /* sa->info.np[1] = sub_3EAA(sa->info.np[1]); */
	sa->info.np[1] = (node_t*)CallOverlay1(sub_3EAA, sa->info.np[1], 32+5);

    if (sa->a_c0 == CONV && 
	sa->info.np[0]->a_c0 == FCONST && 
	/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 3) 
    {
        l1a     = sa->info.np[0];
        l1a->pm = sa->pm;
        freeNode(sa->info.np[1]);
        freeNode(sa);
        /* return l1a; */ ReturnOverlay1(l1a);
    }

    if (l2 == 0 || sa->info.np[0]->a_c0 != CONST)
        /* return sa; */ ReturnOverlay1(sa);

    if (sa->a_c0 == CONV)
        /* return sub_39CA(sa); */ ReturnOverlay1(sub_39CA(sa));

    if (l2 == 8 && sa->info.np[1]->a_c0 != CONST)
        /* return sa; */ ReturnOverlay1(sa);

    if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 2) 
    {
        switch (sa->a_c0) 
	{
        case LT:
        case LEQ:
        case GT:
        case GEQ:
	    if (!warningDone)
	    {
            	prWarning("constant relational expression"); /* warningMsg = "constant relational expression"; */
		warningDone = true;
	    }
            /* return sa; */ ReturnOverlay1(sa);
        case MOD:
        case DIV:
        case RSHIFT:
            sub_3B65(&sa->info.np[0]->info.ul, sa->info.np[1]->info.l, sa->a_c0);
        finish:
            if (l2 == 8)
                sub_475C(sa->info.np[1]);
            sub_3A79(sa, sa->info.np[0]);
            freeNode(sa);
            /* return sa->info.np[0]; */ ReturnOverlay1(sa->info.np[0]);
        }
    }

    switch (sa->a_c0) 
    { /* m10: */
    case NEQL:
    case LT:
    case LEQ:
    case EQL:
    case GT:
    case GEQ:
	if (!warningDone)
	{
            prWarning("constant relational expression"); /* warningMsg = "constant relational expression"; */
	    warningDone = true;
	}
        /* return sa; */ ReturnOverlay1(sa);
    case BAND:
    case LAND:
    case SUB:
    case BXOR:
    case BOR:
    case LOR:
    case ADD:
    case MUL:
    case DIV:
    case MOD:
    case RSHIFT:
    case LSHIFT:
        sub_3BC6(&sa->info.np[0]->info.l, sa->info.np[1]->info.l, sa->a_c0);
        goto finish;
    case MINUS_U:
        sa->info.np[0]->info.l = -sa->info.np[0]->info.l;
        goto finish;
    case NOT:
        sa->info.np[0]->info.l = sa->info.np[0]->info.l == 0;
        goto finish;
    case BNOT:
        sa->info.np[0]->info.l = ~sa->info.np[0]->info.l;
        goto finish;
    default:
        /* return sa; */ ReturnOverlay1(sa);
    }
}

/*********************************************************
 * sub_415E OK     Used in: sub_808,  sub_E43,  sub_4192,
 *			    sub_4DA3, sub_54B6
 *********************************************************/
node_t *sub_415E(long number) 
{
    register node_t *sa;

    sa         = allocNode();
    sa->a_c0   = CONST;
    sa->pm     = typeLong;
    sa->info.l = number;

    return sa;
}

/* end of file tree1.c */

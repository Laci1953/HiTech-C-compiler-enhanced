/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#define MINI
#include "cgen512.h"
#include "dynm512.h"

void       freeNode(node_t *);
node_t    *allocNode(void);
uint16_t   nodesize(node_t *);
void 	   prWarning(char *fmt, ...);
int        invertTest(int);
void *	   allocMem(size_t);	
int        newLocal();
//uint8_t    sub_14F3(node_t *);
long       sub_2C5E(node_t *, long);
void       sub_37ED(node_t *);
void       sub_385B(node_t *);
bool       sub_3968(node_t *);
node_t    *sub_415E(long);
//node_t    *sub_43EF(uint8_t, node_t *, node_t *);
//uint8_t    sub_46F7(long);
void       sub_475C(node_t *);
bool       sub_4B89(node_t *);
//bool       sub_6246(node_t *, node_t *);

/*********************************************************
 * sub_46F7 OK++ PMO Used in: sub_2D09, sub_47B2, sub_4FCE
 *			      sub_54B6
 *********************************************************/
uint8_t O_sub_46F7(long p1) 
{
    uint8_t l1;

    if ((p1 & (p1 - 1)) || p1 < 1)
        /* return 0; */ ReturnOverlay1L(0);

    for (l1 = 0; p1 >>= 1; l1++)
        ;

    /* return (l1 + 1); */ ReturnOverlay1L(l1 + 1);
}

/*********************************************************
 * sub_475C OK++ PMO Used in: sub_808,  sub_E43,  sub_17E0,
 *			      sub_19C1, sub_1B0C, sub_2BD0,
 *			      sub_3DC9, sub_3EAA, sub_43EF,
 *			      sub_475C, sub_4DA3, sub_4FA8,
 *			      sub_508A, sub_54B6
 *********************************************************/
void sub_475C(register node_t *sa) 
{
    int loc;

    if (loc = dopetab[sa->a_c0] & 0xC) 
    {
        sub_475C(sa->info.np[0]);

        if (loc == 8)
            sub_475C(sa->info.np[1]);
    }

    freeNode(sa);
}

/*********************************************************
 * sub_47B2 OK++ PMO			Used in: sub_1F4B
 * The code optimisers generate many differences in the code
 * however the vast majority relate to the choice of the code
 * fragments to share for returning true/false and generating
 * true/false from != condition
 * I have tried multiple variants of code to get better matches
 * for the current code, the other differences are
 * 1) at m49dc:, code for sub_14FE(sa) != 3 is moved
 * 2) case SUB: nodesize(sa) == 2 code moved and missed sharing
 *    of code for or a ! sbc hl,de ! jp nz false
 * 3) case DECR: similar to (2)
 * 4) case GADDR: similar to (2)
 * 5) an instance of missed sharing of code
 * 6) instances of inversion of jp conditions with labels also
 *    flipped
 * Note removing the gotos was initially tried, but the optimiser
 * match was poorer. Similarly using compound returns rather
 * than if / break, also produced a poorer match.
 *********************************************************/
uint8_t O_sub_47B2(register node_t *sa, int p2) 
{
    long l1;

    switch (p2) 
    {
    case MUL_U:
        sa = sa->info.np[1];
    case T_SCOLON:
        /* return sa->info.l >= 1L && sa->info.l < 3L; */
	ReturnOverlay2(sa->info.l >= 1L && sa->info.l < 3L);
    case RPAREN:
        sa = sa->info.np[0];
    case MOD:
        if (nodesize(sa) == 2 && !sub_3968(sa))
            /* return true; */
	    ReturnOverlay2(true);
        break;
    case BAND:
        if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 3)
            /* return false; */
	    ReturnOverlay2(false);
        goto m485f;
    case MUL:
        sa = sa->info.np[0];
    case DOLLAR_U:
    m485f:
        if (nodesize(sa) == 4 && !sub_3968(sa))
            /* return true; */
	    ReturnOverlay2(true);
        break;
    case SCOLON:
        sa = sa->info.np[0];
    case DOLLAR:
        /* return sub_3968(sa); */
	ReturnOverlay2(sub_3968(sa));
    case ADD:
        /* return (uint8_t)sa->info.l == 8 || (uint8_t)sa->info.l == 9; */
	ReturnOverlay2((uint8_t)sa->info.l == 8 || (uint8_t)sa->info.l == 9);
    case INCR:
        l1 = sub_2C5E(sa, sa->info.l);
        /* return -128L <= l1 && l1 < 125L; */
	ReturnOverlay2(-128L <= l1 && l1 < 125L);
    case COLON_U:
        /* return 1L <= sa->info.l && sa->info.l < 5L; */
	ReturnOverlay2(1L <= sa->info.l && sa->info.l < 5L);
    case COLON_S:
        l1 = sa->info.l;
        if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 2 && 
	    (l1 & (uint16_t)(1 << ((int16_t)nodesize(sa) << 3))))
            l1 |= (uint16_t) ~((1 << ((int16_t)nodesize(sa) << 3)) - 1);
        /* return l1 < 0 && l1 >= -4L; */
	ReturnOverlay2(l1 < 0 && l1 >= -4L);
    case LPAREN:
        sa = sa->info.np[1];
    case HASHSIGN:
        /* return nodesize(sa) == 1 && !sub_3968(sa); */
	ReturnOverlay2(nodesize(sa) == 1 && !sub_3968(sa));
    case MINUS_U:
        if (nodesize(sa) == 4)
            goto m49dc;
        break;
    case COMMA:
        if (nodesize(sa) == 2)
        m49dc:
            if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) != 3 && 
		/* sub_14F3(sa->info.np[0]) */ (char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) != 3)
                /* return sub_14F3(sa) == 2 || sub_14F3(sa->info.np[0]) == 2; */
		ReturnOverlay2((char)CallOverlay1(sub_14F3, sa, 32+5) == 2 ||
				(char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) == 2);
        break;
    case LT:
        /* return sa->info.l == 0; */
	ReturnOverlay2(sa->info.l == 0);
    case CONV:
        if (nodesize(sa) == 4)
        m4a48:
            /* return sub_14F3(sa) == 1 && sub_14F3(sa->info.np[0]) == 1; */
	    ReturnOverlay2((char)CallOverlay1(sub_14F3, sa, 32+5) == 1 ||
		  	   (char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) == 1);
        /* return false; */
        ReturnOverlay2(false);
    case SUB:
        if (nodesize(sa) == 2)
            goto m4a48;
        break;
    case PLUS_U:
        /* return 1L == sa->info.l; */
	ReturnOverlay2(1L == sa->info.l);
    case NOT:
        /* return sub_46F7(sa->info.np[1]->info.l); */
	ReturnOverlay2((char)CallOverlay1L(sub_46F7, sa->info.np[1]->info.l, 32+5));
    case NEQL:
        /* return sub_46F7(~sa->info.np[1]->info.l); */
	ReturnOverlay2((char)CallOverlay1L(sub_46F7, ~sa->info.np[1]->info.l, 32+5));
    case COLON:
        if (nodesize(sa) == 4)
            goto m4b02;
        break;
    case DECR:
        if (nodesize(sa) == 2)
            goto m4b02;
        break;
    case GADDR:
        if (nodesize(sa) != 1)
            /* return false; */
	    ReturnOverlay2(false);
    m4b02:
        /* return (sub_14F3(sa) == 1 || sub_14F3(sa) == 2) && sub_14F3(sa->info.np[0]) == 3; */
	ReturnOverlay2(((char)CallOverlay1(sub_14F3, sa, 32+5) == 1 ||
			(char)CallOverlay1(sub_14F3, sa, 32+5) == 2) &&
			(char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) == 3);
    case DIV:
        if (nodesize(sa->info.np[0]) == 1)
            goto dotp;
        break;
    case DOT_DOT:
        if (nodesize(sa->info.np[0]) == 2)
            goto dotp;
        break;
    case DOT:
        if (nodesize(sa->info.np[0]) == 4)
        dotp:
            if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) != 3)
            	/* return false; */
	    	ReturnOverlay2(false);
            else
                /* return sub_14F3(sa->info.np[0]) == 1 || sub_14F3(sa->info.np[0]) == 2; */
		ReturnOverlay2((char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) == 1 ||
				(char)CallOverlay1(sub_14F3, sa->info.np[0], 32+5) == 2);
        break;
    case LAND:
        /* return sub_14F3(sa) == 3; */
	ReturnOverlay2((char)CallOverlay1(sub_14F3, sa, 32+5) == 3);
    }

    /* return false; */
    ReturnOverlay2(false);
}

/*********************************************************
 * sub_4B89 OK++ PMO	       Used in: sub_4C12, sub_54B6

 *********************************************************/
bool sub_4B89(register node_t *sa) 
{
    /* Any operator except "#", "..", "CONST" */
    return (dopetab[sa->a_c0] & 0x10) || (sa->a_c0 == GADDR && sa->info.np[0]->a_c0 == IDOP) ||
           (sa->a_c0 == ADD && sa->info.np[1]->a_c0 == CONST && sub_4B89(sa->info.np[0]) != 0);
}

/*********************************************************
 * sub_4BE5 OK++ PMO			Used in: sub_54B6
 *********************************************************/
bool sub_4BE5(register node_t *sa) 
{
    return sa->a_c0 == CONST && sa->info.l == 0;
}

/*********************************************************
 * sub_4C12++ PMO			Used in: sub_54B6
 *********************************************************/
int sub_4C12(register node_t *sa) 
{
    if (sa->a_c0 == GADDR && sa->info.np[0]->a_c0 == IDOP)
        return 1;

    if (dopetab[sa->a_c0] & 0x10)
        return 0; /* if "#", "..", "CONST"  */

    if (sub_4B89(sa))
        return 1;

    return dopetab[sa->a_c0] & 0xF;
}

/*********************************************************
 * sub_4C6D OK++ PMO			Used in: sub_3CDF
 * bool causes optimiser to use ld l, rather than ld hl
 *********************************************************/
bool sub_4C6D(node_t *p1a) 
{
    return p1a->info.np[0]->a_c0 == USEREG;
}

/*********************************************************
 * sub_4C8B OK++ PMO			Used in: sub_54B6
 * sub_43EF using bool as first parameter is optimised
 * but misses ex (sp),hl rather than pop bc ! push hl
 *********************************************************/
node_t *sub_4C8B(register node_t *sa) 
{
    node_t *l1;

    l1 = sa->info.np[0];

    if (sa->info.np[0]->a_c0 == NOT) 
    {
        l1 = l1->info.np[0];
        freeNode(sa->info.np[0]);
        freeNode(sa);
        return l1;
    }

    if ((dopetab[l1->a_c0] & 0x12C) == 0x28) 
    {
        freeNode(sa);
        l1->a_c0 = invertTest(l1->a_c0);
        return l1;
    }

    if (dopetab[l1->a_c0] & 0x20) 
    {
        l1->info.np[0] = /* sub_43EF(NOT, l1->info.np[0], (node_t *)0) */
			 (node_t*)CallOverlay3(sub_43EF, NOT, l1->info.np[0], (node_t *)0, 32+6);
        l1->info.np[1] = /* sub_43EF(NOT, l1->info.np[1], (node_t *)0) */
			 (node_t*)CallOverlay3(sub_43EF, NOT, l1->info.np[1], (node_t *)0, 32+6);
        freeNode(sa);
        l1->a_c0 = l1->a_c0 == LAND ? LOR : LAND;
        return l1;
    }

    return sa;
}

/*********************************************************
 * sub_4DA3 OK++ PMO			Used in: sub_54B6
 * sub_43EF using bool as first parameter is optimised
 * but misses ex (sp),hl rather than pop bc ! push hl
 *********************************************************/
node_t *sub_4DA3(register node_t *sa) 
{
    node_t *l1a;
    member_t buf;

    ReadMem((char*)&buf, (char*)(sa->info.mp[0]), sizeof(struct member));

    if (/* sa->info.mp[0]->b_flag */ buf.b_flag == 1) 
    {
        l1a        = sub_415E(/* sa->info.mp[0]->b_off */ buf.b_off);
        l1a->pm    = sa->pm;
        l1a->a_i15 = sa->a_i15;
        sub_475C(sa);
        sub_385B(l1a);
        sa         = allocNode();
        *sa        = *l1a;
        sa->a_c0   = USEREG;
        sa->info.l = 8L;
        /* sa         = sub_43EF(ADD, sa, l1a); */
	sa = (node_t*)CallOverlay3(sub_43EF, ADD, sa, l1a, 32+6);
        /* return sub_43EF(MUL_U, sa, 0); */
	return (node_t*)CallOverlay3(sub_43EF, MUL_U, sa, 0, 32+6);
    }

    if (/* sa->info.mp[0]->b_flag */ buf.b_flag == 2) 
    {
        sa->a_c0   = USEREG;
        sa->info.l = /* sa->info.mp[0]->b_memb.i */ buf.b_memb.i;
    }

    return sa;
}

/*********************************************************
 * sub_4E8D OK++ PMO 			Used in: sub_54B6
 * sub_43EF using bool as first parameter is optimised
 * but misses ex (sp),hl rather than pop bc ! push hl
 *********************************************************/
node_t *sub_4E8D(register node_t *sa) 
{
    node_t *l1a;

    if (dopetab[(l1a = sa->info.np[0])->a_c0] & 0x1000) 
    {
        l1a->pm        = sa->pm;
        l1a->a_i15     = sa->a_i15;
        /* sa->info.np[0] = l1a = sub_43EF(GADDR, l1a, 0); */
	sa->info.np[0] = l1a = (node_t*)CallOverlay3(sub_43EF, GADDR, l1a, 0, 32+6);
    } 
    else 
    {
        l1a       = allocNode();
        *l1a      = *sa;
        l1a->a_c0 = TYPE;
        sub_385B(l1a);
        /* sa->info.np[0] = sub_43EF(CONV, sub_43EF(GADDR, sa->info.np[0], 0), l1a); */
	sa->info.np[0] = (node_t*)CallOverlay3(sub_43EF,
					       CONV,
					       CallOverlay3(sub_43EF, GADDR, sa->info.np[0], 0, 32+6),
					       l1a,
					       32+6);
    }

    sa->pm                = l1a->pm;
    sa->a_i15             = l1a->a_i15;
    sa->info.np[1]->a_i15 = sa->a_i15;
    sa->info.np[1]->pm    = sa->pm;
    sa->a_c0              = ADD;
    /* return sub_43EF(MUL_U, sa, 0); */
    return (node_t*)CallOverlay3(sub_43EF, MUL_U, sa, 0, 32+6);
}

/*********************************************************
 * sub_4FA8 OK++ PMO			Used in: sub_54B6
 *********************************************************/
node_t *sub_4FA8(register node_t *sa) 
{
    sub_475C(sa->info.np[1]);
    freeNode(sa);
    return sa->info.np[0];
}

/*********************************************************
 * sub_4FCE OK++ OK			Used in: sub_54B6
 *
 *********************************************************/
node_t *sub_4FCE(register node_t *sa) 
{
    switch (sa->a_c0) 
    {
    case MUL:
        sa->a_c0 = LSHIFT;
        break;
    case ASMUL:
        sa->a_c0 = ASLSHIFT;
        break;
    case DIV:
        if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) != 2)
            return sa;
        sa->a_c0 = RSHIFT;
        break;
    case ASDIV:
        if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) != 2)
            return sa;
        sa->a_c0 = ASRSHIFT;
        break;
    case ASMOD:
        sa->a_c0 = ASAND;
        sa->info.np[1]->info.l--;
        return sa;
    case MOD:
        sa->a_c0 = BAND;
        sa->info.np[1]->info.l--;
        return sa;
    }

    byte_B013              = true;
    sa->info.np[1]->info.l = /* sub_46F7(sa->info.np[1]->info.l) */
				(char)CallOverlay1L(sub_46F7, sa->info.np[1]->info.l, 32+5) - 1;
    return sa;
}

/*********************************************************
 * sub_508A OK++ PMO			Used in: sub_54B6
 * optimiser optimises code for parameter 1 of sub_43EF
 *********************************************************/
node_t *sub_508A(register node_t *sa) 
{
    node_t *l1a;
    node_t *l2a;
    char buf[15];
    long l3;

    if (/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 3 && 
	sa->info.np[0]->a_c0 == CONST) 
{
        l3 = sa->info.np[0]->info.l;
        freeNode(sa->info.np[0]);
        freeNode(sa->info.np[1]);
        sprintf(buf, "%ld", l3);
        sa->info.sv.s = (char*)MyAlloc(strlen(buf) + 1); /* allocMem(strlen(buf) + 1); */
						 /* create string */
        WriteMem(buf, sa->info.sv.s, strlen(buf) + 1); /* strcpy(sa->info.sv.s, buf); */
        sa->info.sv.v = newLocal();
        sa->a_c0      = FCONST;
        return sa;
    }

    if ((dopetab[(l1a = sa->info.np[0])->a_c0] & 0x1000) && nodesize(l1a) >= nodesize(sa) &&
        /* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5) != 3 && 
	/* sub_14F3(l1a) */ (char)CallOverlay1(sub_14F3, l1a, 32+5) != 3) 
    {
        l1a->pm    = sa->pm;
        l1a->a_i15 = sa->a_i15;
        freeNode(sa->info.np[1]);
        freeNode(sa);
        return l1a;
    }

    if (/* sub_6246(sa, l1a) */ (bool)CallOverlay2(sub_6246, sa, l1a, 32+4)) 
    {
        sub_475C(sa->info.np[1]);
        l1a->pm    = sa->pm;
        l1a->a_i15 = sa->a_i15;
        freeNode(sa);
        return l1a;
    }

    if (nodesize(l1a) < nodesize(sa))
        return sa;

    if (l1a->a_c0 == CONV && nodesize(l1a->info.np[0]) >= nodesize(sa)) 
    {
        if (/* sub_6246(sa, l1a->info.np[0]) */ (bool)CallOverlay2(sub_6246, sa, l1a->info.np[0], 32+4) ||
            (/* sub_14F3(sa) == sub_14F3(l1a) */ (char)CallOverlay1(sub_14F3, sa, 32+5) == 
						 (char)CallOverlay1(sub_14F3, l1a, 32+5) &&
	     /* sub_14F3(sa) == sub_14F3(l1a->info.np[0]) */ (char)CallOverlay1(sub_14F3, sa, 32+5) ==
							     (char)CallOverlay1(sub_14F3, l1a->info.np[0], 32+5))) 
	{
            sa->info.np[0] = l1a->info.np[0];
            freeNode(l1a->info.np[1]);
            freeNode(l1a);
            return sa;
        }
    }

    if ((dopetab[l1a->a_c0] & 0xC00) == 0 || 
	 /*sub_14F3(l1a) == 3 */ (char)CallOverlay1(sub_14F3, l1a, 32+5) == 3)
        return sa;

    if ((dopetab[l1a->a_c0] & 0x400) &&
        (l1a->info.np[0]->a_c0 != CONV || nodesize(l1a->info.np[0]->info.np[0]) >= nodesize(l1a)))
        return sa;

    /* l2a = sub_43EF(CONV, l1a->info.np[0], sa->info.np[1]); */
    l2a = (node_t*)CallOverlay3(sub_43EF, CONV, l1a->info.np[0], sa->info.np[1], 32+6);
    *sa = *sa->info.np[1];

    if (dopetab[l1a->a_c0] & 8) 
        /* sa = sub_43EF(CONV, l1a->info.np[1], sa); */
	sa = (node_t*)CallOverlay3(sub_43EF, CONV, l1a->info.np[1], sa, 32+6);
    else 
    {
        freeNode(sa);
        sa = 0;
    }

    /* l2a = sub_43EF((uint8_t)l1a->a_c0, l2a, sa); */
    l2a = (node_t*)CallOverlay3(sub_43EF, (uint8_t)l1a->a_c0, l2a, sa, 32+6);
    freeNode(l1a);

    return l2a;
}

/*********************************************************
 * sub_53EE  OK++ PMO			Used in: sub_54B6
 *********************************************************/
node_t *sub_53EE(register node_t *sa) 
{
    node_t *l1a;

    if (dopetab[(l1a = sa->info.np[0])->info.np[0]->a_c0] & 0x1000) 
    {
        l1a        = l1a->info.np[0];
        l1a->pm    = sa->pm;
        l1a->a_i15 = sa->a_i15;
        freeNode(sa->info.np[0]);
        freeNode(sa);
        return l1a;
    }

    sa->a_c0       = CONV;
    sa->info.np[1] = l1a;
    sa->info.np[0] = l1a->info.np[0];
    l1a->a_c0      = TYPE;
    l1a->pm        = sa->pm;
    l1a->a_i15     = sa->a_i15;
    byte_B013      = true;
    return sa;
}

/* end of file tree3.c */

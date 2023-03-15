/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "cgen512.h"
#include "dynm512.h"

/*********************************************************
 * sub_1C8E OK++ PMO   Used in: sub_B19, sub_17E0, sub_19C1
 *********************************************************/
int varSize(member_t *sb_arg) 
{
    member_t* tmp;
    member_t buf;
    register member_t* sb = &buf;

    ReadMem((char*)&buf, (char*)sb_arg, sizeof(struct member));

    if (sb->b_refl & 1)
        return sb->b_nelem * 2;

    tmp = sb->b_type;

    return /* sb->b_type->b_size */ GetWord((char*)tmp, OFF_b_size) * sb->b_nelem;
}

/*********************************************************
 * nodesize  PMO    Used in: sub_808,  sub_E43,  sub_2C5E,
 * OK++			     sub_2D09, sub_387A, sub_3A79,
 *			     sub_43EF, sub_47B2, sub_508A,
 *			     sub_54B6, sub_60A8, sub_6246,
 *			     sub_62BE
 *********************************************************/
uint16_t nodesize(register node_t *sa) 
{
    if (sa->a_i15 & 2)
        prError("can\'t take sizeof func");

    if (sa->a_i15 & 1)
        return 2;

    return /* sa->pm->b_size */ GetWord((char*)(sa->pm), OFF_b_size);
}

/*********************************************************
 * sub_36E0 PMO    Used in: sub_E43,  sub_39CA, sub_3CDF,
 * OK++ 		    sub_3DC9, sub_3EAA, sub_43EF,
 *			    sub_475C, sub_4C8B, sub_4FA8,
 *			    sub_508A, sub_53EE, sub_54B6,
 *			    sub_5DF6, sub_5F52
 *********************************************************/
void freeNode(register node_t *sa) 
{
    byte_B013 = true;

    if (sa->a_c0 == FCONST)
        free(sa->info.np[0]);

    sa->pm       = nodeFreeList;
    nodeFreeList = sa;
}

/*********************************************************
 * sub_3712  PMO    Used in: sub_E43,  sub_377A, sub_415E,
 * OK++			     sub_4192, sub_43EF, sub_4DA3,
 *			     sub_54B6
 *********************************************************/
node_t *allocNode(void) 
{
    register node_t *sa;

    byte_B013 = true;

    if (nodeFreeList) 
    {
        sa           = nodeFreeList;
        nodeFreeList = sa->pm;
        blkclr((char *)sa, sizeof(node_t));
    } 
    else  /* create node_t */
        sa = allocMem(sizeof(node_t));

    return sa;
}

/*********************************************************
 * sub_1420 OK++ PMO			 Used in: sub_E43
 *********************************************************/
void sub_1420(int par) 
{
    printf((uint16_t)par ? "cp\t%d\n" : "or\ta\n", par);
}

/*********************************************************
 * sub_37ED v1 OK++ OK	      Used in: sub_387A, sub_43EF
 *
 * missed optimisation on reloading l1b but otherwise
 * identical
 *********************************************************/
void sub_37ED(register node_t *sa) 
{
    member_t *l1b;
    member_t buf;
    member_t* p = &buf;

    l1b = sa->pm;

    if (sa->a_i15 != 0)
        sa->a_i15 >>= 2;
    else 
    {
        while (/* l1b->b_refl */ GetWord((char*)l1b, OFF_b_refl) == 0)
            l1b = /* l1b->b_type */ (member_t*)GetWord((char*)l1b, OFF_b_type);

        ReadMem((char*)&buf, (char*)l1b, sizeof(struct member));

        sa->pm    = /* l1b->b_type */ p->b_type;
        sa->a_i15 = /* l1b->b_refl */ p->b_refl >> 2;
    }
}

/*********************************************************
 * sub_385B  PMO    Used in: sub_387A, sub_43EF, sub_4DA3,
 * OK++			     sub_4E8D
 *********************************************************/
void sub_385B(register node_t *sa) 
{
    sa->a_i15 = (sa->a_i15 * 4) | 1;
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
 * sub_D66 OK++ PMO			Used in: sub_E43
 *
 *********************************************************/
void sortCaseLabels(int *pCase, int *pLabel, int nCase) 
{
    bool changed;
    int *pl;
    int cnt;
    int tCase;
    int tLabel;
    register int *pc;

    do 
    {
        changed = false;
        pc      = pCase;
        pl      = pLabel;

        for (cnt = nCase; --cnt > 0; pc++, pl++) 
	{
            if (pc[1] < pc[0]) 
	    {
                changed = true;
                tCase   = pc[0];
                pc[0]   = pc[1];
                pc[1]   = tCase;
                tLabel  = pl[0];
                pl[0]   = pl[1];
                pl[1]   = tLabel;
            } 
	    else if (pc[0] == pc[1]) 
	    {
                prError("Duplicate case label");
                return;
            }
        }
    } 
    while (changed != 0);

    return;
}

/*********************************************************
 * sub_174C OK++ PMO    Used in: sub_E43,  sub_17E0, sub_39CA,
 *			     sub_4192, sub_508A
 *********************************************************/
int newLocal() 
{
    return ++localLabelCnt;
}

/*********************************************************
 * sub_E43 OK++	PMO		 	Used in: sub_6D1
 * Only minor code differences due to the declaration of
 * functions with char / uint8_t parameters rather than
 * defaulting to int.
 * One other change is where the compiler saves l to the high
 * and low byte, but as the optimiser detected that h and l
 * were both 0 this is not a problem
 *********************************************************/
void O_parseSwitch() 
{
    node_t *l1a;
    int codeLabel, swTableLabel, caseRange, caseCnt, defaultCodeLabel;
    int16_t l7, l8;
    bool loTest, hiTest;
    register node_t *sa;
    int caseVals[255], codeLabels[255];

    l1a     = /* parseExpr() */ (node_t*)CallOverlay0(parseExpr, 32+5);
    caseCnt = 0;

    for (;;) 
    {
	/* sa = sub_600E(parseExpr()); */
        sa = CallOverlay1(sub_600E, (node_t*)CallOverlay0(parseExpr, 32+5), 32+4);
        codeLabel = atoi(getToken());

        if (sa->a_c0 == DOT_DOT) 
	{ /* end of switch block */
            defaultCodeLabel = codeLabel;
            freeNode(sa);
            expect(']');

            if (caseCnt != 0)
                break;

            prWarning("No case\tlabels");
            sub_475C(l1a);
            printf("jp\tl%d\n", defaultCodeLabel);
            /* return; */ ReturnOverlay0(0);
        }

        if (sa->a_c0 == CONST) { /* only constants allowed */
            caseVals[caseCnt]     = sa->info.l;
            codeLabels[caseCnt++] = codeLabel;
        } 
	else
            prError("Non-constant case label");

        sub_475C(sa);
    }

    sortCaseLabels(caseVals, codeLabels, caseCnt); /* m6:  */
    caseRange = caseVals[caseCnt - 1] - caseVals[0];

    if (0 <= caseRange && caseRange < 16000 &&
        caseRange * 2 + 20 < caseCnt * 5) 
    { /* if jmp table is shorter */
        /* jump table option is smaller so use it
           note the alternative cmp/jp option may be slower
           even if it is shorter
        */
        sa        = allocNode();
        sa->a_c0  = TYPE;
        sa->pm    = l1a->pm;
        sa->a_i15 = l1a->a_i15;
        /* sa        = sub_43EF(CONV, sub_415E(caseVals[0]), sa); */
	sa = CallOverlay3(sub_43EF,
			  CONV,
			  sub_415E(caseVals[0]),
			  sa,
			  32+6);
        /* sa        = sub_43EF(SUB, l1a, sa); */
	sa = CallOverlay3(sub_43EF, SUB, l1a, sa, 32+6);

        if (nodesize(sa) != 2) {
            l1a       = allocNode();
            l1a->a_c0 = TYPE;
            l1a->pm   = sub_265("us");
            /* sa        = sub_43EF(CONV, sa, l1a); */
	    sa = CallOverlay3(sub_43EF, CONV, sa, l1a, 32+6);
        }

        /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);
        /* sub_3DC9(sub_43EF(RPAREN, sa, 0)); */
	CallOverlay1(sub_3DC9,
		     CallOverlay3(sub_43EF, RPAREN, sa, 0, 32+6),
		     32+5);
        swTableLabel = newLocal(); /* swTableCnt++ */
        /*
            with the switch value in hl, the code
            emitted is effectively
            ld  a,.high. caseRange
            cp  h
            jp  c,l{defaultCodeLabel}
            jp  nz,1f
            ld  a,.low. caseRange
            cp  l
            jp  c,l{defaultCodeLabel}
         1: add hl,hl
            ld  de,S{swTableLabel}
            ld  a,(hl)
            inc hl
            ld  h,(hl)
            ld  l,a
            jp  (hl)
            where {xxx} indicates the respective label number of xxx
        */
        printf("ld\ta,%u\n"
               "cp\th\n"
               "jp\tc,l%d\n"
               "jp\tnz,1f\n"
               "ld\ta,%u\n"
               "cp\tl\n"
               "jp\tc,l%d\n"
               "1:add\thl,hl\n"
               "ld\tde,S%u\n"
               "add\thl,de\n"
               "ld\ta,(hl)\n"
               "inc\thl\n"
               "ld\th,(hl)\n"
               "ld\tl,a\n"
               "jp\t(hl)\n",
               caseRange >> 8, defaultCodeLabel, caseRange & 0xff, defaultCodeLabel, swTableLabel);
        /* prPsect(P_DATA); */ CallOverlay1(prPsect, P_DATA, 32+5);
        printf("S%d:\n", swTableLabel);
        codeLabel = 0;
        caseRange = caseVals[0];

        do 
	{
            if (caseRange == caseVals[codeLabel]) 
	    {
                printf("defw\tl%d\n", codeLabels[codeLabel]);
                codeLabel++;
            } 
	    else
                printf("defw\tl%d\n", defaultCodeLabel); /* fill holes with default */

            caseRange++;
        }
	while (codeLabel < caseCnt);

        /* return; */ ReturnOverlay0(0);
    }

    if (nodesize(l1a) == 2) 
    {
        /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);
        /* sub_3DC9(sub_43EF(RPAREN, l1a, 0)); */
	CallOverlay1(sub_3DC9,
		     CallOverlay3(sub_43EF, RPAREN, l1a, 0, 32+6),
		     32+5);
        l7 = -1;
        l8 = (int)(loTest = hiTest = false);

        for (codeLabel = 0; codeLabel < caseCnt; codeLabel++) 
	{
            l7 &= caseVals[codeLabel];
            l8 |= caseVals[codeLabel];

            if (l7 >> 8 != l8 >> 8 && (l7 & 0xFF) != (l8 & 0xFF)) 
	    {
                if (hiTest | loTest)
                    printf("1:\n"); /* target of previous hi/lo test */

                loTest = hiTest = false;
                l7 = l8 = caseVals[codeLabel];
            }

            if (l7 >> 8 == l8 >> 8) 
	    {
                if (!hiTest) 
		{
                    printf("ld\ta,h\n");
                    sub_1420(l7 >> 8);
                    printf("jp\tnz,1f\n"
                           "ld\ta,l\n");
                    hiTest = true;
                }

                caseVals[codeLabel] &= 0xFF;
            } 
	    else 
	    {
                if (!loTest) 
		{ /* m16: */
                    if (hiTest)
                        printf("1:\n");

                    printf("ld\ta,l\n"); /* m17: */
                    sub_1420(l7 & 0xFF);
                    printf("jp\tnz,1f\n"
                           "ld\ta,h\n");
                    loTest = true;
                }

                caseVals[codeLabel] >>= 8;
                caseVals[codeLabel] &= 0xFF;
            }

            sub_1420(caseVals[codeLabel]); /* m19: */
            printf("jp\tz,l%d\n", codeLabels[codeLabel]);
        }

        if (hiTest | loTest)
            printf("1:\n");

        printf("jp\tl%d\n", defaultCodeLabel); /* m21: */
        /* return; */ ReturnOverlay0(0);
    }

    /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);
    /* sub_3DC9(sub_43EF(INAREG, l1a, 0)); */
    CallOverlay1(sub_3DC9,
		 CallOverlay3(sub_43EF, INAREG, l1a, 0, 32+6),
		 32+5);

    for (codeLabel = 0; codeLabel < caseCnt; codeLabel++) 
    {
        if (caseVals[codeLabel] < 256 && caseVals[codeLabel] >= -128) 
	{
            sub_1420(caseVals[codeLabel]);
            printf("jp\tz,l%d\n", codeLabels[codeLabel]);
        }
    }

    printf("jp\tl%d\n", defaultCodeLabel);
    ReturnOverlay0(0);
}

/*********************************************************
 * sub_43EF OK++ PMO Used in: sub_793,  sub_808,  sub_E43,
 *                            sub_4192, sub_4C8B, sub_4DA3,
 *                            sub_4E8D, sub_508A, sub_54B6,
 *                            sub_5DF6
 *********************************************************/
node_t * O_sub_43EF(uint8_t p1, node_t *p2a, node_t *p3a) 
{
    member_t *l1b;
    long l2;
    register node_t *sa;

    sa             = allocNode();
    sa->a_c0       = p1;
    sa->info.np[0] = p2a;
    sa->info.np[1] = p3a;

    switch (dopetab[sa->a_c0] & 0x30) 
    {
    case 0x10:
        sa->pm = typeLong; /* long	  */
        break;
    case 0x20:
        sa->pm = typeB; /* b	  */
        break;
    case 0x30:
        sa->pm = typeVar; /* variable */
        break;
    default:
        sa->pm    = p2a->pm;
        sa->a_i15 = p2a->a_i15;
    }

    switch (sa->a_c0) 
    {
    case DOT:
        if (p2a->a_c0 == TYPE) 
	{
            l1b = p2a->pm;
            freeNode(sa);
            freeNode(p2a);
            sa          = p3a;
            sa->pm      = l1b;
            p3a->info.l = (long)/* l1b->b_memb.ilist->vals[sa->info.l] */ ((struct _memi*)GetWord((char*)l1b, OFF_b_memb))->vals[sa->info.l];
            break;
        }

        l1b         = /* sa->pm->b_memb.mlist->vals[p3a->info.l] */ ((struct _memb*)GetWord((char*)(sa->pm), OFF_b_memb))->vals[p3a->info.l];
        sa->pm      = /* l1b->b_type */ (member_t*)GetWord((char*)l1b, OFF_b_type);
        sa->a_i15   = /* l1b->b_refl */ GetWord((char*)l1b, OFF_b_refl);
        p3a->info.l = (long) /* l1b->b_off */ GetWord((char*)l1b, OFF_b_off);
        p3a->pm     = typeX; /* x      */

        if (/* l1b->b_sloc */ GetByte((char*)l1b, OFF_b_sloc) & 0x10) 
	{
            p2a             = allocNode();
            *p2a            = *sa;
            p2a->info.np[0] = sa;
            sa              = p2a;
            sa->a_c0        = BFIELD;
            sa->info.mp[1]  = l1b;
        }
        break;
    case CONV:
    case SCOLON:
    case QUEST:
        sa->pm    = sa->info.np[1]->pm;
        sa->a_i15 = sa->info.np[1]->a_i15;
        break;
    case GADDR:
        if (sa->a_i15 == 0 && /* sa->pm->b_nelem */ GetWord((char*)(sa->pm), OFF_b_nelem) > 1) 
	{
            sa->a_i15 = /* sa->pm->b_refl */ GetWord((char*)(sa->pm), OFF_b_refl);
            sa->pm    = /* sa->pm->b_type */ (member_t*)GetWord((char*)(sa->pm), OFF_b_type);
        }
        sub_385B(sa);
        break;
    case LPAREN:
    case MUL_U:
        sub_37ED(sa);
        break;
    case COLON_S:
        sa->pm    = typeChar; /* char   */
        sa->a_i15 = 1;
        break;
    case HASHSIGN:
        if (sa->info.np[0]->a_c0 == IDOP)
            l2 = /* sa->info.np[0]->info.mp[0]->b_size */ GetWord((char*)(sa->info.np[0]->info.mp[0]), OFF_b_size);
        else
            l2 = (uint16_t)nodesize(sa->info.np[0]);
        if (l2 == 0)
            prWarning("Sizeof yields 0");
        sub_475C(sa->info.np[0]);
        sa->a_c0   = CONST;
        sa->pm     = typeLong; /* long	  */
        sa->a_i15  = 0;
        sa->info.l = l2;
        break;
    }

    /* return sa; */
    ReturnOverlay3(sa);
}

/*********************************************************
 * sub_C36 OK++ PMO    Used in: sub_C57, sub_CAB, sub_4192
 *********************************************************/
void prGlobalDef(register member_t *sb) 
{
    printf("global\t%s\n", GetString((char*)sb, OFF_b_name) /* sb->b_name */);
    /* sb->b_sloc |= 2; */
    PutByte((char*)sb, OFF_b_sloc, GetByte((char*)sb, OFF_b_sloc) | 2);
}

/*********************************************************
 * sub_CAB OK++ PMO		Used in: sub_B19, sub_C57
 *
 * Emit "symbolic_name:" (identifier label)
 *********************************************************/
void sub_CAB(member_t *sb_arg) 
{
    member_t buf;
    register member_t* sb = &buf;

    ReadMem((char*)&buf, (char*)sb_arg, sizeof(struct member));

    if ((sb->b_sloc & 2) == 0 && sb->b_flag == 3)
        prGlobalDef(sb_arg); /* Emit "global name" */

    /* sb->b_sloc |= 1; */
    PutByte((char*)sb_arg, OFF_b_sloc, GetByte((char*)sb_arg, OFF_b_sloc) | 1);
    ReadMem(tmpBuf, sb->b_name, 32);
    printf("%s:\n", tmpBuf /* sb->b_name */);
}

/*********************************************************
 * sub_1CC4 OK++ PMO	       Used in: sub_808, sub_1CEF
 *********************************************************/
int sub_1CC4(member_t *sb) 
{
    member_t* tmp;

    if (/* sb->b_refl */ GetWord((char*)sb, OFF_b_refl) != 0)
        return 0;

    tmp = (member_t*)GetWord((char*)sb, OFF_b_type);

    return /* sb->b_type->b_off */ GetWord((char*)tmp, OFF_b_off);
}

/*********************************************************
 * sub_CDF OK++ PMO			Used in: sub_808
 * Emit "defb 0, ..." (num bytes)
 *********************************************************/
void prDefb0s(int num) 
{
    char cnt;

    cnt = 0;             /* Reset counter bytes printed	       */
    while (num-- != 0) 
    { /* While data is available	       */
        if (cnt == 0)
            printf("defb\t0"); /* Initially output "defb 0",   */
        else
            printf(",0");   /* later ",0"		       */
        cnt++;              /* and update number bytes output  */

        if (cnt == 16) 
	{    /* If number bytes in string is 16 */
            cnt = 0;        /* Reset counter and	       */
            fputchar('\n'); /* continue output in next line    */
        }
    } /* continue processing */

    if (cnt != 0)
        fputchar('\n'); /* If line is incomplete, new line */
}

/*********************************************************
 * sub_808 OK++ PMO			 Used in: sub_B19
 * code identical except for where the original code had
 * l = l4b->b_b6 immediately followed
 * by hl = 0. The useless l = l4b->b-b6 code is no longer there
 * Note there appears to be a problem with hitech code generation
 * around << of a long value
 * if the rhs is uint8_t then the calculation is done incorrectly
 *
 *********************************************************/
int sub_808(member_t *sb, register node_t *p2a) 
{
    int16_t l1;
    int16_t l2;
    member_t *l3b;
    member_t *l4b;
    int l5;
    node_t *l6a FORCEINIT;
    struct _memb * membp;
    int bsize;

    l1 = 0;
    if (p2a->a_c0 == COLON_U) 
    {
        sub_475C(p2a);
        l5 = 0;

        if (/* sb->b_class */ GetByte((char*)sb, OFF_b_class) == (char)STRUCT) 
	{
            l3b = sb;

            while ((p2a = /* parseExpr() */ (node_t*)CallOverlay0(parseExpr, 32+5))->a_c0 != DOT_DOT) 
	    {
		membp = (struct _memb *)GetWord((char*)l3b, OFF_b_memb);

                if ((l2 = sub_1CC4(l4b = /* l3b->b_memb.mlist->vals[l5++] */ membp->vals[l5++] ) ) != 0 &&
		    l1 % ++l2 != 0) 
		{
                    l2 -= l1 % l2;
                    l1 += l2;
                    prDefb0s(l2); /* emit defb 0 to pad to item boundary */
                }

                if (/* l4b->b_sloc */ GetByte((char*)l4b, OFF_b_sloc) & 0x10) 
		{
		    /* p2a = sub_43EF(BAND, p2a, sub_415E((1L << (int8_t)(l4b->b_b5)) - 1L)); */
                    p2a = (node_t*)CallOverlay3(sub_43EF, 
						BAND, 
						p2a, 
						sub_415E((1L << (int8_t)(GetByte((char*)l4b, OFF_b_b5))) - 1L),
						32+6);

                    if (/* l4b->b_b6 */ GetByte((char*)l4b, OFF_b_b6) != 0)
                        /* l6a = sub_43EF(BOR, l6a, sub_43EF(LSHIFT, p2a, sub_415E(0L))); */
			l6a = (node_t*)CallOverlay3(sub_43EF,
						    BOR,
						    l6a,
						    CallOverlay3(sub_43EF,
								 LSHIFT,
								 p2a,
								 sub_415E(0L),
						    		 32+6),
						    32+6);
                    else
                        l6a = p2a;

                    if (l5 != /* l3b->b_memb.mlist->cnt */ membp->cnt &&
(/* l3b->b_memb.mlist->vals[l5]->b_sloc */ GetByte((char*)(membp->vals[l5]), OFF_b_sloc) & 0x10) &&
 /* l3b->b_memb.mlist->vals[l5]->b_off */ GetWord((char*)(membp->vals[l5]), OFF_b_off) == /* l4b->b_off */ GetWord((char*)l4b, OFF_b_off))
                        continue;

                    p2a = l6a;
                }

                l1 += sub_808(l4b, p2a);
            }

            sub_475C(p2a);
        } 
	else 
	{
            l3b = /* sb->b_type */ (member_t *)GetWord((char*)sb, OFF_b_type);

            for (;;) 
	    {
                if ((p2a = /* parseExpr() */ (node_t*)CallOverlay0(parseExpr, 32+5))->a_c0 == COLON_U)
                    l1 += sub_808(l3b, p2a);
                else if (p2a->a_c0 == DOT_DOT) 
		{
                    sub_475C(p2a);
                    break;
                } 
		else 
		{
                    l1 += nodesize(p2a);
                    /* sub_2BD0(sub_600E(sub_43EF(ATGIGN, p2a, 0))); */
		    CallOverlay1(sub_2BD0, 
				 CallOverlay1(sub_600E,
					      CallOverlay3(sub_43EF, 
							   ATGIGN, 
							   p2a, 
							   0, 
							   32+6),
					      32+4),
				 32+5);
                }
            }
        }
    } 
    else 
    {
        /* p2a = sub_600E(sub_43EF(ATGIGN, p2a, 0)); */
	p2a = (node_t*)CallOverlay1(sub_600E,
				    CallOverlay3(sub_43EF,
						 ATGIGN, 
						 p2a, 
						 0,
						 32+6),
				    32+4);
        l1  = nodesize(p2a);
        /* sub_2BD0(p2a); */
	CallOverlay1(sub_2BD0, p2a, 32+5);
    }

    bsize = GetWord((char*)sb, OFF_b_size);

    if (l1 < /* sb->b_size */ bsize) {
        prDefb0s(/* sb->b_size */ bsize - l1);
        l1 = /* sb->b_size */ bsize;
    } 
    else if (/* sb->b_size */ bsize < l1 && /* sb->b_size */ bsize != 0)
        prError("Too many initializers");

    return l1; /* m15: */
}

/*********************************************************
 * sub_B19 OK++	PMO			 Used in: sub_6D1
 *********************************************************/
void O_parseInit(void) 
{
    register member_t *sb;
    int l1;
    int l2;

    sb = sub_265(getToken());
    /* prPsect(P_DATA); */ CallOverlay1(prPsect, P_DATA, 32+5);
    sub_CAB(sb); /* Emit "symbolic_name:" (identifier label) */
    l1 = sub_808(sb, /* parseExpr() */ (node_t*)CallOverlay0(parseExpr, 32+5));
    l2 = /* sb->b_refl */ GetWord((char*)sb, OFF_b_refl) ? 2 : /* sb->b_type->b_size */ GetWord((char*)GetWord((char*)sb, OFF_b_type), OFF_b_size);

    if (/* sb->b_nelem */ GetWord((char*)sb, OFF_b_nelem) == 0) 
    { /* make sure nelem is set */
        /* sb->b_nelem = l1 / l2; */
	PutWord((char*)sb, OFF_b_nelem, (short)(l1 / l2));
        /* sb->b_size  = varSize(sb); */
	PutWord((char*)sb, OFF_b_size, (short)varSize(sb)); 
    }

    expect(']');

    ReturnOverlay0(0);
}

/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#define MINI
#include "cgen512.h"
#include "dynm512.h"

char sub_1B2(char *);
member_t *sub_265(char *);
member_t *sub_627(char *, uint16_t *);
//void parseVariable();
void parseCType(int);
//void parseEnum();
//node_t *parseExpr();
//node_t *sub_43EF(uint8_t, node_t *, node_t *);
long atol(char *str);
void *allocMem(size_t);
char *getToken();
int newLocal();
node_t *sub_415E(long);
node_t *allocNode();
void prGlobalDef(member_t *);

/*********************************************************
 * sub_4192  PMO    Used in: sub_6D1,  sub_793,  sub_808,
 * OK++			     sub_B19,  sub_E43,  sub_17E0,
 *			     sub_19C1, sub_1B0C
 * minor variation due to paramater 1 of sub_43EF being declared
 * uint8_t, otherwise identical
 *********************************************************/
node_t *O_parseExpr() 
{
    node_t *l1a;
    char *l2;
    int l3;
    int l4;
    register node_t *sa;

    for (;;) 
    {
        l2 = getToken();

        if (isdigit(*l2) || (*l2 == '-' && isdigit(l2[1]))) 
	{
            if (*l2 == '-') 
	    {
                l4 = 1;
                l2++;
            } 
	    else
                l4 = 0;

            sa = sub_415E(atol(l2));

            if (l4 != 0)
                sa->info.l = -sa->info.l;

            /* return sa; */ ReturnOverlay0(sa);
        }

        if (*l2 == '_' || isalpha(*l2)) 
	{
            (sa = allocNode())->a_c0 = IDOP; /* m8:  */
            sa->info.mp[0]           = sub_265(l2);
            sa->pm = /* sa->info.mp[0]->b_type */ (member_t*)GetWord((char*)(sa->info.mp[0]), OFF_b_type);
            sa->a_i15 = /* sa->info.mp[0]->b_refl */ GetWord((char*)(sa->info.mp[0]), OFF_b_refl);

            if (/* sa->info.mp[0]->b_flag */ GetByte((char*)(sa->info.mp[0]), OFF_b_flag) == 3 && 
		(/* sa->info.mp[0]->b_sloc */ GetByte((char*)(sa->info.mp[0]), OFF_b_sloc) & 2) == 0) 
                prGlobalDef(sa->info.mp[0]); /* Emit "global name" */ 

            /* return sa; */ ReturnOverlay0(sa);
        }

        if (*l2 == '`') 
	{
            sa       = allocNode();
            sa->pm   = sub_627(l2, &sa->a_i15);
            sa->a_c0 = TYPE;
            /* return sa; */ ReturnOverlay0(sa);
        }

        if (*l2 == '.' && l2[1] && l2[1] != '.') 
	{
            sa            = allocNode();
            sa->pm        = typeDouble;                   /* "d" - double */
            sa->info.sv.s = MyAlloc(strlen(l2)); /* (char *)allocMem(strlen(l2)); */
							/* create string */
            WriteMem(l2 + 1, sa->info.sv.s, strlen(l2)); /* strcpy(sa->info.sv.s, l2 + 1); */
            sa->info.sv.v = newLocal(); /* swTableCnt++ */
            sa->a_c0      = FCONST;
            /* return sa; */ ReturnOverlay0(sa);
        }

        l4 = sub_1B2(l2); /* Token search */ /* m12: */
        l3 = dopetab[l4] & 0xC;

        switch (l3) 
	{
        case 0xc:
            switch (l4) 
	    {
            case '9':
                /* parseVariable(); */ CallOverlay0(parseVariable, 32+5);
                continue;
            case '7':
            case '8':
                /* parseMembers(l4); */ CallOverlay1(parseMembers, l4, 32+5);
                continue;
            case '4':
                /* parseEnum(); */ CallOverlay0(parseEnum, 32+5);
                continue;
            default:
                fatalErr("Expression error");
                break;
            }
            /* fall through */
        case 0:
            /* return sub_43EF(l4, 0, 0); */
	    ReturnOverlay0(CallOverlay3(sub_43EF, l4, 0, 0, 32+6));
        case 4:
            /* return sub_43EF(l4, parseExpr(), 0); */
	    ReturnOverlay0(CallOverlay3(sub_43EF,
					l4,
					CallOverlay0(parseExpr, 32+5),
					0,
					32+6));
        case 8:
            /* l1a = parseExpr(); */
	    l1a = CallOverlay0(parseExpr, 32+5);
            /* return sub_43EF(l4, l1a, parseExpr()); */
	    ReturnOverlay0(CallOverlay3(sub_43EF,
					l4,
					l1a,
					CallOverlay0(parseExpr, 32+5),
					32+6));	
        }
    }
}

/* end of file tree2.c */

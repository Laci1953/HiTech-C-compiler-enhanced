/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#define MINI
#include "cgen512.h"
#include "dynm512.h"

void 	   prError(char *fmt, ...);
//void       prPsect(int);
void       prGlobalDef(member_t *);
int        newLocal();
member_t **gethashptr(char *);
member_t  *sub_265(char *);
void       sub_CAB(member_t *);
//uint8_t    sub_14F3(node_t *);
//void       sub_2BD0(node_t *);
//void       sub_3DC9(node_t *);
//node_t    *sub_600E(node_t *);

/*********************************************************
 * sub_B96 OK++ PMO			Used in: sub_2BD0
 * emit_call_ncsv
 *********************************************************/
void prFrameHead(int fId) 
{
    /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);

    if (frameGlobalsEmitted == false) /* if first call	*/
        printf("global\tncsv, cret, indir\n");

    printf("call\tncsv\n");
    frameGlobalsEmitted = true; /* first call is done	*/
    printf("defw\tf%d\n", fId);
}

/*********************************************************
 * sub_BCC OK++ PMO		       Used in: leaveBlock
 *********************************************************/
void O_prFrameTail(int fId, int fSize) 
{
    /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);
    printf("jp\tcret\n"
           "f%d\tequ\t%d\n",
           fId, fSize);
    ReturnOverlay2(0);
}

/*********************************************************
 * sub_BEE OK++ PMO		       Used in: leaveBlock
 *********************************************************/
void O_sub_BEE(int kId, int size) 
{
    /* prPsect(P_TEXT); */ CallOverlay1(prPsect, P_TEXT, 32+5);
    printf("ld\tde,k%d\n"
           "ld\tbc,%d\n"
           "ldir\n"
           "ld\thl,k%d\n",
           kId, size, kId);
    /* prPsect(P_BSS); */ CallOverlay1(prPsect, P_BSS, 32+5);
    printf("k%d:defs\t%d\n", kId, size);
    ReturnOverlay2(0);
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
 * sub_C57 OK++ PMO			Used in: sub_5CF5
 *********************************************************/
void O_sub_C57(member_t *sb_arg) 
{
    member_t buf;
    register member_t* sb = &buf;

    ReadMem((char*)&buf, (char*)sb_arg, sizeof(struct member));

    if ((sb->b_sloc & 1) == 0 && sb->b_nelem != 0 && (sb->b_refl & 2) == 0) 
    {
        /* prPsect(P_BSS); */ CallOverlay1(prPsect, P_BSS, 32+5);

        if (sb->b_flag == 3 && (sb->b_sloc & 2) == 0)
            prGlobalDef(sb_arg); /* Emit "global name" */

        sub_CAB(sb_arg);         /* Emit "symbolic_name:" (identifier label) */
        printf("\tdefs\t%u\n", sb->b_size);
    }

    ReturnOverlay1(0);
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
 * prPsect OK++ PMO  Used in: sub_6D1,  sub_793,  sub_B19,
 * 			      prFrameHead,  prFrameTail,  sub_BEE,
 * Select psect		      sub_C57,  sub_E43
 *********************************************************/
void O_prPsect(int section) 
{  
    static char *psectNames[] = { "", "bss", "text", "data" };

    if (section != curPsect) /* Only when changing section */
        printf("psect\t%s\n", psectNames[curPsect = section]);

    ReturnOverlay1(0);
}

/*********************************************************
 * sub_143F OK++ PMO			Used in: sub_35E6
 *********************************************************/
void prPush(uint8_t reg) 
{
    if (reg == REG_DEHL) 
    {
        printf("push\thl\n"
               "push\tde\n");
        return;
    }

    if (reg <= REG_H)
        reg = reg / 2 + REG_AF;

    printf("push\t%s\n", regNames[reg]);
}

/*********************************************************
 * sub_1489 OK++ PMO			Used in: sub_35E6
 *********************************************************/
void prPop(uint8_t reg) 
{
    if (reg == REG_DEHL) 
    {
        printf("pop\tde\n"
               "pop\thl\n");
        return;
    }

    if (reg <= REG_H)
        reg = reg / 2 + REG_AF;

    printf("pop\t%s\n", regNames[reg]);
}

/*********************************************************
 * sub_14D3 OK++ PMO			Used in: sub_1EDF
 *
 * Assigning register "IY" value formal parameter with
 * type register
 *********************************************************/
void prIXnPush(register member_t *sb) 
{
    int off = GetWord((char*)sb, OFF_b_off);

    printf("ld\tl,(ix+%d)\n"
           "ld\th,(ix+%d)\n"
           "push\thl\n"
           "pop\tiy\n",
           /* sb->b_off */ off, /* sb->b_off */ off + 1);
}

/*********************************************************
 * sub_14F3  PMO   Used in: sub_153A, sub_2D09, sub_39CA,
 * OK++			    sub_3A79, sub_3EAA, sub_47B2,
 *			    sub_4FCE, sub_508A, sub_54B6,
 *			    sub_60A8, sub_6246, sub_628F
 *********************************************************/
uint8_t O_sub_14F3(register node_t *sa) 
{
    member_t buf;
    member_t* p = &buf;

    if (sa->a_i15 & 2)
        /* return 0; */ ReturnOverlay1(0);

    if (sa->a_i15 != 0)
        /* return 2; */ ReturnOverlay1(2);

    ReadMem((char*)&buf, (char*)(sa->pm), sizeof(struct member));

    if (/* sa->pm->b_nelem */ p->b_nelem > 1)
        /* return 0; */ ReturnOverlay1(0);

    /* return p->b_flag; */ ReturnOverlay1(p->b_flag);
}

/*********************************************************
 * sub_153A OK++ PMO			Used in: sub_2D09
 *********************************************************/
void O_sub_153A(register node_t *sa) 
{
    static char array_A542[] = { 0, 'a', 'l', 'f' };

    fputchar(array_A542[/* sub_14F3(sa) */ (char)CallOverlay1(sub_14F3, sa, 32+5)]);
    ReturnOverlay1(0);
}

/*********************************************************
 * sub_155D OK++ PMO			 Used in: sub_793
 *
 * Emit "defb byte1, ..." (from ptr num bytes)
 *********************************************************/
void O_prDefb(register char *ptr, int num) 
{
    char cnt;

    cnt = 0;             		/* Reset counter bytes printed	   */

    while (num-- != 0) 
    { 		/* While data is available	   */
        if (cnt == 0)
            printf("defb\t"); 		/* Initially output "defb",	   */
        else
            fputchar(',');             	/* later "," 			   */

        printf("%d", (uint8_t)*ptr++); 	/* Output byte and advance pointer */
        cnt++;                         	/* update number bytes output 	   */

        if (cnt == 16) 
	{               	/* max 16 bytes per line 	   */
            fputchar('\n');
            cnt = 0;
        }
    }

    if (cnt != 0)
        fputchar('\n'); /* If line is incomplete, new line */

    ReturnOverlay2(0);
}

/*********************************************************
 * xx1	 OK++ PMO	Used in: Explicit calls are absent
 *********************************************************/
void prJmpLabel(int p) 
{
    printf("j%d:\n", p);
}

/*********************************************************
 * xx2	 OK++ PMO	Used in: Explicit calls are absent
 *********************************************************/
void prJump(int p) 
{
    printf("jp\tj%d\n", p);
}

/*********************************************************
 * sub_15FB OK++ PMO			Used in: sub_1B0C
 *********************************************************/
void sub_15FB(register member_t *sb, int p2, int p3) 
{
    bool f = (p2 >= 0);

    PutByte((char*)sb, OFF_b_flag, f ? 2 : 1);

    PutWord((char*)sb, OFF_b_size, f ? ( (p3 <= 255) ? 1 : 2 ) : ( (p3 <= 127 && p2 >= -128) ? 1 : 2 ) );
}

/*********************************************************
 * sub_1659 OK++ PMO			Used in: sub_1CEF
 * Find maximum between two numbers
 *********************************************************/
int max(int num1, int num2) 
{
    return (num1 > num2) ? num1 : num2;
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
 * sub_1754 OK++ PMO    Used in: sub_17E0, sub_19C1, sub_1B0C
 *********************************************************/
member_t *sub_1754(char *token, uint8_t cls) 
{
    member_t **l1b;
    register member_t *sb;
    char* p;

    sb = sub_265(token);                          /* Set pointer to struct associated with   */
                                                  /* pointer to token.			     */
    if (/* sb->b_ndpth */ GetByte((char*)sb, OFF_b_ndpth) != nstdpth) 
    {                 /* If nesting depth not correspond current,*/
        l1b        = gethashptr(token);           /* Get pointer to pointer to struct from   */
        sb         = (member_t*)MyAlloc(sizeof(member_t));  /* hash table. Create new struct and save  */
        /*sb->b_next = *l1b; */                        /* pointer to struct from hash table in    */
	PutWord((char*)sb, OFF_b_next, (short)*l1b);
                                                  /* struct as associated with it. 	     */
        *l1b = sb;                                /* Save pointer to this struct in current  */
                                                  /* position hash table.		     */
	PutWord((char*)sb, OFF_b_name, (short)(p = (char*)MyAlloc(strlen(token) + 1)));
						/* Get memory address allocated for token  */
                                                  /* and assign it to corresponding member.  */
        WriteMem(token, /* sb->b_name */ p, strlen(token) + 1); /* strcpy(sb->b_name, token); */                
						/* Copy specified token to this address.   */
        /* sb->b_ndpth = (uint8_t)nstdpth; */           /* Save current nesting depth in struct    */
	PutByte((char*)sb, OFF_b_ndpth, nstdpth);
    }
    /* sb->b_class = cls; */
    PutByte((char*)sb, OFF_b_class, cls);

    return sb;
}
/* end of code.c */

/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include <stdio.h>

#include "optim1.h"

/****************************************************************
 *	Initialized variables 
 ****************************************************************/

int ccSwap[] = {				/* 68c7 */
    0, 2, 1, 4, 3, 6, 5, 8, 7
};

char * psectNames[] = {				/* 6a59 */
    "",		/*  0 */
    "text",	/*  1 */
    "data",	/*  2 */
    "bss",	/*  3 */
    "data"	/*  4 */
};

struct _s1 regHiLoMap[] = {
    { 0,     0     },				/* 6c50	REG_B  */
    { 0,     0     },				/* 6c52	REG_C  */
    { 0,     0     },				/* 6c54	REG_D  */
    { 0,     0     },				/* 6c56	REG_E  */
    { 0,     0     },				/* 6c58	REG_H  */
    { 0,     0     },				/* 6c5a	REG_L  */
    { 0,     0     },				/* 6c5c	REG_F  */
    { 0,     0     },				/* 6c5e	REG_A  */
    { 0,     0     },				/* 6c60	REG_I  */
    { 0,     0     },				/* 6c62	REG_R  */
    { REG_B, REG_C },				/* 6c64	REG_BC */
    { REG_D, REG_E },				/* 6c66	REG_DE */
    { REG_H, REG_L },				/* 6c68	REG_HL */
};

struct _s2 regHiLoValMap[] = {
    { &regValues[REG_BC], NULL	           },	/* 6c6a	REG_B   */
    { &regValues[REG_BC], NULL	           },	/* 6c6e REG_C   */
    { &regValues[REG_DE], NULL	           },	/* 6c72 REG_D   */
    { &regValues[REG_DE], NULL	           },	/* 6c76 REG_E   */
    { &regValues[REG_HL], NULL	           },	/* 6c7a REG_H   */
    { &regValues[REG_HL], NULL	           },	/* 6c7e REG_L   */
    { &regValues[REG_AF], NULL	           },	/* 6c82 REG_F   */
    { &regValues[REG_AF], NULL	           },	/* 6c86 REG_A   */
    { NULL,               NULL	           },	/* 6c8a REG_I   */
    { NULL,               NULL	           },	/* 6c8e REG_R   */
    { &regValues[REG_B], &regValues[REG_C] },	/* 6c92 REG_BC  */
    { &regValues[REG_D], &regValues[REG_E] },	/* 6c96 REG_DE  */
    { &regValues[REG_H], &regValues[REG_L] },	/* 6c9a REG_HL  */
    { NULL,               NULL	           },	/* 6c9e REG_SP  */
    { &regValues[REG_A], &regValues[REG_F] },	/* 6ca2 REG_AF  */
    { NULL,               NULL	           },	/* 6ca6 REG_AF1 */
    { NULL,               NULL	           },	/* 6caa REG_IX  */
    { NULL,               NULL	           }    /* 6cae REG_IY  */
};

#ifdef STATISTICS
char * opt_msg[] = {				/* 68a3 */
    "Redundant labels",		/*  0	 0 */
    "Jumps to jumps",		/*  1	 1 */
    "Stack adjustments",	/*  2	 2 */
    "Temporary labels",		/*  3	 3 */
    "Unref'ed labels",		/*  4	 4 */
    "Unreachable code",		/*  5	 5 */
    "Jumps to .+1",		/*  6	 6 */
    "Skips over jumps",		/*  7	 7 */
    "Common code seq's",	/*  8	 8 */
    "Redundant exx's",		/*  9	 9 */
    "Ex (sp),hl's used",	/* 10	 a */
    "Redundant operations",	/* 11	 b */
    "Redundant loads/stores",	/* 12	 c */
    "Simplified addresses",	/* 13	 d */
    "Xor a's used",		/* 14	 e */
    "Redundant ex de,hl's",	/* 15	 f */
    "Code motions",		/* 16	10 */
    "Loops inverted"		/* 17	11 */
};
#endif

int regTestMasks[] = {
    0x01,                              		/* 6cb2 REG_B   */
    0x02,                              		/* 6cb4 REG_C   */
    0x04,                              		/* 6cb6 REG_D   */
    0x08,                              		/* 6cb8 REG_E   */
    0x10,                              		/* 6cba REG_H   */
    0x20,                              		/* 6cbc REG_L   */
    0x40,                              		/* 6cbe REG_F   */
    0x80,                              		/* 6cc0 REG_A   */
    0x00,                              		/* 6cc2 REG_I   */
    0x00,                              		/* 6cc4 REG_R   */
    0x03,                              		/* 6cc6 REG_BC  */
    0x0C,                              		/* 6cc8 REG_DE  */
    0x30,                              		/* 6cca REG_HL  */
    0x00,                              		/* 6ccc REG_SP  */
    0xC0,                              		/* 6cce REG_AF  */
    0x00,                              		/* 6cd0 REG_AF1 */
    0x100,                             		/* 6cd2 REG_IX  */
    0x200,                             		/* 6cd4 REG_IY  */
};

operand_t *plo1=&lo1, *plo2=&lo2, *plo3=&lo3, *plo4=&lo4;


/*
 *
 * The zas.h file is part of the restored ZAS.COM program
 * from the Hi-Tech C compiler v3.09
 *
 * Not a commercial goal of this laborious work is to popularize among
 * potential fans of 8-bit computers the old HI-TECH C compiler V3.09
 * (HI-TECH Software) and extend its life, outside of the CP/M environment
 * (Digital Research, Inc), for full operation in a  Unix-like operating
 * system UZI-180 without using the CP/M emulator.
 *
 * The HI-TECH C compiler V3.09 is provided free of charge for any use,
 * private or commercial, strictly as-is. No warranty or product support
 * is offered or implied including merchantability, fitness for a particular
 * purpose, or non-infringement. In no event will HI-TECH Software or its
 * corporate affiliates be liable for any direct or indirect damages.
 *
 * You may use this software for whatever you like, providing you acknowledge
 * that the copyright to this software remains with HI-TECH Software and its
 * corporate affiliates.
 *
 * All copyrights to the algorithms used, binary code, trademarks, etc.
 * belong to the legal owner - Microchip Technology Inc. and its subsidiaries.
 * Commercial use and distribution of recreated source codes without permission
 * from the copyright holderis strictly prohibited.
 *
 * Early work on the decompilation was done by Andrey Nikitin
 * Completion of the work and porting to work under modern compilers done by Mark Ogden
 * 19-May-2022
 */

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char bool;
#define true 1
#define false 0
#define const
#define _Noreturn

typedef char int8_t;
typedef unsigned char uint8_t;

typedef int int16_t;
typedef unsigned int uint16_t;

typedef long int32_t;
typedef unsigned long uint32_t;

#include "kwd.h"

#define MAX_OPERATORS 331
#define MAX_PSECT     17
#define MAX_TMP       580
#define MAX_JOPT      600

#define LIST_FRAGMENT_SIZE 33 /* includes trailing 0*/
#define MAX_LIST_FRAGMENT  10

typedef union { /* types returned from yylex */
    int16_t yVal;
    int32_t yNum;
    float yFloat;
    char *yStr;
    struct _sym *ySym;
} yytype_t;

typedef struct {
    int32_t val;       /* symbol value */
    struct _sym *pSym; /* associated psect if needed */
    struct _sym *eSym; /* associated external symbol if needed */
} rval_t;

typedef struct {
    int32_t curLoc; /* current location in psect. Aligns with val in rval_t */
    int16_t size;   /* psect size */
} psect_t;

typedef struct _sym {
    char *sName;
    union {
        rval_t _sym;   /* normal symbol info */
        psect_t _psec; /* psect info */
    } u;
    uint16_t sFlags;
} sym_t;

typedef struct {
    int16_t tType;
    int16_t tVal;
    rval_t expr;
} operand_t;

/* shorthands as Hi-Tech C v3.09 does not support nameless unions */
#define symRval u._sym /* for the internal structures masking u._ */
#define symPsec u._psec
#define sVal    u._sym.val
#define sPsym   u._sym.pSym
#define sEsym   u._sym.eSym
#define pCurLoc u._psec.curLoc
#define pSize   u._psec.size

#define oVal    expr.val
#define oPsym   expr.pSym
#define oEsym   expr.eSym

typedef struct {
    int16_t tLabel;
    int16_t tLoc;
    sym_t *tPsect;
} tmpLabel_t;

/**************************************************************************
 * Prototype functions are in location order
 * +--  compiles under VC
 * ++-  compiles under Hitech C
 * +++  code compatible
 **************************************************************************/
typedef kwd_t *(*kwdfptr)(register char *s);

void writeObjHeader(); /* 2 01C1 +-- */

void writeRecords();  /* 5 0282 +-- */
void finishRecords(); /* 6 02E5 +-- */

void addObjByte(int16_t n);                /* 8 03D0 +-- */
void addObjRelocWord(register rval_t *pv); /* 9 0461 +-- */
void addObjRelocByte(register rval_t *pv); /* 10 055A +-- */

void addObjAllSymbols(void); /* 17 091F +-- */

void addObjEnd(void); /* 20 0AA0 +-- */

rval_t *evalExpr(void); /* 21 0B67 +-- */

int16_t yylex(void); /* 32 1B8C +-- */

void skipLine(void); /* 39 23F1 +-- */

int main(int argc, char **argv); /* 59 2EF4 +-- */
char *getNextFile(void);         /* 60 329B +-- */

void fatalErr(); /* 61 32BF +-- */
void error();    /* 62 32E6 +-- */
void prMsg();    /* 63 3335 +-- */

int16_t getKwdId(char *s);   /* 67 3623 +-- */
int16_t getPsectId(char *s); /* 68 3639 +-- */

void relocErr(void); /* 70 368F +-- */

void doPass(void); /* 74 393D +-- */

_Noreturn void syntaxErr(void); /* 80 3F83 +-- */

sym_t *getSym(register char *name, int flags);       /* 103 4F7A +-- */
void enterAbsPsect(void);                            /* 105 510B +-- */
void resetVals(void);                                /* 106 5146 +-- */
void addSym(register sym_t *ps);                     /* 110 5366 +-- */
void initLocalLabels(void);                          /* 112 53E5 +-- */
void defTmpLabel(int16_t nLabel);                    /* 113 5404 +-- */
rval_t *findLocalLabel(int16_t nLabel, int tokType); /* 114 546A +-- */

char *fname(char *name);

extern rval_t startAddr;             /* 9799 */
extern char yytext[100];             /* 9b28 */
extern int16_t curLineno;            /* 9bbe */
extern char controls;                /* 9bc5 */
extern char phase;                   /* 9e3a */
extern bool j_opt;                   /* 9e59 */
extern bool n_opt;                   /* 9e68 */
extern bool x_opt;                   /* 9e69 */
extern FILE *objFp;                  /* 9e6e */
extern FILE *asmFp;
extern yytype_t yylval;              /* 9e70 */
extern uint16_t floatMode;           /* 9e74 */
extern uint16_t symFlags;            /* 9e76 */
extern int16_t tokType;              /* 9e7c */
extern sym_t *absPsect;              /* a29c */
extern sym_t *curPsect;              /* a298 */
extern int jmpOptCnt;                /* 9e7c */
extern rval_t retProp;               /* a752 */
extern uint32_t curLoc32;
extern int16_t lineLen;

/*	source is in Upper RAM, destination is in Lower RAM */
void	GetString(char* dest, char* source);

/*	source is in Lower RAM, destination is in Upper RAM */
void	PutString(char* source, char* dest);

/*	source is in Upper RAM */
short	StringLen(char* source);

extern char	tmpBuf[32];
extern char	tmpBufAux[32];


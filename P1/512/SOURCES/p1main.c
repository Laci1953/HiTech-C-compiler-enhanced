/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

 /*
 *
 * The main.c file is part of the restored P1.COM program
 * from the Hi-Tech CP/M Z80 C v3.09
 *
 * Not a commercial goal of this laborious work is to popularize among
 * potential fans of 8-bit computers the old HI-TECH Z80 C compiler V3.09
 * (HI-TECH Software) and extend its life, outside of the CP/M environment
 * for full operation in windows 32/64 and Unix-like operating systems
 *
 * The HI-TECH Z80 C cross compiler V3.09 is provided free of charge for any use,
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
 *
 * See the readme.md file for additional commentary
 *
 * Mark Ogden
 * 10-Jul-2022
 */
#include "p1.h"

char inBuf[128];
char errBuf[128];

char ca9CB1[64];
char *tmpFile = "p1.tmp"; /* 91db */
FILE *crfFp;              /* 9ff7 */
char crfNameBuf[30];
char srcFile[100]; 
char *crfFile;            /* a07b */
bool s_opt;               /* a07d */
bool w_opt;               /* a07e */
int16_t lineNo;           /* a07f */
char *srcFileArg;         /* a081 */
bool l_opt;               /* a083 */
FILE *tmpFp;              /* a084 */
int16_t errCnt;           /* a286 */

int8_t depth;       /* a288 */
uint8_t byte_a289;  /* a289 */
bool unreachable;   /* a28a */
int16_t word_a28b;  /* a28b */
sym_t *curFuncNode; /* a28d */
sym_t *p25_a28f;    /* ad8f */

s2_t *p2List      = &s2_9cf3[20]; /* 8bc7 */
int16_t strId     = 0;            /* 8bd7 */
uint8_t byte_8f85 = 0;            /* 8f85 */
bool byte_8f86 = false;        /* 8f86 */
uint8_t byte_968b;                /* 968b */
int16_t word_968c;                /* 968c */
int16_t tmpLabelId;               /* 968e */

expr_t **s13SP;   /* 9cf1 */
s2_t s2_9cf3[20]; 
char pad9d00[27];
expr_t s13_9d1b; /* 9d1b */
expr_t s13_9d28; /* 9d28 */

expr_t *s13FreeList; /* 9d35 */
uint8_t byte_9d37;   /* 9d37 */
expr_t *s13Stk[20];  /* 9d38 */

char *keywords[] = { /* 8f87 */
                     "asm",      "auto",   "break",  "case",   "char",   "continue", "default",
                     "do",       "double", "else",   "@@@@@",  "enum",   "extern",   "float",
                     "for",      "goto",   "if",     "int",    "long",   "register", "return",
                     "short",    "sizeof", "static", "struct", "switch", "typedef",  "union",
                     "unsigned", "void",   "while"
};

char lastEmitSrc[64];
bool sInfoEmitted;     /* 9da0 */
int16_t inCnt;         /* 9da1 */
char lastEmitFunc[40];
YYTYPE yylval;         /* 9dcb */
char nameBuf[32]; 
uint8_t ungetTok;      /* 9def */

int16_t strChCnt;    /* 9df0 */
bool lInfoEmitted;   /* 9df2 */
int16_t startTokCnt; /* 9df3 */
uint8_t ungetCh;     /*  9df5 */

//sym_t *word_a291;   /* as91 */
//sym_t *s25FreeList; /* a293 */
//sym_t **hashtab;    /* a295 */

header hashtab[HASHTABSIZE];
header SymList;

s12_t *p12_a297;    /* a297 */
uint8_t byte_a299;  /* a299 */
uint8_t byte_a29a;  /* a29a */

int16_t word_9caf; /*9caf */

void prMsg(char *p1, int p2, int p3);
void copyTmp(void);
void closeFiles(void);
void sub_3abf(void);
char* rindex(char*, char);
void maintop(int argc, char *argv[]);
void InitHash(void);

/**************************************************
 * 71: 367E PMO +++
 * basic blocks located differently
 * strcpy 2nd arg optimisation missed
 **************************************************/
int main(int argc, char *argv[]) 
{
/* begin exec code in top psect */

    maintop(argc, argv);

    InitHash();
    sub_07e3();

    Init512Banks();

/* end exec code in top psect */

    if (!InitOverlays())
	fatalErr("Can't open overlays");

    sub_3abf();
    copyTmp();

    if (fclose(stdout) == -1)
        prError("close error (disk space?)");

    closeFiles();
    exit(errCnt != 0);
}

/**************************************************
 * 72: 3936 PMO +++
 **************************************************/
void prMsg(p1, p2, p3) char *p1;
{
    char buf[128];

    sprintf(buf, p1, p2, p3);
    prMsgAt(buf);
}

/**************************************************
 * 73: 396C PMO +++
 **************************************************/
void prError(p1, p2, p3) char *p1;
{
    ++errCnt;
    prMsg(p1, p2, p3);
    fputc('\n', stderr);
}

/**************************************************
 * 74: 399E PMO +++
 **************************************************/
void fatalErr(p1, p2) char *p1;
char *p2;
{
    prError(p1, p2);
    closeFiles();
    exit(1);
}

/**************************************************
 * 75: 39C1 PMO +++
 **************************************************/
void prWarning(p1, p2, p3) char *p1;
{
    if (w_opt)
        return;
    prMsg(p1, p2, p3);
    fprintf(stderr, " (warning)\n");
}

/**************************************************
 * 62: 329A PMO +++
 **************************************************/
void prErrMsg(void) 
{
    register char *iy;

    if (!lInfoEmitted) 
    {
        iy = depth ? /* curFuncNode->nVName */ GetString((char*)curFuncNode, OFF_nVName) : "";

        if (!l_opt && (strcmp(srcFile, lastEmitSrc) || strcmp(iy, lastEmitFunc))) 
	{
            fprintf(stderr, "%s:", srcFile);

            if (*iy)
                fprintf(stderr, " %s()\n", iy);
            else
                fputc('\n', stderr);

            strcpy(lastEmitSrc, srcFile);
            strcpy(lastEmitFunc, iy);
        }

        fprintf(stderr, "%6d:\t%s", lineNo, inBuf);
        lInfoEmitted = true;
    }
}

/**************************************************
 * 63: 3350 PMO +++
 **************************************************/
void prMsgAt(register char *buf) 
{
    int16_t i;
    uint16_t col;

    prErrMsg();

    if (!*inBuf)
        fputs(buf, stderr);
    else 
    {
        fputc('\t', stderr);

        for (col = i = 0; i < startTokCnt - 1; i++)
            if (inBuf[i] == '\t')
                col = (col + 8) & 0xfff8;
            else
                col++;

        if (strlen(buf) + 1 < col)
            fprintf(stderr, "%*s ^ ", col - 1, buf);
        else
            fprintf(stderr, "%*c %s", col + 1, '^', buf);
    }
}

/**************************************************
 * 76: 39F3 PMO +++
 **************************************************/
void expectErr(char *p) 
{
    prError("%s expected", p);
}

/**************************************************
 * 77: 3A07 PMO +++
 **************************************************/
void copyTmp(void) 
{
    int ch;

    fclose(tmpFp);

    if ((tmpFp = fopen(tmpFile, "r")) == NULL)
        fatalErr("Can't reopen %s", tmpFile);

    while ((ch = fgetc(tmpFp)) != EOF)
        fputc(ch, stdout);
}

/**************************************************
 * 78: 3A5E PMO +++
 **************************************************/
void closeFiles(void) 
{
    fclose(stdin);
    fclose(stdout);

    if (tmpFp) 
    {
        fclose(tmpFp);
        unlink(tmpFile); 
    }

    if (crfFp) /* PMO - close missing in original */
        fclose(crfFp);
}
/**************************************************
 * 79: 3A80 PMO +++
 **************************************************/
void *xalloc(size_t size) 
{
    register char *ptr;

    do 
    {
        if ((ptr = malloc(size)) != NULL)
            goto done;
    } 
    while (/* s13ReleaseFreeList() */ (bool)CallOverlay0(s13ReleaseFreeList, 32+5));

    fatalErr("Out of memory in lower RAM");

done:
    blkclr(ptr, size);
    return ptr;
}

char* myalloc(short size)
{
    char* p;

    if (!(p = alloc512(size)))
	fatalErr("Out of memory in upper RAM");

    ZeroFill(p, size);

    return p;
}

/**************************************************
 * 80: 3ABF PMO +++
 **************************************************/
void sub_3abf(void) 
{
    uint8_t tok;

    while ((tok = /* yylex() */ (uint8_t)CallOverlay0(yylex, 32+4)) != T_EOF) 
    {
        ungetTok = tok;
        /* sub_3adf(); */
	CallOverlay0(sub_3adf, 32+4);
    }

    /* checkScopeExit(); */
    CallOverlay0(checkScopeExit, 32+4);
}

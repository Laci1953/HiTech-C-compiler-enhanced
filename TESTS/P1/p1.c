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
 * 09-Jul-2022
 */
#include "p1.h"
//#include "showVersion.h"

#include "cclass.h"

char ccClass[] = { /* 8134 */
                   _Z,                                                                      /* EOF */
                   _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 0  */
                   _S,      _S,      _S,      _Z,      _S,      _S,      _Z,      _Z,       /* 8 */
                   _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 10 */
                   _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 18 */
                   _S,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 20 */
                   _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 28 */
                   _D | _X, _D | _X, _D | _X, _D | _X, _D | _X, _D | _X, _D | _X, _D | _X,  /* 30 */
                   _D | _X, _D | _X, _Z,      _Z,      _Z,      _Z,      _Z,      _Z,       /* 38 */
                   _Z,      _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,       /* 40 */
                   _U,      _U,      _U,      _U,      _U,      _U,      _U,      _U,       /* 48 */
                   _U,      _U,      _U,      _U,      _U,      _U,      _U,      _U,       /* 50 */
                   _U,      _U,      _U,      _Z,      _Z,      _Z,      _Z,      _U,       /* 58 */
                   _Z,      _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,       /* 60 */
                   _L,      _L,      _L,      _L,      _L,      _L,      _L,      _L,       /* 68 */
                   _L,      _L,      _L,      _L,      _L,      _L,      _L,      _L,       /* 70 */
                   _L,      _L,      _L,      _Z,      _Z,      _Z,      _Z,      _Z        /* 78 */
};

char *tmpFile = "p1.tmp"; /* 91db */
char errBuf[512];         /* 9df7 */
FILE *crfFp;              /* 9ff7 */
char crfNameBuf[30];      /* 9ff9 */
char srcFile[100];        /* a017 */
char *crfFile;            /* a07b */
bool s_opt;               /* a07d */
bool w_opt;               /* a07e */
int16_t lineNo;           /* a07f */
char *srcFileArg;         /* a081 */
bool l_opt;               /* a083 */
FILE *tmpFp;              /* a084 */
char inBuf[512];          /* a086 */
int16_t errCnt;           /* a286 */

opStk_t *opSP     = &opStk[20]; /* 8bc7 */
int16_t strId     = 0;          /* 8bd7 */
uint8_t byte_8f85 = 0;          /* 8f85 */
bool lexMember    = false;      /* 8f86 */
uint8_t byte_968b;              /* 968b */
int16_t word_968c;              /* 968c */
int16_t tmpLabelId;             /* 968e */

expr_t **exprSP;   /* 9cf1 */
opStk_t opStk[20]; /* 9cf3 */
char pad9d00[27];
expr_t eZero; /* 9d1b */
expr_t eOne;  /* 9d28 */

expr_t *exprFreeList; /* 9d35 */
uint8_t byte_9d37;    /* 9d37 */
expr_t *exprStk[20];  /* 9d38 */

char *keywords[] = { /* 8f87 */
                     "asm",      "auto",   "break",  "case",   "char",   "continue", "default",
                     "do",       "double", "else",   "@@@@@",  "enum",   "extern",   "float",
                     "for",      "goto",   "if",     "int",    "long",   "register", "return",
                     "short",    "sizeof", "static", "struct", "switch", "typedef",  "union",
                     "unsigned", "void",   "while"
};

char lastEmitSrc[64];  /* 9d60 */
bool sInfoEmitted;     /* 9da0 */
int16_t inCnt;         /* 9da1 */
char lastEmitFunc[40]; /* 9da3 */
YYTYPE yylval;         /* 9dcb */
char nameBuf[32];      /* 9dcf */
uint8_t ungetTok;      /* 9def */

int16_t strChCnt;    /* 9df0 */
bool lInfoEmitted;   /* 9df2 */
int16_t startTokCnt; /* 9df3 */
int16_t ungetCh;     /*  9df5 */

sym_t *sigptr;

/* clang-format off */
op_t opTable[68] = {
{"",    6,    0,    0x200,  7}, /*  60 T_OPBOT */
{"(",   0x1e, 2,    0x200,  3}, /*  61 */
{"",    0x1f, 0,    0,      0}, /*  62 T_LPAREN */
{"",    2,    0,    0,      0}, /*  63 T_RPAREN */
{"",    0x1e, 2,    0,      0}, /*  64 T_ARRAYIDX */
{"",    0x1e, 0,    0,      0}, /*  65 T_LBRACK */
{"",    2,    0,    0,      0}, /*  66 T_RBRACK */
{".",   0x1e, 2,    0xa00,  2}, /*  67 T_DOT */
{"",    0x1e, 2,    0,      0}, /*  68 T_POINTER */
{"*U",  0x1c, 0x1c, 0x4200, 3}, /*  69 D_DEREF */
{"&U",  0x1c, 0x1c, 0x2200, 4}, /*  70 D_ADDRESSOF */
{"-U",  0x1c, 0x1c, 0x30,   1}, /*  71 D_NEG */
{"!",   0x1c, 8,    2,      6}, /*  72 T_LNOT */
{"~",   0x1c, 0x18, 0x20,   1}, /*  73 T_BNOT  */
{"",    0x1c, 8,    0,      0}, /*  74 T_PREINC */
{"",    0x1c, 8,    0,      0}, /*  75 T_POSTINC */
{"",    0x1c, 8,    0,      0}, /*  76 T_PREDEC */
{"",    0x1c, 8,    0,      0}, /*  77 T_POSTDEC */
{"#",   0x1c, 0x18, 0x300,  5}, /*  78 T_SIZEOF */
{"()",  0x1c, 0xa,  0,      0}, /*  79 T_CAST */
{"*",   0x1a, 0x16, 0x70,   1}, /*  80 T_MUL */
{"&",   0x11, 0x16, 0x60,   1}, /*  81 T_BAND */
{"-",   0x18, 0x16, 0x7c,   1}, /*  82 T_MINUS */
{"/",   0x1a, 0x12, 0x70,   1}, /*  83 T_ DIV */
{"%",   0x1a, 0x12, 0x60,   1}, /*  84 T_MOD */
{"+",   0x18, 0x12, 0x74,   1}, /*  85 T_PLUS */
{">>",  0x16, 0x12, 0x60,   1}, /*  86 T_SHR */
{"<<",  0x16, 0x12, 0x60,   1}, /*  87 T_SHL */
{"<",   0x14, 0x12, 0x10f8, 6}, /*  88 T_LT */
{">",   0x14, 0x12, 0x10f8, 6}, /*  89 T_GT */
{"<=",  0x14, 0x12, 0x10f8, 6}, /*  90 T_LE */
{">=",  0x14, 0x12, 0x10f8, 6}, /*  91 T_GE */
{"==",  0x12, 0x12, 0x90f8, 6}, /*  92 T_EQEQ */
{"!=",  0x12, 0x12, 0x90f8, 6}, /*  93 T_NE */
{"^",   0x10, 0x12, 0x60,   1}, /*  94 T_XOR*/
{"|",   0xf,  0x12, 0x60,   1}, /*  95 T_BOR */
{"&&",  0xe,  2,    3,      6}, /*  96 T_LAND */
{"||",  0xd,  2,    3,      6}, /*  97 T_LOR */
{"?",   0xc,  0x1a, 2,      2}, /*  98 T_QUEST */
{":",   0xc,  0x1a, 0x12f8, 1}, /*  99 T_COLON */
{"=",   0xa,  0xa,  0x2600, 1}, /* 100 T_EQ */
{"=+",  0xa,  0xa,  0x2434, 1}, /* 101 P1_EQPLUS */
{"++",  0,    0xa,  0x2434, 1}, /* 102 P1_POSTINC */
{"=-",  0xa,  0xa,  0x2434, 1}, /* 103 P1_EQMINUS */
{"--",  0,    0xa,  0x2434, 1}, /* 104 P1_POSTDEC */
{"=*",  0xa,  0xa,  0x2430, 1}, /* 105 P1_EQMUL */
{"=/",  0xa,  0xa,  0x2430, 1}, /* 106 P1_EQDIV */
{"=%",  0xa,  0xa,  0x2420, 1}, /* 107 P1_EQMOD */
{"=>>", 0xa,  0xa,  0x2020, 1}, /* 108 P1_EQSHR */
{"=<<", 0xa,  0xa,  0x2020, 1}, /* 109 P1_EQSHL */
{"=&",  0xa,  0xa,  0x2420, 1}, /* 110 P1_EQAND */
{"=^",  0xa,  0xa,  0x2420, 1}, /* 111 P1_EQXOR */
{"=|",  0xa,  0xa,  0x2420, 1}, /* 112 P1_EQOR */
{",",   7,    2,    0x200,  7}, /* 113 T_COMMA */
{";",   7,    2,    0x200,  2}, /* 114 T_SEMICOLON */
{"",    0,    1,    0,      0}, /* 115 T_ID */
{"",    0,    0x11, 0,      0}, /* 116 T_ICONST */
{"",    0,    0x11, 0,      0}, /* 117 T_LCONST */
{"",    0,    0x11, 0,      0}, /* 118 T_SCONST */
{"",    0,    0x11, 0,      0}, /* 119 S_TYPE */
{"..",  0,    0,    0,      0}, /* 120 */
{")",   0,    0,    0x200,  7}, /* 121 */
{"$U",  0,    0,    0x200,  7}, /* 122 P1_COND */
{"$",   0,    2,    2,      7}, /* 123 */
{"->",  0,    0x12, 0x880,  2}, /* 124 P1_CONVERT */
{"@",   0,    2,    0x480,  7}, /* 125 */
{"",    0,    1,    0,      0}, /* 126 D_MEMBERID*/
{"",    0,    0x11, 0,      0}, /* 127 T_FCONST */


}; /* 9271 */

int8_t depth;       /* a288 */
uint8_t voidReturn;  /* a289 */
bool unreachable;   /* a28a */
int16_t returnLabel;  /* a28b */
sym_t *curFuncNode; /* a28d */
sym_t *p25_a28f;    /* ad8f */

sym_t *tmpSyms;     /* as91 */
sym_t *symFreeList; /* a293 */
sym_t **hashtab;    /* a295 */
decl_t *curDecl;    /* a297 */
uint8_t defSClass;  /* a299 */
uint8_t byte_a29a;  /* a29a */

char blank[] = "";

// --------------------------------------------------------

int main(int argc, char *argv[]);
#ifdef CPM
void prMsg(char *p1, int p2, int p3);
#else
void prMsg(char *fmt, va_list args);

#endif
void copyTmp(void);
void closeFiles(void);
void mainParseLoop(void);

/**************************************************
 * 71: 367E PMO +++
 * basic blocks located differently
 * strcpy 2nd arg optimisation missed
 **************************************************/
int main(int argc, char *argv[]) {
    register char *st;

    //CHK_SHOW_VERSION(argc, argv);

    initMemAddr(); /* get data area range */

    for (--argc, ++argv; argc && *argv[0] == '-'; --argc, argv++) {
        switch (argv[0][1]) {
        case 'E':
        case 'e':
            if (!freopen(*argv + 2, "a", stderr))
                ;
            setbuf(stderr, errBuf);
            break;
        case 'S':
        case 's':
            s_opt = true;
            break;
        case 'W':
        case 'w':
            w_opt = true;
            break;
        case 'L':
        case 'l':
            l_opt = true;
            break;
        case 'C':
        case 'c':
            if (argv[0][2])
                crfFile = argv[0] + 2;
            else
                crfFile = crfNameBuf;
            break;
        }
    }
    sub_4d92();
    resetExprStack();
    if (argc) {
        if (freopen(argv[0], "r", stdin) == 0)
            fatalErr("can't open %s", *argv);
        srcFileArg = argv[0];
        strcpy(srcFile, srcFileArg);
        if (argc != 1 && freopen(argv[1], "w", stdout) == NULL)
            fatalErr("can't open %s", argv[1]);
        if (argc == 3)
            tmpFile = argv[2];
    } else
        strcpy(srcFile, srcFileArg = "(stdin)");

    if (crfFile) {
        if (*crfFile == '\0') {
            crfFile = crfNameBuf;
            strcpy(crfNameBuf, srcFile);
            if ((st = rindex(crfNameBuf, '.')))
                strcpy(st, ".crf");
            else
                strcat(crfNameBuf, ".crf");
        }
        if (!(crfFp = fopen(crfFile, "a")))
            prWarning("Can't create xref file %s", crfFile);
        else
            fprintf(crfFp, "~%s\n", srcFile);
    }
    if (!(tmpFp = fopen(tmpFile, "w")))
        fatalErr("can't open %s", tmpFile);

    eOne.tType    = T_ICONST;
    eZero.tType         = T_ICONST;
    eZero.attr.dataType = eOne.attr.dataType = DT_INT;
    eZero.t_l      = 0;
    eOne.t_l      = 1;

    mainParseLoop();
    copyTmp();

    if (fclose(stdout) == -1)
        prError("close error (disk space?)");
    closeFiles();
    exit(errCnt != 0);
}

#ifdef CPM
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

#else
/**************************************************
 * 72: 3936 PMO
 **************************************************/
void prMsg(char *fmt, va_list args) {
    char buf[128];

    vsprintf(buf, fmt, args);
    prMsgAt(buf);
}

/**************************************************
 * 73: 396C PMO
 **************************************************/
void prError(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ++errCnt;
    prMsg(fmt, args);
    va_end(args);
    fputc('\n', stderr);
}

/**************************************************
 * 74: 399E PMO
 **************************************************/
void fatalErr(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ++errCnt;
    prMsg(fmt, args);
    va_end(args);
    fputc('\n', stderr);
    closeFiles();
    exit(1);
}

/**************************************************
 * 75: 39C1 PMO
 **************************************************/
void prWarning(char *fmt, ...) {
    if (w_opt)
        return;
    va_list args;
    va_start(args, fmt);
    prMsg(fmt, args);
    va_end(args);
    fprintf(stderr, " (warning)\n");
}



#endif

/**************************************************
 * 76: 39F3 PMO +++
 **************************************************/
void expectErr(char *p) {

    prError("%s expected", p);
}

/**************************************************
 * 77: 3A07 PMO +++
 **************************************************/
void copyTmp(void) {
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
void closeFiles(void) {

    fclose(stdin);
    fclose(stdout);
    if (tmpFp) {
        fclose(tmpFp);
        unlink(tmpFile);
    }
    if (crfFp) /* PMO - close missing in original */
        fclose(crfFp);
}
/**************************************************
 * 79: 3A80 PMO +++
 **************************************************/
void *xalloc(size_t size) {
    register char *ptr;

    do {
        if ((ptr = malloc(size)) != NULL)
            goto done;
    } while (releaseSymFreeList() || releaseExprList());
    fatalErr("Out of memory");
done:
    blkclr(ptr, size);
    return ptr;
}

/**************************************************
 * 80: 3ABF PMO +++
 **************************************************/
void mainParseLoop(void) {
    uint8_t tok;

    while ((tok = yylex()) != T_EOF) {
        ungetTok = tok;
        sub_3adf();
    }
    releaseScopeSym();
}

int16_t lastLineNo; /*9caf */
char emittedSrcFile[64];   /* 9cb1 */
void emitWhere(register FILE *p);
void emitVarVal(register sym_t *p);
void sub_0470(expr_t *p);
void sub_05f1(register expr_t *st);

/**************************************************
 * 1: 013D PMO +++
 **************************************************/
void emitWhere(register FILE *p) {

    if (lastLineNo != lineNo || strcmp(emittedSrcFile, srcFile) != 0) {
        fprintf(p, "\"%d", lineNo);
        if (strcmp(srcFile, emittedSrcFile) != 0)
            fprintf(p, " %s", srcFile);
        fputc('\n', p);
    }
    lastLineNo = lineNo;
    strcpy(emittedSrcFile, srcFile);
    if (s_opt != 0)
        emitSrcInfo();
}

/**************************************************
 * 2: 01C1 PMO +++
 **************************************************/
void emitVarVal(register sym_t *p) {

    if (p->a_nodeType == EXPRNODE)
        sub_0470(p->a_expr);
    else
        putchar('1');
}

/**************************************************
 * 3: 01EC PMO +++
 **************************************************/
void emitDependentVar(register sym_t *p) {

    if (p->a_dataType == DT_COMPLEX && !p->a_indirection && p->a_nextSym->a_nodeType != FUNCNODE &&
        !(p->a_nextSym->flags & S_EMITTED))
        emitVar(p->a_nextSym);
}

/**************************************************
 * 4: 022E PMO +++
 * optimiser has basic blocks moved and eliminates
 * a ld a,(ix+6)
 **************************************************/
void prFuncBrace(uint8_t tok) {
    if (tok == T_RBRACE)
        putchar('}');
    else if (tok == T_LBRACE)
        putchar('{');
    putchar('\n');
}

/**************************************************
 * 5: 0258 PMO +++
 **************************************************/
void emitLocalLabelDef(int16_t p) {

    emitWhere(stdout);
    printf("[e :U %d ]\n", p); /* EXPR :U */
}

/**************************************************
 * 6: 0273 PMO +++
 **************************************************/
void emitLabelDef(register sym_t *st) {

    if (st) {
        emitWhere(stdout);
        printf("[e :U "); /* EXPR :U */
        emitSymName(st, stdout);
        printf(" ]\n");
    }
}

/**************************************************
 * 7: 02A6 PMO +++
 **************************************************/
void emitCase(case_t *p1) {
    int16_t caseCnt;
    register s4_t *st;

    if (p1) {
        emitWhere(stdout);
        printf("[\\ "); /* CASE */
        sub_0470(p1->switchExpr);
        putchar('\n');
        caseCnt = p1->caseCnt;
        for (st = p1->caseOptions; caseCnt--; st++) {
            putchar('\t');
            sub_0470(st->caseVal);
            printf(" %d\n", st->caseLabel);
        }
        putchar('\t');
        printf(".. %d\n", p1->defLabel);
        printf("]\n");
    }
}

/**************************************************
 * 8: 0353 PMO +++
 **************************************************/
void emitStructUnion(sym_t *p, char c) {
    register sym_t *st;

    if (p) {
        for (st = p->memberList; st != p; st = st->memberList)
            emitDependentVar(st);

        emitWhere(stdout);
        putchar('[');
        if (c == D_STRUCT)
            putchar('s'); /* STRUCT */
        else
            putchar('u'); /* UNION */
        printf(" S%d", p->a_labelId);
        for (st = p->memberList; st != p; st = st->memberList) {
            if ((st->flags & S_BITFIELD))
                printf(" :%d", st->bwidth);
            putchar(' ');
            emitAttr(&st->attr);
            putchar(' ');
            emitVarVal(st);
        }
        printf(" ]\n");
    }
}

/**************************************************
 * 9: 042D PMO +++
 **************************************************/
void emitCast(register expr_t *p) {

    emitWhere(stdout);
    printf("[e "); /* EXPR */
    while (p && p->tType == P1_CONVERT)
        p = p->t_lhs;
    sub_0470(p);
    printf(" ]\n");
}

/**************************************************
 * 10: 0470 PMO +++
 **************************************************/
void sub_0470(expr_t *p) {

    if (p)
        sub_05f1(p);
    else
        putchar('1');
}

/**************************************************
 * 11: 0493 PMO +++
 **************************************************/
void emitVar(register sym_t *st) {
    char c;

    if (st) {
        emitWhere(stdout);
        emitDependentVar(st);
        st->flags |= S_EMITTED;
        printf("[v "); /* VAR */
        emitSymName(st, stdout);
        putchar(' ');
        emitAttr(&st->attr);
        putchar(' ');
        if (st->flags & S_MEM)
            emitVarVal(st);
        else
            putchar('0');
        if (st->sclass == D_STACK)
            c = (st->flags & S_ARG) ? 'r' : 'p';
        else
            c = *keywords[(unsigned)st->sclass - T_ASM];
        if (st->flags & 4)
            c += 0xE0;
        printf(" %c ]\n", c);
    }
}

/**************************************************
 * 12: 053F PMO ++
 **************************************************/
void emitAscii(register expr_t *st, char *pc) {
    int16_t len;

    emitWhere(tmpFp);
    fprintf(tmpFp, "[a %d", st->t_id);
    len = st->t_chCnt;
    do {
        fprintf(tmpFp, " %d", *pc++);
    } while (len--);
    fprintf(tmpFp, " ]\n");
}

/**************************************************
 * 13: 05B5 PMO +++
 **************************************************/
void sub_05b5(expr_t *p1) {

    emitWhere(stdout);
    sub_0470(p1);
    putchar('\n');
}

/**************************************************
 * 14: 05D3 PMO +++
 **************************************************/
void sub_05d3(expr_t *p1) {

    emitWhere(stdout);
    sub_0470(p1);
    putchar('\n');
}

/**************************************************
 * 15: 05F1 PMO +++
 * Optimiser passes only uint8_t to sub_5a76 as it
 * has the function prototype
 **************************************************/
void sub_05f1(register expr_t *st) {
    op_t *po;
    sym_t *ps;
    expr_t *pe;

    po = &opTable[st->tType - T_EROOT];
    if (po->operandFlags & O_LEAF) {
        switch (st->tType) {
        case T_ID:
            ps = st->t_pSym;
            if (ps->sclass == D_CONST) {
                printf(". `");
                emitSymName(ps->memberList, stdout);
                printf(" %d", ps->memberId);
            } else
                emitSymName(ps, stdout);
            break;
        case T_ICONST:
            if (isVarOfType(&st->attr, DT_VOID) != 0) {
                printf("%ld", st->t_ul);
                break;
            } else
                printf("-> %ld ", st->t_ul);
        /* FALLTHRU */
        case S_TYPE:
            emitAttr(&st->attr);
            break;
        case T_FCONST:
            printf(".%s", st->t_s);
            break;
        case T_SCONST:
            printf(":s %d", st->t_id);
            break;
        case D_MEMBERID:
            printf("%d", st->t_id);
            break;
        }
    } else {
        if (st->tType == T_SIZEOF)
            printf("-> ");
        if (st->tType == T_SIZEOF && (pe = st->t_lhs)->tType == T_ID &&
            !(pe->t_pSym->flags & S_MEM)) {
            printf("* # ");
            emitAttr(&pe->attr);
            putchar(' ');
            emitVarVal(pe->t_pSym);
            putchar(' ');
            emitAttr(&st->attr);
        } else {
            printf("%.3s ", po->name);
            if (st->tType != T_120) {
                sub_05f1(st->t_lhs);
                if (po->operandFlags & O_BINARY) {
                    putchar(' ');
                    sub_05f1(st->t_rhs);
                } else if (st->tType == T_SIZEOF) {
                    putchar(' ');
                    emitAttr(&st->attr);
                }
            }
        }
    }
}

/**************************************************
 * 16: 07E3 PMO +++
 * optimiser removes call csv & jp cret
 **************************************************/
void resetExprStack(void) {
    exprSP       = &exprStk[20];
    exprFreeList = 0;
}

/* expr.c */
expr_t *sub_0817(register attr_t *st);
bool isConstExpr(register expr_t *st);
bool sub_0b93(register expr_t *st);
bool sub_10a8(void);
expr_t *sub_1340(register expr_t *st, expr_t *p2);
expr_t *newFConstLeaf(char *fltStr);
expr_t *makeBool(register expr_t *st);
expr_t *typeAlign(register expr_t *pe, attr_t *pa);
expr_t *sub_1ccc(expr_t *p1, uint8_t p2);
expr_t *sub_1d02(register expr_t *st);
uint8_t getResultDataType(register expr_t *lhs, expr_t *rhs);
expr_t *sub_1df0(register expr_t *st);
expr_t *sub_1e37(register expr_t *st);
expr_t *newAddressOfNode(register expr_t *st);
expr_t *sub_1ebd(register expr_t *st);
bool sub_1ef1(register expr_t *st);
expr_t *sub_1f5d(register expr_t *st, attr_t *p2, bool unscaled);
expr_t *newExprItem(uint8_t tok);
expr_t *getResultAttr(uint8_t p1, register expr_t *st, expr_t *p3);
expr_t *newNode(uint8_t tok, register expr_t *st, expr_t *p3);
expr_t *newSConstLeaf(void);
void complexErr(void);
expr_t *popExpr(void);
void pushOp(uint8_t p1);
uint8_t popOp(void);

/**************************************************
 * 17: 07F5 PMO +++
 **************************************************/
expr_t *sub_07f5(char p1) {
    char l1;
    register expr_t *st;

    l1        = byte_9d37;
    byte_9d37 = p1;
    st        = sub_0bfc();
    byte_9d37 = l1;
    return st;
}

/**************************************************
 * 18: 0817 PMO +++
 * Minor differences due to addition of missing
 * parameters and use of uint8_t parameter
 * also one missed optimisation
 * of pop bc, push hl to ex (sp),hl
 **************************************************/
expr_t *sub_0817(register attr_t *pa) {
    int16_t var2;
    int16_t argCnt;
    expr_t *var6;
    uint8_t tok;
    expr_t *arr[128];


    if (pa && pa->nodeType != FUNCNODE && pa->indirection == 1 && pa->dataType == DT_COMPLEX &&
        pa->nextAttr->nodeType == FUNCNODE)
        pa = pa->nextAttr;
#ifdef BUGGY
    else if (pa->nodeType != FUNCNODE)
        prError("function or function pointer required");
#else
    /*PMO: fixed else clause incase pa is NULL */
    else if (!pa || pa->nodeType != FUNCNODE)
        prError("function or function pointer required");
#endif

    if (pa && pa->nodeType == FUNCNODE && pa->pFargs) {
        argCnt = pa->pFargs->cnt;
        pa   = pa->pFargs->argVec;
    } else {
        pa   = NULL;
        argCnt = 0;
    }

    var2     = 0;
    ungetTok = tok = yylex();
    for (;;) {
        if (pa && pa->dataType == DT_VARGS) {
            argCnt = 0;
            pa   = 0;
        }
        if (tok != T_RPAREN) {
            if (pa && isVarOfType(pa, DT_VOID)) {
                prError("function does not take arguments");
                argCnt = 0;
                pa   = NULL;
            }
            var6 = parseExpr(T_EROOT, sub_07f5(3), 0);
            if (var6) {
                if (pa && argCnt-- == 0) {
                    prError("too many arguments");
                    pa   = 0;
                    argCnt = 0;
                }
                if (pa)
                    var6 = sub_1f5d(var6, pa++, true);
                else
                    var6 = sub_1d02(var6);
                arr[var2++] = var6;
                tok         = yylex();
                if (tok == T_COMMA)
                    continue;
                ungetTok = tok;
            }
        }
        break;
    }
    if ((argCnt != 1 || var2 != 0 || !isVarOfType(pa, DT_VOID)) && argCnt && pa->dataType != DT_VARGS)
        prError("too few arguments");

    if (var2 == 0)
        return newNode(T_120, NULL, NULL); /* dummy 2nd & 3rd args added */
    argCnt = 0;
    while (argCnt + 1 != var2) {
        arr[argCnt + 1] = newNode(T_COMMA, arr[argCnt], arr[argCnt + 1]);
        ++argCnt;
    }
    return arr[argCnt];
}

/**************************************************
 * 19: 0A83 PMO +++
 * Minor differences due to addition of missing
 * parameters and use of uint8_t parameter
 **************************************************/
expr_t *parseConstExpr(uint8_t n) {
    register expr_t *st;

    byte_9d37 = n;
    if ((st = parseExpr(T_EROOT, sub_0bfc(), 0))) { /* PMO added dummy arg3 */
        if (!isConstExpr(st))
            prError("constant expression required");
        else if (byte_9d37 == 2) {
            if (isValidIndex(&st->attr))
                st = sub_1ccc(st, DT_CONST);
            else
                prError("illegal type for array dimension");
        }
    }
    byte_9d37 = 0;
    return st;
}

/**************************************************
 * 20: 0AED PMO +++
 * Locations of code to set return  values 1/0 swapped
 * rest of code adjusted accordingly
 **************************************************/
bool isConstExpr(register expr_t *st) {
    uint8_t op, flags;

    if (st == 0)
        return true;
    op = st->tType;
    if (op == T_ID)
        return isVarOfType(&st->attr, DT_ENUM);
    if (op == T_SIZEOF)
        return true;
    if (op == D_ADDRESSOF && sub_0b93(st->t_lhs))
        return true;
    flags = opTable[(op - T_EROOT)].operandFlags;
    if (!(flags & 0x10))
        return false;
    if (flags & O_LEAF)
        return true;
    return isConstExpr(st->t_lhs) && (!(flags & O_BINARY) || isConstExpr(st->t_rhs));
}

/**************************************************
 * 21: 0B93 PMO +++
 **************************************************/
bool sub_0b93(register expr_t *st) {
    uint8_t op;

    op = st->tType;
    if (op == T_ID)
        return st->t_pSym->sclass == T_EXTERN || st->t_pSym->sclass == T_STATIC;
    if (op == T_DOT)
        return sub_0b93(st->t_lhs);
    return op == D_DEREF && isConstExpr(st->t_lhs);
}

/**************************************************
 * 22: 0BFC PMO +++
 * many basic blocks moved around, logic still valid
 * some optimisation differences but equivalent code
 **************************************************/
expr_t *sub_0bfc(void) {
    attr_t var8;
    expr_t *leaf FORCEINIT;
    opStk_t *savOpSP;
    expr_t **savExprSP;
    uint8_t tok;
    uint8_t tok2;
    uint8_t hasLhs;
    bool more;
    bool hasMember;
    uint8_t hasRhs;
    uint8_t prec;
    int16_t var17;
    // int16_t var19; /* not used */
    register sym_t *st;

    savExprSP = exprSP;
    savOpSP   = opSP;
    pushOp(T_EROOT);
    hasLhs = false;
    // var19  = 0;
    for (;;) {
        hasMember = lexMember = opSP->op == T_DOT || opSP->op == T_POINTER;
        tok                   = yylex();
        lexMember             = false;
        if (tok < T_EROOT || tok >= T_OPTOP) {
            ungetTok = tok;
            tok      = T_EROOT;
        }
        if (opTable[tok - T_EROOT].operandFlags & O_LEAF) {
            if (hasLhs)
                goto error;
            switch (tok) {
            case T_ID: /* c9a */
                st = yylval.ySym;
                if (!hasMember) {
                    if (st->sclass == 0) {
                        if (peekCh() == '(') {
                            var8.nodeType    = 2;
                            var8.dataType    = DT_INT;
                            var8.pExpr      = NULL;
                            var8.indirection = 0;
                            st               = sub_4eed(st, T_EXTERN, &var8, 0);
                            st->flags |= 0x42;
                            st->level = 0;
                            emitVar(st);
                        } else {
                            prError("undefined identifier: %s", st->nVName);
                            st->sclass        = T_STATIC;
                            st->attr.dataType = DT_INT;
                            st->flags         = 0x10 + S_MEM;
                        }
                    } else {
                        var17 = st->flags;
                        if (!(var17 & S_VAR))
                            prError("not a variable identifier: %s", st->nVName);
                        else if (st->sclass == T_EXTERN && !(var17 & S_EMITTED))
                            emitVar(st);
                    }
                    sub_51cf(st);
                } /* d57 */
                leaf = newIdLeaf(st);
                break;
            case T_ICONST: /* d75 */
                leaf = newIntLeaf(yylval.yNum, DT_INT);
                break;
            case T_LCONST: /* d90 */
                leaf = newIntLeaf(yylval.yNum, DT_LONG);
                break;
            case T_FCONST: /* da5 */
                leaf = newFConstLeaf(yylval.yStr);
                break;
            case T_SCONST: /* dae */
                leaf          = newSConstLeaf();
                leaf->t_chCnt = strChCnt;
                emitAscii(leaf, yylval.yStr);
                free(yylval.yStr);
                break;
            case S_TYPE:
                goto error;
            }
            /* d63 */
            pushExpr(leaf);
            hasLhs = true;
            continue;
        } /* dfa */
        switch (tok) {
        case T_LPAREN:
            ungetTok = tok2 = yylex();
            if (tok2 == S_TYPE || (tok2 == T_ID && yylval.ySym->sclass == T_TYPEDEF)) {
                if (hasLhs)
                    goto error;
                sub_5dd1(0, &var8);
                defineArg(st = sub_69ca(T_CAST, &var8, 0, 0));
                sub_51cf(st);
                tok2 = yylex();
                if (tok2 != T_RPAREN)
                    goto error;
                var8 = st->attr;
                pushExpr(newSTypeLeaf(&var8));
                if (opSP->op == T_SIZEOF) {
                    opSP->prec = 100;
                    hasLhs     = true;
                    continue;
                } else
                    tok = T_CAST;
            } else if (hasLhs) {
                pushOp(D_FUNC);
                pushExpr(sub_0817(&(*exprSP)->attr));
            }
            break;
        case T_EROOT:
            break;
        case T_LBRACK:
            if (!hasLhs)
                goto error;
            tok    = T_ARRAYIDX;
            hasLhs = false;
            break;
        case T_RPAREN:
        case T_RBRACK:
            if (!hasLhs)
                goto error;
            break;
        case T_PREINC:
        case T_PREDEC:
            if (hasLhs) /* have lhs so make post INC/DEC */
                tok++;
            break;
        default:
            if ((opTable[tok - T_EROOT].operandFlags & O_ALT) && !hasLhs)
                tok -= 11; /* map to unary function */
            hasRhs = (opTable[tok - T_EROOT].operandFlags & O_BINARY) != 0;
            if (hasRhs != hasLhs)
                goto error;
            hasLhs = false;
            break;
        }
        /* f23 */
        prec = opTable[tok - T_EROOT].prec;
        if ((byte_9d37 >= 3 && tok == T_COMMA) ||
            (byte_9d37 == 1 && tok == T_COLON && opSP->op != T_QUEST))
            prec = 5;
        /* f8e */
        do {
            more = false;
            if (opSP->prec < prec ||
                (opSP->prec == prec && (opTable[tok - T_EROOT].operandFlags & O_RTOL))) {
                switch (tok) {
                case T_POSTINC:
                case T_POSTDEC:
                    prec = 31;
                    break;
                case T_LPAREN:
                case T_ARRAYIDX:
                    prec = 4;
                    break;
                }
                pushOp(tok);
                opSP->prec = prec;
            } else {
                if (opSP->op == T_EROOT) { /* 1058 */
                    if (tok != T_EROOT)
                        ungetTok = tok;
                    leaf = popExpr();
                    if (leaf && exprSP == savExprSP)
                        goto done;
                    else
                        goto error;
                } else if (opSP->op == T_LPAREN) {
                    if (tok != T_RPAREN) {
                        expectErr(")");
                        ungetTok = tok;
                    }
                } else if (opSP->op == T_ARRAYIDX) {
                    if (tok != T_RBRACK) {
                        expectErr("]");
                        ungetTok = tok;
                    }
                } else
                    more = true;
                /* 1037 */
                if (sub_10a8())
                    goto error;
            }
        } while (more);
    }
error:
    prError("expression syntax");
    skipStmt(tok);
    while (exprSP != savExprSP) /* remove partial expression */
        freeExpr(popExpr());
    leaf = NULL;

done:
    exprSP = savExprSP;
    opSP   = savOpSP;
    return leaf;
}

/**************************************************
 * 23: 10A8 PMO +++
 * Minor differences, use of uint8_ parameter and
 * addition of missing parameters
 **************************************************/
bool sub_10a8(void) {
    expr_t *rhsExpr;
    expr_t *pe;
    uint8_t op;
    register expr_t *lhsExpr FORCEINIT;

    if ((op = popOp()) == T_LPAREN)
        return false;

    rhsExpr = NULL;
    if (op != T_120 &&
        (((opTable[op - T_EROOT].operandFlags & O_BINARY) && (rhsExpr = popExpr()) == NULL) ||
         (lhsExpr = popExpr()) == NULL))
        return true;

    switch (op) {
    case T_ARRAYIDX:
        pushOp(D_DEREF);
        opSP->prec = 100;
        op         = T_PLUS;
        if (isValidIndex(&rhsExpr->attr))
            rhsExpr = sub_1ccc(rhsExpr, DT_CONST);
        else
            prError("illegal type for index expression");
        break;
    case T_PREINC:
    case T_POSTINC:
    case T_PREDEC:
    case T_POSTDEC:
        rhsExpr = &eOne;
        op      = op == T_PREINC    ? P1_EQPLUS
                  : op == T_PREDEC  ? P1_EQMINUS
                  : op == T_POSTINC ? P1_POSTINC
                                    : P1_POSTDEC;
        break;
    case T_POINTER:
        op      = T_DOT;
        lhsExpr = parseExpr(D_DEREF, lhsExpr, 0); /* added dummy 3rd arg */
        break;
    case T_SIZEOF:
        if (lhsExpr->tType == T_SCONST) {
            pe = newIntLeaf((long)lhsExpr->t_chCnt + 1, DT_UINT);
            freeExpr(lhsExpr);
            pushExpr(pe);
            return false;
        }
        if (lhsExpr->tType != S_TYPE && lhsExpr->tType != T_ID) {
            pe                = newSTypeLeaf(&lhsExpr->attr);
            pe->attr.pExpr   = NULL;
            pe->attr.nodeType = 0;
            if (lhsExpr->attr.nodeType == EXPRNODE) {
                pe      = parseExpr(T_SIZEOF, pe, 0);
                rhsExpr = sub_1ccc(cloneExpr(lhsExpr->attr.pExpr), DT_UINT);
                op      = T_MUL;
            }
            freeExpr(lhsExpr);
            lhsExpr = pe;
        } else if (lhsExpr->tType == S_TYPE && lhsExpr->attr.nodeType == EXPRNODE) {
            rhsExpr = sub_1ccc(cloneExpr(lhsExpr->attr.pExpr), DT_UINT);
            lhsExpr = parseExpr(T_SIZEOF, lhsExpr, 0);
            op      = T_MUL;
        }
        break;
    case T_COMMA:
        if (opSP[0].op != T_LPAREN || opSP[1].op != D_FUNC)
            op = T_SEMICOLON;
        break;
    case T_CAST:
        pushExpr(typeAlign(parseExpr(T_EROOT, rhsExpr, 0), &lhsExpr->attr));
        freeExpr(lhsExpr);
        return false;
    }
    if ((op == T_QUEST) != (rhsExpr && rhsExpr->tType == T_COLON))
        return true;
    pushExpr(parseExpr(op, lhsExpr, rhsExpr));
    return false;
}

/**************************************************
 * 24: 1340 PMO +++
 * Optimised uint8_t param
 **************************************************/
expr_t *sub_1340(register expr_t *st, expr_t *p2) {
    sym_t *var2;
    sym_t *var4;

    if (!isVarOfType(&st->attr, DT_STRUCT) && !isVarOfType(&st->attr, DT_UNION))
        prError("struct/union required");
    else if (p2->tType != T_ID)
        prError("struct/union member expected");
    else if ((var4 = st->a_nextSym) == 0)
        ;
    else if (!(var4->flags & 1))
        prError("undefined struct/union: %s", var4->nVName);
    else if ((var2 = findMember(var4, p2->t_pSym->nVName))) {
        sub_51cf(var2);
        st       = newExprItem(D_MEMBERID);
        st->t_id = var2->memberId;
        st->attr = var2->attr;
        freeExpr(p2);
        return st;
    }
    return p2;
}

/**************************************************
 * 25: 1441 PMO +++
 * Minor optimiser differences, but equivalent code
 * Also uint8_t parameter differences
 **************************************************/
expr_t *parseExpr(uint8_t p1, register expr_t *lhs, expr_t *rhs) {
    attr_t tmpExpr;
    expr_t *savedLhs;
    expr_t *minusLhs FORCEINIT;
    bool hasRhs;
    bool minusLhsValid;
    int16_t operatorFlags;
    char *opStr;
    uint8_t var13;

    hasRhs = (opTable[p1 - T_EROOT].operandFlags & O_BINARY) != 0;
    opStr  = opTable[p1 - T_EROOT].name;
    if (!lhs || (hasRhs && rhs == 0)) {
        freeExpr(lhs);
        if (hasRhs)
            freeExpr(rhs);
        return NULL;
    }
    minusLhsValid = false;
    operatorFlags = opTable[p1 - T_EROOT].operatorFlags;
    if (p1 == D_ADDRESSOF && lhs->tType == T_ID && (lhs->t_pSym->flags & S_REG))
        prError("can't take address of register variable");

    if (!(operatorFlags & OP_SIZEOF))
        lhs = sub_1e37(lhs);
    if (hasRhs && !(operatorFlags & OP_MEMBER))
        rhs = sub_1e37(rhs);

    if (p1 == D_FUNC) {
        if ((lhs->attr.indirection & 1) && lhs->attr.nodeType == SYMNODE)
            lhs = parseExpr(D_DEREF, lhs, 0); /* dummy 3rd arg */
    } else
        lhs = sub_1df0(lhs);

    if (hasRhs)
        rhs = sub_1df0(rhs);
    if ((operatorFlags & OP_RTOL) && !sub_1ef1(lhs)) {
        if (p1 == D_ADDRESSOF) {
            if (lhs->tType == D_ADDRESSOF && lhs->t_lhs->attr.nodeType == EXPRNODE)
                return lhs;
            else
                prError("can't take this address");
        } else
            prError("only lvalues may be assigned to or modified");
    }
    if ((operatorFlags & OP_DREF) &&
        (!(lhs->attr.indirection & 1) || lhs->attr.nodeType != SYMNODE))
        prError("pointer required");
    if (!(operatorFlags & (OP_RBOOL | OP_LBOOL))) {
        if (isVarOfType(&lhs->attr, DT_BOOL))
            lhs = sub_1ccc(lhs, DT_INT);
        if (hasRhs && isVarOfType(&rhs->attr, DT_BOOL))
            rhs = sub_1ccc(rhs, DT_INT);
    }
    switch (p1) {
    case T_EROOT:
        return lhs;
    case T_DOT:
        rhs = sub_1340(lhs, rhs);
        break;
    case T_121:
        tmpExpr          = curFuncNode->attr;
        tmpExpr.nodeType = SYMNODE;
        if (isVarOfType(&tmpExpr, DT_VOID))
            prError("void function cannot return value");
        else
            lhs = sub_1f5d(lhs, &tmpExpr, true);
        break;
    }
    if ((operatorFlags & (OP_LBOOL | OP_RBOOL))) {
        if ((operatorFlags & OP_LBOOL))
            lhs = makeBool(lhs);
        if ((operatorFlags & OP_RBOOL))
            rhs = makeBool(rhs);
    } else if ((operatorFlags & OP_SCALE) && (lhs->attr.indirection & 1) &&
               lhs->attr.nodeType == SYMNODE && isIntType(&rhs->attr)) /* 16e1 */
        rhs = sub_1ccc(parseExpr(T_STAR, rhs, sub_1ebd(lhs)),
                       (rhs->attr.dataType & DT_UNSIGNED) ? DT_UCONST : DT_CONST);
    else if (p1 == T_PLUS && (rhs->attr.indirection & 1) && rhs->attr.nodeType == SYMNODE &&
             isIntType(&lhs->attr)) { /* 1740 */
        savedLhs = lhs;
        lhs      = rhs;
        rhs      = sub_1ccc(parseExpr(T_STAR, savedLhs, sub_1ebd(lhs)),
                       (rhs->attr.dataType & DT_UNSIGNED) ? DT_UCONST : DT_CONST);
    } else if ((operatorFlags & 8) && (lhs->attr.indirection & 1) &&
               lhs->attr.nodeType == SYMNODE &&
               (!hasRhs ||
                ((rhs->attr.indirection & 1) && rhs->attr.nodeType == SYMNODE))) { /* 17ab */
        if (!(operatorFlags & OP_EREL) || (!isVoidStar(&lhs->attr) && !isVoidStar(&rhs->attr))) {
            if (hasRhs && !haveSameDataType(&lhs->attr, &rhs->attr))
                prWarning("operands of %.3s not same pointer type", opStr);
            else if (p1 == T_MINUS) {
                minusLhs      = lhs;
                minusLhsValid = true;
                lhs           = sub_1ccc(lhs, DT_CONST);
                rhs           = sub_1ccc(rhs, DT_CONST);
            }
        }
    } else if ((operatorFlags & (OP_FLOAT | OP_INT)) && isSimpleType(&lhs->attr) &&
               (!hasRhs || isSimpleType(&rhs->attr))) { /* 187a */
        if (operatorFlags & OP_UNSIGNED) {
            var13 = getResultDataType(lhs, rhs);
            lhs   = sub_1ccc(lhs, var13);
            rhs   = sub_1ccc(rhs, var13);
        } /* 18fa */
        if (!(operatorFlags & OP_FLOAT) &&
            (!isIntType(&lhs->attr) || (hasRhs && !isIntType(&rhs->attr))))
            prError("integral type required");
    } else if (operatorFlags & OP_VOIDFUNC) { /* 1937 */
        if (isVarOfType(&lhs->attr, DT_VOID) || (hasRhs && isVarOfType(&rhs->attr, DT_VOID))) {
            if (p1 == P1_CONVERT && lhs->tType == D_FUNC && isVarOfType(&rhs->attr, DT_VOID)) {
                lhs->attr = rhs->attr;
                freeExpr(rhs);
                return lhs;
            }
            prError("illegal use of void expression");
        } else if (!(operatorFlags & OP_SEP) && (!isLogicalType(&lhs->attr) ||
                                                 (hasRhs && !isLogicalType(&rhs->attr)))) /* 19cc */
            prError("simple type required for %.3s", opStr);
        else if (operatorFlags & OP_AREL) { /* 1a11 */
            if ((operatorFlags & OP_EREL)) {
                if (isZero(lhs) && (rhs->attr.indirection & 1) && rhs->attr.nodeType == SYMNODE)
                    lhs = typeAlign(lhs, &rhs->attr);
                else if (isZero(rhs) && (lhs->attr.indirection & 1) &&
                         lhs->attr.nodeType == SYMNODE)
                    rhs = typeAlign(rhs, &lhs->attr);
            } /* 1a95 */
            if (!haveSameDataType(&lhs->attr, &rhs->attr))
                prWarning("operands of %.3s not same type", opStr);
        }
    } else if (!(operatorFlags & OP_SEP)) /* 1ac3 */
        prError("type conflict");
    /* 1ad1 */
    if (operatorFlags & 0x400)
        rhs = sub_1f5d(rhs, &lhs->attr, !(operatorFlags & OP_SCALE));

    savedLhs = getResultAttr(p1, lhs, rhs);
    if (minusLhsValid)
        savedLhs = parseExpr(T_DIV, savedLhs, sub_1ebd(minusLhs));
    return savedLhs;
}

/**************************************************
 * 26: 1B4B PMO +++
 **************************************************/
expr_t *newIntLeaf(long num, uint8_t intDt) {
    register expr_t *st;

    st                = newIConstLeaf(num);
    st->attr.dataType = intDt;
    return st;
}

/**************************************************
 * 27: 1B70 PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newFConstLeaf(char *fltStr) {
    register expr_t *st;

    st                = newExprItem(T_FCONST);
    st->t_s           = fltStr;
    st->attr.dataType = DT_DOUBLE;
    return st;
}

/**************************************************
 * 28: 1B94 PMO +++
 * minor optimiser difference, equivalent code
 **************************************************/
expr_t *makeBool(register expr_t *st) {

    if (!isLogicalType(&st->attr))
        prError("logical type required");
    else if (!isVarOfType(&st->attr, DT_BOOL))
        st = parseExpr(T_NE, st, typeAlign(&eZero, &st->attr));

    return st;
}

/**************************************************
 * 29: 1BF7 PMO +++
 * minor optimiser difference, equivalent code
 **************************************************/
expr_t *typeAlign(register expr_t *pe, attr_t *pa) {
    expr_t *pf;

    if (pe->tType == D_FUNC && (pf = pe->t_lhs)->tType == T_ID &&
        (pf->t_pSym->flags & S_IMPLICIT)) {
        prWarning("%s() declared implicit int", pf->t_pSym->nVName);
        pf->t_pSym->flags &= ~S_IMPLICIT;
    }

    if (!haveSameDataType(&pe->attr, pa)) {
        if (pe->tType != T_ICONST || inData(pe))
            pe = newNode(P1_CONVERT, pe, newSTypeLeaf(pa));
        pe->attr = *pa;
    }
    return pe;
}

/**************************************************
 * 30: 1CCC PMO +++
 **************************************************/
expr_t *sub_1ccc(expr_t *p1, uint8_t newDt) {
    attr_t st;

    st.dataType    = newDt;
    st.indirection = 0;
    st.pExpr      = 0;
    st.nodeType    = 0;
    return typeAlign(p1, &st);
}

/**************************************************
 * 31: 1D02 PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *sub_1d02(register expr_t *st) {

    if (st->tType == T_COMMA || st->tType == T_120)
        return st;
    if (st->a_nodeType == SYMNODE && !st->a_indirection) {
        if (st->a_dataType < DT_INT || st->a_dataType == DT_ENUM)
            return sub_1ccc(st, DT_INT);
        if (st->a_dataType == DT_FLOAT)
            return sub_1ccc(st, DT_DOUBLE);
    }
    return st;
}

/**************************************************
 * 32: 1D5A PMO +++
 **************************************************/
uint8_t getResultDataType(register expr_t *lhs, expr_t *rhs) {
    bool mkUnsigned;
    uint8_t resDataType;

    mkUnsigned  = (lhs->a_dataType & DT_UNSIGNED) || (rhs->a_dataType & DT_UNSIGNED);
    resDataType = lhs->a_dataType;
    if (resDataType < rhs->a_dataType)
        resDataType = rhs->a_dataType;
    if (resDataType < DT_INT)
        resDataType = DT_INT;
    if (resDataType == DT_FLOAT || resDataType == DT_DOUBLE)
        return DT_DOUBLE;
    if (resDataType == DT_ENUM)
        resDataType = DT_INT;
    if (mkUnsigned)
        return resDataType | 1;
    return resDataType;
}

/**************************************************
 * 33: 1DF0 PMO +++
 **************************************************/
expr_t *sub_1df0(register expr_t *st) {

    if (st->tType != T_ID || st->attr.nodeType != FUNCNODE)
        return st;
    st->attr.dataType    = DT_COMPLEX;
    st->attr.nodeType    = SYMNODE;
    st->a_nextSym        = st->t_pSym;
    st->attr.indirection = 0;
    return newAddressOfNode(st);
}

/**************************************************
 * 34: 1E37 PMO +++
 **************************************************/
expr_t *sub_1e37(register expr_t *st) {

    if (st->attr.nodeType == EXPRNODE)
        st = newAddressOfNode(st);
    return st;
}

/**************************************************
 * 35: 1E58 PMO +++
 * differences due to added 3rd arg and uint8_t param
 **************************************************/
expr_t *newAddressOfNode(register expr_t *st) {
    expr_t *pi;

    pi                = newNode(D_ADDRESSOF, st, 0); /* PMO missing 3rd arg. added 0 */
    pi->attr          = st->attr;
    pi->attr.pExpr   = 0;
    pi->attr.nodeType = SYMNODE;
    addIndirection(&pi->attr);
    return pi;
}

/**************************************************
 * 36: 1EBD PMO +++
 * differences due to added 3rd arg and uint8_t param
 **************************************************/
expr_t *sub_1ebd(register expr_t *st) {
    st = newSTypeLeaf(&st->attr);
    st->attr.indirection >>= 1;
    return parseExpr(T_SIZEOF, st, 0); /* PMO fixed missing 3rd arg */
}

/**************************************************
 * 37: 1EF1 PMO +++
 **************************************************/
bool sub_1ef1(register expr_t *st) {

    switch (st->tType) {
    case D_DEREF:
        return true;
    case T_ID:
        return (st->t_pSym->flags & S_VAR) && st->t_pSym->sclass != D_CONST &&
               st->attr.nodeType == SYMNODE;
    case T_DOT:
        return st->attr.nodeType == SYMNODE && sub_1ef1(st->t_lhs);
    }
    return false;
}

/**************************************************
 * 38: 1F5D PMO +++
 * minor differences due to uint8_t param
 **************************************************/
expr_t *sub_1f5d(register expr_t *src, attr_t *dAttr, bool unscaled) {
    attr_t *sAttr;

    sAttr = &src->attr;
    if (!haveSameDataType(sAttr, dAttr)) {
        if (isSimpleType(sAttr) && isSimpleType(dAttr)) {
            if (isIntType(dAttr) && isFloatType(sAttr))
                prWarning("implicit conversion of float to integer");
        } else if ((dAttr->indirection & 1) && dAttr->nodeType == SYMNODE && isIntType(sAttr)) {
            if (!unscaled && (isVarOfType(sAttr, DT_CONST) || isVarOfType(sAttr, DT_UCONST)))
                return src;
            if (!isZero(src)) /* a pointer can be set to a zero */
                prWarning("illegal conversion of integer to pointer");
        } else if ((sAttr->indirection & 1) && sAttr->nodeType == SYMNODE && isIntType(dAttr))
            prWarning("illegal conversion of pointer to integer");
        else if ((sAttr->indirection & 1) && sAttr->nodeType == SYMNODE &&
                 (dAttr->indirection & 1) && dAttr->nodeType == SYMNODE) {
            if (!isVoidStar(sAttr) && !isVoidStar(dAttr))
                prWarning("illegal conversion between pointer types");
        } else
            prError("illegal conversion");
    }
    return typeAlign(src, dAttr);
}

/**************************************************
 * 39: 2105 PMO +++
 **************************************************/
bool isZero(register expr_t *st) {

    switch (st->tType) {
    case T_ICONST:
        return st->t_ul == 0L; /* long */
    case P1_CONVERT:
        if (isIntType(&st->attr))
            return isZero(st->t_lhs);
        break;
    }
    return false;
}

/**************************************************
 * 40: 2157 PMO +++
 **************************************************/
bool releaseExprList(void) {
    register expr_t *st;

    if (exprFreeList == 0)
        return false;
    while ((st = exprFreeList)) {
        exprFreeList = st->t_lhs;
        free(st);
    }
    return true;
}

/**************************************************
 * 41: 2186 PMO +++
 **************************************************/
expr_t *newExprItem(uint8_t tok) {
    register expr_t *st;

    if (exprFreeList != 0) {
        st           = exprFreeList;
        exprFreeList = st->t_lhs;
        blkclr(st, sizeof(expr_t));
    } else
        st = xalloc(sizeof(expr_t));
    st->tType         = tok;
    st->attr.dataType = DT_VOID;
    return st;
}

/**************************************************
 * 42: 21C7 PMO +++
 * uint8_t paramater
 **************************************************/
expr_t *cloneExpr(register expr_t *st) {
    expr_t *l1;
    uint16_t operandFlags;

    l1           = newExprItem(0);
    *l1          = *st;
    operandFlags = opTable[st->tType - T_EROOT].operandFlags;
    if ((operandFlags & O_LEAF) || st->tType == T_120)
        return l1;

    l1->t_lhs = cloneExpr(st->t_lhs);
    if (operandFlags & O_BINARY)
        l1->t_rhs = cloneExpr(st->t_rhs);

    return l1;
}

/**************************************************
 * 43: 225A PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *getResultAttr(uint8_t op, register expr_t *lhs, expr_t *rhs) {
    expr_t *res;

    if (op == P1_CONVERT && lhs->tType == T_ICONST) {
        lhs->attr = rhs->attr;
        freeExpr(rhs);
        return lhs;
    }
    res = newNode(op, lhs, rhs);

    switch (opTable[op - 60].nodeType) {
    case 1:
        res->attr = lhs->attr;
        break;
    case 2:
        res->attr = rhs->attr;
        break;
    case 4:
        res->attr = lhs->attr; /* address of */
        addIndirection(&res->attr);
        break;
    case 3:
        res->attr = lhs->attr;
        delIndirection(&res->attr); /* deref */
        break;
    case 5:
        res->attr.dataType = DT_INT; /* sizeof */
        break;
    case 6:
        res->attr.dataType = DT_BOOL; /* bool operators */
        break;
    case 7:
        res->attr.dataType = DT_VOID;
        break;
    }
    return res;
}

/**************************************************
 * 44: 23B4 PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newNode(uint8_t tok, register expr_t *lhs, expr_t *rhs) {
    expr_t *pi;

    pi = newExprItem(tok);
    if (tok != T_120) {
        pi->t_lhs = lhs;
        if (opTable[tok - T_EROOT].operandFlags & O_BINARY)
            pi->t_rhs = rhs;
    }
    return pi;
}

/**************************************************
 * 45: 240E PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newIdLeaf(register sym_t *st) {
    expr_t *pi;

    pi         = newExprItem(T_ID);
    pi->t_pSym = st;
    if ((st->flags & 0x10) || st->sclass == D_MEMBER)
        pi->attr = st->attr;
    return pi;
}

/**************************************************
 * 46: 245D PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newSConstLeaf(void) {
    register expr_t *st;

    st                   = newExprItem(T_SCONST);
    st->attr.dataType    = DT_CHAR;
    st->attr.indirection = 1;
    st->t_id             = ++strId;
    return st;
}

/**************************************************
 * 47: 248A PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newIConstLeaf(long p1) {
    register expr_t *st;

    st      = newExprItem(T_ICONST);
    st->t_l = p1;
    return st;
}

/**************************************************
 * 48: 24B6 PMO +++
 * uint8_t parameter
 **************************************************/
expr_t *newSTypeLeaf(attr_t *p1) {
    register expr_t *st;

    st       = newExprItem(S_TYPE);
    st->attr = *p1;
    return st;
}

/**************************************************
 * 49: 24DE PMO +++
 **************************************************/
void complexErr(void) {

    fatalErr("expression too complex");
}

/**************************************************
 * 50: 24E7 PMO +++
 **************************************************/
void pushExpr(expr_t *p1) {

    if (exprSP == exprStk)
        complexErr();
    *(--exprSP) = p1;
}

/**************************************************
 * 51: 250A PMO +++
 **************************************************/
expr_t *popExpr(void) {

    if (exprSP != &exprStk[20])
        return *(exprSP++);
    return NULL;
}

/**************************************************
 * 52: 2529 PMO +++
 **************************************************/
void pushOp(uint8_t p1) {
    register opStk_t *st;

    if (opSP == opStk)
        complexErr();
    (--opSP)->op      = p1;
    (st = opSP)->prec = opTable[p1 - T_EROOT].prec;
}

/**************************************************
 * 53: 255D PMO +++
 **************************************************/
uint8_t popOp(void) {

    return (opSP++)->op;
}

/**************************************************
 * 54: 2569 PMO +++
 **************************************************/
void freeExpr(register expr_t *st) {
    uint8_t op;

    if (st) {
        op = st->tType;
        if (!(opTable[op - T_EROOT].operandFlags & O_LEAF) && op != T_120) {
            freeExpr(st->t_lhs);
            if (opTable[op - T_EROOT].operandFlags & O_BINARY)
                freeExpr(st->t_rhs);
        }
        if (!inData(st)) {
            if (op == T_FCONST)
                free(st->t_s);
            st->t_lhs    = exprFreeList; /* but on free list for quick reuse */
            exprFreeList = st;
        }
    }
}

/**************************************************
 * 55: 25F7 PMO +++
 * uint8_t parameter + minor optimisation difference
 * code is equivalent
 **************************************************/
expr_t *sub_25f7(register expr_t *st) {

    if (st) {
        if (!inData(st) && st->tType == T_ICONST) {
            st->t_ul += 1;
        } else if (st->tType == T_PLUS)
            st->t_rhs = sub_25f7(st->t_rhs);
        else
            st = parseExpr(T_PLUS, st, newIntLeaf(1L, DT_INT)); /* m3: */
    }
    return st; /* m4: */
}

uint8_t parseNumber(int16_t ch);
uint8_t parseName(int8_t ch);
void parseAsm(void);
void parseString(int16_t ch);
int16_t getCh(void);
void prErrMsg(void);
int16_t skipWs(void);
int8_t escCh(int16_t ch);

/**************************************************
 * 56: 2671 PMO +++
 * location of two basic blocks swapped, code equivalent
 **************************************************/
uint8_t yylex(void) {
    int16_t ch;
    uint8_t tok;
    char buf[50];
    register char *s;

    if (ungetTok) {
        tok      = ungetTok;
        ungetTok = 0;
        if (tok == T_ID && lexMember)
            yylval.ySym = sub_4e90(nameBuf);
        return tok;
    }
    for (;;) {
        ch          = skipWs();
        startTokCnt = inCnt;
        if (Isalpha(ch))
            return parseName((int8_t)ch);
        if (Isdigit(ch))
            return parseNumber(ch);
        switch (ch) {
        case EOF:
            return T_EOF;
        case '#':
            do {
                ch = getCh();
            } while (Isspace(ch) && ch != '\n');
            if (Isdigit(ch) && parseNumber(ch) == T_ICONST) {
                lineNo = (int16_t)(yylval.yNum - 1);
                do {
                    ch = getCh();
                } while (Isspace(ch) && ch != '\n');
                if (ch == '"') {
                    for (s = buf; (ch = getCh()) != '"' && ch != '\n';)
                        *s++ = (char)ch;
                    *s = '\0';
                    if (buf[0])
                        strcpy(srcFile, buf);
                    else if (srcFileArg)
                        strcpy(srcFile, srcFileArg);
                    else
                        *srcFile = '\0';
                    if (crfFp)
                        fprintf(crfFp, "~%s\n", srcFile);
                }
                break;
            } else {
                s = buf;
                do {
                    *s++ = (char)ch;
                    ch   = getCh();
                } while (ch != '\n' && !Isspace(ch));
                *s = '\0';
                while (ch != '\n')
                    ch = getCh();
                if (strcmp(buf, "asm") == 0) {
                    parseAsm();
                    break;
                } else
                    fatalErr("illegal '#' directive");
            }
            /* FALLTHRU */
        case '"':
            parseString('"');
            return T_SCONST;
        case '\'':
            ch          = getCh();
            yylval.yNum = (ch == '\\') ? escCh(getCh()) : ch;
            ch          = getCh();
            if (ch == '\n')
                expectErr("closing quote");
            else if (ch != '\'')
                prError("char const too long");

            while (ch != '\n' && ch != '\'')
                ch = getCh();
            return T_ICONST;
        case ';':
            return T_SEMI;
        case ':':
            return T_COLON;
        case '+':
            ch = getCh();
            if (ch == '+')
                return T_PREINC;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return P1_EQPLUS;
            ungetCh = ch;
            return T_PLUS;
        case '-':
            ch = getCh();
            if (ch == '-')
                return T_PREDEC;
            if (ch == '>')
                return T_POINTER;

            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return P1_EQMINUS;
            ungetCh = ch;
            return T_MINUS;
        case '*':
            ch = skipWs();
            if (ch == '=')
                return P1_EQMUL;
            ungetCh = ch;
            return T_MUL; /* deref or multiply */
        case '/':
            ch = skipWs();
            if (ch == '=')
                return P1_EQDIV;
            ungetCh = ch;
            return T_DIV;
        case '%':
            ch = skipWs();
            if (ch == '=')
                return P1_EQMOD;
            ungetCh = ch;
            return T_MOD;
        case '&':
            ch = getCh();
            if (ch == '&')
                return T_LAND;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return P1_EQAND;
            ungetCh = ch;
            return T_BAND;
        case '|':
            ch = getCh();
            if (ch == '|')
                return T_LOR;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return P1_EQOR;
            ungetCh = ch;
            return T_BOR;
        case '^':
            ch = skipWs();
            if (ch == '=')
                return P1_EQXOR;
            ungetCh = ch;
            return T_XOR;
        case '<':
            ch = getCh();
            if (ch == '<') {
                ch = skipWs();
                if (ch == '=')
                    return P1_EQSHL;
                else {
                    ungetCh = ch;
                    return T_SHL;
                }
            } else if (ch == '=')
                return T_LE;
            ungetCh = ch;
            return T_LT;
        case '>':
            ch = getCh();
            if (ch == '>') {
                ch = skipWs();
                if (ch == '=')
                    return P1_EQSHR;
                else {
                    ungetCh = ch;
                    return T_SHR;
                }
            } else if (ch == '=')
                return T_GE;
            ungetCh = ch;
            return T_GT;
        case '=':
            ch = getCh();
            if (ch == '=')
                return T_EQEQ;
            ungetCh = ch;
            return T_EQ;
        case '!':
            ch = getCh();
            if (ch == '=')
                return T_NE;
            ungetCh = ch;
            return T_LNOT;
        case '~':
            return T_BNOT;
        case '(':
            return T_LPAREN;
        case ')':
            return T_RPAREN;
        case '[':
            return T_LBRACK;
        case ']':
            return T_RBRACK;
        case '{':
            return T_LBRACE;
        case '}':
            return T_RBRACE;
        case '.':
            ch = getCh();
            if (Isdigit(ch)) {
                ungetCh = ch;
                return parseNumber('.');
            }
            if (ch == '.') {
                ch = getCh();
                if (ch != '.')
                    prError("'.' expected after '..'");
                return T_3DOT;
            }
            ungetCh = ch;
            return T_DOT;

        case '?':
            return T_QUEST;
        case ',':
            return T_COMMA;
        default:
            prError("illegal character 0%o", ch /* ,ch*/); /* original had ch twice!! */
            break;
        }
    }
}

/**************************************************
 * 57: 2CC3 PMO +++
 * two blocks change from ex de,hl ld de,xxx
 * to ld hl,xxx ex de,hl (xxx are to locations on stack)
 * optimiser also uses byte compare for digit >= base
 **************************************************/
uint8_t parseNumber(int16_t ch) {
    long lval;
    uint8_t base;
    char buf[50];
    uint8_t digit;
    register char *s = buf;

    while (Isdigit(ch)) {
        *s++ = (char)ch;
        ch   = getCh();
    }
    if (ch == '.' || ch == 'e' || ch == 'E') {
        if (ch == '.')
            do {
                *s++ = (char)ch;
                ch   = getCh();
            } while (Isdigit(ch));
        if (ch == 'e' || ch == 'E') {
            *s++ = 'e';
            ch   = getCh();
            if (ch == '+' || ch == '-') {
                *s++ = (char)ch;
                ch   = getCh();
            }
            if (Isdigit(ch))
                do {
                    *s++ = (char)ch;
                    ch   = getCh();
                } while (Isdigit(ch));
            else
                prError("exponent expected");
        }
        ungetCh = ch;
        *s++      = 0;
        if (*buf == '.')
            s++;
        yylval.yStr = xalloc(s - buf);
        if (*buf == '.')
            strcat(strcpy(yylval.yStr, "0"), buf);
        else
            strcpy(yylval.yStr,buf);
        return T_FCONST;
    }
    base = 10;

#ifdef BUGGY
    if (ch == 'x' || (ch == 'X' && *buf == '0')) {
#else
    /* 
        original code would allow invalid numbers such as
        99x123 and 0999X123 both as 0x123
    */
    if ((ch == 'x' || ch == 'X') && s == buf + 1 && *buf == '0') {
#endif
        base = 16;
        s    = buf;
        while (Isxdigit(ch = getCh()))
            *s++ = (char)ch;
        if (s == buf)
            prError("hex digit expected");
    } else if (*buf == '0')
        base = 8;
    lval = 0L;
    *s   = 0;
    s    = buf;
    while (*s) {
        if (*s >= 'A')
            digit = (*(uint8_t *)s++ | 0x20) - 'a' + 10;
        else
            digit = *(uint8_t *)s++ - '0';
        if (digit >= base) {
            prError("digit out of range");
            break;
        }
        lval = lval * base  + digit;
    }
    yylval.yNum = lval;
    if (ch == 'l' || ch == 'L')
        return T_LCONST;
    ungetCh = ch;
    return T_ICONST;
}

/**************************************************
 * 58: 2F75 PMO +++
 * uses ld hl,xxx ex de,hl compared to ex de,hl ld de,xxx
 * equivalent code
 **************************************************/
uint8_t parseName(int8_t ch) {
    int16_t len;
    uint8_t mid;
    uint8_t lo;
    uint8_t hi;
    int16_t cmp;
    register char *s = nameBuf;

    len              = 0;
    do {
        if (len != sizeof(nameBuf) - 1) {
            *s++ = ch;
            len++;
        }
        ch = (int8_t)getCh();
    } while (Isalnum(ch));
    ungetCh = ch;
    *s      = 0;
    lo      = T_ASM;
    hi      = T_WHILE;
    do {
        mid = (lo + hi) / 2;
        cmp = (int16_t)strcmp(nameBuf, keywords[mid - T_ASM]);
        if (cmp <= 0)
            hi = mid - 1;
        if (cmp >= 0)
            lo = mid + 1;
    } while (hi >= lo);
    if (hi < lo - 1) {
        switch (mid) {
        case T_AUTO:
        case T_EXTERN:
        case T_REGISTER:
        case T_STATIC:
        case T_TYPEDEF:
            yylval.yVal = mid;
            return S_CLASS;
        case T_CHAR:
        case T_DOUBLE:
        case T_ENUM:
        case T_FLOAT:
        case T_INT:
        case T_LONG:
        case T_SHORT:
        case T_STRUCT:
        case T_UNION:
        case T_UNSIGNED:
        case T_VOID:
            yylval.yVal = mid;
            return S_TYPE;
        case _T_SIZEOF:
            return T_SIZEOF;
        }
        return mid;
    }
    yylval.ySym = sub_4e90(nameBuf);
    return T_ID;
}

/**************************************************
 * 59: 308B PMO +++
 **************************************************/
void parseAsm(void) {
    int16_t ch;
    char buf[512];
    register char *s;

    for (;;) {
        s = buf;
        while ((ch = getCh()) != '\n' && ch != EOF)
            *s++ = (char)ch;
        *s = 0;
        if (ch == EOF)
            fatalErr("EOF in #asm");
        if (strncmp(buf, "#endasm", 7) == 0)
            return;
        printf(";; %s\n", buf);
    }
}

/**************************************************
 * 60: 310B PMO +++
 * one basic block relocated. Code equivalent
 **************************************************/
void parseString(int16_t ch) {
    char *var2;
    char *var4;
    char buf[1024];
    register char *s = buf;

    while (ch == '"') {
        while ((ch = getCh()) != '"') {
            if (ch == '\n') {
                expectErr("closing quote");
                break;
            }
            if (ch == '\\') {
                if ((ch = getCh()) != '\n')
                    *s++ = escCh(ch);
            } else
                *s++ = (char)ch;
        }
        ch = skipWs();
    }
    ungetCh  = ch;
    *s       = 0;
    strChCnt = (int16_t)(s - buf);
    var2 = var4 = xalloc(strChCnt + 1);
    ch          = strChCnt + 1; /* unwound memcpy. Note strcpy cannot handle embedded '\0' */
    s           = buf;
    while (ch--)
        *var2++ = *s++;
    yylval.yStr = var4;
}

/**************************************************
 * 61: 320D PMO +++
 * move of 2 basic blocks code equivalent
 **************************************************/
int16_t getCh(void) {
    int16_t ch;
#if !defined(CPM) && !defined(_WIN32)
    do {
#endif
        if (ungetCh) {
            ch      = ungetCh;
            ungetCh = 0;
        } else if ((ch = inBuf[inCnt++]) == 0) {
            if (s_opt)
                emitSrcInfo();
            sInfoEmitted = false;
            lInfoEmitted = false;

            if (!fgets(inBuf, 512, stdin))
                return EOF;
            ch          = inBuf[0];
            inCnt       = 1;
            startTokCnt = 0;
            lineNo++;
            if (l_opt)
                prErrMsg();
        }
#if !defined(CPM) && !defined(_WIN32)
        if (ch == 0x1a)
            return EOF;
    } while (ch == '\r');
#endif
    return ch;
}

/**************************************************
 * 62: 329A PMO +++
 **************************************************/
void prErrMsg(void) {
    register char *iy;
    if (!lInfoEmitted) {
        iy = depth && curFuncNode ? curFuncNode->nVName : "";

        if (!l_opt && (strcmp(srcFile, lastEmitSrc) || strcmp(iy, lastEmitFunc))) {
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
void prMsgAt(register char *buf) {
    int16_t i;
    uint16_t col;
    prErrMsg();
    if (!*inBuf)
        fputs(buf, stderr);
    else {
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
 * 64: 3429 PMO +++
 **************************************************/
void emitSrcInfo(void) {
    register char *s;

    if (!sInfoEmitted && inBuf[0]) {
        for (s = inBuf; *s && Isspace(*s); s++)
            ;
        if (*s && *s != '#')
            printf(";; ;%s: %d: %s", srcFile, lineNo, inBuf);
    }
    sInfoEmitted = true;
}

/**************************************************
 * 65: 347A PMO +++
 * equivalent code
 * uses ex de,hl ld de,xxx, cf. ld hl,xxx ex de,hl
 **************************************************/
int16_t skipWs(void) {
    int16_t ch;
    while (Isspace(ch = getCh()))
        ;
    return ch;
}

/**************************************************
 * 66: 3495 PMO +++
 * basic block move and different equivalent code
 * and optimisations
 **************************************************/
int8_t escCh(int16_t ch) {
    int16_t val;
    int8_t cnt;
    if (Isdigit(ch)) {
        val = ch - '0';
        ch  = getCh();
        if (Isdigit(ch)) {
            val = val * 8 + ch - '0';
            if (Isdigit(ch = getCh()))
                val = val * 8 + ch - '0';
            else
                ungetCh = ch;
        } else
            ungetCh = ch;
        return (int8_t)val;
    }
    switch (ch) {
    case 'n':
        return '\n';
    case 'b':
        return '\b';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'f':
        return '\f';
    case 'a':
        return '\a';
    case 'v':
        return '\v';
    case 'x':
        val = 0;
        cnt = 3;
        do {
            ch = getCh();
            if (!Isxdigit(ch)) {
                ungetCh = ch;
                return (int8_t)val;
            }
            val *= 16;
            if (Isupper(ch))
                ch |= 0x20;
            if (Islower(ch))
                val += ch - 'a' + 10;
            else
                val += ch - '0';
        } while (cnt--);
        return (int8_t)val;
    }
    return (int8_t)ch;
}

/**************************************************
 * 67: 35F7 PMO +++
 **************************************************/
int16_t peekCh(void) {
    int16_t ch;
    ungetCh = ch = skipWs();
    return ch;
}

/**************************************************
 * 68: 3610 PMO +++
 **************************************************/
void skipStmt(uint8_t tok) {

    while (tok > T_RBRACE)
        tok = yylex();
    if (tok == T_EOF)
        fatalErr("unexpected EOF");
    ungetTok = tok;
}

/**************************************************
 * 69: 363F PMO +++
 * uint8_t parameter
 **************************************************/
void expect(uint8_t etok, char *msg) {
    uint8_t tok;

    if ((tok = yylex()) == etok)
        return;
    expectErr(msg);
    skipStmt(tok);
}

/**************************************************
 * 70: 3666 PMO +++
 **************************************************/
void skipToSemi(void) {
    uint8_t tok;

    do {
        tok = yylex();
    } while (tok != T_SEMI);
    ungetTok = T_SEMI;
}

int16_t parseInitializer(register sym_t *st, bool p2);

/**************************************************
 * 81: 3ADF +++
 * minor differences due to adding missing arg and use
 * of uint8_t paramater
 * one use of add a,255 vs sub a,1 i.e. equivalent
 **************************************************/
void sub_3adf(void) {
    uint8_t tok;
    uint8_t scType;
    attr_t attr;
    uint8_t varb;
    uint8_t scFlags;

    register sym_t *st;

    scFlags = sub_5dd1(&scType, &attr);
    if ((tok = yylex()) == T_SEMI)
        return;
    ungetTok = tok;
    varb     = true;

    for (;;) {
        p25_a28f = 0;
        st       = sub_69ca(scType, &attr, scFlags & ~1, 0);
        tok      = yylex();

        if (st && (st->flags & S_VAR) && st->a_nodeType == FUNCNODE) {
            if (varb && tok != T_COMMA && tok != T_SEMI && scType != T_TYPEDEF) {
                voidReturn = st->a_nodeType == FUNCNODE && st->a_indirection == 0 &&
                            (st->a_dataType == DT_VOID || st->a_dataType == DT_INT);
                ungetTok = tok;
                defineArg(st);
                emitVar(st);
                curFuncNode = st;
                parseFunction();
                return;
            }
            if (p25_a28f && !(p25_a28f->flags & 8))
                expectErr("function body");
            ++depth;
            releaseScopeSym();
            --depth;
            emitVar(st);
        } else if (tok == T_EQ) {
            if (scType == T_TYPEDEF)
                prError("illegal initialisation");
            defineArg(st);
            emitVar(st);
            sub_3c7e(st);
            tok = yylex();
        } else if (st) {
            if ((scFlags & 1) || st->sclass != T_EXTERN) {
                defineArg(st);
                emitVar(st);
            } else
                emitDependentVar(st);
        }
        if (tok == T_ID || tok == T_STAR) {
            expectErr(",");
            ungetTok = tok;
        } else if (tok != T_COMMA)
            break;
        varb = false;
    }

    if (tok != T_SEMI) {
        expectErr(";");
        while (tok > T_RBRACE)
            tok = yylex();
    }
}

/**************************************************
 * 82: 3C7E PMO +++
 * use of uint8_t param
 **************************************************/
void sub_3c7e(sym_t *p1) {
    int16_t var2;
    register sym_t *st;

    if (p1) {
        printf("[i ");
        emitSymName(p1, stdout);
        putchar('\n');
        st = p1;
        if ((var2 = parseInitializer(st, true)) < 0) {
            prError("initialisation syntax");
            skipToSemi();
        } else if (st->a_nodeType == EXPRNODE && st->a_expr && isZero(st->a_expr)) {
            freeExpr(st->a_expr);
            st->a_expr = newIConstLeaf(var2);
        }
        printf("]\n");
    } else
        skipToSemi();
}

/**************************************************
 * 83: 3D24 +++
 * minor optimiser differences including moving basic
 * blocks. Use of uint8_t parameter
 **************************************************/
int16_t parseInitializer(register sym_t *st, bool p2) {
    int16_t initCnt;
    uint8_t tok;
    char *s;



#ifdef CPM
    /* manual string optimisation */
    static char Ustr[] = ":U ..\n";
    static char Dstr[] = "..\n";
#else
#define Ustr ":U ..\n"
#define Dstr "..\n"
#endif

    initCnt = -1;
    if (p2 && st->a_nodeType == EXPRNODE && st->a_expr) {
        bool inBraces;

        printf(":U ..\n");
        if ((inBraces = ((tok = yylex()) == T_LBRACE)))
            tok = yylex();
        if (tok == T_SCONST && st->attr.indirection == 0 && (st->attr.dataType & ~1) == DT_CHAR) {
            for (initCnt = 0, s = yylval.yStr; initCnt < strChCnt; initCnt++)
                printf("-> %d `c\n", *s++);
            free(yylval.yStr);
            if (isZero(st->a_expr)) {
                printf("-> %d `c\n", 0);
                ++initCnt;
            }
            if (inBraces)
                tok = yylex();
        } else if (!inBraces) /* 3e4a */
            expectErr("{");
        else {
            ungetTok = tok;
            if (st->a_indirection == 0 && st->a_dataType == DT_COMPLEX)
                st = st->a_nextSym;
            else
                p2 = false;
            initCnt = 0;
            for (;;) {
                if (parseInitializer(st, p2) < 0)
                    break;
                initCnt++;
                if ((tok = yylex()) == T_RBRACE || tok != T_COMMA || (tok = yylex()) == T_RBRACE)
                    break;
                ungetTok = tok;
            }
        }
        /* 3e22 */
        if (inBraces && tok != T_RBRACE) { /* end of initialiser list */
            expectErr("}");
            initCnt = -1;
        }
        printf(Dstr);
    } else if ((!p2 || st->a_nodeType != EXPRNODE) && st->a_indirection == 0 &&
               st->a_dataType == DT_STRUCT) { /* 3ec6 */
        sym_t *pNextSym;
        sym_t *pMember;
        bool inBraces;

        if (p2)
            printf(Ustr);
        printf(Ustr);
        if ((pNextSym = st->a_nextSym)) {
            if (!(inBraces = (tok = yylex()) == T_LBRACE))
                ungetTok = tok;
            pMember = pNextSym->memberList;
            do {
                if ((parseInitializer(pMember, true) < 0))
                    break;
                if ((pMember = pMember->memberList) == pNextSym) /* end of circular list */
                    break;
                if ((tok = yylex()) != T_COMMA) { /* end of this set of initialisers */
                    ungetTok = tok;
                    break;
                }
            } while (!inBraces || (ungetTok = tok = yylex()) != T_RBRACE);
            /* 3f7c: */
            if (inBraces) {
                if ((tok = yylex()) == T_COMMA) /* allow trailing , before } */
                    tok = yylex();
                if (tok != T_RBRACE)
                    expectErr("}");
                else
                    initCnt = 1;
            } else
                initCnt = 1;
        } /* 3fcd */
        printf(Dstr);
        if (p2)
            printf(Dstr);

    } else if ((p2 && st->attr.nodeType == EXPRNODE) || st->attr.nodeType == FUNCNODE ||
               (!(st->a_indirection & 1) && st->attr.dataType >= T_AUTO))
        prError("illegal initialisation");
    else {
        expr_t *vard;
        bool inBraces ;

        if (!(inBraces = (tok = yylex()) == T_LBRACE))
            ungetTok = tok;
        vard          = newSTypeLeaf(&st->attr);
        vard->attr.nodeType = SYMNODE;
        if ((vard = parseExpr(T_125, vard, parseConstExpr(3))))
            sub_05b5(vard->t_rhs);

        if (inBraces && yylex() != T_RBRACE)
            expectErr("}");
        freeExpr(vard);
        initCnt = 1;
    }
    return initCnt;
}

void parseStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseCompoundStmt(int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
void parseAsmStmt(void);
void parseWhileStmt(case_t *p3);
void parseDoStmt(case_t *p3);
void parseIfStmt(int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
void parseSwitchStmt(int16_t p1);
void parseForStmt(case_t *p1);
void parseBreak_ContinueStmt(int16_t label);
void parseDefaultStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseCastStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseReturnStmt(void);
void parseGotoStmt(void);
void parseStmtLabel(register sym_t *ps, int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
sym_t *sub_4ca4(register sym_t *ps);
void sub_4ce8(int16_t n);
void sub_4d15(int16_t n, register expr_t *st, char c);
void sub_4d67(register expr_t *st);

/**************************************************
 * 84: 409B PMO +++
 * use of uint8_t parameter
 **************************************************/
void parseFunction(void) {
    uint8_t tok;

    enterScope();
    sub_5c19(D_STACK);
    defineFuncSig();
    if ((tok = yylex()) != T_LBRACE) {
        expectErr("{");
        skipStmt(tok);
    }
    emitLabelDef(curFuncNode);
    unreachable = false;
    sub_5c19(0x14);
    returnLabel = newTmpLabel();
    while ((tok = yylex()) != T_RBRACE) {
        ungetTok = tok;
        parseStmt(0, 0, 0, 0);
    }
    if (!unreachable && !voidReturn)
        prWarning("implicit return at end of non-void function");
    emitLocalLabelDef(returnLabel);
    exitScope();
}

/**************************************************
 * 85: 4126 PMO +++
 * trivial optimiser differences, use of uint8_t param
 * and addition of dummy parameters
 **************************************************/
void parseStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4) {
    uint8_t tok;
    expr_t *var3;

    tok = yylex();
    if (unreachable && tok != T_SEMI && tok != T_LBRACE) {
        if (tok != T_CASE && tok != T_DEFAULT && (tok != T_ID || peekCh() != ':'))
            prWarning("Unreachable code");
        unreachable = false;
    }
    switch (tok) {
    case T_SEMI:
        break;
    case T_LBRACE:
        parseCompoundStmt(p1, p2, p3, p4);
        break;
    case T_ASM:
        parseAsmStmt();
        /* FALLTHRU */
    case T_WHILE:
        parseWhileStmt(p3);
        break;
    case T_DO:
        parseDoStmt(p3);
        break;
    case T_FOR:
        parseForStmt(p3);
        break;
    case T_IF:
        parseIfStmt(p1, p2, p3, p4);
        break;
    case T_SWITCH:
        parseSwitchStmt(p1);
        break;
    case T_CASE:
        parseCastStmt(p1, p2, p3, p4);
        break;
    case T_DEFAULT:
        parseDefaultStmt(p1, p2, p3, p4);
        break;
    case T_BREAK:
        if (p4)
            *p4 = 1;
        parseBreak_ContinueStmt(p2);
        break;
    case T_CONTINUE:
        parseBreak_ContinueStmt(p1);
        break;
    case T_RETURN:
        parseReturnStmt();
        break;
    case T_GOTO:
        parseGotoStmt();
        break;
    case T_ELSE:
        prError("inappropriate 'else'");
        break;
    case T_ID:
        if (peekCh() == ':') {
            tok = yylex();
            parseStmtLabel(yylval.ySym, p1, p2, p3, p4);
            break;
        }
        /* FALLTHRU */
    default:
        ungetTok = tok;
        var3     = parseExpr(T_EROOT, sub_0bfc(), 0); /* dummy 3rd arg added */
        emitCast(var3);
        freeExpr(var3);
        expect(T_SEMI, ";");
        break;
    }
}

/**************************************************
 * 86: 4300 PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseCompoundStmt(int16_t p1, int16_t p2, case_t *p3, int16_t *p4) {
    bool haveDecl;
    uint8_t tok;

    haveDecl = (tok = yylex()) == S_CLASS || tok == S_TYPE ||
               (tok == T_ID && yylval.ySym->sclass == T_TYPEDEF);
    if (haveDecl) {
        ungetTok = tok;
        enterScope();
        sub_5c19(T_AUTO);
        tok = yylex();
    }
    while (tok != T_RBRACE) {
        ungetTok = tok;
        parseStmt(p1, p2, p3, p4);
        tok = yylex();
    }
    if (haveDecl)
        exitScope();
}

/**************************************************
 * 87: 4390 PMO +++
 * use of uint8_t and the code fix
 **************************************************/
void parseAsmStmt(void) {
    uint8_t tok;
    if ((tok = yylex()) != T_LPAREN) {
        expectErr("(");
        ungetTok = tok;
    }
    if ((tok = yylex()) != T_SCONST) {
        expectErr("string");
        ungetTok = tok;
    } else {
        printf(";; %s\n", yylval.yStr);
        free(yylval.yStr);
    }
#ifdef BUGGY
    if ((tok = yylex()) != ')') {
#else
    /* fix to check against token for ) i.e. T_RPAREN */
    if ((tok = yylex()) != T_RPAREN) {
#endif
        expectErr(")");
        ungetTok = tok;
    }
    expect(T_SEMI, ";");
}
/**************************************************
 * 88: 4406 PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseWhileStmt(case_t *p3) {
    uint8_t tok;
    int16_t continueLabel;
    int16_t breakLabel;
    int16_t loopLabel;
    register expr_t *pe;

    if ((tok = yylex()) != T_LPAREN) {
        expectErr("(");
        ungetTok = tok;
    }
    sub_4ce8(continueLabel = newTmpLabel());
    emitLocalLabelDef(loopLabel = newTmpLabel());
    pe = sub_0bfc();
    if ((tok = yylex()) != T_RPAREN) {
        expectErr(")");
        ungetTok = tok;
    }
    parseStmt(continueLabel, breakLabel = newTmpLabel(), p3, 0);
    emitLocalLabelDef(continueLabel);
    sub_4d15(loopLabel, pe, 1);
    emitLocalLabelDef(breakLabel);
    unreachable = false;
}

/**************************************************
 * 89: 44AF PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseDoStmt(case_t *p3) {
    uint8_t tok;
    int16_t continueLabel;
    int16_t breakLabel;
    int16_t loopLabel;
    register expr_t *pe;

    continueLabel = newTmpLabel();
    breakLabel    = newTmpLabel();
    emitLocalLabelDef(loopLabel = newTmpLabel());
    parseStmt(continueLabel, breakLabel, p3, 0);
    emitLocalLabelDef(continueLabel);
    if ((tok = yylex()) != T_WHILE)
        expectErr("while");
    if (tok == T_WHILE || tok == T_FOR)
        tok = yylex();
    else if (tok != T_LPAREN) {
        skipStmt(tok);
        return;
    }
    if (tok != T_LPAREN) {
        expectErr("(");
        ungetTok = tok;
    }
    pe = sub_0bfc();
    expect(T_RPAREN, ")");
    sub_4d15(loopLabel, pe, 1);
    emitLocalLabelDef(breakLabel);
    unreachable = false;
    if ((tok = yylex()) != T_SEMI)
        expectErr(";");
}

/**************************************************
 * 90: 4595 PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseIfStmt(int16_t p1, int16_t p2, case_t *p3, int16_t *p4) {
    uint8_t tok;
    uint16_t endElseLabel;
    uint16_t endIfLabel;
    uint8_t endifUnreachable;
    register expr_t *pe;

    if ((tok = yylex()) != T_LPAREN) {
        expectErr("(");
        ungetTok = tok;
    }
    pe = sub_0bfc();
    if ((tok = yylex()) != T_RPAREN) {
        expectErr(")");
        ungetTok = tok;
    }
    endIfLabel = newTmpLabel();
    sub_4d15(endIfLabel, pe, 0);
    parseStmt(p1, p2, p3, p4);
    endifUnreachable = unreachable;
    unreachable      = false;
    if ((tok = yylex()) == T_ELSE) {
        sub_4ce8(endElseLabel = newTmpLabel());
        emitLocalLabelDef(endIfLabel);
        parseStmt(p1, p2, p3, p4);
        unreachable = unreachable && endifUnreachable;
        emitLocalLabelDef(endElseLabel);
    } else {
        ungetTok = tok;
        emitLocalLabelDef(endIfLabel);
    }
}

/**************************************************
 * 91: 469B PMO +++
 * trivial optimiser differences, use of uint8_t param
 * and addition of dummy paramaters
 **************************************************/
void parseSwitchStmt(int16_t p1) {
    uint8_t tok;
    int16_t endLabel;
    int16_t var5;
    int16_t haveBreak;
    int16_t cnt;
    case_t caseInfo;
    register attr_t *ps;

    if ((tok = yylex()) != T_LPAREN) {
        expectErr("(");
        ungetTok = tok;
    }
    haveBreak         = 0;
    caseInfo.defLabel = 0;
    caseInfo.caseCnt  = 0;
    if ((caseInfo.switchExpr = parseExpr(T_EROOT, sub_0bfc(), 0))) {
        ps = &caseInfo.switchExpr->attr;
        if (!isVarOfType(ps, DT_ENUM) && (!isIntType(ps) || ps->dataType >= DT_LONG))
            prError("illegal type for switch expression");
    }

    if ((tok = yylex()) != T_RPAREN) {
        expectErr(")");
        ungetTok = tok;
    }
    endLabel = newTmpLabel();
    var5     = newTmpLabel();
    sub_4ce8(var5);
    unreachable = true;
    parseStmt(p1, endLabel, &caseInfo, &haveBreak);
    if (caseInfo.defLabel == 0) {
        caseInfo.defLabel = endLabel;
        haveBreak         = true;
    }
    sub_4ce8(endLabel);
    emitLocalLabelDef(var5);
    emitCase(&caseInfo);
    freeExpr(caseInfo.switchExpr);
    cnt = caseInfo.caseCnt;
    while (--cnt >= 0)
        freeExpr(caseInfo.caseOptions[cnt].caseVal);
    emitLocalLabelDef(endLabel);
    unreachable = !haveBreak;
}

/**************************************************
 * 92: 4838 PMO +++
 * trivial optimiser differences, use of uint8_t param
 * and addition of dummy paramaters
 **************************************************/
void parseForStmt(case_t *p1) {
    int16_t continueLabel;
    int16_t breakLabel;
    int16_t bodyLabel;
    int16_t condLabel;
    int16_t haveCond;
    uint8_t tok;
    expr_t *condExpr;
    expr_t *stepExpr;
    register expr_t *st;

    haveCond = false;
    tok      = yylex();
    if (tok != T_LPAREN)
        expectErr("(");

    if ((tok = yylex()) != T_SEMI) {
        ungetTok = tok;
        st       = parseExpr(T_EROOT, sub_0bfc(), 0);
        emitCast(st);
        freeExpr(st);
        expect(T_SEMI, ";");
    }
    if ((tok = yylex()) != T_SEMI) {
        haveCond = true;
        ungetTok = tok;
        condExpr = sub_0bfc();
        expect(T_SEMI, ";");
    } else
        condExpr = NULL;
    if ((tok = yylex()) != T_RPAREN) {
        ungetTok = tok;
        stepExpr = parseExpr(T_EROOT, sub_0bfc(), 0);
        tok      = yylex();
        if (tok != T_RPAREN) {
            expectErr(")");
            ungetTok = tok;
        }
    } else
        stepExpr = NULL;
    bodyLabel     = newTmpLabel();
    breakLabel    = newTmpLabel();
    continueLabel = newTmpLabel();
    if (condExpr)
        sub_4ce8(condLabel = newTmpLabel());
    emitLocalLabelDef(bodyLabel);
    parseStmt(continueLabel, breakLabel, p1, &haveCond);
    emitLocalLabelDef(continueLabel);
    if (stepExpr) {
        emitCast(stepExpr);
        freeExpr(stepExpr);
    }
    if (condExpr) {
        emitLocalLabelDef(condLabel);
        sub_4d15(bodyLabel, condExpr, 1);
    } else
        sub_4ce8(bodyLabel);
    emitLocalLabelDef(breakLabel);
    unreachable = !haveCond;
}

/**************************************************
 * 93: 49E1 PMO +++
 * trivial optimiser differences
 **************************************************/
void parseBreak_ContinueStmt(int16_t label) {
    uint8_t tok;
    if (label) {
        sub_4ce8(label);
        unreachable = true;
    } else
        prError("inappropriate break/continue");
    if ((tok = yylex()) != T_SEMI)
        expectErr(";");
}

/**************************************************
 * 94: 4A1E PMO +++
 **************************************************/
void parseDefaultStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4) {
    uint8_t tok;

    if ((tok = yylex()) != T_COLON)
        expectErr(":");
    if (p3)
        if (p3->defLabel)
            prError("default case redefined");
        else
            emitLocalLabelDef(p3->defLabel = newTmpLabel());
    else
        prError("'default' not in switch");
    unreachable = false;
    parseStmt(p1, p2, p3, p4);
}

/**************************************************
 * 95: 4A90 PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseCastStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4) {
    uint8_t tok;
    expr_t *var3;
    int16_t caseLabel;
    int16_t caseIdx;
    s4_t *var9;

    var3 = parseConstExpr(1);
    if ((tok = yylex()) != T_COLON) {
        expectErr(":");
        ungetTok = tok;
    }
    emitLocalLabelDef(caseLabel = newTmpLabel());
    if (p3) {
        if ((caseIdx = p3->caseCnt++) == 255)
            fatalErr("Too many cases in switch");
        var9            = &p3->caseOptions[caseIdx];
        var9->caseLabel = caseLabel;
        if (var3 && p3->switchExpr) {
            var3          = parseExpr(0x7d, newSTypeLeaf(&p3->switchExpr->attr), var3);
            var9->caseVal = var3->t_rhs;
            var3->t_rhs   = NULL;
            freeExpr(var3);
        }
    } else
        prError("'case' not in switch");
    unreachable = false;
    parseStmt(p1, p2, p3, p4);
}

/**************************************************
 * 96: 4BAA PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseReturnStmt(void) {
    uint8_t tok;
    if ((tok = yylex()) != T_SEMI) {
        ungetTok = tok;
        sub_4d67(parseExpr(T_EROOT, sub_0bfc(), 0));
        if (yylex() != T_SEMI) {
            expectErr(";");
            ungetTok = tok;
        }
    } else if (!voidReturn)
        prWarning("non-void function returns no value");
    sub_4ce8(returnLabel);
    unreachable = true;
}

/**************************************************
 * 97: 4C03 PMO +++
 * trivial optimiser differences and  use of uint8_t param
 **************************************************/
void parseGotoStmt(void) {
    uint8_t tok;
    register sym_t *ps;

    if ((tok = yylex()) != T_ID)
        expectErr("label identifier");
    else {
        ps = sub_4ca4(yylval.ySym);
        if (ps) {
            sub_4ce8(ps->a_labelId);
            ps->flags |= 2;
        }
        unreachable = true;
        tok         = yylex();
        if (tok != T_SEMI)
            expectErr(";");
    }
}

/**************************************************
 * 98: 4C57 PMO +++
 **************************************************/
void parseStmtLabel(register sym_t *ps, int16_t p1, int16_t p2, case_t *p3, int16_t *p4) {
    ps = sub_4ca4(ps);
    if (ps) {
        emitLocalLabelDef(ps->a_labelId);
        ps->flags |= 1;
    }
    unreachable = false;
    parseStmt(p1, p2, p3, p4);
}

/**************************************************
 * 99: 4CA4 PMO +++
 * difference due to two additional dummy parameters
 **************************************************/
sym_t *sub_4ca4(register sym_t *ps) {
    if (ps->sclass) {
        if (ps->sclass != D_LABEL) {
            prError("not a label identifier: %s", ps->nVName);
            return NULL;
        }
    } else {
        ps        = sub_4eed(ps, D_LABEL, 0, 0);
        ps->level = 1;
    }
    return ps;
}

/**************************************************
 * 100: 4CE8 PMO +++
 * differences due to dummy parameter and
 * use of uint8_t param
 **************************************************/
void sub_4ce8(int16_t n) {
    register expr_t *st;
    st = parseExpr(P1_COND, newIConstLeaf(n), 0);
    emitCast(st);
    freeExpr(st);
}

/**************************************************
 * 101: 4D15 PMO +++
 * differences due to dummy parameter and
 * use of uint8_t param
 **************************************************/
void sub_4d15(int16_t n, register expr_t *st, char c) {

    if (st) {
        if (c == 0)
            st = parseExpr(T_LNOT, st, 0);

        st = parseExpr(T_123, st, newIConstLeaf(n));
        emitCast(st);
        freeExpr(st);
    }
}

/**************************************************
 * 102: 4D67 PMO +++
 * differences due to dummy parameter and
 * use of uint8_t param
 **************************************************/
void sub_4d67(register expr_t *st) {

    if (st) {
        st = parseExpr(T_121, st, 0);
        emitCast(st);
        freeExpr(st);
    }
}

sym_t **lookup(char *buf);
sym_t *symAlloc(char *s);
void reduceNodeRef(register sym_t *pn);
void freeArgs(register args_t *st);

/**************************************************
 * 103: 4D92 PMO +++
 **************************************************/
void sub_4d92(void) {

    symFreeList = tmpSyms = 0;
    hashtab               = xalloc(HASHTABSIZE * sizeof(hashtab[0]));
}

/**************************************************
 * 104: 4DA7 PMO +++
 **************************************************/
sym_t **lookup(char *buf) {
    sym_t **ps;
    char *s;
    uint16_t crc;
    uint8_t sclass;
    register sym_t *cp;

    for (crc = 0, s = buf; *s; s++)
        crc += crc + *(uint8_t *)s;
    for (ps = &hashtab[crc % 271]; (cp = *ps); ps = &cp->symList) {
        if (buf && strcmp(cp->nVName, buf) == 0) {
            if (((byte_8f85 == ((sclass = cp->sclass) == D_STRUCT || sclass == D_UNION)) &&
                 lexMember == (sclass == D_MEMBER)) ||
                sclass == 0)
                break;
        }
    }
    return ps;
}

/**************************************************
 * 105: 4E90 PMO +++
 **************************************************/
sym_t *sub_4e90(register char *buf) {
    sym_t **ps = lookup(buf);
    if (*ps == 0)
        *ps = symAlloc(buf);
    if (crfFp && buf)
        fprintf(crfFp, "%s %d\n", buf, lineNo);
    return *ps;
}

/**************************************************
 * 106: 4EED PMO +++
 **************************************************/
sym_t *sub_4eed(register sym_t *st, uint8_t p2, attr_t *p3, sym_t *p4) {
    sym_t **ppSym;
    char *var4;
    int16_t var6;
    if (st->sclass) {
        if (depth == st->level &&
            (p2 != D_MEMBER || (st->sclass == D_MEMBER && st->memberList == p4))) {
            var4 = 0;
            if (p2 != st->sclass)
                var4 = "storage class";
            else if (st->flags & 0x10) {
                if (!haveSameDataType(p3, &st->attr))
                    var4 = "type";
                if (p3->nodeType == FUNCNODE) {
                    if (p3->pFargs && !st->attr.pFargs)
                        var4 = "arguments";
                    else if (p3->pFargs) {
                        if (p3->pFargs->cnt != st->attr.pFargs->cnt)
                            var4 = "no. of arguments";
                        else {
                            var6 = p3->pFargs->cnt;
                            while (var6--) {
                                if (!haveSameDataType(&p3->pFargs->argVec[var6],
                                                      &st->attr.pFargs->argVec[var6])) {
                                    var4 = "arguments";
                                    break;
                                }
                            }
                        }
                    }
                }
            } /* 4fea */
            if (var4)
                prError("%s: %s redeclared", st->nVName, var4);
            else if (p3->nodeType == EXPRNODE && p3->pExpr && p3->pExpr != st->attr.pExpr) {
                freeExpr(st->attr.pExpr);
                st->attr.pExpr = p3->pExpr;
            } else if (p3->nodeType == FUNCNODE) {
                if (!st->attr.pFargs)
                    st->attr.pFargs = p3->pFargs;
                else if (p3->pFargs && p3->pFargs != st->attr.pFargs) {
                    freeArgs(st->attr.pFargs);
                    st->attr.pFargs = p3->pFargs;
                }
            } /* 50d1 */
            return st;
        } /* 50d7 */
        ppSym             = lookup(st->nVName);
        *ppSym            = symAlloc(st->nVName);
        (*ppSym)->symList = st;
        st                = *ppSym;
    } /* 5116 */
    switch (st->sclass = p2) {
    case DT_USHORT:
    case DT_INT:
    case DT_UINT:
        st->attr.labelId = newTmpLabel();
        return st;
    case D_CONST:
        st->flags |= 0x10;
        /* FALLTHRU */
    case DT_LONG:
        st->memberList = p4;
        break;
    case DT_ULONG:
        return st;
    case T_TYPEDEF:
        break;
    default:
        st->flags |= S_VAR;
        break;
    }
    st->attr = *p3;
    return st;
}
/**************************************************
 * 107: 516C PMO +++
 **************************************************/
void defineArg(register sym_t *st) {
    if (st && !(st->flags & S_NAMEID)) {
        if (st->flags & S_MEM)
            prError("identifier redefined: %s", st->nVName);
        st->flags |= S_MEM;
        if (crfFp && st->nVName && !(st->flags & S_NAMEID)) {
            fprintf(crfFp, "#%s %d\n", st->nVName, lineNo);
        }
    }
}

/**************************************************
 * 108: 51CF PMO +++
 **************************************************/
void sub_51cf(register sym_t *st) {
    if (st)
        st->flags |= 2;
}

/**************************************************
 * 109: 51E7 PMO +++
 * uin8_t param
 **************************************************/
void defineFuncSig(void) {
    int16_t cnt;
    int16_t i;
    args_t *argList;
    register sym_t *st;

    argList = curFuncNode->a_args;
    for (st = p25_a28f; st; st = st->memberList) {
        st->flags &= ~0x28;
        if (argList)
            st->flags |= S_ARG;
        if (st->a_nodeType == EXPRNODE) {
            freeExpr(st->a_expr);
            st->a_expr     = 0;
            st->a_nodeType = SYMNODE;
            addIndirection(&st->attr);
        } /* 523d */
        defineArg(st);
        emitVar(st);
    }
    if (!argList)
        return;
    i   = 0;
    cnt = argList->cnt;
    if (cnt == 1 && isVarOfType(argList->argVec, DT_VOID) && !p25_a28f)
        return;
    for (st = p25_a28f; st && cnt--; st = st->memberList, i++) {
        if (argList->argVec[i].dataType == DT_VARGS) {
            st  = NULL;
            cnt = 0;
            break;
        } else if (!haveSameDataType(&argList->argVec[i], &st->attr))
            break;
    }
    if (st || (cnt && argList->argVec[i].dataType != DT_VARGS))
        prError("argument list conflicts with prototype");
    cnt = 1;
}

/**************************************************
 * 110: 5356 PMO +++
 **************************************************/
bool releaseSymFreeList(void) {
    register sym_t *st;

    if (!symFreeList)
        return false;

    while ((st = symFreeList)) {
        symFreeList = st->memberList;
        free(st);
    }
    return true;
}

/**************************************************
 * 111: 5384 PMO +++
 **************************************************/
sym_t *symAlloc(char *s) {
    register sym_t *ps;
    static int16_t symCnt = 0;

    if (symFreeList) {
        ps          = symFreeList;
        symFreeList = ps->memberList;
        blkclr(ps, sizeof(sym_t));
    } else {
        ps = xalloc(sizeof(sym_t));
    }
    ps->level   = depth;
    ps->nRefCnt = 1;
    ps->symId  = ++symCnt;
    if (s) {
        ps->nVName = (char *)xalloc(strlen(s) + 1);
        strcpy(ps->nVName, s);
    } else
        ps->nVName = blank;
    return ps;
}

/**************************************************
 * 112: 540C PMO +++
 * optimiser has better code for --pn->nRefCnt
 **************************************************/
void reduceNodeRef(register sym_t *pn) {
    /* printf("%p %d %d %s\n", pn, pn->level, pn->nRefCnt,
           pn->a_nodeType == 0   ? (*pn->nVName ? pn->nVName : "blank")
           : pn->a_nodeType == 1 ? "EXPR"
                                 : "FUNC"
                                 ); */
    if (--pn->nRefCnt == 0) {
        if (pn->sclass != 0 && pn->sclass != D_LABEL && pn->sclass != D_STRUCT &&
            pn->sclass != D_UNION && pn->sclass != D_ENUM) {
            if (pn->a_nodeType == FUNCNODE)
                freeArgs(pn->a_args);
            else if (pn->a_nodeType == EXPRNODE)
                freeExpr(pn->a_expr);
#if BUGGY
            /* this code is prone to release symbols too early */
            if (ps->a_dataType == DT_COMPLEX)
                reduceNodeRef(ps->a_nextSym);
#endif
        }
        if (pn->nVName != blank)
            free(pn->nVName);
        pn->memberList = symFreeList;
        symFreeList    = pn;
    }
}

/**************************************************
 * 113: 549C PMO +++
 * use of uint8_t param
 **************************************************/
void enterScope(void) {

    prFuncBrace(T_LBRACE);
    ++depth;
}

/**************************************************
 * 114: 54AC PMO +++
 * use of uint8_t param
 **************************************************/
void exitScope(void) {

    releaseScopeSym();
    --depth;
    prFuncBrace(T_RBRACE);
}

/**************************************************
 * 115: 54C0 PMO +++
 **************************************************/
void releaseScopeSym(void) {
    sym_t **pSlot;
    sym_t **ppSym;
    uint8_t sclass;
    char *msg;
    register sym_t *pSym;

    for (pSlot = hashtab; pSlot < &hashtab[HASHTABSIZE]; pSlot++) {
        ppSym = pSlot;
        while ((pSym = *ppSym)) {
            if (pSym->level == depth) {
                msg    = 0;
                sclass = pSym->sclass;
                if ((pSym->flags & 3) == 2) {
                    switch (sclass) {
                    case D_LABEL:
                        msg = "label";
                        break;
                    case D_STRUCT:
                    case D_UNION:
                    case T_EXTERN:
                        break;
                    default:
                        msg = "variable";
                        break;
                    }
                    if (msg)
                        prError("undefined %s: %s", msg, pSym->nVName);
                } else if ((depth || sclass == T_STATIC) && !(pSym->flags & 2)) { /* 5555  */
                    switch (sclass) {
                    case D_LABEL:
                        msg = "label";
                        break;
                    case D_STRUCT:
                        msg = "structure";
                        break;
                    case D_UNION:
                        msg = "union";
                        break;
                    case D_MEMBER:
                        msg = "member";
                        break;
                    case D_ENUM:
                        msg = "enum";
                        break;
                    case D_CONST:
                        msg = "constant";
                        break;
                    case T_TYPEDEF:
                        msg = "typedef";
                        break;
                    case D_STACK:
                        msg = 0;
                        break;
                    default:
                        if (sclass) {
                            if (pSym->flags & S_MEM)
                                msg = "variable definition";
                            else
                                msg = "variable declaration";
                        }
                        break;
                    }
                    if (msg)
                        prWarning("unused %s: %s", msg, pSym->nVName);

                } /* 55d2 */
                *ppSym = pSym->symList;
                reduceNodeRef(pSym);
            } else
                ppSym = &pSym->symList;
        }
    }
    ppSym = &tmpSyms;
    while ((pSym = *ppSym)) {
        if (pSym->level == depth) {
            *ppSym = pSym->symList; /* remove from list and reduce its ref count */
            reduceNodeRef(pSym);
        } else
            ppSym = &pSym->symList; /* skip to next entry */
    }
}

/**************************************************
 * 116: 56A4 PMO +++
 **************************************************/
sym_t *newTmpSym(void) {
    register sym_t *st;

    st = symAlloc(0);
    st->flags |= S_NAMEID + 2 + S_MEM;
    st->symList = tmpSyms;
    tmpSyms     = st;
    return st;
}

/**************************************************
 * 117: 56CD PMO +++
 **************************************************/
sym_t *findMember(sym_t *pSym, char *name) {
    register sym_t *ps;

    for (ps = pSym->memberList; pSym != ps; ps = ps->memberList)
        if (strcmp(ps->nVName, name) == 0)
            return ps;
    prError("%s is not a member of the struct/union %s", name, pSym->nVName);
    return NULL;
}

/**************************************************
 * 118: 573B PMO +++
 **************************************************/
void emitSymName(register sym_t *st, FILE *fp) {

    if (st) {
        if (st->flags & S_NAMEID)
            fprintf(fp, "F%d", st->symId);
        else
            fprintf(fp, "_%s", st->nVName);
    }
}

/**************************************************
 * 119: 5785 PMO +++
 **************************************************/
int16_t newTmpLabel(void) {

    return ++tmpLabelId;
}

/**************************************************
 * 120: 578D PMO +++
 * trivial optimiser differences
 **************************************************/
args_t *cloneArgs(register args_t *pArgs) {
    args_t *newList;
    attr_t *pAttr;
    int16_t i;
    if (!pArgs)
        return pArgs;
    newList = xalloc(sizeof(args_t) + (pArgs->cnt - 1) * sizeof(attr_t));
    i = newList->cnt = pArgs->cnt;
    while (i--) {
        pAttr  = &newList->argVec[i];
        *pAttr = pArgs->argVec[i];
        if (pAttr->dataType == DT_COMPLEX)
            pAttr->nextSym->nRefCnt++;
    }
    return newList;
}

/**************************************************
 * 121: 583A PMO +++
 **************************************************/
void freeArgs(register args_t *pArgs) {
    attr_t *pAttr;
    if (pArgs) {
        for (pAttr = pArgs->argVec; pArgs->cnt--; pAttr++) {
            if (pAttr->nodeType == FUNCNODE && pAttr->pFargs)
                freeArgs(pAttr->pFargs);
            if (pAttr->dataType == DT_COMPLEX)
                reduceNodeRef(pAttr->nextSym);
        }
        free(pArgs);
    }
}

/**************************************************
 * 122: 58BD PMO +++
 **************************************************/
void cloneAttr(register attr_t *st, attr_t *p2) {
    *p2 = *st;
    if (p2->nodeType == EXPRNODE)
        p2->pExpr = cloneExpr(p2->pExpr);
    else if (p2->nodeType == FUNCNODE)
        p2->pFargs = cloneArgs(p2->pFargs);
}

/**************************************************
 * 123: 591D PMO +++
 * some optimiser differences including movement
 * of some basic blocks
 **************************************************/
bool haveSameDataType(register attr_t *st, attr_t *p2) {
    int16_t var2;

    if (st == p2)
        return true;
    if (st->nodeType != p2->nodeType || st->dataType != p2->dataType ||
        st->indirection != p2->indirection)
        return false;
    switch (st->dataType) {
    case DT_ENUM:
    case DT_STRUCT:
    case DT_UNION:
        return st->nextSym == p2->nextSym;
    case DT_COMPLEX:
        return haveSameDataType(st->nextAttr, p2->nextAttr);
    }
    if (st->nodeType != FUNCNODE || !st->pFargs || !p2->pFargs)
        return true;
    if (st->pFargs->cnt != p2->pFargs->cnt)
        return false;
    var2 = st->pFargs->cnt;
    do {
        if (var2-- == 0)
            return true;
    } while (haveSameDataType(&st->pFargs->argVec[var2], &p2->pFargs->argVec[var2]));
    return false;
}

/**************************************************
 * 124: 5A4A PMO +++
 **************************************************/
bool isVoidStar(register attr_t *st) {
    return st->dataType == DT_VOID && st->indirection == 1;
}

/**************************************************
 * 125: 5A76 PMO +++
 **************************************************/
bool isVarOfType(register attr_t *st, uint8_t p2) {

    return st->dataType == p2 && st->indirection == 0 && st->nodeType == SYMNODE;
}

/**************************************************
 * 126: 5AA4 PMO +++
 **************************************************/
bool isLogicalType(register attr_t *st) {
    return st->nodeType == SYMNODE &&
           (((st->indirection & 1) && st->nodeType == SYMNODE) || st->dataType < DT_VOID);
}

/**************************************************
 * 127: 5AD5 PMO +++
 **************************************************/
bool isSimpleType(register attr_t *st) {
    return st->nodeType == SYMNODE && st->indirection == 0 && st->dataType <= DT_ENUM;
}

/**************************************************
 * 128: 5B08 PMO +++
 **************************************************/
bool isIntType(register attr_t *st) {
    return (st->nodeType == SYMNODE && st->indirection == 0 && st->dataType < DT_FLOAT);
}

/**************************************************
 * 129: 5B38 PMO +++
 **************************************************/
bool isFloatType(register attr_t *st) {
    return st->nodeType == SYMNODE && st->indirection == 0 &&
           (st->dataType == DT_FLOAT || st->dataType == DT_DOUBLE);
}

/**************************************************
 * 130: 5B69 PMO +++
 **************************************************/
bool isValidIndex(register attr_t *st) {

    return isIntType(st) || isVarOfType(st, DT_ENUM);
}

/**************************************************
 * 131: 5B99 PMO +++
 * uint8_t parameter
 **************************************************/
void delIndirection(register attr_t *st) {
    if (st->nodeType == FUNCNODE)
        st->nodeType = SYMNODE;
    else
        st->indirection >>= 1;

    if (isVarOfType(st, DT_COMPLEX))
        *st = *(st->nextAttr);
}

void sub_5c50(void);
void sub_6531(register sym_t *st);
args_t *parseArgs(uint16_t p1);
void parseDeclaration(uint8_t p1);
uint16_t normaliseIndirection(uint16_t n);

/**************************************************
 * 132: 5BE1 PMO +++
 **************************************************/
void addIndirection(register attr_t *st) {
    uint16_t ch;
    ch = st->indirection;
    if (ch & 0x8000)
        prError("too much indirection");
    st->indirection = (ch << 1) | 1;
}

/**************************************************
 * 133: 5C19 PMO +++
 **************************************************/
void sub_5c19(uint8_t sclass) {
    register sym_t *st; /* may not be needed */
    uint8_t tok;

    defSClass = sclass;
    for (;;) {
        ungetTok = tok = yylex();
        if (tok == S_CLASS || tok == S_TYPE ||
            (tok == T_ID && (st = yylval.ySym)->sclass == T_TYPEDEF))
            sub_5c50();
        else
            break;
    }
    defSClass = 0;
}

/**************************************************
 * 134: 5C50 PMO +++
 * trivial optimiser differences, use of uint8_t arg
 * and dummy arg
 **************************************************/
void sub_5c50(void) {
    uint8_t scType;
    attr_t attr;
    uint8_t tok;
    uint8_t scFlags;
    uint8_t varc;
    bool vard;
    register sym_t *st;

    scFlags = sub_5dd1(&scType, &attr);
    if (scType != D_STACK && scType != T_REGISTER && defSClass == D_STACK) {
        prError("only register storage class allowed");
        scType = D_STACK;
    }
    if ((tok = yylex()) == T_SEMI)
        return;
    ungetTok = tok;
    for (;;) {
        st   = sub_69ca(scType, &attr, scFlags & ~1, 0); /* dummy last param */
        vard = st && (st->flags & S_VAR) && st->attr.nodeType == FUNCNODE;
        varc = (scFlags & 1) && scType != D_STACK && !vard;
        if ((tok = yylex()) == T_EQ) {
            if (vard || scType == D_STACK || scType == T_EXTERN || scType == T_TYPEDEF)
                prError("illegal initialisation");
            if (scType == T_STATIC || scType == T_EXTERN) {
                defineArg(st);
                if (depth && scType == T_STATIC)
                    st->flags |= S_NAMEID;
                emitVar(st);
                sub_3c7e(st);
            } else {
                defineArg(st);
                emitVar(st);
                sub_6531(st);
            }
            tok = yylex();
        } else if (scType != D_STACK) {
            if (varc)
                defineArg(st);
            if (depth && scType == T_STATIC)
                st->flags |= S_NAMEID;
            emitVar(st);
        } /* 5d95 */
        if (tok == T_ID || tok == T_STAR) {
            expectErr(",");
            ungetTok = tok;
        } else if (tok != T_COMMA) {
            if (tok != T_SEMI) {
                expectErr(";");
                skipStmt(tok);
            }
            return;
        }
    }
}

/**************************************************
 * 135: 5DD1 PMO +++
 * use of uint8_t arg
 * some optimisations different some better some worse
 **************************************************/
uint8_t sub_5dd1(uint8_t *pscType, register attr_t *attr) {
    uint8_t scType;
    uint8_t dataType;
    int16_t sizeIndicator; /* -1 short, 0 int, 1 long */
    bool isUnsigned;
    uint8_t tok;
    uint8_t scFlags; /* storage class */
    sym_t *var9;
    sym_t *ps;

    attr->indirection = 0;
    attr->pExpr      = 0;
    attr->nextSym   = NULL;
    attr->nodeType    = 0;
    scType = dataType = 0;
    sizeIndicator     = 0;
    isUnsigned        = false;
    scFlags           = 0;

    for (;;) {
        if ((tok = yylex()) == S_CLASS) {
            if (pscType == NULL)
                prError("storage class illegal");
            else {
                switch (tok = yylval.yVal) {
                case T_REGISTER:
                    scFlags |= S_REG;
                    break;
                case T_AUTO:
                    if (!depth)
                        prError("bad storage class");
                    /* FALLTHRU */
                case T_STATIC:
                case T_TYPEDEF:
                    scFlags |= S_MEM;
                    /* FALLTHRU */
                case T_EXTERN:
                    if (scType && scType != tok)
                        prError("inconsistent storage class");
                    else
                        scType = tok;
                    break;
                }
            }
        } else if (tok == S_TYPE) { /* 5e78 */
            switch (tok = yylval.yVal) {
            case T_SHORT:
                sizeIndicator--;
                break;
            case T_LONG:
                sizeIndicator++;
                break;
            case T_UNSIGNED:
                isUnsigned = true;
                break;
            case T_UNION:
                dataType        = DT_UNION;
                attr->nextSym = sub_60db(D_UNION);
                if (attr->nextSym)
                    sub_51cf(attr->nextSym);
                break;
            case T_STRUCT:
                dataType        = DT_STRUCT;
                attr->nextSym = sub_60db(D_STRUCT);
                if (attr->nextSym)
                    sub_51cf(attr->nextSym);
                break;
            case T_ENUM:
                dataType        = DT_ENUM;
                attr->nextSym = sub_6360();
                sub_51cf(attr->nextSym);
                break;
            case T_CHAR:
            case T_DOUBLE:
            case T_FLOAT:
            case T_INT:
            case T_VOID:
                if (dataType)
                    prError("inconsistent type");
                else
                    dataType = tok == T_INT     ? DT_INT
                               : tok == T_CHAR  ? DT_CHAR
                               : tok == T_VOID  ? DT_VOID
                               : tok == T_FLOAT ? DT_FLOAT
                                                : DT_DOUBLE;
                break;
            }
        } else if (tok == T_ID && yylval.ySym->sclass == T_TYPEDEF && dataType == 0) { /* 5f68 */
            ps = yylval.ySym;
            sub_51cf(ps);
            var9 = ps;
            if (var9->a_nodeType) {
                dataType        = DT_COMPLEX;
                attr->nextSym = ps;
            } else {
                dataType          = var9->a_dataType;
                attr->u1          = var9->attr.u1;
                attr->u2          = var9->attr.u2;
                attr->indirection = var9->attr.indirection;
            }
        } else
            break;

    } /* 6003 */
    ungetTok = tok;
    if (scType == 0) {
        scType = depth ? defSClass : T_EXTERN;
        scFlags |= S_MEM;
    }
    if ((scFlags & S_REG) && scType != T_AUTO && scType != D_STACK && scType != D_14 &&
        scType != D_15)
        prError("can't be a register");
    if (dataType == 0)
        dataType = DT_INT;
    if (sizeIndicator > 0) {
        if (dataType == DT_FLOAT || dataType == DT_INT)
            dataType += 2; /* to DT_DOUBLE or DT_LONG*/
        else
            prError("can't be long");
    } else if (sizeIndicator < 0) {
        if (dataType == DT_INT)
            dataType = DT_SHORT;
        else
            prError("can't be short");
    }
    if (isUnsigned) {
        if (dataType < DT_FLOAT)
            dataType |= 1;
        else
            prError("can't be unsigned");
    }
    if (pscType)
        *pscType = scType;
    attr->dataType = dataType;
    return scFlags;
}

/**************************************************
 * 136: 60DB PMO +++
 * differences due to dummy args, use of uint8_t args
 * and equivalent optimiser differences.
 **************************************************/
sym_t *sub_60db(uint8_t p1) {
    sym_t *pSym;
    sym_t **ppMembers;
    uint8_t tok;
    int16_t id;
    attr_t attr;
    register sym_t *st;

    byte_8f85 = true;
    tok       = yylex();
    byte_8f85 = false;
    if (tok == T_ID) {
        st = yylval.ySym;
        if (st->sclass != p1)
            st = sub_4eed(st, p1, 0, 0);
        tok = yylex();
    } else {
        st = sub_4eed(newTmpSym(), p1, 0, 0);
        if (tok != T_LBRACE)
            expectErr("struct/union tag or '{'");
    }
    ppMembers = NULL;
    if (tok == T_LBRACE) {
        if ((st->flags & (S_NAMEID | S_MEM)) == S_MEM)
            prError("struct/union redefined: %s", st->nVName);
        else
            ppMembers = &st->memberList;
        id = 0;
        for (;;) {
            sub_5dd1(0, &attr);
            lexMember = true;
            do {
                pSym = sub_69ca(D_MEMBER, &attr, 0, st);
                if (pSym) {
                    if (pSym->attr.nodeType == FUNCNODE)
                        prError("members cannot be functions");
                    defineArg(pSym);
                    if (ppMembers) {
                        *ppMembers = pSym;
                        ppMembers  = &pSym->memberList;
                    }
                    pSym->memberId = id++;
                }
                if ((tok = yylex()) == T_COLON) {
                    if (!(pSym->attr.dataType & DT_UNSIGNED))
                        pSym->attr.dataType |= DT_UNSIGNED;
                    if (!isVarOfType(&pSym->attr, DT_UINT))
                        prError("bad bitfield type");
                    if ((tok = yylex()) != T_ICONST)
                        prError("integer constant expected");
                    else {
                        if (!pSym) {
                            defineArg(pSym = sub_4eed(newTmpSym(), D_MEMBER, &attr, st));
                            if (ppMembers) {
                                *ppMembers = pSym;
                                ppMembers  = &pSym->memberList;
                            }
                            pSym->memberId = id++;
                        } /* 62ce */
                        pSym->flags |= S_BITFIELD;
                        pSym->bwidth = (int16_t)yylval.yNum;
                        tok          = yylex();
                    }
                }
            } while (tok == T_COMMA); /* 62f2 */
            lexMember = false;
            if (tok != T_SEMI)
                expectErr(";");
            if ((tok = yylex()) == T_RBRACE) {
                if (!(st->flags & S_MEM))
                    defineArg(st);
                if (ppMembers) {
                    *ppMembers = st;
                    emitStructUnion(st, p1);
                }
                return st;
            }
            ungetTok = tok;
        }
    }
    ungetTok = tok;
    return st;
}
/**************************************************
 * 137: 6360 PMO +++
 * differences due to dummy and uint8_t args
 **************************************************/
sym_t *sub_6360(void) {
    attr_t var8;
    sym_t *vara;
    expr_t *varc;
    int16_t vare;
    uint8_t tok;
    register sym_t *st;

    if ((tok = yylex()) == T_ID) {
        st = yylval.ySym;
        if ((tok = yylex()) != T_LBRACE) {
            if (!(st->flags & S_MEM))
                prError("undefined enum tab: %s", st->nVName);
            ungetTok = tok;
        }
    } else if (tok == T_LBRACE)
        st = newTmpSym();
    else {
        expectErr("enum tag or {");
        st = NULL;
    }
    if (tok == T_LBRACE) {
        defineArg(st = sub_4eed(st, D_ENUM, 0, 0));
        var8.dataType    = DT_ENUM;
        var8.nextSym   = st;
        var8.indirection = 0;
        var8.pExpr      = NULL;
        var8.nodeType    = SYMNODE;
        printf("[c ");
        emitSymName(st, stdout);
        putchar('\n');
        vare = 0;
        varc = newIntLeaf(0, DT_INT);
        for (;;) {
            if ((tok = yylex()) != T_ID) {
                expectErr("identifier");
                break;
            } else { /* 6474 */
                if ((vara = sub_4eed(yylval.ySym, DT_CONST, &var8, st)))
                    vara->memberId = vare++;
                if ((tok = yylex()) == T_EQ) {
                    freeExpr(varc);
                    parseConstExpr(T_LBRACE);
                    if (!isIntType(&varc->attr) || varc->attr.dataType >= DT_LONG)
                        prError("integer expression required");
                    tok = yylex();
                }
                defineArg(vara);
                sub_05d3(varc);
                if (tok != T_COMMA)
                    break;
                varc = sub_25f7(varc);
            }
        }
        printf(".. ]\n");
        freeExpr(varc);
        if (tok != T_RBRACE) {
            expectErr("}");
            skipStmt(tok);
        }
    }
    return st;
}
/**************************************************
 * 138: 6531 PMO +++
 * differences due to dummy and uint8_t args
 **************************************************/
void sub_6531(register sym_t *st) {
    expr_t *var2;
    uint8_t tok;
    bool var4;

    if (st && !isLogicalType(&st->attr)) {
        prError("can't initialise auto aggregates");
        skipToSemi();
    } else {
        if (!(var4 = (tok = yylex()) == T_LBRACE))
            ungetTok = tok;
        if ((var2 = parseExpr(T_EROOT, sub_07f5(T_RBRACE), 0)) && st) {
            var2 = parseExpr(T_EQ, newIdLeaf(st), var2);
            emitCast(var2);
            freeExpr(var2);
        }
        if (var4 && yylex() != T_RBRACE)
            expectErr("}");
    }
}

/**************************************************
 * 139: 65E2 PMO +++
 * equivalent optimiser differences including basic
 * block moves
 * differences due to dummy and uint8_t args
 **************************************************/
args_t *parseArgs(uint16_t p1) {
    uint8_t scType;
    attr_t attr;
    attr_t *pAttr;
    uint8_t tok;
    sym_t *vare FORCEINIT;
    uint8_t scFlags;
    uint8_t var10;
    bool protoArg;
    bool nonProtoArg;
    attr_t var1a;
    int16_t i;
    struct {
        int16_t cnt;
        attr_t argVec[128];
    } args;
    register sym_t *st;

    var10             = defSClass;
    defSClass         = D_15;
    args.cnt          = 0;
    nonProtoArg       = false;
    protoArg          = false;
    var1a.dataType    = DT_INT;
    var1a.indirection = 0;
    var1a.pExpr      = 0;
    var1a.nodeType    = 0;
    for (;;) { /* 6619 */
        if ((tok = yylex()) == T_3DOT) {
            args.argVec[args.cnt].dataType      = DT_VARGS;
            args.argVec[args.cnt].pExpr        = 0;
            args.argVec[args.cnt].nodeType      = 0;
            args.argVec[args.cnt++].indirection = 0;
            tok                                  = yylex();
            break;
        } /* 66db */
        if (tok == T_ID && yylval.ySym->sclass != T_TYPEDEF)
            if (p1)
                nonProtoArg = true;
            else
                prError("type specifier reqd. for proto arg");
        else
            protoArg = true;
        if (protoArg && nonProtoArg) {
            nonProtoArg = false;
            prError("can't mix proto and non-proto args");
        }
        ungetTok = tok;
        scFlags  = sub_5dd1(&scType, &attr);
        if (scType != D_15 && scType != T_REGISTER)
            prError("bad storage class");
        scType = p1 ? D_15 : D_14;
        st     = sub_69ca(scType, &attr, scFlags & ~1, 0);
        pAttr  = &st->attr;
        if (pAttr->nodeType == FUNCNODE) {
            pAttr->nextSym   = sub_4eed(newTmpSym(), T_TYPEDEF, pAttr, 0);
            pAttr->dataType    = DT_COMPLEX;
            pAttr->nodeType    = 0;
            pAttr->indirection = 1;
        }
        if (protoArg)
            cloneAttr(pAttr, &args.argVec[args.cnt++]);
        scFlags &= ~1;
        scType = D_STACK;
        st     = sub_4eed(st, scType, protoArg ? pAttr : &var1a, 0);
        if (p1 && !isVarOfType(pAttr, T_AUTO)) {
            if (!p25_a28f) {
                p25_a28f = st;
                vare     = st;
                st->flags |= scFlags | S_ARGDECL;
            } else if (st->flags & S_ARGDECL) /* 6893 */
                prError("argument redeclared: %s", st->nVName);
            else {
                vare->memberList = st;
                st->flags |= scFlags | S_ARGDECL;
                vare = st;
            }
            if (protoArg)
                st->flags |= 0x208;
            st->memberList = 0;
        }
        if ((tok = yylex()) == T_EQ) {
            prError("can't initialize arg");
            skipStmt(tok);
        }
        if (tok == T_ID || tok == S_CLASS || tok == S_TYPE) {
            expectErr(",");
            ungetTok = tok;
        } else if (tok != T_COMMA)
            break;
    } /* 669c */
    defSClass = var10;
    if (tok != T_RPAREN) {
        expectErr(")");
        skipStmt(tok);
    }
    if (args.cnt == 0)
        return 0;
    i = args.cnt;
    while (i--) {
        if (args.argVec[i].nodeType == EXPRNODE) {
            args.argVec[i].nodeType = SYMNODE;
            freeExpr(args.argVec[i].pExpr);
            addIndirection(&args.argVec[i]);
        }
    }
    return cloneArgs((args_t *)&args);
}

/**************************************************
 * 140: 69CA PMO +++
 * minor code optimiser differences and deltas
 * due to dummy and uit8_t args
 **************************************************/
sym_t *sub_69ca(uint8_t p1, register attr_t *p2, uint8_t p3, sym_t *p4) {
    uint16_t indirection;
    sym_t *var4;
    decl_t *savDecl;
    decl_t decl;
    uint8_t tok;
    attr_t attr;

    savDecl              = curDecl;
    curDecl              = &decl;
    attr.pExpr          = NULL; /* other options */
    attr.nextSym       = NULL;
    attr.nodeType        = 0;
    attr.indirection     = 0;
    attr.dataType        = 0;
    curDecl->pAttr       = &attr;
    curDecl->indirection = 0;
    curDecl->pSym1       = NULL;
    curDecl->pSym2       = NULL;
    curDecl->uca         = false;
    curDecl->needDim     = false;
    curDecl->badInd  = false;
    curDecl->ucb         = false;
    parseDeclaration(p1);
    ungetTok = tok = yylex();
    if (curDecl->ucb) {
        if (p1 == T_CAST || p1 == D_14 || p1 == D_15 || (p1 == D_MEMBER && tok == T_COLON)) {
            curDecl->pSym1 = newTmpSym();
            if (p1 == T_CAST)
                p1 = T_TYPEDEF;
        } else
            prError("no identifier in declaration");
    } /* 6aaf */
    curDecl->pAttr->dataType = p2->dataType;
    curDecl->pAttr->u1       = p2->u1;

    for (indirection = p2->indirection; indirection; indirection >>= 1) {
        if (curDecl->indirection & 1) {
            curDecl->badInd = true;
            break;
        }
        curDecl->indirection = (curDecl->indirection >> 1) | ((indirection & 1) << 15);
    }
    /* ^^^ */
    for (;;) {
        if (curDecl->pAttr->nodeType == SYMNODE && curDecl->pAttr->dataType == DT_COMPLEX &&
            (curDecl->indirection == 0 || curDecl->pAttr->nextAttr->nodeType == SYMNODE)) {
            for (indirection = curDecl->pAttr->nextAttr->indirection; indirection;
                 indirection >>= 1) {
                if (curDecl->indirection & 1) {
                    curDecl->badInd = true;
                    break;
                }
                curDecl->indirection = (curDecl->indirection >> 1) | ((indirection & 1) << 15);
            }
            cloneAttr(curDecl->pAttr->nextAttr, curDecl->pAttr);
        } else
            break;
    } /* 6c13 vvv */
    if (curDecl->badInd)
        prError("declarator too complex");
    curDecl->pAttr->indirection = normaliseIndirection(curDecl->indirection);
    if (curDecl->pSym1 && p1 != T_TYPEDEF && p1 != D_14 && p1 != D_15 &&
        isVarOfType(curDecl->pAttr, DT_VOID) && curDecl->pAttr->nodeType != FUNCNODE)
        prError("only functions may be void");
    else if (isVarOfType(curDecl->pAttr, DT_COMPLEX)) {
        if (curDecl->pAttr->nodeType == FUNCNODE &&
            curDecl->pAttr->nextAttr->nodeType == EXPRNODE)
            prError("functions can't return arrays");
        else if (curDecl->pAttr->nodeType == EXPRNODE &&
                 curDecl->pAttr->nextAttr->nodeType == FUNCNODE)
            prError("can't have array of functions");
    }
    if (curDecl->pSym2) {
        curDecl->pSym2 = sub_4eed(curDecl->pSym2, T_TYPEDEF, &curDecl->pSym2->attr, 0);
        defineArg(curDecl->pSym2);
        sub_51cf(curDecl->pSym2);
        if (curDecl->pSym2->attr.nodeType != FUNCNODE || !(curDecl->pSym2->flags & 0x80))
            emitVar(curDecl->pSym2);
    } /* 6d95 */
    if (p1 != T_TYPEDEF && curDecl->pAttr->indirection == 0 &&
        (isVarOfType(p2, DT_STRUCT) || isVarOfType(p2, DT_UNION)) && p2->nextSym &&
        !(p2->nextSym->flags & 1))
        prError("undefined struct/union: %s", curDecl->pAttr->nextSym->nVName);
    if (curDecl->pSym1) { /* 6e0b */
        if (defSClass == D_STACK && p1 != D_MEMBER) {
            if (curDecl->pSym1->flags & 8)
                prError("argument redeclared: %s", curDecl->pSym1->nVName);
            else if (!(curDecl->pSym1->flags & S_ARGDECL))
                prError("not an argument: %s", curDecl->pSym1->nVName);
            else {
                curDecl->pSym1->flags |= p3 | 8;
                curDecl->pSym1->attr = attr;
                if (isVarOfType(&attr, DT_FLOAT)) {
                    prWarning("float param coerced to double");
                    curDecl->pSym1->attr.dataType = DT_DOUBLE;
                }
            }
        } else if (defSClass != D_14 && defSClass != D_15) { /* 6ecd */
            if (p1 == T_AUTO && attr.nodeType == FUNCNODE)
                p1 = T_EXTERN;
            if ((curDecl->pSym1 = sub_4eed(curDecl->pSym1, p1, &attr, p4)))
                curDecl->pSym1->flags |= p3;
        } else {
            if (curDecl->pSym1->sclass && curDecl->pSym1->level != depth) /* 6f39 */
                curDecl->pSym1 = sub_4eed(curDecl->pSym1, 0, &attr, 0);
            curDecl->pSym1->attr = attr;
        }
    }
    /* 6f91 */
    var4    = curDecl->pSym1;
    curDecl = savDecl;
    return var4;
}
/**************************************************
 * 141: 6FAB PMO +++
 * minor equivalent optimiser differences
 * differnces due to dummy & uint8_t args
 **************************************************/
void parseDeclaration(uint8_t p1) {
    bool savLexMember;
    uint8_t starCnt;
    uint8_t tok;
    uint8_t sclass;
    uint8_t var5;
    register expr_t *st;

    byte_a29a = false;
    sclass    = 1;
    for (starCnt = 0; (tok = yylex()) == T_STAR; starCnt++)
        ;
    if (tok == T_ID) {
        curDecl->pSym1 = yylval.ySym;
        tok            = yylex();
        sclass         = curDecl->pSym1->sclass;
        if (!curDecl->pSym1->sclass)
            curDecl->pSym1->sclass = p1;
        byte_a29a = p1 != D_14;
    } else if (tok == T_LPAREN) { /* 701b */
        ungetTok = tok = yylex();
        if (tok == T_RPAREN || tok == S_CLASS || tok == S_TYPE ||
            (tok == T_ID && yylval.ySym->sclass == T_TYPEDEF))
            tok = T_LPAREN;
        else {
            parseDeclaration(p1);
            if ((tok = yylex()) != T_RPAREN)
                expectErr(")");
            tok = yylex();
        }
    } /* 707b */
    curDecl->ucb = curDecl->pSym1 == NULL;
    for (;;) { /* 7091 */
        if (tok == T_LPAREN) {
            if (curDecl->needDim) {
                prError("can't have array of functions");
                curDecl->needDim = false;
            }
            if (curDecl->pAttr->nodeType == FUNCNODE && curDecl->indirection == 0)
                prError("functions can't return functions");
            if (curDecl->indirection & 0x8000) { /* 70e7 */
                curDecl->pAttr->dataType    = DT_COMPLEX;
                curDecl->pAttr->indirection = normaliseIndirection(curDecl->indirection);
                curDecl->indirection        = 0;
                curDecl->pAttr->nextSym   = newTmpSym();
                if (curDecl->pSym2)
                    curDecl->pSym2 = sub_4eed(curDecl->pSym2, T_TYPEDEF, &curDecl->pSym2->attr, 0);
                curDecl->pSym2 = curDecl->pAttr->nextSym;
                curDecl->pAttr = &curDecl->pSym2->attr;
            } /* 7195 */
            curDecl->pAttr->nodeType = FUNCNODE;
            byte_a29a                = byte_a29a && !depth;
            if (!byte_a29a) {
                var5 = true;
                depth++;
            } else
                var5 = false;
            /* 71cc */
            depth++;
            if ((tok = yylex()) == S_CLASS || tok == S_TYPE || tok == T_ID) {
                ungetTok               = tok;
                curDecl->pAttr->pFargs = parseArgs(byte_a29a);
            } else if (tok != T_RPAREN) {
                expectErr(")");
                skipStmt(tok);
            }
            byte_a29a = false;
            if (var5) {
                releaseScopeSym();
                depth--;
            }
            depth--;
        } else if (tok == T_LBRACK) { /* 7248 */
            if (curDecl->pAttr->nodeType == FUNCNODE)
                prError("functions can't return arrays");
            if (curDecl->uca || (curDecl->indirection & 0x8000)) {
                curDecl->uca                = false;
                curDecl->pAttr->dataType    = DT_COMPLEX;
                curDecl->pAttr->indirection = normaliseIndirection(curDecl->indirection);
                curDecl->indirection        = 0;
                curDecl->pAttr->nextSym   = newTmpSym();
                if (curDecl->pSym2)
                    curDecl->pSym2 = sub_4eed(curDecl->pSym2, T_TYPEDEF, &curDecl->pSym2->attr, 0);
                curDecl->pSym2 = curDecl->pAttr->nextSym;
                curDecl->pAttr = &curDecl->pSym2->attr;
            } /* 732a */
            savLexMember = lexMember;
            lexMember    = false;
            if ((tok = yylex()) == T_RBRACK) {
                if (curDecl->needDim)
                    prError("dimension required");
                st = &eZero;
            } else {
                ungetTok = tok;
                st       = parseConstExpr(T_SEMI);
                if ((tok = yylex()) != T_RBRACK) {
                    expectErr("]");
                    skipStmt(tok);
                }
            } /* 738a */
            curDecl->uca             = true;
            curDecl->needDim         = true;
            lexMember                = savLexMember;
            curDecl->pAttr->nodeType = EXPRNODE;
            curDecl->pAttr->pExpr   = st;
        } else { /* 73c1 */
            ungetTok = tok;
            if (!sclass)
                curDecl->pSym1->sclass = 0;
            if (!starCnt)
                return;
            curDecl->needDim = false;
            do {
                if (curDecl->indirection & 1)
                    curDecl->badInd = true;
                else
                    curDecl->indirection = (curDecl->indirection >> 1) | 0x8000; /* rotate */
            } while (--starCnt);
            return;
        }
        tok = yylex();
    }
}

/**************************************************
 * 142: 742A PMO +++
 **************************************************/
uint16_t normaliseIndirection(uint16_t n) {
    if (n)
        while (!(n & 1))
            n >>= 1;
    return n;
}

/**************************************************
 * 143: 7454 PMO +++
 **************************************************/
void emitAttr(register attr_t *st) {
    uint16_t indirection;
    uint8_t dataType;

    putchar('`');
    for (;;) {
        if (st->nodeType == FUNCNODE)
            putchar('(');
        for (indirection = st->indirection; indirection; indirection >>= 1)
            if (indirection & 1)
                putchar('*');
        if (st->dataType == DT_COMPLEX && st->nextAttr->nodeType == FUNCNODE)
            st = st->nextAttr;
        else
            break;
    }
    dataType = st->dataType;
    switch (dataType) {
    case DT_ENUM:
    case DT_COMPLEX:
        emitSymName(st->nextSym, stdout);
        break;
    case DT_STRUCT:
    case DT_UNION:
        printf("S%d", st->nextSym->a_labelId);
        break;
    default:
        if (dataType & 1) {
            putchar('u');
            dataType &= ~1;
        }
        putchar("?bcsilxfd?v"[dataType >> 1]);
        break;
    }
}


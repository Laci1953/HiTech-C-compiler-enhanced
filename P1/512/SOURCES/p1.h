/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "cclass.h"
#include "stdio.h"
#include "tok.h"
#include <stdlib.h>
#include <string.h>

#include "dynm512.h"
#include "overlays.h"

typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned long uint32_t;
typedef long int32_t;

#define bool char
#define true 1
#define false 0

#define _EOF 0xFF

#define _Noreturn

#define FORCEINIT
#undef putchar
int putchar(int ch);

#define inData(p) (((char*)p) < (char*)0x4000)

#define HASHTABSIZE 61

typedef struct
{
    char* first;
    char* last;
} header;

typedef union {
    char *yStr;
    int32_t yNum;
    uint8_t yVal;
    struct _sym *ySym;
} YYTYPE;

typedef struct _s8 {
    union {
        struct _sym *_nextSym;
        struct _s8 *_nextInfo;
        int16_t _labelId;
    } u1;
    union {
        struct _sym *_pSym;
        struct _expr *_pExpr;
        struct _args *_pArgs;
        struct _s8 *_pInfo;
    } u2;
    uint16_t i4;
    uint8_t dataType;
    char c7;
} s8_t;

#define i_nextSym  u1._nextSym
#define i_nextInfo u1._nextInfo
#define i_labelId  u1._labelId

#define i_sym      u2._pSym
#define i_expr     u2._pExpr
#define i_args     u2._pArgs
#define i_info     u2._pInfo

typedef struct _sym {
    s8_t attr;
    struct _sym *nMemberList;
    int16_t nodeId;
    int16_t m14;
    int16_t m16;
    int16_t m18;
    uint8_t m20;
    char m21;
    char nRefCnt;
    char *nVName;
    char* next;
    char* prev;
} sym_t;

#define a_labelId  attr.i_labelId
#define a_i4       attr.i4
#define a_dataType attr.dataType
#define a_c7       attr.c7
#define a_nextSym  attr.i_nextSym
#define a_expr     attr.i_expr
#define a_sym      attr.i_sym
#define a_info     attr.i_info
#define a_args     attr.i_args

typedef struct {
    char s0[3];
    char c3;
    uint8_t uc4;
    int16_t i5;
    uint8_t c7;
} t8_t;

typedef struct _args {
    int16_t cnt;
    s8_t s8array[1];
} args_t;

typedef struct _s12 {
    sym_t *p25;
    s8_t *p8;
    sym_t *p25_1;
    uint16_t i6;
    uint8_t uc8;
    bool uc9;
    bool uca;
    bool ucb;
} s12_t;

typedef struct _expr {
    uint8_t tType;
    union {
        unsigned long _t_ul;
        long _t_l;
        char *_t_s;
        struct _sym *_t_pSym;
        struct {
            struct _expr *_t_next;
            struct _expr *_t_alt;
        } s1;
        struct {
            int16_t _t_i0;
            int16_t _t_i2;

        } s2;
    } u1;
#define t_ul   u1._t_ul
#define t_l    u1._t_l
#define t_s    u1._t_s
#define t_next u1.s1._t_next
#define t_alt  u1.s1._t_alt
#define t_i0   u1.s2._t_i0
#define t_i2   u1.s2._t_i2
#define t_pSym u1._t_pSym

    s8_t attr;
} expr_t;

typedef struct {
    expr_t *caseVal;
    int16_t caseLabel;
} s4_t;

typedef struct {
    expr_t *switchExpr;
    int16_t caseCnt;
    int16_t defLabel;
    s4_t caseOptions[255];
} case_t;

typedef struct {
    uint8_t type1;
    uint8_t type2;
} s2_t;

extern s2_t *p2List;          /* 8bc7 */
extern int16_t strId;         /* 8bd7 */
extern uint8_t byte_8f85;     /* 8f85 */
extern bool byte_8f86;        /* 8f86 */
extern char *keywords[];      /* 8f87 */
extern char *tmpFile;         /* 91db */
extern t8_t opTable[68];      /* 9271 */
extern uint8_t byte_968b;     /* 968b */
extern int16_t word_968c;     /* 968c */
extern int16_t tmpLabelId;    /* 968e */
extern int16_t word_9caf;     /* 9caf */
extern char ca9CB1[64];       /* 9cb1 */
extern expr_t **s13SP;        /* 9cf1 */
extern s2_t s2_9cf3[20];      /* 9cf3 */
extern expr_t s13_9d1b;       /* 9d1b */
extern expr_t s13_9d28;       /* 9d28 */
extern expr_t *s13FreeList;   /* 9d35 */
extern uint8_t byte_9d37;     /* 9d37 */
extern expr_t *s13Stk[20];    /* 9d38 */
extern char lastEmitSrc[64];  /* 9d60 */
extern bool sInfoEmitted;     /* 9da0 */
extern int16_t inCnt;         /* 9da1 */
extern char lastEmitFunc[40]; /* 9da3 */
extern YYTYPE yylval;         /* 9dcb */
extern char nameBuf[32];      /* 9dcf */
extern uint8_t ungetTok;      /* 9def */
extern int16_t strChCnt;      /* 9df0 */
extern bool lInfoEmitted;     /* 9df2 */
extern int16_t startTokCnt;   /* 9df3 */
extern uint8_t ungetCh;       /* 9df5 */
extern char errBuf[128];      /* 9df7 */
extern FILE *crfFp;           /* 9ff7 */
extern char crfNameBuf[30];   /* 9ff9 */
extern char srcFile[100];     /* a017 */
extern char *crfFile;         /* a07b */
extern bool s_opt;            /* a07d */
extern bool w_opt;            /* a07e */
extern int16_t lineNo;        /* a07f */
extern char *srcFileArg;      /* a081 */
extern bool l_opt;            /* a083 */
extern FILE *tmpFp;           /* a084 */
extern char inBuf[512];       /* a086 */
extern int16_t errCnt;        /* a286 */
extern int8_t depth;         /* a288 */
extern uint8_t byte_a289;     /* a289 */
extern bool unreachable;      /* a28a */
extern int16_t word_a28b;     /* a28b */
extern sym_t *curFuncNode;    /* a28d */
extern sym_t *p25_a28f;       /* ad8f */
//extern sym_t *word_a291;      /* as91 */
//extern sym_t *s25FreeList;    /* a293 */
//extern sym_t **hashtab;       /* a295 */
extern s12_t *p12_a297;       /* a297 */
extern uint8_t byte_a299;     /* a299 */
extern uint8_t byte_a29a;     /* a29a */

/* emit.c */
#ifdef EMIT
void sub_013d(register FILE *p);
void sub_01c1(sym_t *p_arg);
void sub_01ec(register sym_t *p);
void prFuncBrace(uint8_t tok);
void emitLabelDef(int16_t p);
void sub_0273(register sym_t *st);
void sub_02a6(case_t *p1);
void sub_0353(sym_t *p, char c);
void sub_042d(register expr_t *p);
void sub_0470(expr_t *p);
void sub_0493(register sym_t *st);
void sub_053f(register expr_t *st, char *pc);
void sub_05b5(expr_t *p1);
void sub_05d3(expr_t *p1);
void sub_05f1(register expr_t *st);
void sub_07e3(void);
void O_emitSrcInfo(void);
#endif

/* expr.c */
#ifdef EXPR
void pushS13(expr_t *p1);
#endif

/* lex.c */
#ifdef LEX
uint8_t parseNumber(uint8_t ch);
uint8_t parseName(uint8_t ch);
void parseAsm(void);
void parseString(uint8_t ch);
uint8_t getCh(void);
void prErrMsg(void);
void prMsgAt(register char *buf);
void emitSrcInfo(void);
uint8_t skipWs(void);
uint8_t escCh(uint8_t ch);
uint8_t peekCh(void);
void skipStmt(uint8_t tok);
void expect(uint8_t etok, char *msg);
void skipToSemi(void);
#endif

/* main.c */
#ifdef MAIN
int main(int argc, char *argv[]);
void prError(char *fmt, ...);
void fatalErr(char *fmt, ...);
void prWarning(char *fmt, ...);
void expectErr(char *p);
void *xalloc(size_t size);
void prMsgAt(register char *buf);
void prErrMsg(void);
#endif

/* program.c */
#ifdef PROG
int16_t sub_3d24(sym_t *st, uint8_t p2);
#endif

/* stmt.c */
#ifdef STMT
void sub_409b(void);
void parseStmt(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseStmtGroup(int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
void parseStmtAsm(void);
void parseStmtWhile(case_t *p3);
void parseStmtDo(case_t *p3);
void parseStmtIf(int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
void parseStmtSwitch(int16_t p1);
void parseStmtFor(case_t *p1);
void parseStmtBreak_Continue(int16_t label);
void parseStmtDefault(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseStmtCase(int16_t p1, int16_t p2, register case_t *p3, int16_t *p4);
void parseStmtReturn(void);
void parseStmtGoto(void);
void parseStmtLabel(register sym_t *ps, int16_t p1, int16_t p2, case_t *p3, int16_t *p4);
sym_t *sub_4ca4(register sym_t *ps);
void sub_4ce8(int16_t n);
void sub_4d15(int16_t n, register expr_t *st, char c);
void sub_4d67(register expr_t *st);
#endif

/* sym.c */
#ifdef SYM
void sub_4d92(void);
sym_t *sub_4e90(register char *buf);
void sub_516c(register sym_t *st);
void sub_51cf(register sym_t *st);
void sub_51e7(void);
bool releaseNodeFreeList(void);
void enterScope(void);
void exitScope(void);
sym_t *sub_56a4(void);
sym_t *findMember(sym_t *p1, char *p2);
void sub_573b(register sym_t *st, FILE *fp);
int16_t newTmpLabel(void);
bool sub_5a4a(register s8_t *st);
bool sub_5a76(register s8_t *st, bool vst, uint8_t p2);
bool sub_5aa4(register s8_t *st);
bool sub_5ad5(register s8_t *st);
bool sub_5b08(register s8_t *st);
bool sub_5b38(register s8_t *st);
bool isValidDimType(register s8_t *st);
void sub_5b99(register s8_t *st);
#endif

/* type.c */
#ifdef TYPE
void sub_5c50(void);
sym_t *sub_60db(uint8_t p1);
sym_t *sub_6360(void);
void sub_6531(register sym_t *st);
args_t *sub_65e2(uint16_t p1);
void sub_6fab(uint8_t p1, bool *v);
uint16_t sub_742a(uint16_t n);
#endif

/* Overlay entries */
#define	yylex			(char*)0x4000
#define emitSrcInfo		(char*)0x4003
#define peekCh			(char*)0x4006
#define skipStmt		(char*)0x4009
#define skipToSemi		(char*)0x400C
#define sub_3adf		(char*)0x400F
#define sub_3c7e		(char*)0x4012
#define sub_4eed		(char*)0x4015
#define sub_51e7		(char*)0x4018
#define checkScopeExit		(char*)0x401B
#define sub_578d		(char*)0x401E
#define sub_583a		(char*)0x4021
#define sub_58bd		(char*)0x4024
#define sub_591d		(char*)0x4027
#define reduceNodeRef		(char*)0x402A
#define sub_07f5		(char*)0x402D
#define sub_0a83		(char*)0x4030
#define sub_0bfc		(char*)0x4033
#define sub_1441		(char*)0x4036
#define sub_1b4b		(char*)0x4039
#define sub_2105		(char*)0x403C
#define sub_21c7		(char*)0x403F
#define s13ReleaseFreeList	(char*)0x4042
#define allocId			(char*)0x4045
#define allocIConst		(char*)0x4048
#define allocSType		(char*)0x404B
#define sub_2569		(char*)0x404E
#define sub_25f7		(char*)0x4051
#define sub_5be1		(char*)0x4054
#define sub_5c19		(char*)0x4057
#define sub_5dd1		(char*)0x405A
#define sub_69ca		(char*)0x405D
#define sub_7454		(char*)0x4060

char* myalloc(short size);
void setleds(char);
void breakpoint(void);

// offsets in sym_t structure

#define	OFF_s8_t_labelId	0
#define	OFF_s8_t_nextSym	0
#define	OFF_s8_t_nextInfo	0
#define	OFF_s8_t_pSym		2
#define	OFF_s8_t_pExpr		2
#define	OFF_s8_t_pArgs		2
#define	OFF_s8_t_pInfo		2
#define	OFF_s8_t_a_i4		4	
#define	OFF_s8_t_a_dataType	6	
#define	OFF_s8_t_c7		7
#define	OFF_nMemberList		8	
#define	OFF_nodeId		10	
#define	OFF_m14			12	
#define	OFF_m16			14	
#define	OFF_m18			16	
#define	OFF_m20			18	
#define	OFF_m21			19	
#define	OFF_nRefCnt		20	
#define	OFF_nVName		21
#define OFF_next		23
#define OFF_prev		25	

// offsets in attr structure

#define OFF_ATTR_u1		0
#define OFF_ATTR_nextSym	0
#define OFF_ATTR_nextInfo	0
#define OFF_ATTR_labelId	0
#define OFF_ATTR_u2		2
#define OFF_ATTR_pSym		2
#define OFF_ATTR_pExpr		2
#define OFF_ATTR_pArgs		2
#define OFF_ATTR_pInfo		2
#define OFF_ATTR_i4		4
#define OFF_ATTR_dataType	6
#define OFF_ATTR_c7		7

/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "cgen512.h"

/* ===== start bss section ======= */
int curPsect;
bool frameGlobalsEmitted; /* First call ncsv */
int localLabelCnt;
int lineno;             /* getToken, sub_6AD0, prMsg  */
char progname[MAXNAME]; /* getToken, prWarning, prMsg */

member_t *typeLong; 	/* "l" - long	*/    /* sub_1680, sub_415E, sub_43EF */
int array_AE13[MAXFUN]; /*		*/    /* leaveBlock,  sub_17E0, sub_5CF5 */
member_t *typeUChar;                          /* "uc" - uint8_t	   */
uint16_t nstdpth;                             /* Current nesting depth   */
int array_AE57[MAXFUN];                       /*			   */
int array_AE97[MAXFUN];                       /*			   */
member_t *typeDouble;                         /* "d" - double 	   */
member_t *typeChar;                           /* "c" - char 		   */
member_t *array_AEDB[MAXFUN];                 /*			   */
member_t *typeB;                              /* "b"			   */
member_t *typeX;                              /* "x"			   */
member_t *hashtab[101];                       /* hash table (array_AF1F) */
member_t *typeVar;                            /* "v" - variable  	   */
int lvlidx; /* Level nested function   */     /* leaveBlock, sub_1680 */

int word_AFF8;            /* sub_2D09, sub_3564 */
char byte_AFFA;           /* sub_283E, sub_2B79 */
int word_AFFB;            /* sub_2D09, sub_35E6 */
node_t *array_AFFD[0x14]; /* sub_3CDF, sub_3DC9 */
void *nodeFreeList;       /* sub_36E0, allocNode, releaseNodeFreeList */
char bf1;
bool byte_B013;           /* sub_36E0, allocNode, ... */
char bf2;
uint8_t byte_B014;        /* sub_3CDF, sub_3DC9 */
/* char *warningMsg; */         /* Pointer for printf      */
int word_B017;            /* leaveBlock, sub_6D1, sub_283E, sub_2BD0, sub_61AA */
bool wflag;               /* Warning messages	   */
bool pflag;               /* Not used		   */
bool hflag;               /* Not used		   */
int errcnt;               /* Number of errors	   */
bool bflag;               /* Not used		   */
bool eflag;               /* Not used		   */
bool rflag;               /* Not used		   */
char	tmpBuf[32];		/* temporary buffer for strings handling */
bool warningDone;

/* ===== End of bss section ======= */


char *regNames[] = {
    "", "a", "c", "b", "e", "d", "l", "h", "ix", "iy", "sp", "af", "bc", "de", "hl"
};

int array_AAE8[]     = { 0,     1,    2,    4,    8,    0x10, 0x20, 0x40, 0x80, 0x100,    /* 0-9 */
                         0x200, 1,    6,    0x18, 0x60, 0x78, 0x66, 0x7E, 0x7F, 0,        /* 10-19 */
                         0x79,  0x61, 0x19, 7,    0x7E, 0x1E, 0x3F, 0x3E, 0x7F, 0x1E };   /* 20-29 */

uint16_t dopetab[72] = {
    /* array_A7C0 */
    0x000C, /*  0000 0000 0000 1100	 0  0	""      NULSTR		*/
    0x0227, /*  0000 0010 0010 0111	 1  1	"!"	NOT 		*/
    0x0468, /*  0000 0100 0110 1000	 2  2	"!="    NEQL	EQUOP	*/
    0x0014, /*  0000 0000 0001 0100	 3  3	"#"     HASHSIGN	*/
    0x4208, /*  0100 0010 0000 1000	 4  4	"$"     DOLLAR		*/
    0x4004, /*  0100 0010 0000 1000	 5  5	"$U"    DOLLARU		*/
    8,      /*  0000 0000 0000 1000	 6  6	"%"    -MOD 		*/
    0x2848, /*  0010 1000 0100 1000	 7  7	"&"    -BAND	BITWOP	*/
    0x4328, /*  0100 0011 0010 1000	 8  8	"&&"	LAND 	LOGICOP	*/
    4,      /*  0000 0000 0000 0100	 9  9	"&U"    GADDR		*/
    0x4008, /*  0100 0000 0000 1000	10  A	"("	LPAREN 		*/
    0x4004, /*  0100 0000 0000 0100	11  B	")"	RPAREN		*/
    0x2048, /*  0010 0000 0100 1000	12  C	"*"    +MUL   	ARITMOP */
    0x1004, /*  0001 0000 0000 0100	13  D	"*U"    MULU		*/
    0x2C48, /*  0010 1100 0100 1000	14  E	"+"    -ADD   	ARITMOP */
    8,      /*  0000 0000 0000 1000	15  F	"++" 	INCR   		*/
    4,      /*  0000 0000 0000 0100	16 10	"+U"    PLUSU		*/
    8,      /*  0000 0000 0000 1000	17 11	","     COMMA		*/
    0x0C08, /*  0000 1100 0000 1000	18 12	"-"    -SUB   	ARITMOP */
    8,      /*  0000 0000 0000 1000	19 13	"--" 	DECR   		*/
    0x0208, /*  0000 0010 0000 1000	20 14	"->"    TCAST		*/
    0x0C07, /*  0000 1100 0000 0111	21 15	"-U"    MINUSU		*/
    8,      /*  0000 0000 0000 1000	22 16	"."     DOT		*/
    0x0030, /*  0000 0000 0011 0000	23 17	".."    TDOT		*/
    8,      /*  0000 0000 0000 1000	24 18	"/"    -DIV	ARITMOP */
    0x4008, /*  0100 0000 0000 1000	25 19	":"     COLON		*/
    0x4004, /*  0100 0010 0000 1000	26 1A	":U"    COLONU		*/
    0x4004, /*  0100 0010 0000 1000	27 1B	":s"    COLONS		*/
    0x4108, /*  0100 0001 0000 1000	28 1C	";"     SCOLON		*/
    0x000C, /*  0000 0000 0000 1100	29 1D	";;"    TSCOLON		*/
    0x0428, /*  0000 0100 0010 1000	30 1E	"<"     LT	RELOP	*/
    0x0C88, /*  0000 1100 1000 1000	31 1F	"<<"   -LSHIFT  SHIFTOP	*/
    0x0428, /*  0000 0100 0010 1000	32 20	"<="  	LEQ  	RELOP	*/
    8,      /*  0000 0000 0000 1000	33 21	"="   	ASSIGN		*/
    0x0408, /*  0000 0100 0000 1000	34 22	"=%"  	ASMOD		*/
    0x0C08, /*  0000 1100 0000 1000	35 23	"=&"  	ASAND		*/
    0x0C08, /*  0000 1100 0000 1000	36 24	"=*"  	ASMUL		*/
    0x0C08, /*  0000 1100 0000 1000	37 25	"=+"  	ASADD		*/
    0x0C08, /*  0000 1100 0000 1000	38 26	"=-"  	ASSUB		*/
    8,      /*  0000 0000 0000 1000	39 27	"=/"  	ASDIV		*/
    0x0C88, /*  0000 1100 1000 1000	40 28	"=<<" 	ASLSHIFT	*/
    0x0468, /*  0000 0100 0110 1000	41 29	"=="  	EQL  	EQUOP	*/
    0x0088, /*  0000 0000 1000 1000	42 2A	"=>>" 	ASRSHIFT	*/
    0x0C08, /*  0000 1100 0000 1000	43 2B	"=^"  	ASEXOR		*/
    0x0C08, /*  0000 1100 0000 1000	44 2C	"=|"  	ASEOR		*/
    0x0428, /*  0000 0100 0010 1000	45 2D	">"   	GT  	RELOP	*/
    0x0428, /*  0000 0100 0010 1000	46 2E	">="  	GEQ	RELOP	*/
    0x0088, /*  0000 0000 1000 1000	47 2F	">>"   -RSHIFT  SHIFTOP	*/
    0x4308, /*  0100 0011 0000 1000	48 30	"?"     QUEST		*/
    0x4004, /*  0100 0010 0000 1000	49 31	"@"     ATGIGN		*/

    0x000C, /*  0000 0000 0000 1100	50 32	"[\\"   CASE		*/
    0x000C, /*  0000 0000 0000 1100	51 33	"[a"    UNKNOWN		*/
    0x000C, /*  0000 0000 0000 1100	52 34	"[c"    ENUM		*/
    0x000C, /*  0000 0000 0000 1100	53 35	"[e"    EXPR		*/
    0x000C, /*  0000 0000 0000 1100	54 36	"[i"    INIT		*/
    0x000C, /*  0000 0000 0000 1100	55 37	"[s"    STRUCT		*/
    0x000C, /*  0000 0000 0000 1100	56 38	"[u"    UNION		*/

    0x000C, /*  0000 0000 0000 1100	57 39	"[v"    		*/
    0x2848, /*  0010 1000 0100 1000	58 3A	"^"    -BXOR   	BITWOP	*/
    0x000C, /*  0000 0000 0000 1100	59 3B	"{"     LBRACE		*/
    0x2848, /*  0010 1000 0100 1000	60 3C	"|"    -BOR	BITWOP	*/
    0x4328, /*  0100 0011 0010 1000	61 3D	"||" 	LOR   	LOGICOP	*/
    0x000C, /*  0000 0000 0000 1100	62 3E	"}"     RBRACE		*/
    0x0C07, /*  0000 1100 0000 0111	63 3F	"~"  	BNOT	BITWOP	*/
    4,      /*  0000 0000 0000 0100	64 40	"RECIP"	   		*/
    0,      /*  0000 0000 0000 0000	65 41	"TYPE"     		*/
    0x1002, /*  0001 0000 0000 0010	66 42	"ID"	   		*/
    0x0010, /*  0000 0000 0001 0000	67 43	"CONST"    		*/
    3,      /*  0000 0000 0000 0011	68 44	"FCONST"   		*/
    2,      /*  0000 0000 0000 0010	69 45	"REG"      		*/
    0x4004, /*  0100 0010 0000 1000	70 46	"INAREG"   		*/
    4       /*  0000 0000 0000 0100	71 47	"BITFIELD" 		*/
};


uint8_t array_AB24[] = { 0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
                         0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
                         0,  0,  1,  0,  2,  3,  4,  5,  6, 7,   
                         13, 14, 12, 14, 12, 15, 1,  17, 0, 0,
                         15, 1,  14, 1,  13, 1,  12, 1 };

uint8_t array_AB54[] = { 14, 13, 12, 0, 0, 0,         /* groups of 6 bytes, each is index into array_AAE8 */
                         13, 12, 0,  0, 0, 0,
                         1,  4,  5,  2, 3, 6,
                         4,  5,  2,  3, 6, 0,
                         21, 22, 23, 0, 0, 0,
                         12, 13, 0,  0, 0, 0 };


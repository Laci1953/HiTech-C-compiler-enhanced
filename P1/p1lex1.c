/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

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
uint8_t ungetCh;     /*  9df5 */

sym_t *sub_4e90(register char *buf);
void expectErr(char *p);
void prError(int, ...);
void fatalErr(int, ...);

/**************************************************
 * 56: 2671 PMO +++
 * location of two basic blocks swapped, code equivalent
 **************************************************/
uint8_t tmp_yylex(void)
{
    uint8_t ch;
    char buf[50];
    register char *s;

    for (;;) {
        ch          = skipWs();
        startTokCnt = inCnt;
        if (Isalpha(ch))
            return parseName(ch);
        if (Isdigit(ch))
            return parseNumber(ch);
        switch (ch) {
        case _EOF:
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
                        *s++ = ch;
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
                    *s++ = ch;
                    ch   = getCh();
                } while (ch != '\n' && !Isspace(ch));
                *s = '\0';
                while (ch != '\n')
                    ch = getCh();
                if (strcmp(buf, "asm") == 0) {
                    parseAsm();
                    break;
                } else
                    /* fatalErr("illegal '#' directive"); */
		    fatalErr(96);
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
                /* prError("char const too long"); */
		prError(54);

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
                return T_INC;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return T_PLUSEQ;
            ungetCh = ch;
            return T_PLUS;
        case '-':
            ch = getCh();
            if (ch == '-')
                return T_DEC;
            if (ch == '>')
                return T_POINTER;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return T_MINUSEQ;
            ungetCh = ch;
            return T_MINUS;
        case '*':
            ch = skipWs();
            if (ch == '=')
                return T_MULEQ;
            ungetCh = ch;
            return T_STAR; /* deref or multiply */
        case '/':
            ch = skipWs();
            if (ch == '=')
                return T_DIVEQ;
            ungetCh = ch;
            return T_DIV;
        case '%':
            ch = skipWs();
            if (ch == '=')
                return T_MODEQ;
            ungetCh = ch;
            return T_MOD;
        case '&':
            ch = getCh();
            if (ch == '&')
                return T_LAND;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return T_ANDEQ;
            ungetCh = ch;
            return T_BAND;
        case '|':
            ch = getCh();
            if (ch == '|')
                return T_LOR;
            ungetCh = ch;
            ch      = skipWs();
            if (ch == '=')
                return T_OREQ;
            ungetCh = ch;
            return T_BOR;
        case '^':
            ch = skipWs();
            if (ch == '=')
                return T_XOREQ;
            ungetCh = ch;
            return T_XOR;
        case '<':
            ch = getCh();
            if (ch == '<') {
                ch = skipWs();
                if (ch == '=')
                    return T_SHLEQ;
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
                    return T_SHREQ;
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
		    /* prError("'.' expected after '..'"); */
		    prError(53);
                return T_3DOT;
            }
            ungetCh = ch;
            return T_DOT;

        case '?':
            return T_QUEST;
        case ',':
            return T_COMMA;
        default:
	    buf[0] = ch;
	    buf[1] = 0;
	    prError(155, buf);
            break;
        }
    }
}

uint8_t yylex(void) 
{
    uint8_t tok;

    if (ungetTok) {
        tok      = ungetTok;
        ungetTok = 0;
        if (tok == T_ID && byte_8f86)
            yylval.ySym = sub_4e90(nameBuf);
        return tok;
    }

    return tmp_yylex();
}


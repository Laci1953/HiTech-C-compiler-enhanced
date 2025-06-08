/*
 *
 * The lex.c file is part of the restored ZAS.COM program
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
#include "cclass.h"
#include "zas.h"

char yytext[100];                                      /* 9b28 */
int16_t lineLen;                                       /* 9b8d */
int16_t curLineno;                                     /* 9bbe */
char l9bc2h;                                           /* 9bc2 */
char *inPtr;                                           /* 9bc3 */
char controls;                                         /* 9bc5 */
int16_t tmpId;                                         /* 9bc6 */
char inBuf[256];                                       /* 9bcc */

char tmpBuf[32];
char tmpBufAux[32];

char *skipToNextToken(register char *s);       /* 24 1217 +-- */
int32_t zatol(register char *s, uint8_t base); /* 25 1276 +-- */
int16_t isFloat(char *s);                      /* 29 180A +-- */
int parseNumber(void);                         /* 31 1985 +-- */
int16_t getText(void);                                /* 36 225C +-- */

/* pseudo functions to make code readable */

#define PeekCh() (lineLen > 0 ? *inPtr : 0)
#define GetCh()  (--lineLen >= 0 ? *inPtr++ : getText())

/**************************************************************************
24	skipToNextToken	+++
**************************************************************************/
char *skipToNextToken(register char *s) {
    char *t;

    for (; *s && !Isspace(*s); ++s)
        ;
    for (; Isspace(*s); ++s)
        ;
    for (t = s; *t && *t != '\n'; ++t)
        ;
    *t = 0;
    return s;
}

/**************************************************************************
 25	zatol	+++
 **************************************************************************/
int32_t zatol(register char *s, uint8_t base) {
    int32_t val;
    int16_t digit;

    for (val = 0; *s != 0; val += digit) {
        val *= base;
        if (Isdigit(*s)) {
            digit = *s - '0';
            ++s;
        } else if (Isxdigit(*s)) {
            digit = Tolower(*s) - 'a' + 10;
            ++s;
        }

        if (digit >= base) {
            error("Digit out of range");
            return 0;
        }
    }
    return val;
}

/**************************************************************************
29	sub_180ah +++
**************************************************************************/
int16_t isFloat(char *s) {
    bool hasE = false;

    if (*s == '+' || *s == '-')
        s++;

    for (; *s; ++s) {
        if (*s == 'e' || *s == 'E') {
            hasE = true;
            if (s[1] == '+' || s[1] == '-')
                return true;
            continue;
        }
        if (*s == '.')
            return true;
        else if (!Isdigit(*s))
            return false;
    }
    return hasE;
}

/**************************************************************************
 31 1985 +++
 * minor variant as zatol now declared with base as uint8_t
 * also different way of doing --iy, neither optimal
 **************************************************************************/
int parseNumber() {
    register char *s = yytext + 1;
    uint8_t base     = 0;

    while (Isxdigit(PeekCh()) || (floatMode == 2 && PeekCh() == '.'))
        *s++ = (char)GetCh();
    switch (PeekCh()) {
    case 'H':
    case 'h':
        base = 16;
        GetCh();
        break;
    case 'O':
    case 'o':
    case 'Q':
    case 'q':
        base = 8;
        GetCh();
        break;
    case '+':
    case '-':
        if (s[-1] == 'e' || s[-1] == 'E') {
            do {
                *s++ = (char)GetCh();
            } while (Isdigit(PeekCh()));
            break;
        }
        /* fall through */

    default:
        if (s[-1] == 'f' || s[-1] == 'b') {
            s--;
            tmpId       = *s == 'f' ? G_FWD : G_BWD;
            *s          = 0;
            yylval.yNum = atol(yytext);
            return tmpId;
        } else if (s[-1] == 'B') {
            s--;
            base = 2;
        }
        break;
    }
    *s = 0;
    if (base == 0 && floatMode == 2 && isFloat(yytext)) {
        yylval.yFloat = (float)atof(yytext);
        return G_FLOAT;
    }
    if (base == 0)
        base = 10;
    yylval.yNum = zatol(yytext, base);
    return G_INT;
}

/**************************************************************************
32	1b8c +++
**************************************************************************/
int16_t yylex() {
    int ch;
    char *tokStart;
    register char *s;
    for (;;) {
        s        = yytext;
        tokStart = inPtr;
        switch (ch = GetCh()) {
        case -1:
            return -1;
        case '\t':
        case ' ':
            continue;
        case '\f':
            //topOfPage();
            continue;
        case '\'':
            while (PeekCh() && PeekCh() != '\'')
                *s++ = (char)GetCh();
            if (PeekCh() == '\'')
                GetCh();
            else
                error("Unterminated string");
            *s          = 0;
            yylval.yStr = yytext;
            return G_STR;
        case '.':
            *s++ = ch;
            while (Isalpha(PeekCh()))
                *s++ = (char)GetCh();
            if (PeekCh() == '.')
                *s++ = (char)GetCh();
            break;
        case ',':
            return T_COMMA;
        case ':':
            return T_COLON;
        case '(':
            yylval.yVal = 9; /* change precedence */
            return T_LPAREN;
        case ')':
            yylval.yVal = 1;
            return T_RPAREN;
        case '+':
            yylval.yVal = 5;
            return T_PLUS;
        case '-':
            yylval.yVal = 5;
            return T_MINUS;
        case '\0':
        case ';':
            return T_EOL;
        default:
            *s++ = ch;
            if (Isalpha(ch)) {
                while (Isalnum(PeekCh()))
                    *s++ = (char)GetCh();
                if (s == yytext + 2 && PeekCh() == '\'')
                    *s++ = (char)GetCh();
            } else if (Isdigit(ch))
                return parseNumber();
            break;
        }

        while (Isspace(PeekCh()))
            GetCh();
        *s = 0;
        if (symFlags) {
            tmpId = getPsectId(yytext);
            if (tmpId != -1)
                return tmpId;
        }
        if (yytext[0] != '_') {
            tmpId = getKwdId(yytext);
            if (tmpId != -1)
                return tmpId;
        }
        if (Isalpha(yytext[0])) {
            yylval.ySym = getSym(yytext, symFlags);
            return G_SYM;
        } else
            error("Lexical error");
    }
}

/**************************************************************************
 36	getText	+++
 **************************************************************************/
int16_t getText() 
{
    if (fgets(inBuf, 256, asmFp) == NULL)
	return EOF;

    lineLen = (int16_t)strlen(inBuf);
    ++curLineno;
    inPtr = inBuf;
    l9bc2h = 0;

    if (inBuf[lineLen - 1] == '\n')
        inBuf[lineLen - 1] = 0;

    --lineLen;

    return *(uint8_t *)inPtr++;
}

/**************************************************************************
 39	skipLine	+++
 **************************************************************************/
void skipLine() {
    if (lineLen >= 0)
        inPtr += lineLen;
    lineLen = 0;
}


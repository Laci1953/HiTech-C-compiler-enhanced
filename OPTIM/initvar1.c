/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

/*
 * The initvar1.c file is part of the restored optimization program
 * from the Hi-Tech C compiler v3.09 package.
 *
 *      Andrey Nikitin & Mark Ogden 11.11.2021
 */
#include <stdio.h>
#include <setjmp.h>
#include "optim1.h"

/****************************************************************
 *      Descriptions of uninitialized variables and arrays
 ****************************************************************/

char               yyline[80];                  /* 6f00 Working buffer */
int                charsLeft;                   /* 6f50 Length string in input buffer */
char             * ptr_inbuf;                   /* 6f52 Pointer to input buffer  */
int                yytype;                      /* 6f54 */
char               inp_buf[80];                 /* 6f56 Input buffer (In the original it was 80) */
//bool               key_l;                       /* 6fa6 Prints additional information on each pass */
bool               key_f;                       /* 6fa8 The action is not clear */
//bool               key_s;                       /* 6fa9 Key_s = 1 Unbuffered stdout */
int                num_warn;                    /* 6faa Number of warnings */
//bool               key_r;                       /* 6fac */
operator_t const * tableBase;                   /* 6fad Pointer to keyword[] */
char             * yytext;                      /* 6faf */
YYSTYPE            yylval;                      /* 6fb1 Contains different types of data */
int                symbolId;                    /* 6fb3 */
sym_t            * gPs;                         /* 6fb5 */
inst_t           * gPi;                         /* 6fb7 */
/*int              word_6fb9;                      6fb9 Moved to function as static variable */
bool               hasChanged;                  /* 6fbb */
bool               usesIXorIY;                  /* 6fbc */
//int                optimiseCnters[18];  /* 6fbd Array of counters types of optimizations */
list_t           * freeOperandLst;              /* 6fe1 ptr to struct size 6 */
inst_t           * seq1;                        /* 6fe3 */
inst_t           * seq2;                        /* 6fe5 */
inst_t           * freeInstList;                /* 6fe7 */
char               psect;                       /* 6fe9 Program section */
int                cur_psect;                   /* 6fea Current program section */
int                expectCond;                  /* 6fec */
inst_t           * word_6fee;                   /* 6fee */
int                tokType;                     /* 6ff0 Token value */
inst_t           * switchVectors; /* word_6ff2; */      /* 6ff2 */
/*static term_t    termTmp;                        6ff4 [4] Moved to function as static variable */
inst_t           * root;                        /* 6ff8 */
int                cntOperand;                  /* 6ffa Used only in sub_39a3 */
inst_t           * word_6ffc;                   /* 6ffc */
//jmp_buf            jmpbuf;                      /* 6ffe [8] */
int                hlDelta; /* word_7006;*/                     /* 7006 */
operand_t          regValues[19];               /* 7008 */
char             * alloct;                      /* 707a is the top of the current region */
char             * name_fun;                    /* 707c Function name */
list_t           * freeItemList;                /* 7080 */
char             * allocs;                      /* 7082 is the current next allocated heap location */
char             * programBreak;                /* 7084 ok */
sym_t            * hashtab[HASHSIZE];           /* 7086 */
operand_t 	   lo1,lo2,lo3,lo4;
term_t 		   termStack[30];
op_t 	           opStack[30];
term_t 		   exprResult; /* so pointer to term can be returned */

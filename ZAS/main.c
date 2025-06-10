/*
 *
 * The main.c file is part of the restored ZAS.COM program
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
 *
 * Modified to allocate labels in the extended 128/512 KB RAM
 * by Ladislau Szilagyi, June 2025
 *
 */
#include "zas.h"

char phase;           /* 9e3a */
bool j_opt;           /* 9e59 */
int numerror;         /* 9e65 */
bool n_opt;           /* 9e68 */
bool x_opt;           /* 9e69 */
FILE *objFp;          /* 9e6e */
FILE *asmFp;

extern char *asmFileName;

char bufitoa[7];
char* buf8000 = "-32768";

char* itoa(int i)
{
    	register char* pbuf = bufitoa+6;  // points to terminating zero
    	char sign = 0;

	if (i == 0x8000)
		return buf8000;

    	*pbuf = 0;

    	if (i < 0)
	{
         	sign = 1;
		i = -i;
	}

    	do
	{
         	*--pbuf = '0' + (i % 10);
         	i = i / 10;
         	if (i == 0) 
	      		break;
	}
    	while (1 == 1);

    	if (sign == 1)
	 	*--pbuf = '-';

	return pbuf;
}

// only %d %s
int	_doprnt(FILE* file, char* format, int* args)
{
	char ch;
	register char* pstr;

	while (ch = *format++)
	{
		if(ch != '%')
			putc(ch, file);
		else 
		{
			ch = *format++;

			if (ch == 's')
				pstr = *(char **)args++;
			else // must be 'd'
				pstr = itoa(*args++);

			while (ch = *pstr++)
				putc(ch, file);
		}
	}

	return 0;
}

void maintop(int argc, char **argv);

/***************************************************************
 59	main	sub_2ef4 +++
 * a block of 3 basic blocks is located differently, specifically
 * the for block code just after the if (!objFileName)
 * also uses bc instead of hl for assignment in case 'o'
 * code is otherwise same
 ***************************************************************/
int main(int argc, char **argv) {

    maintop(argc, argv);

    SetupSymTab();

    enterAbsPsect();
    phase = 0;

    if (j_opt) { /* jump optimizations */
        doPass();
        resetVals();
        phase  = 1;
    }

    doPass();
    phase = 2;
    resetVals();
    writeObjHeader();

    doPass();
    writeRecords();

    if (j_opt)
        printf("\n%d jump optimizations\n", jmpOptCnt);

    addObjAllSymbols(); /* m31: */
    addObjEnd();
    exit(numerror != 0);
}

/**************************************************************************
 61	fatalErr	sub_32bfh	+++
 **************************************************************************/
_Noreturn void fatalErr(p1, p2, p3) char *p1;
{
    prMsg(p1, p2, p3);
    exit(1);
}

/**************************************************************************
 62	error	sub-32e6h	+++
 **************************************************************************/
void error(p1, p2, p3) char *p1;
{
    if (phase != 2)
        return;

    prMsg(p1, p2, p3);

    if (++numerror < 100)
        return;

    fatalErr("Too many errors");
}

/**************************************************************************
 63	prMsg	+++
 **************************************************************************/
void prMsg(p1, p2, p3) char *p1;
{
    fprintf(stderr, "%s:%d:\t", asmFileName, curLineno);
    fprintf(stderr, p1, p2, p3);
    fputc('\n', stderr);
}

/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128/512KB systems by Ladislau Szilagyi   *
*		ladislau_szilagyi@yahoo.com	      *
*	   December 2022 - May 2025		      *
******************************************************/

/* Not a commercial goal of this laborious work is to popularize among
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
 * The solution to this problem is to recreate the object code being moved,
 * replace the CP/M system functions (I/O, memory allocation, etc.) with
 * similar UZI-180 calls, and compile an executable file for this operating
 * system.
 *
 *	Mark Ogden & Andrey Nikitin   24.12.2021
 */

/*
 * File - main.c
 */

#include "cgen.h"
#include "alloccg.h"

char bufltoa[16];

char* ltoa(long l)
{
    	register char* pbuf = bufltoa+15;  // points to terminating zero
    	char sign = 0;

    	*pbuf = 0;

    	if (l < 0)
	{
         	sign = 1;
		l = -l;
	}

    	do
	{
         	*--pbuf = '0' + (l % 10);
         	l = l / 10;
         	if (l == 0) 
	      		break;
	}
    	while (1 == 1);

    	if (sign == 1)
	 	*--pbuf = '-';

	return pbuf;
}

char* itoa(int i)
{
	return ltoa((long)i);
}

// only %d %l %s
int	_doprnt(FILE* file, char* format, int* args)
{
	char ch;
	register char* pstr;
	int low, high;

	while (ch = *format++)
	{
		if(ch != '%')
			putc(ch, file);
		else 
		{
			ch = *format++;

			if (ch == 's')
				pstr = *(char **)args++;
			else if (ch == 'l')
			{
				low = *args++;
				high = *args++;
				pstr = ltoa(((long)high << 16) | (long)low);
			}
			else // must be 'd'
				pstr = itoa(*args++);

			while (ch = *pstr++)
				putc(ch, file);
		}
	}

	return 0;
}

#ifdef CHECKSUM

unsigned char CksText(void);
unsigned char CksData(void);

unsigned char cksData0, cksData1, cksText0, cksText1;

void Check(void)
{
	cksText1 = CksText();
	cksData1 = CksData();

	if (cksText0 != cksText1)
		printf("Altered code!\n");

	if (cksData0 != cksData1)
		printf("Altered data!\n");

}

#endif

/*********************************************************
 * prMsg OK PMO	      	  Used in: ferror, prError
 * Difference due to use of stdarg
 *********************************************************/
void prMsg(char* p)
{
    fprintf(stderr, "%s:%d:\t", progname, lineno);
    fprintf(stderr, p);
    fputc('\n', stderr);
}

/*********************************************************
 * sub_6AA2 OK PMO Used in: badIntCode,  sub_17E0, sub_19C1,
 *			    sub_2BD0, sub_4192, sub_6B1D,
 * fatalErr error  	    sub_6B9B,
 * Difference due to change to use stdarg
 *********************************************************/
void fatalErr(char* p)
{
#ifdef CHECKSUM
    Check();
#endif

    prMsg(p);
    fclose(stdout);
    exit(1);
}

/*********************************************************
 * prError OK PMO
 * Nonfatal error
 * Difference due to use of stdarg
 *********************************************************/
void prError(char* p)
{
    prMsg(p);

    ++errcnt;
}

//*********************************************************
// * sub_6AD0  OK PMO  Used in: sub_E43,  sub_3DC9, sub_43EF,
// * Warning message	      sub_54B6, sub_5CF5, sub_600E
// * Difference due to change to use stdarg
// *********************************************************
//void prWarning(char* p)
//{
//        if (wflag == 0) 
//	    prMsg(p);
//}

/*********************************************************
 * main OK++ PMO
 *
 *********************************************************/
int main(int argc, char **argv) {

#ifdef CHECKSUM
	cksText0 = CksText();
	cksData0 = CksData();
#endif

    top_main(argc, argv);

    parseStmt();                 /* Compiling intermediate code */

    if (fclose(stdout) == EOF) { /* Close output file */
        prMsg("Error closing output file");
    }

//    if (nflag)
//	fprintf(stderr, "%d KB used from the upper 64KB RAM\r\n", (unsigned short)TotalSize() / 1024);

#ifdef CHECKSUM
	Check();
#endif

    exit(errcnt != 0); 
}

/*********************************************************
 * allocMem OK    Used in: sub_265,  sub_1754, sub_19C1,
 *			     sub_1B0C, allocNode, sub_39CA,
 * alloc_mem & return ptr    sub_4192, sub_508A, sub_5DF6
 *********************************************************/
void *allocMem(size_t size) {
    register char *ptr;

//    do {
//        if (ptr = malloc(size))
//            goto done;
//    } while (releaseNodeFreeList());
//    fatalErr("Lower RAM full");
//done:

    ptr = malloc(size);

    if(!ptr)
	fatalErr("Lower RAM full");

    blkclr(ptr, size);
    return ptr;
}

char* MyAlloc(void)
{
    register char *ptr;

    ptr = (char*)myalloc();

    if (!ptr)
    	fatalErr("Upper RAM full");

    return ptr;
}

/* end of file main.c */

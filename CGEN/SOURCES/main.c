/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
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

void top_main(int argc, char **argv);
/*********************************************************
 * main OK++ PMO
 *
 *********************************************************/
int main(int argc, char **argv) {

    top_main(argc, argv);

    sub_1680();                  /* First_init */
    parseStmt();                 /* Compiling intermediate code */

    if (fclose(stdout) == EOF) { /* Close output file */
        prError("Error closing output file");
    }

//    if (nflag)
//	fprintf(stderr, "%d KB used from the upper 64KB RAM\r\n", (unsigned short)TotalSize() / 1024);

    /* Exit with error code */
    exit(errcnt != 0); /* Generated code is not significantly different */
}

/*********************************************************
 * sub_6AA2 OK PMO Used in: badIntCode,  sub_17E0, sub_19C1,
 *			    sub_2BD0, sub_4192, sub_6B1D,
 * fatalErr error  	    sub_6B9B,
 * Difference due to change to use stdarg
 *********************************************************/
_Noreturn void fatalErr(fmt, p2, p3) char* fmt;
{
    prMsg(fmt, p2, p3);
    fclose(stdout);
    exit(2);
}
/*********************************************************
 * sub_6AD0  OK PMO  Used in: sub_E43,  sub_3DC9, sub_43EF,
 * Warning message	      sub_54B6, sub_5CF5, sub_600E
 * Difference due to change to use stdarg
 *********************************************************/
void prWarning(fmt, p2, p3) char *fmt;
{
        if (wflag == 0) 
	    prMsg(fmt, p2,p3);
}

/*********************************************************
 * prError OK PMO
 * Nonfatal error
 * Difference due to use of stdarg
 *********************************************************/
void prError(fmt, p2, p3) char * fmt;
{
    prMsg(fmt, p2, p3);

    if (++errcnt >= MAXERR)
        fatalErr("Too many errors");
}

/*********************************************************
 * prMsg OK PMO	      	  Used in: ferror, prError
 * Difference due to use of stdarg
 *********************************************************/
void prMsg(fmt, p2, p3) char *fmt;
{
    fprintf(stderr, "%s:%d:\t", progname, lineno);
    fprintf(stderr, fmt, p2, p3);
    fputc('\n', stderr);
}

/*********************************************************
 * allocMem OK    Used in: sub_265,  sub_1754, sub_19C1,
 *			     sub_1B0C, allocNode, sub_39CA,
 * alloc_mem & return ptr    sub_4192, sub_508A, sub_5DF6
 *********************************************************/
void *allocMem(size_t size) {
    register char *ptr;

    do {
        if (ptr = malloc(size))
            goto done;
    } while (releaseNodeFreeList());
    fatalErr("Lower RAM full");
done:
    blkclr(ptr, size);
    return ptr;
}

char* MyAllocStr(size_t size)
{
    register char *ptr;

    if (ptr = myallocstr(size))
      return ptr;

    fatalErr("Upper RAM full");
    return 0;
}

char* MyAllocMbr(void)
{
    register char *ptr;

    if (ptr = myallocmbr())
      return ptr;

    fatalErr("Upper RAM full");
    return 0;
}

/* end of file main.c */

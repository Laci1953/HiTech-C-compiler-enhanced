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
    exit(2);
}

/*********************************************************
 * prError OK PMO
 * Nonfatal error
 * Difference due to use of stdarg
 *********************************************************/
void prError(char* p)
{
    prMsg(p);

    if (++errcnt >= MAXERR)
        fatalErr("Too many errors");
}

/*********************************************************
 * sub_6AD0  OK PMO  Used in: sub_E43,  sub_3DC9, sub_43EF,
 * Warning message	      sub_54B6, sub_5CF5, sub_600E
 * Difference due to change to use stdarg
 *********************************************************/
void prWarning(char* p)
{
        if (wflag == 0) 
	    prMsg(p);
}

/*********************************************************
 * main OK++ PMO
 *
 *********************************************************/
int main(int argc, char **argv) {
    char tmp[80];

#ifdef CHECKSUM
	cksText0 = CksText();
	cksData0 = CksData();
#endif

    InitDynM();	

    --argc, ++argv;
    while (argc > 0 && **argv == '-') { /* Parsing options */
        switch (argv[0][1]) {
//        case 'N':
//        case 'n':
//            nflag = true;
//            break; /* Enables statistics printing */
//        case 'P':
//        case 'p':
//            pflag = true;
//            break; /* Not use */
        case 'W':
        case 'w':
            wflag = true;
            break; /* Displaying warnings */
        case 'R':
	case 'r':
            rflag = true;
            break;
//        case 'B':
//            bflag = true;
//            break; /* Not use */
//        case 'E':
//            eflag = true;
//            break; /* Not use */
//        case 'H':
//            hflag = true;
//            break; /* Not use */
        default:
	    sprintf(tmp, "Illegal switch %s", argv[0]);
            fatalErr(tmp);
            break;
        }
        argv++, argc--;
    }

    if (argc-- > 0) {
        if (freopen(*argv, "r", stdin) == NULL) /* Open input file */
	{
	    sprintf(tmp, "Can not open %s", *argv);
            fatalErr(tmp);
	}
        else if (argc > 0 && freopen(argv[1], "w", stdout) == NULL) /* Open output file */
	{
	    sprintf(tmp, "Can not create %s", argv[1]);
            fatalErr(tmp);
	}
    }

    sub_1680();                  /* First_init */
    parseStmt();                 /* Compiling intermediate code */

    if (fclose(stdout) == EOF) { /* Close output file */
        prError("Error closing output file");
    }

//    if (nflag)
//	fprintf(stderr, "%d KB used from the upper 64KB RAM\r\n", (unsigned short)TotalSize() / 1024);

#ifdef CHECKSUM
	Check();
#endif

    /* Exit with error code */
    exit(errcnt != 0); /* Generated code is not significantly different */
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

/*********************************************************
 * sub_1680 OK++ PMO			    Used in: main
 *
 * first_init
 *********************************************************/
#define NVARS 14

struct type {
    char *t_str;
    int t_size;
    int t_alig;
    char t_flag;
};

void sub_1680() {
    member_t *sb;
    int16_t cnt;
    register struct type *tp;

    /*
     *	Initializaion of type pointers
     */
    static struct type vars[NVARS] = { /* sub_1680 */
                                       { "i", 2, 0, 1 },  { "s", 2, 0, 1 },  { "c", 1, 0, 1 },
                                       { "l", 4, 0, 1 },  { "ui", 2, 0, 2 }, { "us", 2, 0, 2 },
                                       { "uc", 1, 0, 2 }, { "ul", 4, 0, 2 }, { "f", 4, 0, 3 },
                                       { "d", 4, 0, 3 },  { "x", 2, 0, 1 },  { "ux", 2, 0, 2 },
                                       { "b", 0, 0, 0 },  { "v", 0, 0, 0 }
    };

    /* Clear hash table */

    blkclr((char *)hashtab, sizeof(hashtab));

    /* Create a hash table of templates for standard types */

    cnt = NVARS;
    tp  = vars;
    do {
        sb          = sub_265(tp->t_str);
        /* sb->b_class = TYPE; */
	PutByte((char*)sb, OFF_b_class, TYPE);
        /* sb->b_off   = tp->t_alig; */
	PutWord((char*)sb, OFF_b_off, tp->t_alig);
        /* sb->b_size  = tp->t_size; */
	PutWord((char*)sb, OFF_b_size, tp->t_size);
        /* sb->b_flag  = tp->t_flag; */
        PutByte((char*)sb, OFF_b_flag, tp->t_flag); 
        tp++;
    } while (--cnt != 0);

    /* Additional patterns for types */

    typeLong   = sub_265("l");  /* long	  	*/
    typeDouble = sub_265("d");  /* double 	*/
    typeB      = sub_265("b");  /* b   	  	*/
    typeVar    = sub_265("v");  /* variable 	*/
    typeChar   = sub_265("c");  /* char   	*/
    typeUChar  = sub_265("uc"); /* uint8_t  	*/
    typeX      = sub_265("x");  /* x      	*/
    lvlidx     = -1;
}

/* end of file main.c */

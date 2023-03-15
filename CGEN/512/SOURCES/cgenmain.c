/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

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
 * 10-Jul-2022
 */

#include "cgen512.h"
#include "dynm512.h"
#include "overlays.h"

// PSECT top - begin -------------------------------------------

void top_main(int argc, char **argv)
{
    if (!InitOverlays())
	fatalErr("Can't open overlays");

    Init512Banks();

    --argc, ++argv;
    while (argc > 0 && **argv == '-') { /* Parsing options */
        switch (argv[0][1]) {
        case 'P':
        case 'p':
            pflag = true;
            break; /* Not use */
        case 'W':
        case 'w':
            wflag = true;
            break; /* Displaying warnings */
        case 'R':
            rflag = true;
            break;
        case 'B':
            bflag = true;
            break; /* Not use */
        case 'E':
            eflag = true;
            break; /* Not use */
        case 'H':
            hflag = true;
            break; /* Not use */
        default:
            fatalErr("Illegal\tswitch %c", argv[0][1]);
            break;
        }
        argv++, argc--;
    }

    if (argc-- > 0) {
        if (freopen(*argv, "r", stdin) == NULL) /* Open input file */
            fatalErr("Can't open %s", *argv);
        else if (argc > 0 && freopen(argv[1], "w", stdout) == NULL) /* Open output file */
            fatalErr("Can't create %s", argv[1]);
    }
}

#define NVARS 14

struct type {
    char *t_str;
    int t_size;
    int t_alig;
    char t_flag;
};

    /*
     *	Initializaion of type pointers
     */
    struct type vars[NVARS] = { /* sub_1680 */
                                       { "i", 2, 0, 1 },  { "s", 2, 0, 1 },  { "c", 1, 0, 1 },
                                       { "l", 4, 0, 1 },  { "ui", 2, 0, 2 }, { "us", 2, 0, 2 },
                                       { "uc", 1, 0, 2 }, { "ul", 4, 0, 2 }, { "f", 4, 0, 3 },
                                       { "d", 4, 0, 3 },  { "x", 2, 0, 1 },  { "ux", 2, 0, 2 },
                                       { "b", 0, 0, 0 },  { "v", 0, 0, 0 }
    };


/*********************************************************
 * sub_1680 OK++ PMO			    Used in: main
 *
 * first_init
 *********************************************************/
void sub_1680() {
    member_t *sb;
    int16_t cnt;
    register struct type *tp;

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

// PSECT top - end -------------------------------------------

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

    /* Exit with error code */
    return (errcnt != 0); /* Generated code is not significantly different */
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
    quit(2);
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
    } while (/* releaseNodeFreeList() */ (bool)CallOverlay0(releaseNodeFreeList, 32+5));
    fatalErr("Out of memory in lower RAM");
done:
    blkclr(ptr, size);
    return ptr;
}

char* MyAlloc(size_t size)
{
    register char *ptr;

    if (ptr = alloc512(size))
    {
	ZeroFill(ptr, size);
      	return ptr;
    }

    fatalErr("Out of memory in upper RAM");
    return 0;
}

int fgetchar() {
    return fgetc(stdin);
}

int fputchar(int c) {
    fputc(c, stdout);
}



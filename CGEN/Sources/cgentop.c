/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128/512KB systems by Ladislau Szilagyi   *
*		ladislau_szilagyi@yahoo.com	      *
*	   December 2022 - April 2025		      *
******************************************************/

// Compile-it using -s then edit assembler file changing
// psect * to psect top
// and assemble-it
// At link, this code will be placed above the BSS
// to gain some space for malloc's

#include "cgen.h"
#include "alloccg.h"

void top_main(int argc, char **argv)
{
    char tmp[80];
    InitDynM();	

#ifdef CPM
    baseHeap = sbrk(0); /* Current highest memory */
#endif
    --argc, ++argv;
    while (argc > 0 && **argv == '-') { /* Parsing options */
        switch (argv[0][1]) {
//        case 'N':
//        case 'n':
//            nflag = true;
//            break; /* Enables statistics printing */
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


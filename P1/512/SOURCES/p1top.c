/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

extern bool s_opt;
extern bool w_opt;
extern bool l_opt;
extern char *crfFile;
extern char crfNameBuf[30];
extern char *srcFileArg;
extern char srcFile[100];
extern char *tmpFile;
extern FILE *crfFp;
extern FILE *tmpFp;
extern expr_t **s13SP;   
extern expr_t *s13Stk[20];
extern expr_t s13_9d1b; 
extern expr_t s13_9d28; 
extern expr_t *s13FreeList;

extern header hashtab[];
extern header SymList;

char* rindex(char*, char);
void *xalloc(size_t size);

void maintop(int argc, char *argv[]) 
{
    register char *st;

    for (--argc, ++argv; argc && *argv[0] == '-'; --argc, argv++) 
    {
        switch (argv[0][1]) 
	{
        case 'E':
        case 'e':
            freopen(*argv + 2, "a", stderr);
            setbuf(stderr, errBuf);
            break;
        case 'S':
        case 's':
            s_opt = true;
            break;
        case 'W':
        case 'w':
            w_opt = true;
            break;
        case 'L':
        case 'l':
            l_opt = true;
            break;
        case 'C':
        case 'c':
            if (argv[0][2])
                crfFile = argv[0] + 2;
            else
                crfFile = crfNameBuf;
            break;
        }
    }

    if (argc) 
    {
        if (freopen(argv[0], "r", stdin) == 0)
            fatalErr("can't open %s", *argv);
        srcFileArg = argv[0];
        strcpy(srcFile, srcFileArg);
        if (argc != 1 && freopen(argv[1], "w", stdout) == NULL)
            fatalErr("can't open %s", argv[1]);
        if (argc == 3)
            tmpFile = argv[2];
    } 
    else
        strcpy(srcFile, srcFileArg = "(stdin)");

    if (crfFile) 
    {
        if (*crfFile == '\0') 
	{
            crfFile = crfNameBuf;
            strcpy(crfNameBuf, srcFile);
            if ((st = rindex(crfNameBuf, '.')))
                strcpy(st, ".crf");
            else
                strcat(crfNameBuf, ".crf");
        }
        if (!(crfFp = fopen(crfFile, "a")))
            prWarning("Can't create xref file %s", crfFile);
        else
            fprintf(crfFp, "~%s\n", srcFile);
    }

    if (!(tmpFp = fopen(tmpFile, "w")))
        fatalErr("can't open %s", tmpFile);

    s13_9d28.tType    = T_ICONST;
    s13_9d1b.tType    = T_ICONST;
    s13_9d1b.attr.dataType = s13_9d28.attr.dataType = DT_INT;
    s13_9d1b.t_l      = 0;
    s13_9d28.t_l      = 1;
}

/**************************************************
 * 16: 07E3 PMO +++
 * optimiser removes call csv & jp cret
 **************************************************/
void sub_07e3(void) 
{
    s13SP       = &s13Stk[20];
    s13FreeList = 0;
}

void InitHash(void)
{
	int n;
	header* h;

	for (n = 0; n < HASHTABSIZE; n++)
	{
		h = &hashtab[n];
		h->first = h->last = h;
	}

	SymList.first = SymList.last = &SymList; 
}


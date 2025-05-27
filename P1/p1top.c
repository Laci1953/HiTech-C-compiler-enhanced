#include "p1.h"

void InitDynM();
void InitHash();
char* rindex(char*, char);

void runtop(int argc, char *argv[])
{
    register char *st;

    InitDynM();	//init dynamic memory
    InitHash(); //init hashtable

    for (--argc, ++argv; argc && *argv[0] == '-'; --argc, argv++) {
        switch (argv[0][1]) {
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
//	case 'N':
//	case 'n':
//	    n_opt = true;
//	    break;
        }
    }

    if (argc) {
        if (freopen(argv[0], "r", stdin) == 0)
            fatalErr(100+95, *argv);
        srcFileArg = argv[0];
        strcpy(srcFile, srcFileArg);
        if (argc != 1 && freopen(argv[1], "w", stdout) == NULL)
            fatalErr(100+95, argv[1]);
        if (argc == 3)
            tmpFile = argv[2];
    } else
        strcpy(srcFile, srcFileArg = "(stdin)");

    if (crfFile) {
        if (*crfFile == '\0') {
            crfFile = crfNameBuf;
            strcpy(crfNameBuf, srcFile);
            if ((st = (char*)rindex(crfNameBuf, '.')))
                strcpy(st, ".crf");
            else
                strcat(crfNameBuf, ".crf");
        }
        if (!(crfFp = fopen(crfFile, "a")))
            prWarning(100+92, crfFile);
        else
            fprintf(crfFp, "~%s\n", srcFile);
    }
    if (!(tmpFp = fopen(tmpFile, "w")))
        fatalErr(100+95, tmpFile);

    s13_9d28.tType    = T_ICONST;
    s13_9d1b.tType         = T_ICONST;
    s13_9d1b.attr.dataType = s13_9d28.attr.dataType = DT_INT;
    s13_9d1b.t_l      = 0;
    s13_9d28.t_l      = 1;
}

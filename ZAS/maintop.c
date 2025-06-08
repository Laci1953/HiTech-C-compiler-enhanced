#include "zas.h"

extern char phase;           /* 9e3a */
extern bool j_opt;           /* 9e59 */
extern bool n_opt;           /* 9e68 */
extern bool x_opt;           /* 9e69 */
char *objFileName;    /* 9e6c */
char *asmFileName;    /* 9e6c */
extern FILE *objFp;          /* 9e6e */
extern FILE *asmFp;

void SetupSymTab(void);
void InitDynM(void);

//char *fname(char *name) {
//    char *s;
//    return (s = strchr(name, ':')) ? s + 1 : name;
//}

void maintop(int argc, char **argv)
{
    InitDynM();

    for (--argc, ++argv; argc > 0 && **argv == '-'; ++argv, --argc) {
        switch (argv[0][1]) {
        case 'J':
        case 'j': /* Attempt to optimize jumps to branches */
            j_opt = true;
            break;
       case 'N':
        case 'n': /* Ignore arithmetic overflow in expressions */
            n_opt = true;
            break;
        case 'O':
        case 'o': /* Place the object code in file */
            objFileName = &argv[0][2];
            //if (*objFileName == '\0')
            //    fatalErr("No arg to -o");
            break;
        case 'X':
        case 'x': /* Prevent inclusion of local symbols in object file */
            x_opt = true;
            break;
        default:
            //fatalErr("Illegal switch %c", argv[0][1]);
            break;
        }
    }
    //if (argc <= 0)
    //    fatalErr("No file arguments");

/*-------------- -Ofile.obj is mandatory when using C command line -------------------------
    if (!objFileName /* || c_opt ) 
    { /* PMO fix incase -O and -C specified 
        char *name = fname(argv[0]);
        char *dot = strrchr(name, '.');
        extPt     = dot ? (dot - name) : strlen(name);
        if (extPt > 25)
            extPt = 25;
        if (!objFileName) {
            strncpy(objNameBuf, name, extPt);
            strcat(objNameBuf, ".obj"); /* relies on objNameBuf having all zeros at start 
            objFileName = objNameBuf;
        }
    }
--------------------------------------------------------------------------------------------*/

    asmFileName = argv[0];

    if (!(asmFp = fopen(asmFileName, "r")))
        fatalErr("Can't open file %s", asmFileName);

    if (!(objFp = fopen(objFileName, "wb")))
        fatalErr("Can't create file %s", objFileName);
}


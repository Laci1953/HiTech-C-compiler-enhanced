/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

// use -dMAIN at compile time

#include "p1.h"

char *tmpFile = "p1.tmp"; /* 91db */
char errBuf[128];         /* 9df7 */
FILE *crfFp;              /* 9ff7 */
char crfNameBuf[15];      /* 9ff9 */
char srcFile[15];         /* a017 */
char *crfFile;            /* a07b */
bool s_opt;               /* a07d */
bool w_opt;               /* a07e */
int16_t lineNo;           /* a07f */
char *srcFileArg;         /* a081 */
bool l_opt;               /* a083 */
FILE *tmpFp;              /* a084 */
char inBuf[128];          /* a086 */
int16_t errCnt;           /* a286 */
//bool n_opt;

void sub_4d92(void);
void sub_07e3(void);
bool releaseNodeFreeList(void);
bool s13ReleaseFreeList(void);
uint8_t yylex(void);
int rindex(char*, char);
void sub_3adf(void);

void runtop(int argc, char *argv[]);

/**************************************************
 * 71: 367E PMO +++
 * basic blocks located differently
 * strcpy 2nd arg optimisation missed
 **************************************************/
int main(int argc, char *argv[]) {

    runtop(argc, argv);

    sub_07e3();
    sub_3abf();
    copyTmp();

    if (fclose(stdout) == -1)
        prError(93);
    closeFiles();

//    if (n_opt)
//	fprintf(stderr, "%d KB used from the upper 64KB RAM\r\n", (unsigned short)TotalSize() / 1024);

    exit(errCnt != 0);
}

/**************************************************
 * 72: 3936 PMO +++
 **************************************************/
// prints p2 & p3 using format p1
void prMsg(p1, p2, p3) char *p1;
{
    char buf[128];

    sprintf(buf, p1, p2, p3);
    prMsgAt(buf);
}

void format_and_print(int p1, char* p2, char* p3, char* p4, char* msg) 
{
    char* s = " %s";
    char buf[128];

    if (p1 < 100)
    	sprintf(buf, msg, p1);		// only "Error #" will be printed
    else if (p1 == 100)
	strcpy(buf, s);			// only p2 will be printed
    else if (p1 < 200)
    {
	sprintf(buf, msg, p1-100);
	strcat(buf, s);			// Error # + p2 will be printed 
    }
    else if (p1 == 200 || p1 == 300)
    {				
    	prMsg(p2, p3, p4);		// p3 & p4 will be printed using format p2
	return;
    }
    else
    {
	sprintf(buf, msg, p1-200);
	strcat(buf, s);
	strcat(buf, s);			// Error # + p2 + p3 will be printed
    }
    
    prMsg(buf, p2, p3);
}

/**************************************************
 * 73: 396C PMO +++
 **************************************************/
// p1 = 1...99 		: prints only error # p1
// p1 = 100		: prints string p2
// p1 = 101...199	: prints error # (p1-100) + string p2
// p1 = 200		: prints by format of string p2 the string p3
// p1 = 201...299	: prints error # (p1-200) + string p2 + string p3
// p1 = 300		: prints p3 & p4 by format of p2
void prError(p1, p2, p3, p4) int p1;
{
    format_and_print(p1, (char*)p2, (char*)p3, (char*)p4, "Error #%d");

    ++errCnt;

    fputc('\n', stderr);
}

/**************************************************
 * 74: 399E PMO +++
 **************************************************/
// p1 = 1...99 		: prints only error # p1
// p1 = 100		: prints string p2
// p1 = 101...199	: prints error # (p1-100) + string p2
// p1 = 200		: prints by format of string p2 the string p3
// p1 = 201...299	: prints error # (p1-200) + string p2 + string p3
// p1 = 300		: prints p3 & p4 by format of p2
void fatalErr(p1, p2, p3) int p1;
{
    prError(p1, p2, p3);
    closeFiles();
    exit(1);
}

/**************************************************
 * 75: 39C1 PMO +++
 **************************************************/
// p1 = 1...99 		: prints only warning # p1
// p1 = 100...199	: prints warning # (p1-100) + string p2
// p1 = 200...299	: prints warning # (p1-200) + string p2 + string p3
// p1 = 300		: prints p3 & p4 by format of p2
void prWarning(p1, p2, p3, p4) int p1;
{
    format_and_print(p1, (char*)p2, (char*)p3, (char*)p4, "Warning #%d");

    fprintf(stderr, " (warning)\n");
}


/**************************************************
 * 76: 39F3 PMO +++
 **************************************************/
void expectErr(char *p) {
    prError(200, "%s expected", p);
}

/**************************************************
 * 77: 3A07 PMO +++
 **************************************************/
void copyTmp(void) {
    int ch;

    fclose(tmpFp);

    if ((tmpFp = fopen(tmpFile, "r")) == NULL)
        fatalErr(200, "Can't reopen %s", tmpFile);

    while ((ch = fgetc(tmpFp)) != EOF)
        fputc(ch, stdout);
}

/**************************************************
 * 78: 3A5E PMO +++
 **************************************************/
void closeFiles(void) {
    fclose(stdin);
    fclose(stdout);
    if (tmpFp) {
        fclose(tmpFp);
        unlink(tmpFile);
    }
    if (crfFp) /* PMO - close missing in original */
        fclose(crfFp);
}
/**************************************************
 * 79: 3A80 PMO +++
 **************************************************/
void *xalloc(size_t size) {
    register char *ptr;

    do {
        if ((ptr = malloc(size)) != NULL)
            goto done;
    } while (releaseNodeFreeList() || s13ReleaseFreeList());
    fatalErr(100, "Out of memory in lower RAM");
done:
    blkclr(ptr, size);
    return ptr;
}

/**************************************************
 * 80: 3ABF PMO +++
 **************************************************/
void sub_3abf(void) {
    uint8_t tok;

    while ((tok = yylex()) != T_EOF) {
        ungetTok = tok;
        sub_3adf();
    }
    checkScopeExit();
}


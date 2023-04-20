/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

/*
 * File - getchar.c
 * simple file used for function versions of getchar & putchar
 */

#include "cgen.h"

#ifdef CPM
int fgetchar() {
    return fgetc(stdin);
}
int fputchar(int c) {
    fputc(c, stdout);
}
#else
#if !defined(_WIN32)
/* assume unix convention and ingore \r on input and add \r on output */
int fgetchar() {
    int c;
    while ((c = fgetc(stdin)) == '\r')
        ;
    return c;
}
int fputchar(int c) {
    if (c == '\n')
        fputc('\r', stdout);
    return fputc(c, stdout);
}
#endif
#endif
/* end of file getchar.c */

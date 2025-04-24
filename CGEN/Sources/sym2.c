/******************************************************
*     Decompiled by Mark Ogden & Andrey Nikitin       *
* Adapted to 128/512KB systems by Ladislau Szilagyi   *
*		ladislau_szilagyi@yahoo.com	      *
*	   December 2022 - April 2025		      *
******************************************************/

/*
 * File - sym2s.c
 */

#include "cgen.h"

int        sub_13D(int, int);
uint8_t    sub_6589(int, int);
uint8_t    sub_47B2(node_t *, int);
int        sub_66BC(int, int, int, char *);
uint8_t    sub_63B8(int, int, int);
int        sub_2B2A(node_t *);
uint8_t    sub_2B79(node_t *);

/* macros to make reading the code easier */
/* clang-format off */
/* don't reformat as hitech cpp includes the extra spaces
   which can cause line overflow
*/
#ifndef TOPBIT
#define TOPBIT 0x8000
#define TopBitSet(n) ((n)&TOPBIT)
#define MapVal(n) (TopBitSet(n)?~(n):array_AAE8[n])
#endif
/* clang-format on */

/********************************************************************************************

static int array_A162[] = {
    0,    9,    -10,  25,   -28, 21,   25,   -28,  7,    21,   25,   28,   -29,  11,   12,
    13,   14,   -15,  4,    5,   21,   25,   -28,  6,    21,   25,   -28,  1,    21,   -28,
    17,   20,   21,   22,   25,  26,   28,   -29,  3,    21,   25,   -28,  4,    5,    20,
    21,   25,   -28,  2,    21,  25,   -28,  19,   23,   24,   -27,  20,   -28,  239,  -240,
    133,  -134, 46,   -47,  43,  44,   46,   -47,  43,   44,   46,   47,   -48,  43,   44,
    -47,  43,   44,   45,   46,  47,   -48,  43,   44,   45,   46,   -47,  45,   -47,  116,
    117,  118,  119,  120,  121, 122,  123,  124,  127,  130,

    131,  -132, 117,  118,  120, 125,  128,  131,  -132, 117,  118,  120,  126,  129,  131,
    -132, 245,  -246, 212,  214, -217, 218,  -221, 228,  229,  230,  231,  232,  233,  234,
    235,  -236, 211,  -227, 224, 225,  -226, 214,  -219, 213,  -214, 135,  -136, 71,   72,
    73,   -97,  93,   -94,  77,  78,   79,   80,   81,   -82,  85,   -86,  75,   -83,  73,
    -91,  87,   -88,  30,   -31, 145,  146,  -147, 143,  -144, 175,  -178, 161,  -207, 185,
    -204, 162,  163,  182,  183, 184,  -185, 161,  198,  199,  200,  -201, 173,  -180, 172,
    -181, 161,  185,  186,  187, 205,  206,  208,  209,  -210,

    169,  -171, 159,  165,  185, 194,  195,  196,  -203, 174,  -179, 163,  165,  188,  189,
    191,  -193, 190,  -192, 176, -179, 168,  -171, 177,  -178, 162,  -185, 161,  -163, 248,
    -249, 40,   41,   -42,  38,  39,   40,   41,   -42,  40,   -41,  37,   38,   39,   40,
    41,   -42,  38,   39,   40,  -41,  109,  -110, 243,  -244, 51,   -52,  152,  -153, 53,
    55,   -56,  54,   -55,  107, -108, 59,   60,   -61,  58,   -61,  58,   59,   -61,  105,
    -106, 101,  -102, 101,  102, -103, 156,  -157, 138,  -140
};

*********************************************************************************************/

// 3 bytes per 2 items, instead of 4 bytes

static unsigned char byte_array_A162[] = {
0, 144, 0,
246, 159, 1,
228, 95, 1,
25, 64, 254,
7, 80, 1,
25, 192, 1,
227, 191, 0,
12, 208, 0,
14, 16, 255,
4, 80, 0,
21, 144, 1,
228, 111, 0,
21, 144, 1,
228, 31, 0,
21, 64, 254,
17, 64, 1,
21, 96, 1,
25, 160, 1,
28, 48, 254,
3, 80, 1,
25, 64, 254,
4, 80, 0,
20, 80, 1,
25, 64, 254,
2, 80, 1,
25, 64, 254,
19, 112, 1,
24, 80, 254,
20, 64, 254,
239, 0, 241,
133, 160, 247,
46, 16, 253,
43, 192, 2,
46, 16, 253,
43, 192, 2,
46, 240, 2,
208, 191, 2,
44, 16, 253,
43, 192, 2,
45, 224, 2,
47, 0, 253,
43, 192, 2,
45, 224, 2,
209, 223, 2,
209, 79, 7,
117, 96, 7,
119, 128, 7,
121, 160, 7,
123, 192, 7,
127, 32, 8,
131, 192, 247,
117, 96, 7,
120, 208, 7,
128, 48, 8,
124, 95, 7,
118, 128, 7,
126, 16, 8,
131, 192, 247,
245, 160, 240,
212, 96, 13,
39, 175, 13,
35, 79, 14,
229, 96, 14,
231, 128, 14,
233, 160, 14,
235, 64, 241,
211, 208, 241,
224, 16, 14,
30, 111, 13,
37, 95, 13,
42, 127, 8,
120, 127, 4,
72, 144, 4,
159, 223, 5,
162, 223, 4,
78, 240, 4,
80, 16, 5,
174, 95, 5,
170, 191, 4,
173, 159, 4,
165, 127, 5,
168, 239, 1,
225, 31, 9,
146, 208, 246,
143, 0, 247,
175, 224, 244,
161, 16, 243,
185, 64, 243,
162, 48, 10,
182, 112, 11,
184, 112, 244,
161, 96, 12,
199, 128, 12,
55, 223, 10,
76, 207, 10,
75, 31, 10,
185, 160, 11,
187, 208, 12,
206, 0, 13,
209, 224, 242,
169, 80, 245,
159, 80, 10,
185, 32, 12,
195, 64, 12,
53, 239, 10,
77, 63, 10,
165, 192, 11,
189, 240, 11,
63, 239, 11,
64, 15, 11,
77, 143, 10,
85, 31, 11,
78, 47, 10,
71, 31, 10,
93, 143, 15,
7, 143, 2,
41, 96, 253,
38, 112, 2,
40, 144, 2,
214, 143, 2,
215, 95, 2,
38, 112, 2,
40, 144, 2,
214, 111, 2,
39, 128, 2,
215, 223, 6,
146, 63, 15,
12, 63, 3,
204, 143, 9,
103, 95, 3,
55, 128, 252,
54, 144, 252,
107, 64, 249,
59, 192, 3,
195, 175, 3,
195, 175, 3,
59, 48, 252,
105, 96, 249,
101, 160, 249,
101, 96, 6,
153, 207, 9,
99, 175, 8,
116, 15
};

int getnr(unsigned char* array, int index);

/*********************************************************
 * sub_1F4B OK++ PMO			Used in: sub_2BD0
 * one small block of code moved by optimiser otherwise code
 * identical
 * Note needs standalone ported optimiser to optimise
 *********************************************************/
int sub_1F4B(node_t *p1a, int p2, int p3, int p4, int *p5) {
    int l1, l2, l3, l4, l5, l6;
    uint16_t l7, l8;
    register struct codeFrag_t *sz;
    static int i; /* afed */
    static char *s;
    static char *t;
    static char ch;
    static int m;
    static int n;

    if (p1a->a_c1 == 0)
        p1a->a_c2 = 0;

    if ((l1 = sub_13D(p2, p1a->a_c0)) == 0)
        return (-1);

    if (p4 != 0 && TopBitSet(p4) == 0 && sub_6589(p3, p4) == 0 &&
        (p1a->a_c0 != USEREG || sub_6589(array_AAE8[p1a->info.l], p4) == 0))
        return (-1);

    do {
        if (l1 > 0)
	{
            //i = l5 = array_A162[l1++];
	    i = l5 = getnr(byte_array_A162, l1); l1++;
	}
        else
            i = l5 = l1;

        if (i < 0)
            i = -i;

        sz = &codeFrag[i];

        if (sz->c_2 != 0 && sub_6589(p3, sz->c_2) == 0)
            continue;

        if ((uint8_t)sz->c_3 < 'H' && sz->c_4 != 0 &&
            (sub_13D((uint8_t)sz->c_4, p1a->info.np[0]->a_c0) == 0 ||
             (sz->c_5 != 0 && sub_13D(sz->c_5, p1a->info.np[1]->a_c0) == 0)))
            continue;

        if (sz->c_0 != 0 && sub_47B2(p1a, sz->c_0) == 0)
            continue;

        l6 = sz->c_1;
        l7 = 0;
        if (p4 == 0 && sz->c_3 == 'E')
            p4 = sz->c_1;

        if (p4 != 0 && (!TopBitSet(p4) || l6 == 0 || sub_6589(p4 & 0x7fff, l6) != 0)) {
            if ((s = sz->p_8) != 0) {
                if (s[0] == 'L' && s[1] == 0)
                    s = "GL";
                while (*s) {
                    ch = *s;
                    if (ch == 'X' || ch == 'G' || (ch == 'S' && p1a->a_c0 == USEREG)) {
                        if (isdigit(*++s))
                            t = s;
                        else
                            t = 0;
                        while (*s < 'A')
                            s++;
                        switch (*s) {
                        case 'L':
                            if ((l6 = sub_66BC(l6, p4, p3, t)) == 0)
                                l6 = -1;
                            break;
                        case 'R':
                            l7 = p4;
                            break;
                        case 'N':
                            if (ch == 'S') {
                                if (p1a->a_c0 == USEREG &&
                                    sub_66BC(l6, p4, array_AAE8[p1a->info.l], t) == 0)
                                    l6 = -1;
                            } else if (sub_63B8(p3, p4, sz->c_2) == 0 &&
                                       (!TopBitSet(p4) || sub_66BC(sz->c_2, p4, p3, t) == 0))
                                l6 = -1;
                            break;
                        default:
            		    continue;
                        }
                        break;
                    } else
                        s++;
                }
            } else if ((l6 = sub_66BC(l6, p4, p3, 0)) == 0)
                l6 = -1;
        }

        if (l6 == -1)
            continue;
        l4 = 0;
        if ((uint8_t)sz->c_3 >= 'H') {
            p1a->a_c1 = 0;

            if (sub_1F4B(p1a, (uint8_t)sz->c_3, p3, l6, &l2) < 0) {
                p1a->a_c1 = 0;
                continue;
            }
        } else if (sz->c_4 != 0) {
            p1a->info.np[0]->a_c1 = 0;

            if (sub_1F4B(p1a->info.np[0], (uint8_t)sz->c_4, p3, l6, &l2) < 0)
                continue;
            if (sz->c_5 != 0) {
                p1a->info.np[1]->a_c1 = 0;
                l8                    = sub_2B2A(p1a->info.np[0]);
                if (l7 == 0) {
                    if (sz->c_2 != 0)
                        l7 = array_AAE8[(uint8_t)sub_63B8(p3, p4, sz->c_2)];
                    else
                        l7 = 0;
                    l7 = (l8 | l7) ? (l8 | l7 | 0x8000) : 0;
                }

                if (sub_1F4B(p1a->info.np[1], sz->c_5, p3, l7, &l3) < 0) /* 29B3 */
                    continue;
                l4 = sub_2B2A(p1a->info.np[1]);
                if (l4 & l8) {
                    p1a->info.np[1]->a_c1 = 0;

                    if (sub_1F4B(p1a->info.np[1], sz->c_5, p3 & ~l8, l7, &l3) <= 0)
                        continue;
                    l4 = sub_2B2A(p1a->info.np[1]);
                }
                if ((dopetab[p1a->a_c0] & 0x100) || (l4 & l2)) {
                    if (l8 & l3) {
                        if (dopetab[p1a->a_c0] & 0x100)
                            continue;
                        p1a->a_c2 = 2;
                        l4        = 0;
                    } else
                        p1a->a_c2 = 1;
                } else
                    p1a->a_c2 = 0;
            }
        } else if (l6 != 0)
            p4 = l6;

        if (sz->c_2 != 0) {
            m = p3;
            if (!(sz->c_2 & 0x40) || (n = sub_63B8(sub_2B2A(p1a), p4, sz->c_2)) == 0) {
                if ((sz->c_2 & 0x40) == 0)
                    m &= ~sub_2B2A(p1a);
                m &= ~l4;
                n = sub_63B8(m, p4, sz->c_2);
                if (n == 0 && (n = sub_6589(m, sz->c_2)) == 0)
                    continue;
            }
            p1a->a_uc9[p1a->a_c1] = n;
        } else
            p1a->a_uc9[p1a->a_c1] = 0;

        p1a->a_c3[p1a->a_c1++]    = (uint8_t)(sz - codeFrag);
        n                         = (uint8_t)sub_2B79(p1a);
        p1a->a_reg[p1a->a_c1 - 1] = n;
        if (n == 0 && p1a->a_c0 == USEREG)
            p1a->a_reg[p1a->a_c1 - 1] = n = p1a->info.l;
        /* clang-format off */
        if ((p4 != 0 && !TopBitSet(p4) && (n == 0 || sub_6589(MapVal(p4), n) != n)) ||
            (TopBitSet(p4) && (MapVal(p4) & MapVal(n)) != MapVal(n))) {
                p1a->a_c1--;
                continue;
            }
        /* clang-format on */
        *p5 = array_AAE8[p1a->a_uc9[p1a->a_c1 - 1]];

        if ((uint8_t)sz->c_3 >= 'H' || sz->c_4 != 0)
            *p5 |= l2;
        if (sz->c_5 != 0)
            *p5 |= l3;
        return 1;
    } while (l5 >= 0);
    return -1;
}

/* end of file sym2s.c */

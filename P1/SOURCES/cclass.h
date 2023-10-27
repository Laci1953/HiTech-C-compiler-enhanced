extern char ccClass[129];

#define _Z          0  /* 0000 0000 */
#define _U          1  /* 0000 0001 */
#define _L          2  /* 0000 0010 */
#define _D          4  /* 0000 0100 */
#define _X          8 /* 0100 0000 */
#define _S          16  /* 0001 0000 */

#define Isalpha(c)  ((ccClass + 1)[(int16_t)(c)] & (_U | _L))      /*(and	3) */
#define Isupper(c)  ((ccClass + 1)[(int16_t)(c)] & _U)             /*(bit	0,(hl)) */
#define Islower(c)  ((ccClass + 1)[(int16_t)(c)] & _L)             /*(bit	0,(hl)) */
#define Isdigit(c)  ((ccClass + 1)[(int16_t)(c)] & _D)             /*(bit	2,(hl)) */
#define Isxdigit(c) ((ccClass + 1)[(int16_t)(c)] & _X)             /*(bit	6,(hl)) */
#define Isspace(c)  ((ccClass + 1)[(int16_t)(c)] & _S)             /*(bit	3,(hl)) */
#define Isalnum(c)  ((ccClass + 1)[(int16_t)(c)] & (_U | _L | _D)) /*(and	7) */
#define Tolower(c)  (Isupper(c) ? c + 0x20 : c)


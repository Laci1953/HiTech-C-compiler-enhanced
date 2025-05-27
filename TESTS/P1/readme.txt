The original P1 sources
Compiles on Z80 based CP/M

c>c -v -o p1new.c
HI-TECH C COMPILER (CP/M-80) V3.09
Copyright (C) 1984-87 HI-TECH SOFTWARE
0:CPP -DCPM -DHI_TECH_C -Dz80 -I p1new.C $CTMP1.$$$
0:P1 $CTMP1.$$$ $CTMP2.$$$ $CTMP3.$$$
0:CGEN $CTMP2.$$$ $CTMP1.$$$
0:OPTIM $CTMP1.$$$ $CTMP2.$$$
0:ZAS -J -N -op1new.OBJ $CTMP2.$$$
Z80AS Macro-Assembler V4.8

Errors: 0

Jump optimizations done: 500
Finished.
ERA $CTMP1.$$$
ERA $CTMP2.$$$
ERA $CTMP3.$$$
0:LINK -Z -Ptext=0,data,bss -C100H -Op1new.COM CRTCPM.OBJ p1new.OBJ LIBC.LIB
ERA CGEN.OBJ
ERA $$EXEC.$$$

c>

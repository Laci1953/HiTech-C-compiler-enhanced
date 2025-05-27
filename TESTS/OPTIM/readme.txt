The original OPTIM sources

Compiles on 128KB/512KB RAM Z80 based CP/M computers 

c>c -v -o optimnew.c
HI-TECH C COMPILER (CP/M-80) V3.09
Copyright (C) 1984-87 HI-TECH SOFTWARE
0:CPP -DCPM -DHI_TECH_C -Dz80 -I optimnew.C $CTMP1.$$$
0:P1 $CTMP1.$$$ $CTMP2.$$$ $CTMP3.$$$
0:CGEN $CTMP2.$$$ $CTMP1.$$$
0:OPTIM $CTMP1.$$$ $CTMP2.$$$
0:ZAS -J -N -ooptimnew.OBJ $CTMP2.$$$
Z80AS Macro-Assembler V4.8

Errors: 0

Jump optimizations done: 500
Finished.
ERA $CTMP1.$$$
ERA $CTMP2.$$$
ERA $CTMP3.$$$
0:LINK -Z -Ptext=0,data,bss -C100H -Ooptimnew.COM CRTCPM.OBJ optimnew.OBJ LIBC.LIB
ERA CGEN.OBJ
ERA $$EXEC.$$$

c>

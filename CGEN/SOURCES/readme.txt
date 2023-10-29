How to compile / assemble the source files

Edit alloccg.as to choose the target hardware.

First, compile cgentop.c to obtain cgentop.as
>C -v -o -s cgentop.c

Then, 

1. edit cgentop.as, replacing "psect text" with "psect top"

2. >submit makecgen

3. >submit linkcgen

Now, the CGENNEW.COM is built.

Depending on your hardware/software, it is possible to receive "not enough memory" messages, during the compilation.
In this case, you will have to split the problematic source file in two, or to use a development platform where the enhanced HiTech C is already installed. 

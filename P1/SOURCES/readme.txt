How to compile / assemble the source files

Edit p1alloc.as & p1.h to choose the target hardware.

First, compile p1top.c to obtain p1top.as
>C -v -o -s p1top.c

Then, 

1. edit p1top.as, replacing "psect text" with "psect top"

2. >submit makep1

3. >submit linkp1

Now, the P1NEW.COM is built.

Depending on your hardware/software, it is possible to receive "not enough memory" messages, during the compilation.
In this case, you will have to split the problematic source file in two, or to use a development platform where the enhanced HiTech C is already installed. 

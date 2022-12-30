# HiTech-C-compiler-enhanced
Allowing the HiTech C compiler to be used on 128KB RAM hardware configurations

The HiTech C Compiler v3.09 is one of the best available C compilers that can be used on 8-bit CP/M 2.2 systems.

However, the high quality of this toolset is critically affected by the available RAM memory constraints. It is very frequent that the compiler reports “out-of-memory” error messages for C source files enough small (some hundreds code lines…).

In the same time, currently there are available a lot of Z80 based hardware configurations provided with 128 KB RAM banked memory; of course, Z80 may use only a 64KB address space, but in these hardware configurations, the available 64KB RAM bank can be selected using I/O ports.

This opens the possibility to “enhance” the capabilities of the C compiler, by allowing him to allocate memory in both of the 64 KB RAM banks.

Of course, this is possible only by keeping a small piece of code ( the “shadow” code) at the same address in both of the 64KB RAM banks, usually on the highest available address space; this code will be responsible with moving bytes from/to the two available 64KB RAM banks.

This way, it is possible to “allocate” memory on the “upper” 64 KB RAM bank, in order to store there data for the C compiler.

Of course, this means that the C compiler code must be modified, in order to handle these “special” malloc calls.

Benefiting from the high quality work of Mark Ogden, who published the decompiled C sources on GitHub, I modified these source files to implement this ideea of a enhanced C compiler.

The following components were modified:

- P1.COM
- CGEN.COM
- OPTIM.COM
- ZAS.COM (starting from the Hector Peraza’s ZSM4 published on GitHub)

The included .COM files are built to be used on RC2014 + SC108 hardware configuration.

The resulting C toolset can be customized to be used on any available 128KB RAM Z80 hardware system configuration ( RC2014 provided with SC108, SC114, SC118 or the Phillip Stevens memory module, or any other configuration).

The CP/M must be setup using the PutSys from the folder PutSys. This is necessary because of the "shadow code", that must be stored to high RAM address space.

The results are spectacular.

Large C source files can now be compiled, without any problem (see the TESTS folder for some examples).

The C compiler's modified source files are included.


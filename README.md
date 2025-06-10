# HiTech-C-compiler-enhanced
Enhancing the HiTech C compiler for 128KB RAM / 512KB RAM hardware configurations

The HiTech C Compiler v3.09 is one of the best available C compilers that can be used on 8-bit CP/M 2.2 systems.

However, the high quality of this toolset is critically affected by the available RAM memory constraints. It is very frequent that the compiler reports “out-of-memory” error messages for C source files enough small (some hundreds code lines…).

In the same time, currently there are available a lot of Z80 based hardware configurations provided with 128 KB RAM or 512KB RAM banked memory; of course, Z80 may use only a 64KB address space, but in these hardware configurations, the available extra RAM banks can be selected using I/O ports.

This opens the possibility to “enhance” the capabilities of the C compiler, by allowing him to allocate memory in the extra RAM banks.

For the 2 x 64KB RAM systems, this is possible only by keeping a small piece of code ( the “shadow” code) at the same address in both of the 64KB RAM banks, usually on the highest available address space; this code will be responsible with moving bytes from/to the two available 64KB RAM banks.

This way, it is possible to “allocate” memory on the “upper” 64 KB RAM bank, in order to store there data for the C compiler.

For the 512KB RAM systems, a "buddy-system" memory allocator can allocate memory in this 512KB RAM space.

Of course, this means that the C compiler code must be modified, in order to handle these “special” malloc calls.

Obviously, this means some extra code is executed, compared to the "original" HiTech tools, resulting in a longer execution time.

However, this loss of speed ( 10 to 15% ) is largely compensated by the big advantage obtained: to be able to compile larger C source files.

Benefiting from the high quality work of Andrey Nikitin and Mark Ogden, who published the decompiled C sources of HiTech C compiler's toolset on GitHub, I modified these source files to implement this concept of a enhanced C compiler.

The following components were modified:

- CPP.COM
- P1.COM
- CGEN.COM
- OPTIM.COM
- ZAS.COM

Each of these components has its own folder, containing the sources and the executables.

The resulting C toolset can be used on the following 128KB RAM / 512KB RAM Z80 hardware system configurations:

- RC2014 provided with SC108, SC114, SC118, SC150, SC152 or the Phillip Stevens memory module, or the 512KB RAM + 512KB ROM module, or
- RCBUS-based systems provided with SC707, SC714, or
- Z80ALL (which has 4 x 32KB RAM) or
- Simple80 (2 x 64KB RAM)
- RomWBW (v3.5 or later)

For RC2014 or RCBUS 128KB RAM systems, the CP/M must be setup using one of the files from the folder PutSys. This is necessary because of the "shadow code", that must be stored to high RAM address space.

The results are spectacular.

Large C source files can now be compiled, without any problem (see the TESTS folder for some examples).

The C compiler's modified source files are included.

See "How to install.txt" for the enhanced HiTech C detailed installation procedure

(update on April, 2025)

The RomWBW version is now available. You will need RomWBW v 3.5 (or later).

(update on May 13, 2025)

Fixed a bug in CGEN (related to float type variables)

(update on May 27, 2025)

New version - significant increase on the size of C files that can be compiled

It is possible now to compile all the HiTech C toolchain components (P1, CGEN and OPTIM), each of them represented as a single file!

And those are huge files.

For example, the CGEN source code, as a C file, has 168KB ( over 5000 lines ), 
and the Z80 assembler source file generated during the compilation process has 241KB ( over 23000 lines ).

See the folder TESTS.

(update on June 8, 2025)

New version of ZAS - significant increase on the size of assembler files that can be assembled.


Enhanced ZAS, for Z80 systems provided with 128/512KB RAM (including RomWBW systems), used by the HiTech C compiler.

The labels are now stored in the extended RAM.

IMPORTANT NOTE:

This enhanced version of ZAS must be used ONLY when called from the C compiler (after CPP, P1, CGEN and OPTIM)

It accepts only the following command line options:

c>zas -otest.obj test.as
or
c>zas -j -otest.obj test.as
or
c>zas -j -n -otest.obj test.as

To assemble Z80 source files, outside the framework provided by the C compiler, 
you may still use the original ZAS or Z80AS ( https://github.com/Laci1953/Z80AS ).

Comparing the enhanced ZAS with the original ZAS and Z80AS:

Enhanced ZAS: 
- runs ONLY on 128/512KB RAM systems (including RomWBW systems, starting from v3.5)
- it is called by the C compiler to assemble the output of CGEN (or OPTIM)
- no macros, no include, no conditional assembly
- can handle larger assembler files, compared with the original HiTech's ZAS or even Z80AS

Original ZAS:
- runs on any Z80 CP/M system
- basic support for macros, conditional assembly, includes may be used
- cannot handle large assembler files (e.g. fails to assemble BDOS+BIOS as a single file)

Z80AS:
- runs on any Z80 CP/M system
- extensive support for macros, conditional assembly, includes may be used
- may handle undocumented Z80 instructions
- can handle large assembler files (e.g. can assemble BDOS+BIOS as a single file), but not very large ones

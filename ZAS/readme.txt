Enhanced ZAS, for Z80 systems provided with 128/512KB RAM (including RomWBW systems), used by the HiTech C compiler.

The labels are now stored in the extended RAM.

IMPORTANT NOTE:

This enhanced version of ZAS must be used ONLY when called from the C compiler (after CPP, P1, CGEN and OPTIM)

To assemble Z80 source files, outside the framework provided by the C compiler, 
you may still use Z80AS ( https://github.com/Laci1953/Z80AS ).

Comparing the enhanced ZAS with Z80AS:

Enhanced ZAS: 
- runs ONLY on 128/512KB RAM systems (including RomWBW systems, starting from v3.5)
- it is called by the C compiler to assemble the output of CGEN (or OPTIM)
- no macros, include, conditional assembly
- can handle larger assembler files, compared with the original HiTech's ZAS or even Z80AS

Z80AS:
- runs on any Z80 CP/M system
- macros, conditional assembly, includes may be used
- may handle undocumented Z80 instructions

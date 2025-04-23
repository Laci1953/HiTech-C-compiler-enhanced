The folder contains the enhanced CGEN version, for 128KB RAM / 512KB RAM systems

It improves the old CGEN version, allowing processing of larger C files.

April 22, 2025: 

The new version of CGEN.com is now about the same size as tho original HiTech CGEN.com
This means that more space is available for CGEN, in the main 64KB RAM, to store data related to code generation.
In particular, this makes possible to compile also some particular C programs, 
which use heavy and complex arithmetic computations.

A key factor in improving the GCEN performance was, surprisingly, replacing the original "malloc & free" routines from the LIBC.LIB, with a more performant version of "malloc & free".

You can compare both versions here:

initial_malloc_from_libc.c - the old LIBC.LIB version 
malloc.c - the improved version

Although the improved version is a bit larger in size, it still manages to allocate more efficiently...

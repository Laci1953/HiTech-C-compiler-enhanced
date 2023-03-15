;
;	Szilagyi Ladislau
;
;	ladislau_szilagyi@euroqst.ro
;
;	December 2022 - March 2023
;

	global	_quit
	global 	_main, __Hbss, __Lbss, __argc_, startup

ENTRY	equ	5

	psect	text
				;to gain some RAM space
				;save HiTech's EXEC to RAM bank 7
	ld	hl,(ENTRY+1)
	ld	(execjpbdos),hl	;save exec jp to bdos
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl		;HL = BDOS addr
	ld	sp,hl		;set SP
	ld	(ENTRY+1),hl	;store real BDOS addr to ENTRY+1
	dec	hl		;HL = pointer to last byte of EXEC
	ld	(lastbyteEXEC),hl
	ld	de,0BFFFH	;DE = pointer to last byte of RAM bank 8000-C000
	ld	bc,400H		;1KB
	ld	a,32+7		;select RAM bank 7
	out	(7AH),a		;to 8000H
	lddr			;save EXEC to RAM bank 7
	ld	a,32+2		;re-select RAM bank 2
	out	(7AH),a		;to 8000H
				;
	ld	de,__Lbss	; Start of BSS segment
	or	a		; clear carry
	ld	hl,__Hbss
	sbc	hl,de		; Size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			; Clear memory
	ld	hl,nularg
	push	hl
	ld	hl,80h		; Argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,0
	add	hl,bc
	ld	(hl),0		; zero terminate it
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		; unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
store:
	ld	(80H),hl	;store value returned by main
				;restore EXEC from RAM bank 7
	ld	de,(lastbyteEXEC);DE = pointer to last byte of EXEC
	ld	hl,0BFFFH	;HL = pointer to last byte of RAM bank 8000-C000
	ld	bc,400H		;1KB
	ld	a,32+7		;select RAM bank 32+7
	out	(7AH),a		;to 8000H
	lddr			;restore EXEC from RAM bank 32+7
	ld	a,32+2		;re-select RAM bank 2
	out	(7AH),a		;to 8000H
	ld	hl,(execjpbdos)	;restore EXEC jp to BDOS 
	ld	(ENTRY+1),hl

	jp	0

_quit:	
	pop	hl		;drop ret addr
	pop	hl		;value to return
	jr	store

execjpbdos:	defs	2
lastbyteEXEC:	defs	2

	psect	bss

	GLOBAL	nularg
	GLOBAL	_left
	GLOBAL	_ffile
	GLOBAL	_base
	GLOBAL	__sibuf
	GLOBAL	__argc_
	GLOBAL	buf_81B2
	GLOBAL	memtop

_left:          defs    1       
_ffile:         defs    2      
_base:          defs    1       
__sibuf:        defs    512
__argc_:        defs    2
buf_81B2:	defs    2
memtop: 	defs	2
nularg:		defs	1
		defs	4

        psect   data

	GLOBAL	__iob
	GLOBAL	__fcb
	GLOBAL	_empty_string

_empty_string:
	defb	0

__iob:
	defw    __sibuf
	defw    0
	defw    __sibuf
	defb    9
	defb    0
	defw    0
	defw    0
	defw    0
	defb    6
	defb    1
	defw    0
	defw    0
	defw    0
	defb    6
	defb    2
	defb    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	defb    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	defb    0,0,0,0,0,0,0,0

;       #if     z80
;       #define MAXFILE         8       /* max number of files open */
;       #else   z80
;       #define MAXFILE         15      /* max number of files open */
;       #endif  z80
;       #define SECSIZE         128     /* no. of bytes per sector */
;
;       extern struct   fcb {
;        uchar   dr;             /* drive code */
;        char    name[8];        /* file name */
;        char    ft[3];          /* file type */
;        uchar   ex;             /* file extent */
;        char    fil[2];         /* not used */
;        char    rc;             /* number of records in present extent */
;        char    dm[16];         /* CP/M disk map */
;        char    nr;             /* next record to read or write */
;        uchar   ranrec[3];      /* random record number (24 bit no. ) */
;        long    rwp;            /* + 34 read/write pointer in bytes */
;        uchar   use;            /* use flag */
;        uchar   uid;            /* user id belonging to this file */
;       }       _fcb[MAXFILE];
;
;
__fcb:          ;150h
;stdin
 defb 00H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,00H,00H,00H,00H ;16
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H ;32
 defb 00H,00H
 defb 00H,00H ;34
 defb 00H,00H,00H,00H,04H,00H
;stdout
 defb 00H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,00H,00H,00H,00H ;16
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H ;32
 defb 00H,00H
 defb 00H,00H ;34
 defb 00H,00H,00H,00H,04H,00H
;stderr
 defb 00H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,00H,00H,00H,00H ;16
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H ;32
 defb 00H,00H
 defb 04H,00H ;34
 defb 00H,00H,00H,00H,04H,00H

 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H,00H
 defb 00H,00H,00H,00H,00H,00H,00H,00H,00H


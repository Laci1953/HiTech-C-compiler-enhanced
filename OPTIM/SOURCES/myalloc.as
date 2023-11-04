;
;	Dynamic memory routines
;
;	for 128KB / 512KB RAM
;
;	Szilagyi Ladislau
;
;	October 2023
;
;	only one of ?2X64K , ?512K, ?Z80ALL must be set to 1
;
?2X64K	equ	1	;1 : SC108 or 32KB ROM + 128KB RAM Phillip Stevens memory module or SC114 or Simple80
?512K	equ	0	;1 : 512KB RAM + 512KB EPROM
?Z80ALL equ	0	;1 : Z80ALL 4 x 32KB RAM
;
;	in case ?2X64K is set to 1,
;	only one of SC108, MM, S80 must be set to 1
;
SC108	equ	0	;1 : SC108
MM	equ	0	;1 : 32KB ROM + 128KB RAM Phillip Stevens memory module or SC114
S80	equ	1	;1 : Simple80 
;
;	Dynamic memory allocator 
;	Data access routines
;	Shadow routines
;
	GLOBAL	_InitDynM
	GLOBAL	_myalloc
	GLOBAL	_GetByte
	GLOBAL	_PutByte
	GLOBAL	_GetWord
	GLOBAL	_PutWord
	GLOBAL	_ReadMem
	GLOBAL	_WriteMem
	GLOBAL	_IsValid

;-------------------------------------------------------------2X64K
IF	?2X64K
;
DYNM_START	equ	1
IF 	SC108 .or. MM
DYNM_END	equ	0FF63H
ENDIF
IF	S80
DYNM_END	equ	0FF00H
ENDIF

IF 	SC108 .or. MM

$ReadByte 	equ 	0FF63H
$WriteByte	equ 	0FF6DH
$ReadWord	equ 	0FF77H
$WriteWord	equ 	0FF83H
$WriteString	equ 	0FF8FH
$ReadString	equ 	0FFA1H
$StringLen	equ	0FFB2H
$ReadMem	equ	0FFC7H
$WriteMem	equ	0FFD7H
$FillZero	equ	0FFE7H

ELSE

$ReadByte	equ        0FF00H
$WriteByte	equ        0FF11H
$ReadWord	equ        0FF22H
$WriteWord	equ        0FF35H
$WriteString	equ        0FF48h
$ReadString	equ        0FF61H
$StringLen	equ        0FF79H
$ReadMem        equ        0FF95H
$WriteMem       equ        0FFACH
$FillZero       equ        0FFC3H

ENDIF

FREE		equ	0
ALLOCATED	equ	0FFH
;
;	allocated/free bloc header structure
;
STATUS		equ	0	;+0	byte - status (0=free,FF=allocated)
				;+1	data - 6 bytes
;
HEADERSIZE	equ	1
BLOCKSIZE	equ	7	;1+6
;
MACRO	PutByte
	call	$WriteByte
ENDM

MACRO	GetByte
	call	$ReadByte
ENDM

MACRO	PutWord
	call	$WriteWord
ENDM

MACRO	GetWord
	call	$ReadWord
ENDM

;---------------------------------------------------SC108
IF	SC108

MEMP_PORT       equ     38H

;       ROM     0000 to 8000H
;
ROM_OUT_CMD     equ     00000001B
ROM_IN_CMD      equ     00000000B
LOWER_64RAM     equ     00000000B
UPPER_64RAM     equ     10000000B
;
MACRO	LOW_RAM
	ld	a,LOWER_64RAM .or. ROM_OUT_CMD
	out	(MEMP_PORT),a
ENDM

MACRO	UP_RAM
	ld	a,UPPER_64RAM .or. ROM_OUT_CMD
	out	(MEMP_PORT),a	
ENDM

MACRO	ROM_IN
	ld	a,LOWER_64RAM .or. ROM_IN_CMD
	out	(MEMP_PORT),a
ENDM

MACRO	ROM_OUT
	ld	a,LOWER_64RAM .or. ROM_OUT_CMD
	out	(MEMP_PORT),a
ENDM

ENDIF
;---------------------------------------------------SC108

;---------------------------------------------------MM
IF	MM

MM_RAM_P	equ	30H

MM_UP_RAM	equ	1
MM_LOW_RAM	equ	0

MACRO	LOW_RAM
	ld	a,MM_LOW_RAM
	out	(MM_RAM_P),a
ENDM

MACRO	UP_RAM
	ld	a,MM_UP_RAM
	out	(MM_RAM_P),a
ENDM

MM_ROM_P	equ	38H

MM_ROM_IN	equ	0
MM_ROM_OUT	equ	1

MACRO	ROM_IN
	ld	a,MM_ROM_IN
	out	(MM_ROM_P),a
ENDM

MACRO	ROM_OUT
	ld	a,MM_ROM_OUT
	out	(MM_ROM_P),a
ENDM

ENDIF
;---------------------------------------------------MM

	psect bss

TOP:	defs    2	        ;top available space

	psect	top

;---------------------------------------------------SC108 or MM
IF	SC108 .or. MM

SHARED_SET:defb	0		;0 : NOT set, 1 : SET

;
;	Init dynamic memory
;
_InitDynM:
				;is shared code set?
	ld	hl,SHARED_SET
	ld	a,(hl)
	or	a
	jr	nz,isset
	inc	a		;no, set-it
	ld	(hl),a
	push	ix		;save IX
	call	StoreShared	;setup shadow routines
	pop	ix		;restore IX
isset:	
	ld	hl,DYNM_END
	ld	(TOP),hl        ;set top available space
	ret
;
;	Store shared code to Upper & Lower RAM
;
;	SP must be set at top of TPA 
;
StoreShared:
				;move shared code to Lower RAM
	ld	de,SHARED	;to
	ld	hl,SHARED_START	;from
	ld	bc,SHARED_END - SHARED_START
	ldir
				;move "mover" to 0A000H
	ld	de,0A000H	;to
	ld	hl,mover_start	;from
	ld	bc,mover_end - mover_start
	ldir
	jp	0A000H		;call "mover" to store shared code to Upper RAM and return
;
mover_start:
				;move shared code to Upper RAM
	ROM_IN
				;first search at 7F00H
	ld	hl,7F00H
	ld	a,(hl)
	cp	0c3H
	jr	nz,searchscm
	inc	hl
	inc	hl
	inc	hl
	ld	a,(hl)
	cp	0c3H
	jr	nz,searchscm
				;found "mover" in CPM BOOT EPROM
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=address of "move 1 byte to upper RAM" routine
	jp	moveit
;
searchscm:
				;search "move 1 byte to upper RAM" routine in SCM
	ld	hl,69H
searchjp:
	ld	a,(hl)
	cp	0c3H
	jr	z,jpfound
	inc	hl
	jr	searchjp
jpfound:
	ld	b,6
searchj7thjp:
	inc	hl
	inc	hl
	inc	hl
	ld	a,(hl)
	cp	0c3H
	jr	nz,searchjp
	djnz	searchj7thjp
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl
	ld	bc,2bH
	add	hl,bc
	add	hl,bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl		;HL=LTOUP_SCM
moveit:				
	push	hl
	pop	iy		;IY=SCM function to move A --> UpperRAM in (DE)
	ld	ix,SHARED	;from
	ld	de,SHARED	;to
	ld	a,SHARED_END - SHARED_START
looptoup:
	push	af
	ld	a,(ix+0)
	ld	hl,retadr - mover_start + 0A000H
	push	hl
	jp	(iy)
retadr:	inc	ix
	inc	de
	pop	af
	dec	a
	jr	nz,looptoup
	ROM_OUT
	ret
;
mover_end:
;---------------------------------------------------------------------------------------
;	stored at 0FF63H in both Lower & Upper RAM
;
SHARED	equ	DYNM_END

SHARED_START:
;
;	Read one byte from Upper 64KB RAM
;
;	HL=address of the byte
;
;	returns C=byte
;
;	affects reg A
;
$$ReadByte:
	UP_RAM
	ld	c,(hl)
	LOW_RAM
	ret
;
;	Write one byte to Upper 64KB RAM
;
;	HL=address of the byte
;	C=byte
;
;	affects reg A
;
$$WriteByte:
	UP_RAM
	ld	(hl),c
	LOW_RAM
	ret
;
;	Read one word from Upper 64KB RAM
;
;	HL=address of the byte
;
;	returns BC=word
;
;	affects reg A,HL
;
$$ReadWord:
	UP_RAM
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	LOW_RAM
	ret
;
;	Write one word to Upper 64KB RAM
;
;	HL=address of the byte
;	BC=word
;
;	affects reg A
;
$$WriteWord:
	UP_RAM
	ld	(hl),c
	inc	hl
	ld	(hl),b
	LOW_RAM
	ret
;
;	WriteString to Upper 64KB RAM
;
;	DE=source (in Lower RAM)
;	HL=destination (in Upper RAM)
;
;	Reg C affected
;
$$WriteString:
	ld	a,(de)
	ld	c,a
	UP_RAM
	ld	(hl),c
	LOW_RAM
	ld	a,c
	or	a
	ret	z
	inc	de
	inc	hl
	jr	$$WriteString
;
;	ReadString from Upper 64KB RAM
;
;	HL=source (in Upper RAM)
;	DE=destination (in Lower RAM)
;
;	Reg C affected
;
$$ReadString:
	UP_RAM
	ld	c,(hl)
	LOW_RAM
	ld	a,c
	ld	(de),a
	or	a
	ret	z
	inc	de
	inc	hl
	jr	$$ReadString
;
;	StringLen from Upper 64KB RAM
;
;	HL = string (in Upper RAM)
;
$$StringLen:
	ld	b,0FFH
$$loop:	UP_RAM
	ld	c,(hl)
	LOW_RAM
	inc	hl
	inc	b
	ld	a,c
	or	a
	jr	nz,$$loop
	ld	h,0
	ld	l,b
	ret
;
;	Read up to 256 bytes from the Upper 64KB RAM
;
;	HL = source (in Upper RAM)
;	DE = dest (in Lower RAM)
;	B = count (256 if 0)
;
;	returns HL=HL+B
;		DE=DE+B
;
$$ReadMem:
	UP_RAM
	ld	c,(hl)
	LOW_RAM
	ld	a,c
	ld	(de),a
	inc	hl
	inc	de
	djnz	$$ReadMem
	ret
;
;	Write up to 256 bytes from the Upper 64KB RAM
;
;	HL = dest (in Upper RAM)
;	DE = source (in Lower RAM)
;	B = count (256 if 0)
;
;	returns HL=HL+B
;		DE=DE+B
;
$$WriteMem:
	ld	a,(de)
	ld	c,a
	UP_RAM
	ld	(hl),c
	LOW_RAM
	inc	hl
	inc	de
	djnz	$$WriteMem
	ret
;
;	Fill with 00H in Upper 64KB RAM
;
;	HL = start
;	B = count
;
$$FillZero:
	UP_RAM
	xor	a
1:	ld	(hl),a
	inc	hl
	djnz	1b
	LOW_RAM
	ret
;	
SHARED_END:			;at 0FFF6H
;---------------------------------------------------------------------------------------

ENDIF
;---------------------------------------------------SC108 or MM

;---------------------------------------------------Simple80
IF	S80

;
;	Init dynamic memory
;
_InitDynM:
	ld	hl,DYNM_END
	ld	(TOP),hl        ;set top available space
        ret

ENDIF
;---------------------------------------------------Simple80
;
;	myalloc
;
;void*	myalloc(void)
;
;	returns addr of allocated data block
;		or NULL
_myalloc:
        ld      hl,(TOP)	;get top of available memory
        ld      a,h
        or      a		;if < 100H
	jr	z,full			
				;CARRY=0
	ld	bc,BLOCKSIZE	;size of data
	sbc	hl,bc
        ld      (TOP),hl
	ld	c,ALLOCATED	;mark status = allocated
	PutByte
	inc	hl
	ld	d,h
	ld	e,l		;HL = DE = data pointer
	ld	bc,0		;erase data
	PutWord
	inc	hl
	PutWord
	inc	hl
	PutWord
	ex	de,hl		;HL = data pointer
	ret			;return data pointer
full:	
	ld	hl,0		;return NULL
	ret
;
;	IsValid - is the address allocated?
;
;	bool IsValid(char* addr)
;
_IsValid:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=addr
	ld	a,(TOP+1)	;is addr in the allocated area?
	dec	a
	cp	h
	jr	nc,invalid	;no, so is invalid...
				;yes, see marker
	dec	hl
	call	$ReadByte	;C=0FFH if valid
	ld	a,c
	ld	l,1		;prepare return=TRUE
	inc	a		;zero if valid
	ret	z		;return L=1 if valid
invalid:
	ld	l,0
	ret			;return L=zero if invalid
;
;	PutByte(char* dest, unsigned char offset, char b)
;
;	dest is in Upper RAM
;
_PutByte:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,0		;BC=offset
	inc	hl
	ld	a,(hl)		;A=byte
	ex	de,hl		;HL=dest
	add	hl,bc		;HL=dest+offset
	ld	c,a		;C=byte		
	jp	$WriteByte
;
;char	GetByte(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetByte:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=source
	inc	hl
	ld	l,(hl)
	ld	h,0		;HL=offset
	add	hl,bc		;HL=source+offset
	call	$ReadByte
	ld	l,c
	ret
;
;	PutWord(char* dest, unsigned char offset, short w)
;
;	dest is in Upper RAM
;
_PutWord:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)	
	inc	hl	
	ld	b,0		;BC=offset
	inc	hl
	ex	de,hl		;HL=dest
	add	hl,bc		;HL=dest+offset
	ex	de,hl		;DE=dest+offset
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=word
	ex	de,hl		;HL=dest+offset, BC=word
	jp	$WriteWord
;
;short	GetWord(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetWord:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=source
	inc	hl
	ld	l,(hl)
	ld	h,0		;HL=offset
	add	hl,bc		;HL=source+offset
	call	$ReadWord
	ld	h,b
	ld	l,c
	ret
;
;	ReadMem(char* dest, char* source, unsigned char count)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;
_ReadMem:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=source
	inc	hl
	ld	a,(hl)		;A=count
	ld	h,b
	ld	l,c		;HL=source
	ld	b,a		;B=count
	jp	$ReadMem
;
;	WriteMem(char* source, char* dest, unsigned char count)
;
;	source is in Lower RAM
;	destination is in Upper RAM
;
_WriteMem:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=dest
	inc	hl
	ld	a,(hl)		;A=count
	ld	h,b
	ld	l,c		;HL=dest
	ld	b,a		;B=count
	jp	$WriteMem
;
;-------------------------------------------------------------2X64K
ENDIF

;-------------------------------------------------------------?512K OR ?Z80ALL
IF ?512K OR ?Z80ALL

MACRO	BANK_BACK
IF	?512K
	ld	a,32+1		;bank 1 back to 4000H
	out	(79H),a
ELSE
	ld	a,0		;bank 0 back to 0000H
	out	(1FH),a
ENDIF
ENDM

;-------------------------------------------------------------?512K
IF	?512K

; Memory block structure
;
; 	defw	next 	;pointer to next block
;	defw	prev 	;pointer to prev block
;	defb	status 	;0=available, 0FF=allocated
;	defb	size	;block size : from 0(=10H) to 10(=4000H) for 512K or 11(=8000H) for Z80ALL
;	data
;
OFF_STS		equ	4
OFF_SIZE	equ	5
OFF_DATA	equ	6

Buf16K		equ	4000H	;address of dynamic memory bank
BANKS_CNT	equ	8	;8 banks of 16KB each are available
MAX_SIZE	equ	10	;max block = 4000H

LISTS_NR	equ	MAX_SIZE+1
AVAILABLE	equ	0
ALLOCATED	equ	0FFH

	psect bss
;
;	Available block list headers
;
L0:	defs	4*BANKS_CNT	;L0 bank0,L0 bank1,...L0 bank27
L1:	defs	4*BANKS_CNT
L2:	defs	4*BANKS_CNT
L3:	defs	4*BANKS_CNT
L4:	defs	4*BANKS_CNT
L5:	defs	4*BANKS_CNT
L6:	defs	4*BANKS_CNT
L7:	defs	4*BANKS_CNT
L8:	defs	4*BANKS_CNT
L9:	defs	4*BANKS_CNT
L10:	defs	4*BANKS_CNT
;
Lists:	defs	LISTS_NR*2*BANKS_CNT	;bank0(L0,L1,...L10),bank1(L0+4,L1+4,...L10+4)... 
;
Buddy:	defs	LISTS_NR*2
;
;	Maximum Available
;	contains (Size+1) if available, or zero if unavailable
;
MaxAv:	defs	BANKS_CNT

;Virtual Pointer to allocated buffer (size = 10H for 512K)
;
;01aaaaaa aaaa0bbb : 4000H to 7FF7H for 512K
;where
;              bbb : 0 to 7 = virtual RAM bank 
;				(Physical RAM bank = 32 + 4 + virtual RAM bank for 512K)
;00aaaaaa aaaa0000 : 0000H to 3FF0H for 512K = offset in RAM bank
;
	psect	top
;
;	Converts bank & offset to virtual pointer
;
;	A = virtual bank (0bbb = 0...7)
;	HL = offset in RAM bank (00aaaaaa aaaa0000)
;
;	returns HL = 01aaaaaa aaaa0bbb
;
ToVirtual:
	or	l
	ld	l,a		
	set	6,h		;HL = 01aaaaaa aaaa0bbb
	ret
;
;	Converts virtual pointer to bank & offset
;
;	HL = virtual pointer (01aaaaaa aaaa0bbb)
;
;	returns A = virtual bank (bbb = 0...7)
;		HL = offset (00aaaaaa aaaa0000)
;
FromVirtual:
	ld	a,l
	ld	b,a
	and	0F0H
	ld	l,a		
	res	6,h		;HL = 00aaaaaa aaaa0000
	ld	a,b		
	and	07H		;A = 00000bbb		
	ret
;
;	AdjustAddr
;
;	HL = virtual pointer
;	selects the correct RAM bank
;	returns HL = real address to data
;
AdjustAddr:
	call	FromVirtual
				;A = virtual bank (bbbb = 0...15)
				;HL = offset (00aaaaaa aaaa0000)
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 2 (4000H-8000H)
	set	6,h		;adjust offset to 4000H+offset
	ld	a,l		;add 6=OFF_DATA
	add	a,OFF_DATA
	ld	l,a		;now HL points to data part
	ret
;
;void	InitDynM(void)
;
_InitDynM:
	push	iy
				;init list headers for each bank
	ld	bc,BANKS_CNT * LISTS_NR	;total lists counter
	ld	hl,L0		;HL=first list header
initL:	
	ld	d,h		;DE=HL
	ld	e,l
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,initL
				;init lists pointers for each bank
	ld	iy,Lists
	ld	a,BANKS_CNT
	ld	bc,4 * BANKS_CNT;delta for list headers
	ld	de,0		;offset in lists headers
initLHB:			;for each bank
	push	af
	ld	hl,L0
	add	hl,de
	ld	a,LISTS_NR
initLH:				
	ld	(iy+0),l
	ld	(iy+1),h
	inc	iy
	inc	iy
	add	hl,bc
	dec	a
	jr	nz,initLH
				;next bank
	inc	de		;increment offset in list headers
	inc	de
	inc	de
	inc	de	
	pop	af
	dec	a
	jr	nz,initLHB
				;init Buddy
	ld	hl,Buddy
	ld	de,10H
	ld	a,LISTS_NR
initBDY:ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	dec	a
	jr	nz,initBDY
				;init largest block list headers for each bank
	ld	b,BANKS_CNT
	xor	a
	ld	hl,L10		;HL=L10 lists header
initLMAX:
	push	af
	push	bc
	push	hl
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 1 (4000-8000)
	ld	de,Buf16K
	call	__AddToL	;add DE to HL header
	ld	hl,Buf16K+OFF_STS;HL=pointer of block status
	ld	(hl),AVAILABLE	;set block status = free
	inc	hl		;HL=pointer of block size
	ld	(hl),MAX_SIZE	;set size = 16KB
				;next bank
	pop	hl
	pop	bc
	pop	af
	inc	hl		;next header
	inc	hl
	inc	hl
	inc	hl
	inc	a		;next bank
	djnz	initLMAX
				;init MaxAv vector
	ld	b,BANKS_CNT
	ld	hl,MaxAv
	ld	a,MAX_SIZE+1
loopav:	ld	(hl),a
	inc	hl
	djnz	loopav

	pop	iy
	BANK_BACK
	ret	
;
;void*	myalloc(void)
;	
;	Alloc operand in 8 x 16KB 
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myalloc:
	ld	c,0		;alloc 10H
alloc:
	ld	b,BANKS_CNT	;B=banks counter
	push	iy		;save IY
	ld	hl,MaxAv	;HL=MaxAv vector
loop:
				;compare bSize ? MaxAv[bank]
	ld	a,c
	cp	(hl)		;bSize ? MaxAv[bank]
	jr	c,try		;if < , try to allocate in this bank
	jr	toobig		;if >= , it's too big, try to allocate in the next bank
try:
	ld	a,BANKS_CNT
	sub	b		;try to allocate in this bank (A=bank nr)
	add	a,32+4 		;select physical RAM bank number A in logical bank 2 (4000-8000)
	out	(79H),a
	push	hl		;save MaxAv
	push	bc		;B=banks counter, C=bSize on stack
	sub	32+4		;A=current bank nr. (0,1,...27)
	call	__alloc		;try to alloc
	jr	nz,alloc_ok
				;local alloc failed
	pop	bc		;B=banks counter, C=bSize
	pop	hl		;HL=MaxAv
	ld	(hl),c		;store failed bSize
toobig:
	inc	hl		;increment MaxAv pointer
	djnz	loop
	BANK_BACK		;global alloc failed
	pop	iy		;restore IY
	ld	hl,0		;return NULL
	ret
;
alloc_ok:			;HL=allocated block
	ld	d,h
	ld	e,l		;HL=DE=allocated block
	ld	a,l
	add	a,OFF_DATA	;erase data part (6 bytes)
	ld	l,a
	ld	b,6
	xor	a
set0:	ld	(hl),a
	inc	l
	djnz	set0
	BANK_BACK
	ex	de,hl		;HL = allocated block
	res	6,h		;HL = allocated block-4000H = offset in RAM bank
	pop	bc		;B=banks counter
	pop	de		;drop MaxAv
	pop	iy		;restore IY
	ld	a,BANKS_CNT
	sub	b		;A=current bank
	jp	ToVirtual	;return HL=virtual pointer
;
;	Allocate a memory block of given size
;
;	A=current bank nr. (0,1,...7)
;	returns Z=0 and HL=pointer to memory block if available, 
;		else Z=1 if no memory available
;	Local variables: DE = Element, 
;			BC on stack (B=Size, C=CrtSize), 
;			IY=Lists[crt.bank],
;	Affected regs: A,BC,DE,HL
;	IX not affected
;
__alloc:
				;compute IY=Lists+(crt.bank)*(LISTS_NR*2)
				;A=current bank nr. (0,1,...27)
	ld	de,LISTS_NR*2	;DE=LISTS_NR*2
	ld	iy,Lists
	ld	b,3		;how many times to shift A
2:
	rra			;shift right A
	jr	nc,1f
	add	iy,de
1:
	sla	e		;DE=DE*2
	rl	d
	djnz	2b
				;IY=Lists+(crt.bank)*(LISTS_NR*2)
;CrtSize=Size
	ld	b,c		;CrtSize=Size
5:	
;do {
;  Element=FirstFromL(Lists[Lists[CrtSize])
				;
	push	bc		;B=Size, C=CrtSize on stack
	ld	a,c
	add	a,a		;A=CrtSize*2
	push	iy
	pop	hl		;HL=Lists+(crt.bank)*(LISTS_NR*2)		
	ld	d,0
	ld	e,a
	add	hl,de		;HL=Lists+(crt.bank)*(LISTS_NR*2)+(CrtSize*2)
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=CrtL=Lists[CrtSize]
	call	__GetFromL	;HL=GetFromL(CrtL)
	jr	z,7f
				;
;  if (Element) {
				;HL=Element
	ld	d,h
	ld	e,l		;save DE=Element
;    Element->Status=ALLOCATED
	ld	a,OFF_STS
	add	a,l
	ld	l,a		;HL=&El.Status
	ld	a,0FFH
	ld	(hl),a		;ALLOCATED
				;
	pop	bc		;B=Size, C=CrtSize
	inc	l		;HL=&El.Size
;    Element->Size=Size
	ld	(hl),b		;set ElSize = Size
	ld	a,c
;    if ( CrtSize == Size)
	cp	b	
;      return Element
	jr	nz,6f
	ex	de,hl		;HL=Element
	inc	a		;Z=0
	ret			;return HL=Element
6:	
;    do {
;      CrtSize--
	dec	c		;decrement CrtSize
				;
	push	bc		;B=Size, C=CrtSize back on stack
				;
;      ElementBuddy = Element XOR Buddy_XOR_Mask
	ld	a,c
	add	a,a		;A=CrtSize*2
	ld	hl,Buddy
	add	a,l
	ld	l,a
	jr	nc,1f
	inc	h		
1:				;HL=pointer to Buddy_XOR_mask
	push	de		;DE=Element on stack
	ld	a,e
	xor	(hl)
	ld	e,a
	inc	l
	ld	a,d
	xor	(hl)
	ld	d,a		;DE = ElementBuddy = Element XOR Buddy_XOR_Mask
;      CrtL=Lists[CrtSize]
	ld	a,c		;C=CrtSize
	add	a,a		;A=CrtSize*2
	push	iy
	pop	hl		;HL=Lists+(crt.bank)*(LISTS_NR*2)		
	ld	b,0
	ld	c,a
	add	hl,bc		;HL=Lists+(crt.bank)*(LISTS_NR*2)+(CrtSize*2)
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=CrtL=Lists[CrtSize]
;      AddToL(CrtL, ElementBuddy)
	call	__AddToL	;AddToL(HL=CrtL, DE=ElementBuddy)
				;returned HL=ElementBuddy
	pop	de		;DE=Element
;      ElementBuddy->Status=AVAILABLE
	ld	a,OFF_STS
	add	a,l
	ld	l,a
	ld	(hl),AVAILABLE	
;      ElementBuddy->Size=CrtSize
				;
	pop	bc		;B=Size, C=CrtSize
				;
	inc	l
	ld	a,c
	ld	(hl),a		;set ElBuddy Size = CrtSize	
;    while (--CrtSize != Size)
	cp	b
	jr	nz,6b
	inc	a		;Z=0
	ex	de,hl		;HL=Element
	ret			;return HL=Element	
;
;   end if (Element) }
7:	
;}
;while (CrtSize++ < MAX_SIZE)
	pop	bc		;B=Size, C=CrtSize
	inc	c		;++CrtSize
	ld	a,c
	cp	MAX_SIZE+1
	jr	nz,5b
;				;alloc failed
	xor	a		;Z=1, no available memory
	ret
;
;	__AddToL
;
;	HL=list header, DE=new
;	return HL=new
;	affected regs: A,BC,DE,HL
;	IX,IY not affected
;
__AddToL:
        ld      a,l
        ld      (de),a
        inc     de
        ld      a,h
        ld      (de),a
        dec     de              ;New.Next=ListHeader
        inc     hl
        inc     hl
        ld      c,(hl)
        ld      (hl),e
        inc     hl
        ld      b,(hl)
        ld      (hl),d          ;BC=Last, ListHeader.Last=New
        ld      a,e
        ld      (bc),a
        inc     bc
        ld      a,d
        ld      (bc),a
        dec     bc              ;Last.Next=New
        ld      l,e
        ld      h,d             ;return HL=New
        inc     de
        inc     de
        ld      a,c
        ld      (de),a
        inc     de
        ld      a,b
        ld      (de),a          ;New.Prev=Last
	ret
;
;	__GetFromL
;
;	HL=list header
;	returns (HL=elem and Z=0) or (HL=0 and Z=1 if list empty)
;	affected regs: A,BC,DE,HL
;	IX,IY not affected
;
__GetFromL:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        dec     hl		;DE=First, HL=ListHeader
				;compare HL ? DE 
	or	a		;CARRY=0
	sbc	hl,de
        ret	z	        ;list empty, return HL=0
	ex	de,hl		;HL will be returned after removing element from list
        ld      e,(hl)		;Remove HL=Element
        inc     hl
        ld      d,(hl)
        inc     hl              ;DE=Next
        ld      c,(hl)
        inc     hl
        ld      b,(hl)          ;BC=Prev
        ld      a,e
        ld      (bc),a
        inc     bc
        ld      a,d
        ld      (bc),a          ;Prev.Next=Next
        dec     bc
        inc     de
        inc     de
        ld      a,c
        ld      (de),a
        inc     de
        ld      a,b
        ld      (de),a          ;Next.Prev=Prev
	dec	hl
	dec	hl
	dec	hl		;HL=element to be returned
	or	h		;Z=0
	ret
;
;	char	IsValid(char* p)
;
;	should be 01aaaaaa aaaa0bbb
;
_IsValid:
	ld	hl,3
	add	hl,sp
	ld	a,(hl)		;high part
	and	0C0H
	cp	040H		;is high part = 01aaaaaa ?
	jr	nz,notvalid
				;yes, now check record status
	ld	a,(hl)
	dec	hl
	ld	l,(hl)
	ld	h,a
	call	FromVirtual
				;A = virtual bank (bbb = 0...7)
				;HL = offset (00aaaaaa aaaa0000)
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 1 (4000H-8000H)
	set	6,h		;adjust offset to 4000H+offset
	ld	a,l		;add 4=OFF_STS
	add	a,OFF_STS
	ld	l,a		;now HL points to the requested byte
	ld	a,(hl)		;FFH if valid
	inc	a
	BANK_BACK
	jr	nz,notvalid
	ld	l,1		;valid
	ret
notvalid:
	ld	l,0
	ret
;
ENDIF
;-------------------------------------------------------------?512K

;-------------------------------------------------------------?Z80ALL
IF ?Z80ALL

BUF_START	equ	8
BUF_END		equ	8000H
BANKS_CNT	equ	2

;	record structure
; 
;	1 byte mark 0AAH
;	1 byte mark 055H
;	6 bytes data
;
MARK1	equ	0AAH
MARK2	equ	055H

OFF_DATA	equ	2

RECLEN		equ	8

	psect	bss

pBigBuf:	defs	4

;Virtual Pointer to allocated buffer (size = 8 for Z80ALL)
;
;0aaaaaaa aaaaa00b 
;where
;                b : 0 or 1 = virtual RAM bank 
;				(Physical RAM bank = 1 + virtual RAM bank for Z80ALL)
;0aaaaaaa aaaaa000 : 0000H to 7FF8H for Z80ALL = offset in RAM bank
;
	psect	top
;
;	Converts bank & offset to virtual pointer
;
;	A = virtual bank (b = 0...7)
;	HL = offset in RAM bank (0aaaaaaa aaaaa000)
;
;	returns HL = 0aaaaaaa aaaaa00b
;
ToVirtual:
	or	l
	ld	l,a		;HL = 1aaaaaaa aaaaa00b
	ret
;
;	Converts virtual pointer to bank & offset
;
;	HL = virtual pointer (0aaaaaaa aaaaa00b)
;
;	returns A = virtual bank (b = 0 or 1)
;		HL = offset (0aaaaaaa aaaaa000)
;
FromVirtual:
	ld	a,l
	ld	b,a
	and	0F8H
	ld	l,a		;HL = 0aaaaaaa aaaaa000
	ld	a,b		
	and	1		;A = 0000000b		
	ret
;
;	AdjustAddr
;
;	HL = virtual pointer (0aaaaaaa aaaaa00b)
;	selects the correct RAM bank
;	returns HL = real address to data
;
AdjustAddr:
	call	FromVirtual
				;A = virtual bank (b = 0 or 1)
				;HL = offset (0aaaaaaa aaaaa000)
	inc	a
	out	(1FH),a		;select physical RAM bank number A in 0000
	inc	hl
	inc	hl		;now HL points to the data part
	ret
;
;void	InitDynM(void)
;
_InitDynM:
	ld	hl,BUF_START
	ld	(pBigBuf),hl
	ld	(pBigBuf+2),hl
	ret
;
;void*	myalloc(void)
;	
;	Alloc operand in 2 x 32KB RAM
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myalloc:
	push	iy		;save IY
	ld	iy,pBigBuf	;IY = pBigBuf
	ld	b,BANKS_CNT	;B=counter
aloop:
	push	bc		;save counter
	ld	h,(IY+1)	;get HL = pointer
	ld	l,(IY+0)
	ld	d,h		
	ld	e,l		;save current pointer in DE
	ld	bc,RECLEN	
	add	hl,bc		;add record length
	ld	a,h
	cp	80H		;have we reached 8000H ?
	jr	nc,trynext
	ld	(IY+1),h	;no, update pointer
	ld	(IY+0),l
	ld	h,d
	ld	l,e		;DE = HL = allocated record
	pop	bc		;B=counter
	push	bc
	ld	a,BANKS_CNT
	sub	b
	inc	a
	out	(1FH),a		;select crt bank in 0000
	ld	a,MARK1		;set marks
	ld	(hl),a		;mark 1
	inc	hl		
	ld	a,MARK2
	ld	(hl),a		;mark 2
	inc	hl		;fill data part with 0
	xor	a		;A=0
	ld	b,6
zero:	ld	(hl),a
	inc	hl
	djnz	zero
	ex	de,hl		;HL = allocated record
	BANK_BACK		;bank 0 back to 0000
	pop	bc		;B=counter
	ld	a,BANKS_CNT
	sub	b		;A=current bank
	pop	iy		;restore IY
	jp	ToVirtual	;return HL=virtual pointer
trynext:
	inc	iy
	inc	iy		;select next pBigBuf
	pop	bc
	djnz	aloop
full:
	pop	iy		;restore IY
	ld	hl,0		;return NULL
	ret	
;
;	char	IsValid(char* p)
;
;	should be 0aaaaaaa aaaaa00b
;
_IsValid:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL = p
	bit	7,h		;should be Z
	jr	nz,notvalid
				;now check the marks
	call	FromVirtual
				;A = virtual bank (b = 0 or 1)
				;HL = offset (0aaaaaaa aaaaa000)
	inc	a
	out	(1FH),a		;select physical RAM bank number A in 0000
	ld	a,(hl)
	cp	MARK1
	jr	nz,notv
	inc	hl
	ld	a,(hl)
	cp	MARK2
	jr	nz,notv
	BANK_BACK
	ld	hl,1		;valid
	ret
notv:	
	BANK_BACK
notvalid:
	ld	hl,0		;not valid
	ret
;
ENDIF
;-------------------------------------------------------------?Z80ALL
;
;char	GetByte(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetByte:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source
	inc	hl
	ld	l,(hl)
	ld	h,0		;HL=offset
	ex	de,hl		;HL=source, DE=offset
	call	AdjustAddr
	add	hl,de		;add offset
	ld	l,(hl)		;get L=byte
	BANK_BACK
	ret
;
;	PutByte(char* dest, unsigned char offset, char b)
;
;	dest is in Upper RAM
;
_PutByte:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,0		;BC=offset
	inc	hl
	ld	a,(hl)		;A=byte	
	ex	de,hl		;HL=dest
	ld	e,a		;E=byte
	push	bc		;save offset on stack
	call	AdjustAddr
	pop	bc		;BC=offset
	add	hl,bc		;add offset
	ld	(hl),e		;store byte
	BANK_BACK
	ret
;
;short	GetWord(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetWord:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source
	inc	hl
	ld	l,(hl)
	ld	h,0		;HL=offset
	ex	de,hl		;HL=source, DE=offset
	call	AdjustAddr
	add	hl,de		;add offset
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=word
	ex	de,hl		;HL=word
	BANK_BACK
	ret
;
;	PutWord(char* dest, unsigned char offset, short w)
;
;	dest is in Upper RAM
;
_PutWord:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,0		;BC=offset
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=word
	ex	de,hl		;HL=dest, DE=word
	push	bc		;save offset on stack
	call	AdjustAddr
	pop	bc		;restore BC=offset
	add	hl,bc		;add offset
	ld	(hl),e		;store word
	inc	hl
	ld	(hl),d
	BANK_BACK
	ret
;
;	ReadMem(char* dest, char* source, short count)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;	count <= 256
;
_ReadMem:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=source
	inc	hl
	ld	a,(hl)		;A=count
	ld	h,b
	ld	l,c		;HL=source
	ld	c,a		;C=count
	call	AdjustAddr
moveit:
	ld	b,0		;BC=count
	ldir
	BANK_BACK
	ret
;
;	WriteMem(char* source, char* dest, short count)
;
;	source is in Lower RAM
;	destination is in Upper RAM
;	count <= 256
;
_WriteMem:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=dest
	inc	hl
	ld	a,(hl)		;A=count
	ld	h,b
	ld	l,c		;HL=dest
	ld	c,a		;C=count
	call	AdjustAddr
	ex	de,hl		;DE=dest, HL=source
	jr	moveit
;
ENDIF
;-------------------------------------------------------------?512 OR ?Z80ALL

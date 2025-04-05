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
;	GLOBAL	_TotalSize
	GLOBAL	_myallocsym
	GLOBAL	_myallocstr
	GLOBAL	_GetByte
	GLOBAL	_PutByte
	GLOBAL	_GetWord
	GLOBAL	_PutWord
	GLOBAL	_ReadMem
	GLOBAL	_WriteMem
	GLOBAL	_PutString
	GLOBAL	_GetString
	GLOBAL	_StringLen
	GLOBAL	_IsValid

;-------------------------------------------------------------2X64K
IF	?2X64K
;
;	Upper 64KB RAM structure
;
;<         strings>		<symbols       ><hashtab headers>	<shadow code>
;                  ^		^               ^                       ^
;		   |            |		|			|
;        	   TOP_STRINGS	BOTTOM_SYM	HEADERS_START		HEADERS_END
;
HEADERS_COUNT	equ	271

IF 	SC108 .or. MM
HEADERS_END	equ	0FF63H
ELSE
HEADERS_END	equ	0FF00H
ENDIF

HEADERS_START	equ	HEADERS_END - ((HEADERS_COUNT + 1) * 4)

DYNM_START	equ	1
DYNM_END	equ	HEADERS_START

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
				;+1	data - 27 bytes
;
HEADERSIZE	equ	1
BLOCKSIZE	equ	28	;1+27
;
TOP_STRINGS:	defw	DYNM_START	;top strings area (must be < BOTTOM_SYM)
BOTTOM_SYM:	defw	DYNM_END	;bottom symbols area (must be > TOP_STRINGS)
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

	psect	top

;---------------------------------------------------SC108 or MM
IF	SC108 .or. MM
;
;	Init dynamic memory
;
_InitDynM:
	push	ix			;save IX
	call	StoreShared		;setup shadow routines
					;init upper ram with 0
	ld	hl,DYNM_START
	ld	de,DYNM_END-DYNM_START
	ld	c,0
loopi:	PutByte
	inc	hl
	dec	de
	ld	a,d
	or	e
	jr	nz,loopi
	pop	ix			;restore IX
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
				;move "mover" to 0D000H
	ld	de,0D000H	;to
	ld	hl,mover_start	;from
	ld	bc,mover_end - mover_start
	ldir
	jp	0D000H		;call "mover" to store shared code to Upper RAM and return
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
	ld	hl,retadr - mover_start + 0D000H
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
SHARED		equ	HEADERS_END

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
                           ;init upper ram from 0000H to FF00H with 0
         ld      hl,0
         ld      bc,0FFH   ;B = 0, C = FFH
1:
         call    $FillZero ; fills with zero from (HL) B bytes (256 bytes)
         dec     c         ; HL = HL + 100H, B = 0
         jr      nz,1b
         ret

ENDIF
;---------------------------------------------------Simple80

	psect	text
;
;	Get_hashtab(int index)
;
	GLOBAL	_Get_hashtab
;
_Get_hashtab:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL = index
gethdr:	add	hl,hl
	add	hl,hl		;HL=index*4
	ld	bc,HEADERS_START
	add	hl,bc		;HL=hashtab addr
	ret
;
;	Get_SymList(void)
;
	GLOBAL	_Get_SymList
;
_Get_SymList:
	ld	hl,HEADERS_COUNT
	jr	gethdr
;
;	short	StringLen(char* source)
;
;	source is in Upper RAM
;
_StringLen:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=source in Upper RAM
	jp	$StringLen
;
;	GetString(char* dest, char* source)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;
_GetString:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest in Lower RAM
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=source in Upper RAM
	jp	$ReadString
;
;	PutString(char* source, char* dest)
;
;	source is in Lower RAM
;	destination is in Upper RAM
;
_PutString:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source in Lower RAM
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=dest in Upper RAM
	jp	$WriteString
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
;	IsValid - is the address allocated?
;
;	short IsValid(char* addr)
;
_IsValid:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=addr
	ld	a,(BOTTOM_SYM+1);is addr in the allocated area?
	dec	a
	cp	h
	jr	nc,invalid	;no, so is invalid...
				;yes, see marker
	dec	hl
	call	$ReadByte	;C=0FFH if valid
	ld	a,c
	ld	l,0FFH		;prepare return=TRUE
	inc	a		;zero if valid
	ret	z		;return L=0FFH if valid
invalid:
	ld	l,0
	ret			;return L=zero if invalid
;
	GLOBAL	_IsHeader
;
;	bool IsHeader(char* addr)
;
_IsHeader:
	ld	hl,2
	add	hl,sp		;CARRY=0
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=addr
	ld	bc,HEADERS_START
	sbc	hl,bc
	ld	l,0FFH
	ret	nc
	inc	l
	ret
;
;void*	myallocsym(void)
;	
;	Alloc symbol in Upper 64KB RAM
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myallocsym:
	ld	a,(TOP_STRINGS+1)	;get top of allocated strings area
	inc	a
	ld	hl,(BOTTOM_SYM)		;compare to bottom of allocated symbols area
	cp	h
	jr	z,full			
	or	a			;CARRY=0
	ld	bc,BLOCKSIZE		;size of data
	sbc	hl,bc
	ld	(BOTTOM_SYM),hl		;update bottom of allocated symbols area
	ld	c,ALLOCATED		;mark status = allocated
	PutByte
	inc	hl
	ret				;return data pointer
full:
	ld	hl,0			;return zero
	ret
;
;	myallocstr
;
;void*	myallocstr(unsigned char size)
;
;	Alloc string in Upper 64KB RAM
;
;	size=string len + 1
;	returns pointer of data part (or NULL if no more available memory)
;
_myallocstr:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	ld	b,0			;BC=size
	ld	hl,(TOP_STRINGS)	;get top of allocated strings area
	ld	a,(BOTTOM_SYM+1)	;compare to bottom of allocated symbols area
	dec	a
	cp	h
	jr	z,fullstr
	push	hl			
	add	hl,bc
	ld	(TOP_STRINGS),hl	;update top of allocated strings area
	pop	hl
	ret				;return data pointer
fullstr:
	ld	hl,0			;return zero
	ret	
;
;-------------------------------------------------------------2X64K
ENDIF

;-------------------------------------------------------------?512K OR ?Z80ALL
IF ?512K OR ?Z80ALL

;Virtual Pointer to allocated buffer (size = 10H to 4000H for 512K or 8000H for Z80ALL)
;
;1aaaaaaaaaaabbbb : 0000H to 3FFFH for 512K or 7FFFH for Z80ALL
;where
;            bbbb : 0 to 0FH = virtual RAM bank (Physical RAM bank = 32 + 4 + virtual RAM bank)
;0aaaaaaaaaaa0000 : 0000H to 3FF0H for 512K or 7FF0H for Z80ALL = offset in RAM bank

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

IF	?512K
Buf16K		equ	4000H	;address of dynamic memory bank
BANKS_CNT	equ	8	;8 banks of 16KB each are available
MAX_SIZE	equ	10	;max block = 4000H
ELSE
Buf32K		equ	0000H	;address of dynamic memory bank
BANKS_CNT	equ	2	;2 banks of 32KB each are available
MAX_SIZE	equ	11	;max block = 8000H
ENDIF

LISTS_NR	equ	MAX_SIZE+1
AVAILABLE	equ	0
ALLOCATED	equ	0FFH
;
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
IF	?Z80ALL
L11:	defs	4*BANKS_CNT
ENDIF
;
Lists:	defs	LISTS_NR*2*BANKS_CNT	;bank0(L0,L1,...L10),bank1(L0+4,L1+4,...L10+4)... 
;
Buddy:	defs	LISTS_NR*2
;
;	Maximum Available
;	contains (Size+1) if available, or zero if unavailable
;
MaxAv:	defs	BANKS_CNT

	psect	top

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
				;fill each bank with 0
	xor	a		;A=banks index
filll:
	push	af
IF	?512K
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 2 (4000-8000)
	ld	hl,Buf16K
	ld	c,40H		;4000H = 40H x 100H
ELSE
	inc	a
	out	(1FH),a
	ld	hl,Buf32K
	ld	c,80H		;8000H = 80H x 100H
ENDIF
	xor	a
	ld	b,a
3:	ld	(hl),a
	inc	hl
	djnz	3b
	dec	c
	jr	nz,3b
				;next bank
	pop	af
	inc	a
	cp	BANKS_CNT
	jr	nz,filll
				;init largest block list headers for each bank
	ld	b,BANKS_CNT
	xor	a
IF	?512K
	ld	hl,L10		;HL=L10 lists header
ELSE
	ld	hl,L11		;HL=L11 lists header
ENDIF
initLMAX:
	push	af
	push	bc
	push	hl
IF	?512K
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 1 (4000-8000)
	ld	de,Buf16K
	call	__AddToL	;add DE to HL header
	ld	hl,Buf16K+OFF_STS;HL=pointer of block status
ELSE
	inc	a
	out	(1FH),a		;select physical RAM bank number A in logical bank 0000-8000
	ld	de,Buf32K
	call	__AddToL	;add DE to HL header
	ld	hl,Buf32K+OFF_STS;HL=pointer of block status
ENDIF
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
IF	?Z80ALL
	xor	a		;bank 0 back in 0000-8000
	out	(1FH),a
ELSE
	ld	a,32+1
	out	(79H),a		;set back physical RAM bank number 33 in logical bank 1 (4000-8000)
ENDIF
	ret	

	psect	text
;
;	Converts bank & offset to virtual pointer
;
;	A = virtual bank (bbbb = 0...15)
;	HL = offset in RAM bank (0aaaaaaa aaaa0000)
;
;	returns HL = 1aaaaaaa aaaabbbb
;
ToVirtual:
	or	l
	ld	l,a		
	set	7,h		;HL = 1aaaaaaa aaaabbbb
	ret
;
;	Converts virtual pointer to bank & offset
;
;	HL = virtual pointer (1aaaaaaa aaaabbbb)
;
;	returns A = virtual bank (bbbb = 0...15)
;		HL = offset (0aaaaaaa aaaa0000)
;
FromVirtual:
	ld	a,l
	ld	b,a
	and	0F0H
	ld	l,a		
	res	7,h		;HL = 0aaaaaaa aaaa0000
	ld	a,b		
	and	0FH		;A = 0000bbbb		
	ret
;
;void myallocstr(unsigned char size);
;
_myallocstr:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	ld	b,0		;BC=size
	inc	bc		;add 6
	inc	bc
	inc	bc
	inc	bc
	inc	bc
	inc	bc
	call	__allocS	;C=bSize
	jr	alloc
;
;void*	myallocsym(void)
;	
;	Alloc symbol in 8 x 16KB / 2 x 32KB RAM
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myallocsym:
	ld	c,2		;alloc 40H
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
IF	?512K
	add	a,32+4 		;select physical RAM bank number A in logical bank 2 (4000-8000)
	out	(79H),a
	push	hl		;save MaxAv
	push	bc		;B=banks counter, C=bSize on stack
	sub	32+4		;A=current bank nr. (0,1,...27)
ELSE
	inc	a 		;select physical RAM bank number A in 0000-8000
	out	(1FH),a
	push	hl		;save MaxAv
	push	bc		;B=banks counter, C=bSize on stack
	dec	a		;A=current bank nr. (0,1)
ENDIF
	call	__alloc		;try to alloc
IF	?Z80ALL
	ld	a,0
	out	(1FH),a		;bank 0 back in 0000-8000
ELSE
	ld	a,32+1
	out	(79H),a		;set back physical RAM bank number 33 in logical bank 1 (4000-8000)
ENDIF
	jr	nz,alloc_ok
				;local alloc failed
	pop	bc		;B=banks counter, C=bSize
	pop	hl		;HL=MaxAv
	ld	(hl),c		;store failed bSize
toobig:
	inc	hl		;increment MaxAv pointer
	djnz	loop
				;global alloc failed
	pop	iy		;restore IY
	ld	hl,0		;return NULL
	ret
;
alloc_ok:			;HL=allocated block
IF	?512K
	res	6,h		;HL = allocated block-4000H = offset in RAM bank
ENDIF
	pop	bc		;B=banks counter
	pop	de		;drop MaxAv
	pop	iy		;restore IY
	ld	a,BANKS_CNT
	sub	b		;A=current bank
	jp	ToVirtual	;return HL=virtual pointer
;	
;	allocS
;
;	BC=memory size (must be <= 4000H or 8000H)
;	Affected regs: A,BC
;	Returns C=bElement size
;
__allocS:
	dec	bc		;bc = memory size-1
	ld	a,b
IF	?512K
	and	3FH		;keep it <= 3FH
ELSE
	and	7FH		;keep it <= 7FH
ENDIF
	or	a
	jr	z,1f
				;high part != 0
IF	?Z80ALL
	ld	c,11		;prepare bSize for 8000H
	bit	6,a
	ret	nz		;if bit#14=1 return 11 for 8000H
	dec	c		;C=10
ELSE
	ld	c,10		;prepare bSize for 4000H
ENDIF
	bit	5,a
	ret	nz		;if bit#13=1 return 10 for 4000H
	dec	c		;C=9
	bit	4,a
	ret	nz		;if bit#12=1 return 9 for 2000H
	dec	c		;C=8
	bit	3,a
	ret	nz		;if bit#11=1 return 8 for 1000H
	dec	c		;C=7
	bit	2,a
	ret	nz		;if bit#10=1 return 7 for 800H
	dec	c		;C=6
	bit	1,a
	ret	nz		;if bit#9=1 return 6 for 400H
	dec	c		;C=5
	ret			;else return 5 for 200H
1:	ld	a,c		;high part == 0
	ld	c,4		;C=4
	bit	7,a
	ret	nz		;if bit#7=1 return 4 for 100H
	dec	c		;C=3
	bit	6,a
	ret	nz		;if bit#6=1 return 3 for 80H
	dec	c		;C=2
	bit	5,a
	ret	nz		;if bit#5=1 return 2 for 40H
	dec	c		;C=1
	bit	4,a		
	ret	nz		;if bit#4=1 return 1 for 20H
	dec	c		;C=0
	ret			;else return 0 for 10H
;
;	Allocate a memory block of given size
;
;	called under DI
;	A=current bank nr. (0,1,...27)
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
IF	?512K
	ld	b,3		;how many times to shift A
ELSE
	ld	b,1
ENDIF
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
	inc	hl
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
;	must be called under interrupts DISABLED
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
;	__RemoveFromL
;
;	must be called under interrupts DISABLED
;	HL=elem to be removed
;	Returns HL=Element
;	affected regs: A,BC,DE,HL
;	IX,IY not affected
;
__RemoveFromL:
        ld      e,(hl)
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
	dec	hl		;HL=element
	ret
;
;	__GetFromL
;
;	must be called under interrupts DISABLED
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
;	AdjustAddr
;
;	HL = virtual pointer
;	selects the correct RAM bank
;	returns HL = real address to data
;
AdjustAddr:
	call	FromVirtual
				;A = virtual bank (bbbb = 0...15)
				;HL = offset (0aaaaaaa aaaa0000)
IF	?512K
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 2 (4000H-8000H)
	set	6,h		;adjust offset to 4000H+offset
ELSE
	inc	a
	out	(1FH),a
ENDIF
	ld	a,l		;add 6=OFF_DATA
	add	a,OFF_DATA
	ld	l,a		;now HL points to the requested byte
	ret	
;
;	restore RAM bank
;
MACRO	BANK_BACK
IF	?512K
	ld	a,32+1
	out	(79H),a
ELSE
	ld	a,0
	out	(1FH),a
ENDIF
ENDM
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
;	char	IsValid(char* p)
;
;	should be 0aaaaaaa aaaabbbb
;
_IsValid:
	ld	hl,3
	add	hl,sp
	ld	a,(hl)
	dec	hl
	ld	l,(hl)
	ld	h,a		;HL=pointer
	rla			;is H bit 7 = 1 ?
	jr	nc,notvalid	;no, it's not valid
				;yes, now check record status
	call	FromVirtual
				;A = virtual bank (bbbb = 0...15)
				;HL = offset (0aaaaaaa aaaa0000)
IF	?512K
	add	a,32+4
	out	(79H),a		;select physical RAM bank number A in logical bank 1 (4000H-8000H)
	set	6,h		;adjust offset to 4000H+offset
ELSE
	inc	a
	out	(1FH),a
ENDIF
	ld	a,l		;add 4=OFF_STS
	add	a,OFF_STS
	ld	l,a		;now HL points to the requested byte
	ld	a,(hl)		;FFH if valid
	inc	a
	BANK_BACK
	jr	nz,notvalid
	ld	l,0FFH
	ret
notvalid:
	ld	l,0
	ret
;
;	short	StringLen(char* source)
;
;	source is in Upper RAM
;
_StringLen:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)		
	ld	l,a		;HL=source in Upper RAM
	call	AdjustAddr
	ld	b,0FFH
2:	ld	a,(hl)
	inc	hl
	inc	b
	or	a
	jr	nz,2b
	ld	h,a
	ld	l,b
	BANK_BACK
	ret
;
;	GetString(char* dest, char* source)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;
_GetString:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=dest in Lower RAM
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=source in Upper RAM
	call	AdjustAddr
movestr:			;HL=source, DE=dest
	ld	a,(hl)
	ld	(de),a
	inc	hl
	inc	de
	or	a
	jr	nz,movestr
	BANK_BACK
	ret
;
;	PutString(char* source, char* dest)
;
;	source is in Lower RAM
;	destination is in Upper RAM
;
_PutString:
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=source in Lower RAM
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=dest in Upper RAM
	call	AdjustAddr
	ex	de,hl		;HL=source, DE=dest
	jr	movestr
;
ENDIF
;-------------------------------------------------------------?512 OR ?Z80ALL

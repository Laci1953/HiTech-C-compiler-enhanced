;
;	Dynamic memory routines
;
;	for 128KB / 512KB RAM
;
;	Szilagyi Ladislau
;
;	May 2025
;
;	only one of ?2X64K , ?512K, ?Z80ALL, ?ROMWBW must be set to 1
;
?2X64K	equ	0	;1 : SC108 or 32KB ROM + 128KB RAM Phillip Stevens memory module or SC114 or Simple80
?512K	equ	0	;1 : 512KB RAM + 512KB EPROM
?Z80ALL equ	1	;1 : Z80ALL 4 x 32KB RAM
?ROMWBW	equ	0	;1 : ROMWBW 16 X 32KB RAM
;
;	in case ?2X64K is set to 1,
;	only one of SC108, MM, S80 must be set to 1
;
SC108	equ	0	;1 : SC108
MM	equ	0	;1 : 32KB ROM + 128KB RAM Phillip Stevens memory module or SC114
S80	equ	0	;1 : Simple80 
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

;---------------------------------------------------SC108 or MM
IF	SC108 .or. MM

	psect	data

SHARED_SET:defb	0		;0 : NOT set, 1 : SET

	psect	top
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

	psect heap
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

	psect top
;
;	Init dynamic memory
;
_InitDynM:
	ld	hl,DYNM_END
	ld	(TOP),hl        ;set top available space
        ret

ENDIF
;---------------------------------------------------Simple80

	psect top
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

;-------------------------------------------------------------?512K OR ?Z80ALL OR ?ROMWBW
IF ?512K OR ?Z80ALL OR ?ROMWBW

Buf		equ	0000H	;address of dynamic memory bank

IF	?512K

BANK_SIZE	equ	4000H	;16KB
BANKS_CNT	equ	4	;4 banks of 16KB each are available

MACRO	SELECT
				;A=bank index
	add	a,32+4
	out	(78H),a		;select physical RAM bank in logical bank 0 (0000-4000)
ENDM

MACRO	DEFAULT
	ld	a,32
	out	(78H),a		;set back physical RAM bank number 32 in logical bank 0 (0000-4000)
ENDM

ENDIF

IF	?Z80ALL

BANK_SIZE	equ	8000H	;32KB
BANKS_CNT	equ	2	;2 banks of 32KB each are available

MACRO	SELECT
				;A=bank index
	inc	a
	out	(1FH),a		;select bank in 0000-8000
ENDM

MACRO	DEFAULT
	xor	a		;bank 0 back in 0000-8000
	out	(1FH),a
ENDM

ENDIF

IF	?ROMWBW

BANK_SIZE	equ	8000H	;32KB

HBIOS		equ	0FFF0H
BNKSEL		equ	0FFF3H

SYSGETBNK	equ	0F3H
APPBNKSB	equ	0F8H
APPBNKSC	equ	0F5H

MACRO	SELECT
				;A=bank index
	push	hl		;save HL
	ld	b,a
	ld	a,(base)
	add	a,b
	di
	call	BNKSEL		;select bank A
	ei
	pop	hl		;restore HL
ENDM

MACRO	DEFAULT
	push	hl		;save HL
	ld	a,(default)
	di
	call	BNKSEL
	ei
	pop	hl		;restore HL
ENDM

ENDIF

	psect bss

crtbnk:	defs	1		;current bank index (0...)
crtptr: defs	2		;current pointer of free area in bank

IF	?ROMWBW

;	Base bank index & count
;
count:	defs	1
base:	defs	1
;
;	Initial bank index
;
default:defs	1

	psect	data

Done:	defb	0

ENDIF

	psect	top

_InitDynM:
	xor	a
	ld	(crtbnk),a
	ld	hl,Buf
	ld	(crtptr),hl

IF	?ROMWBW
				;are WBW params set?
	ld	hl,Done
	ld	a,(hl)
	or	a
	ret	nz
	jp	setwbw

	psect	heap

setwbw:
	inc	a		;no, set-it
	ld	(hl),a

	di
	ld	b,SYSGETBNK
	call	HBIOS		;C=crt bank
	ld	a,c
	ld	(default),a	;default bank at 0000H
	ld	b,APPBNKSB
	ld	c,APPBNKSC
	call	HBIOS		;H=base,L=count
	ld	a,l		;keep count <= 2
	cp	3
	jr	c,1f
	ld	l,2
1:
	ld	(count),hl	;init base & count
	ei
	ret
ELSE
	ret	
ENDIF

	psect	top
;	
;	Alloc operand (6+1 bytes)
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myalloc:
	ld	bc,7		;BC=size
	ld	hl,(crtptr)	;HL=current pointer
	push	hl		;save-it
				;increment pointer of available free area
	add	hl,bc
	ld	(crtptr),hl	;save-it for now...
	xor	a		;check-it against end-of-bank
	ld	de,BANK_SIZE
	sbc	hl,de
	pop	hl		;restore current pointer
	jr	c,retval
				;pointer reached end of bank, 
	ld	a,(crtbnk)	;increment current pointer and bank
	inc	a
IF	?ROMWBW
	ld	hl,count
	cp	(hl)
ELSE
	cp	BANKS_CNT	;check current available bank
ENDIF
	jr	z,full
	ld	(crtbnk),a
	ld	hl,Buf		;HL=current pointer	
	push	hl
	add	hl,bc		
	ld	(crtptr),hl	;save new pointer of available free area
	pop	hl
retval:				;HL=pointer of allocated element
	ld	a,(crtbnk)	;A=current bank
	SELECT			;HL not affected
	ld	(hl),0FFH	;store marker
	inc	hl
	push	hl
	ld	b,6		;fill with 6 zero
	xor	a
fill0:	ld	(hl),a
	inc	hl
	djnz	fill0
	pop	hl
	DEFAULT			;HL not affected
	ld	a,(crtbnk)
	call	ToVirtual	;HL=virtual pointer
	ret
full:	ld	hl,0		;no more free bytes
	ret
;
;	char	IsValid(char* p)
;
_IsValid:
				;compute top allocated addr
	ld	a,(crtbnk)
	ld	hl,(crtptr)
	call	ToVirtual
	ex	de,hl		;DE=top
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a		;HL=data virtual pointer
	ex	de,hl		;HL=top,DE=data pointer
	sbc	hl,de		;in range?
	jr	c,notval
				;yes...
	ex	de,hl		;HL=data pointer
				;check record status
	call	FromVirtual	;A=bank,HL=pointer
	SELECT			;HL not affected
	dec	hl		;now HL points the possible marker
	ld	l,(hl)		;L=0FFH if valid
	DEFAULT			;HL not affected
	ld	a,l
	cp	0FFH
	ld	l,1		;valid
	ret	z
notval:
	ld	l,0		;not valid
	ret
;
;Virtual Pointer to allocated buffer 
;
;baaaaaaaaaaaaaaa : for Z80ALL or ROMWBW
;bbaaaaaaaaaaaaaa : for 512
;
;	Converts bank & offset to virtual pointer
;
;	A = virtual bank 
;	HL = offset in RAM bank
;
;	returns HL = virtual pointer
;
ToVirtual:
	rrca
IF	?512K
	rrca
ENDIF
	or	h
	ld	h,a
	ret
;
;	Converts virtual pointer to bank & offset
;
;	HL = virtual pointer 
;
;	returns A = virtual bank 
;		HL = offset 
;
FromVirtual:
	ld	b,h
	ld	a,h
IF	?512K
	and	03FH
ELSE
	and	07FH
ENDIF
	ld	h,a
	ld	a,b
	rlca
IF	?512K
	rlca
	and	3
ELSE
	and	1
ENDIF
	ret
;
;	AdjustAddr
;
;	HL = virtual pointer
;	selects the correct RAM bank
;	returns HL = real address to data
;	conserve DE,BC
;
AdjustAddr:
	push	bc
	call	FromVirtual
				;A = virtual bank 
				;HL = offset
	push	de
	SELECT			;HL not affected
	pop	de
	pop	bc
	ret
	
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
	DEFAULT			;HL not affected
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
	call	AdjustAddr
	add	hl,bc		;add offset
	ld	(hl),e		;store byte
	DEFAULT			;HL not affected
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
	DEFAULT			;HL not affected
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
	call	AdjustAddr
	add	hl,bc		;add offset
	ld	(hl),e		;store word
	inc	hl
	ld	(hl),d
	DEFAULT			;HL not affected
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
	DEFAULT			;HL not affected
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
;-------------------------------------------------------------?512K OR ?Z80ALL OR ?ROMWBW

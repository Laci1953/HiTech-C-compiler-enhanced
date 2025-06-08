TRACE_CALLS	equ	0
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
	GLOBAL	_PutString
	GLOBAL	_GetString
	GLOBAL	_StringLen
	GLOBAL	_SetupSymTab

	psect	top

;
;	space for file buffers
;
	REPT	800H
	defb	0
	ENDM

_SetupSymTab:
	ld	hl,(6)			;get bdos base
	xor	a
	ld	bc,512
	sbc	hl,bc			;HL=end of sym area
	ld	de,__Hbss		;DE=start of sym area
	ld	(_pSymTab),de
	sbc	hl,de			;HL=size of sym area
	push	hl			;size of sym area on stack
	ld	b,h
	ld	c,l			;BC=size of sym area
	ex	de,hl			;HL=start of sym area
	ld	(hl),a			;first byte=0
	ld	d,h
	ld	e,l
	inc	de			;DE=start of sym area + 1
	ldir				;fill with zero the sym area
	pop	hl			;restore size of sym area
	ld	de,12			;sizeof(sym_t)
	call	adiv
	ld	(_SymTabSize),hl
	ret

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

	psect	top
;
;	Init dynamic memory
;
_InitDynM:
	push	ix		;save IX
	call	StoreShared	;setup shadow routines
	pop	ix		;restore IX	
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

;	void* myalloc(int size)
;
;	Alloc memory
;
;	returns HL=pointer of data (or NULL if no more available memory)
;
_myalloc:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=size
        ld      hl,(TOP)	;get top of available memory
        ld      a,h
        or      a		;if < 100H
	jr	z,full			
				;CARRY=0, BC=size of data
	sbc	hl,bc
        ld      (TOP),hl	;HL = data pointer
	ret			;return data pointer
full:	
	ld	hl,0		;return NULL
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
;-------------------------------------------------------------2X64K
ENDIF

;-------------------------------------------------------------?512K OR ?Z80ALL OR ?ROMWBW
IF ?512K OR ?Z80ALL OR ?ROMWBW

;################################################################
IF	TRACE_CALLS 

	psect	top
;
;	Out character to console
;
;	C=char
;
OutChar::
        in 	a,(89H)  	;get status
        bit 	2,a         	;transmit reg full?
        jr 	z,OutChar
        ld 	a,c          	;regC contains the character to send out
        out 	(88H),a		;transmit the character
	ret

MACRO	PRINT_CALL 
	ld	c,'>'
	call	OutChar
	pop	bc
	push	bc
	call	PrintWord
ENDM

buf4:	defs	4
	defb	0DH,0AH,0
;
;	Word to ascii
;
;	BC = word
;
;	Store hexa ascii string in buf4
;
WordToAscii:
	ld	hl,buf4
	ld	a,b
	call	ByteToNibbles
	ld	a,d
	call	NibbleToASCII
	ld	(hl),a
	inc	hl
	ld	a,e
	call	NibbleToASCII
	ld	(hl),a
	inc	hl
	ld	a,c
	call	ByteToNibbles
	ld	a,d
	call	NibbleToASCII
	ld	(hl),a
	inc	hl
	ld	a,e
	call	NibbleToASCII
	ld	(hl),a
	ret
;
;       Byte To Nibbles
;
;       Convert byte to nibbles
;       A = Hex byte
;       returns D = Most significant nibble, E = Least significant nibble
;       registers not affected (except AF)
;
ByteToNibbles:
        ld      e,a
        rra
        rra
        rra
        rra
        and     0FH
        ld      d,a
        ld      a,e
        and     0FH
        ld      e,a
        ret
;
;       Converts Nibble A to ASCII
;
;       Converts Nibble (0-15) to its ASCII value ('0' to '9', or 'A' to 'F')
;
;       A=Nibble
;       returns A=ASCII value of byte (letters in uppercase)
;       registers not affected (except AF)
;
NibbleToASCII:
        cp      10              ;digit?
        jr      nc,1f
        add     a,'0'           ;it's a digit
        ret
1:      add     a,'A'-10        ;no, it's a letter (A to F)
        ret
;
;	PrintWord
;
;	Print BC-3
;
PrintWord:
	dec	bc
	dec	bc
	dec	bc
	call	WordToAscii
	ld	hl,buf4
loopp:	
	ld	a,(hl)
	or	a
	ret	z
	ld	c,a
	call	OutChar
	inc	hl
	jr	loopp
;
;	Print PC value
;
_PrintPC::
	ld	c,'@'
	call	OutChar
	pop	de
	pop	bc
	push	bc
	push	de
	call	PrintWord
	ret
;

csv::	
	call	_PrintPC
	pop	hl		;return address
	push	iy
	push	ix
	ld	ix,0
	add	ix,sp		;new frame pointer
	jp	(hl)

ncsv::
	call	_PrintPC
	pop	hl
	push	iy
	push	ix
	ld	ix,0
	add	ix,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ex	de,hl
	add	hl,sp
	ld	sp,hl
	ex	de,hl
	jp	(hl)

cret::	ld	sp,ix
	pop	ix
	pop	iy
	ret

indir::	jp	(hl)

ENDIF
;################################################################

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

ENDIF

	psect	top

_InitDynM:
	xor	a
	ld	(crtbnk),a
	ld	hl,Buf+1	;start from 1 (0 == NULL)
	ld	(crtptr),hl
IF	?ROMWBW
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
ENDIF
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
;
;	void * myalloc(int size)
;
;	Alloc memory
;
;	returns HL=pointer of data (or NULL if no more available memory)
;
_myalloc:
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=size
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
	ld	hl,Buf		;HL=current pointer (ToVirtual will set it NOT NULL) 
	push	hl
	add	hl,bc		
	ld	(crtptr),hl	;save new pointer of available free area
	pop	hl
retval:				;HL=pointer of allocated element
	ld	a,(crtbnk)	;A=current bank
	call	ToVirtual	;HL=virtual pointer
	ret
full:	ld	hl,0		;no more free bytes
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
	DEFAULT
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
	DEFAULT
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
;-------------------------------------------------------------?512K OR ?Z80ALL OR ?ROMWBW

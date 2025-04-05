;
;	Dynamic memory routines for ROMWBW
;
;	Szilagyi Ladislau
;
;	April 2025
;
;--------------------------------------------------------
;	this code must be executed at addresses > 8000H
;--------------------------------------------------------
;
	GLOBAL	_InitDynM
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

HBIOS		equ	0FFF0H
BNKSEL		equ	0FFF3H

SYSGETBNK	equ	0F3H
APPBNKSB	equ	0F8H
APPBNKSC	equ	0F5H

;Virtual Pointer to allocated buffer (size = 10H to 8000H)
;
;1aaaaaaaaaaa00bb 
;where
;            00bb : 0 to 3 = virtual RAM bank 
;0aaaaaaaaaaa0000 : 0000H to  7FF0H = offset in RAM bank

; Memory block structure
;
; 	defw	next 	;pointer to next block
;	defw	prev 	;pointer to prev block
;	defb	status 	;0=available, 0FF=allocated
;	defb	size	;block size : from 0(=10H) to 11(=8000H)
;	data
;
OFF_STS		equ	4
OFF_SIZE	equ	5
OFF_DATA	equ	6

Buf32K		equ	0000H	;address of dynamic memory bank
BANKS_CNT	equ	4	;4 banks of 32KB each are available
MAX_SIZE	equ	11	;max block = 8000H

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
L11:	defs	4*BANKS_CNT
;
Lists:	defs	LISTS_NR*2*BANKS_CNT	;bank0(L0,L1,...L10),bank1(L0+4,L1+4,...L10+4)... 
;
Buddy:	defs	LISTS_NR*2
;
;	Base bank index & count
;
count:	defs	1
base:	defs	1

;
;	Initial bank index
;
default:defs	1
;

MACRO	DEFAULT
	push	hl		;save HL
	ld	a,(default)
	call	BNKSEL
	pop	hl		;restore HL
ENDM

	psect	top

_InitDynM:
	di
	push	iy
	ld	b,SYSGETBNK
	call	HBIOS		;C=crt bank
	ld	a,c
	ld	(default),a	;default bank at 0000H
	ld	b,APPBNKSB
	ld	c,APPBNKSC
	call	HBIOS		;H=base,L=count
	ld	a,l		;allow max 4 as count
	cp	5
	jr	c,1f
	ld	l,4
1:
	ld	(count),hl	;init base & count
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
	ld	de,(count)	;D=base, E=count
filll:
	ld	a,d
	push	de
	call	BNKSEL		;select bank A
	pop	de
	ld	hl,Buf32K	;HL=buffer of 8000H
	xor	a
	ld	c,80H		;8000H = 80H x 100H
4:	ld	b,0
3:	ld	(hl),a
	inc	hl
	djnz	3b
	dec	c
	jr	nz,4b
				;next bank
	inc	d
	dec	e
	jr	nz,filll
				;init largest block list headers for each bank
	ld	a,(count)
	ld	b,a		;B=banks counter
	ld	a,(base)	;A=crt bank
	ld	hl,L11		;HL=L11 lists header
initLMAX:
	push	af
	push	bc
	push	hl
	call	BNKSEL		;select bank A
	ld	de,Buf32K
	call	__AddToL	;add DE to HL header
	ld	hl,Buf32K+OFF_STS;HL=pointer of block status
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

	DEFAULT			;restore default bank
	pop	iy
	ei
	ret	

	psect	text
;
;	Converts bank & offset to virtual pointer
;
;	A = virtual bank (bb = 0...3)
;	HL = offset in RAM bank (0aaaaaaa aaaa0000)
;
;	returns HL = 1aaaaaaa aaaa00bb
;
ToVirtual:
	or	l
	ld	l,a		
	set	7,h		;HL = 1aaaaaaa aaaa00bb
	ret
;
;	Converts virtual pointer to bank & offset
;
;	HL = virtual pointer (1aaaaaaa aaaa00bb)
;
;	returns A = virtual bank (bb = 0...3)
;		HL = offset (0aaaaaaa aaaa0000)
;
FromVirtual:
	ld	a,l
	ld	b,a
	and	0F0H
	ld	l,a		
	res	7,h		;HL = 0aaaaaaa aaaa0000
	ld	a,b		
	and	0FH		;A = 000000bb		
	ret
;
;void* myallocstr(unsigned char size);
;
_myallocstr:
	di
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
;	Alloc symbol
;
;	returns HL=pointer of data part (or NULL if no more available memory)
;
_myallocsym:
	di
	ld	c,2		;alloc 40H (bSize=2)
alloc:
	ld	a,(count)
	ld	b,a		;B=banks counter
loop:
				;compute (base) + (count) - B
	ld	a,(base)	;A=base
	ld	l,a
	ld	a,(count)
	add	l		;A=base+count
	sub	b		;A=base+count-B
	push	bc		;B=banks counter, C=bSize on stack
	call	BNKSEL
	pop	bc		;B=banks counter, C=bSize
	push	bc		;B=banks counter, C=bSize on stack
				;compute bank index = (count) - B
	ld	a,(count)
	sub	b		;A=bank index(0...7)
	push	iy
	call	__alloc		;try to alloc
	pop	iy
	jr	nz,alloc_ok
				;local alloc failed
	pop	bc		;B=banks counter, C=bSize
	djnz	loop
				;global alloc failed
	DEFAULT
	ld	hl,0		;return NULL
	ei
	ret
;
alloc_ok:			;HL=allocated block
	DEFAULT
	pop	bc		;B=banks counter
				;compute bank index = (count) - B
	ld	a,(count)
	sub	b		;A=current bank nr.
	call	ToVirtual	;return HL=virtual pointer
	ei
	ret
;	
;	allocS
;
;	BC=memory size (must be <= 8000H)
;	Affected regs: A,BC
;	Returns C=bElement size
;
__allocS:
	dec	bc		;bc = memory size-1
	ld	a,b
	and	7FH		;keep it <= 7FH
	or	a
	jr	z,1f
				;high part != 0
	ld	c,11		;prepare bSize for 8000H
	bit	6,a
	ret	nz		;if bit#14=1 return 11 for 8000H
	dec	c		;C=10
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
;	A=current bank nr. (0,...3)
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
	ld	b,2		;how many times to shift A
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
;	conserve DE,BC
;
AdjustAddr:
	push	bc
	call	FromVirtual
				;A = bank index (bb = 0...3)
				;HL = offset (0aaaaaaa aaaa0000)
	ld	b,a
	ld	a,(base)
	add	a,b
	push	hl
	push	de
	call	BNKSEL		;select bank A
	pop	de
	pop	hl
	ld	a,l		;add 6=OFF_DATA
	add	a,OFF_DATA
	ld	l,a		;now HL points to data part
	pop	bc
	ret
;	
;char	GetByte(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetByte:
	di
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
	DEFAULT
	ei
	ret
;
;	PutByte(char* dest, unsigned char offset, char b)
;
;	dest is in Upper RAM
;
_PutByte:
	di
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
	DEFAULT
	ei
	ret
;
;short	GetWord(char* source, unsigned char offset)
;
;	source is in Upper RAM
;
_GetWord:
	di
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
	DEFAULT
	ei
	ret
;
;	PutWord(char* dest, unsigned char offset, short w)
;
;	dest is in Upper RAM
;
_PutWord:
	di
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
	DEFAULT
	ei
	ret
;
;	ReadMem(char* dest, char* source, short count)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;	count <= 256
;
_ReadMem:
	di
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
	DEFAULT
	ei
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
;	should be 1aaaaaaa aaaa00bb
;
_IsValid:
	di
	ld	hl,3
	add	hl,sp
	ld	a,(hl)
	dec	hl
	ld	l,(hl)
	ld	h,a		;HL=pointer
	rla			;is H bit 7 = 1 ?
	jr	nc,notvalid	;no, it's not valid
	ld	a,l		;should be aaaa00bb
	and	0CH
	jr	nz,notvalid
				;yes, now check record status
	call	FromVirtual
				;A = virtual bank (bb = 0...3)
				;HL = offset (0aaaaaaa aaaa0000)
	ld	b,a
	ld	a,(base)
	add	a,b
	push	hl
	call	BNKSEL
	pop	hl
	ld	a,l		;add 4=OFF_STS
	add	a,OFF_STS
	ld	l,a		;now HL points to the requested byte
	ld	a,(hl)		;FFH if valid
	inc	a
	push	af
	DEFAULT
	pop	af
	jr	nz,notvalid
	ld	l,0FFH
	ei
	ret
notvalid:
	ld	l,0
	ei
	ret
;
;	short	StringLen(char* source)
;
;	source is in Upper RAM
;
_StringLen:
	di
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
	ei
	ret
;
;	GetString(char* dest, char* source)
;
;	source is in Upper RAM
;	destination is in Lower RAM
;
_GetString:
	di
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
	ei
	ret
;
;	PutString(char* source, char* dest)
;
;	source is in Lower RAM
;	destination is in Upper RAM
;
_PutString:
	di
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

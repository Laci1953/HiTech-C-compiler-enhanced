PACK_DATA	equ	0
CKS		equ	0
TRACE_CALLS	equ	0

;
;       Parts of LIBC.LIB
;
;       dissasembled by Andrey Nikitin & Ladislau Szilagyi
;
	psect	text

;=========================================================
; 7EEA alrsh:	alrsh.asm
;=========================================================		
; arithmetic long right shift
; value in HLDE, count in B

alrsh::
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	33
	jr	c,1f		;limit shift to 32 bits
	ld	b,32
1:
	sra	h
	rr	l
	rr	d
	rr	e
	djnz	1b
	ret

;=========================================================
; 7EFE almul:	lmul.asm
; 7F26 mult8b:	
;=========================================================		
; Long multiplication for Z80

; Called with 1st arg in HLDE, 2nd arg on stack. Returns with
; result in HLDE, other argument removed from stack

almul::
llmul::
	ex	de,hl
	ex	(sp),hl		;return address now in hl
	exx
	pop	de		;low word in de
	pop	bc		;low word of multiplier in bc
	exx
	pop	bc		;hi word of multiplier
	push	hl		;restore return address
	ld	hl,0		;initialize product
	exx			;get lo words back
	ld	hl,0
	ld	a,c
	ld	c,b
	call	mult8b
	ld	a,c
	call	mult8b
	exx
	ld	a,c
	exx
	call	mult8b
	exx
	ld	a,b
	exx
	call	mult8b
	push	hl		;low word
	exx
	pop	de
	ret

mult8b:	ld	b,8
3:
	srl	a
	jr	nc,1f
	add	hl,de
	exx
	adc	hl,de
	exx
1:	ex	de,hl
	add	hl,hl
	ex	de,hl
	exx
	ex	de,hl
	adc	hl,hl
	ex	de,hl
	exx
	djnz	3b
	ret

;=========================================================
; 	sbrk.asm
;=========================================================		

	psect	bss

base_pool::	defs	2
top_pool::	defs	2

	psect	text

_sbrk::
	pop	bc
	pop	de
	push	de
	push	bc
	ld	hl,(base_pool)
	add	hl,de
	jr	c,2f		;if overflow, no room
	ld	bc,512+64	;allow 512+64 bytes stack overhead (CGEN !!!)
	add	hl,bc
	jr	c,2f		;if overflow, no room
	sbc	hl,sp
	jr	c,1f
2:
	ld	hl,-1		;no room at the inn
	ret

1:	ld	hl,(base_pool)
	push	hl
	add	hl,de
	ld	(base_pool),hl
	pop	hl
	ret

;=========================================================
; 	(simple) malloc 
;	NO free !!!
;=========================================================		
_malloc::
	pop	de
	pop	bc		;BC=size
	push	bc
	push	de
	ld	hl,(base_pool)
	push	hl		;old pool base on stack
	add	hl,bc
	ex	de,hl		;DE=base_pool+size
	ld	hl,(top_pool)
	sbc	hl,de
	pop	hl		;restore HL=old pool base
	jr	c,full
	ld	(base_pool),de	;save new pool base
	ret			;return HL=old pool base
full:	
	ld	hl,0
	ret

;=========================================================
; 7F80 shar:	shar.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment type operations, when it is in the memory
; location pointed to by HL

shar::
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	16		;16 bits is maximum shift
	jr	c,1f		;is ok
	ld	b,16
1:
	sra	h
	rr	l
	djnz	1b
	ret

;=========================================================
; 7F90 shll:	shll.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment type operations, when it is in the memory
; location pointed to by HL

shll::
shal::
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	16		;16 bits is maximum shift
	jr	c,1f		;is ok
	ld	b,16
1:
	add	hl,hl		;shift left
	djnz	1b
	ret

;=========================================================
; 7F9D shlr:	shlr.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment type operations, when it is in the memory
; location pointed to by HL

shlr::
	ld	a,b		;check for zero shift
	or	a
	ret	z
	cp	16		;16 bits is maximum shift
	jr	c,1f		;is ok
	ld	b,16
1:
	srl	h
	rr	l
	djnz	1b
	ret

;=========================================================
; * 70CB toupper
;=========================================================
;    char toupper(char c) {
;	if(c >= 'a' && c <= 'z') c += 'A' - 'a';
;	return c;
;    }
;
_toupper::
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	cp	'a'
	ret	c
	cp	'z'+1
	ret	nc
	and	0DFH
	ret

;=========================================================
; 7FAD _strcat:	strcat.asm
;=========================================================

_strcat::
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	c,e		;save destination pointer
	ld	b,d

1:	ld	a,(de)
	or	a
	jr	z,2f
	inc	de
	jr	1b

2:	ld	a,(hl)
	ld	(de),a
	or	a
	jr	z,3f
	inc	de
	inc	hl
	jr	2b

3:
	ld	l,c	;restore destination
	ld	h,b
	ret

;=========================================================
; 7FC8 _strcmp:	strcmp.asm
;=========================================================

_strcmp::
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc

1:	ld	a,(de)
	cp	(hl)
	jr	nz,2f
	inc	de
	inc	hl
	or	a
	jr	nz,1b
	ld	hl,0
	ret

2:	ld	hl,1
	ret	nc
	dec	hl
	dec	hl
	ret

;=========================================================
; 7FE2 _strcpy:	strcpy.asm
;=========================================================

_strcpy::
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	c,e
	ld	b,d		;save destination pointer

1:	ld	a,(hl)
	ld	(de),a
	inc	de
	inc	hl
	or	a
	jr	nz,1b
	ld	l,c
	ld	h,b
	ret

;=========================================================
; 7FF4 _strlen:	strlen.asm
;=========================================================

_strlen::
	pop	hl
	pop	de
	push	de
	push	hl
	ld	hl,0

1:	ld	a,(de)
	or	a
	ret	z
	inc	hl
	inc	de
	jr	1b

;=========================================================
; 8002 _strncpy:	strncpy.asm
;=========================================================		

_strncpy::
	call	rcsv
	push	hl

1:
	ld	a,c
	or	b
	jr	z,2f
	dec	bc
	ld	a,(de)
	ld	(hl),a
	inc	hl
	or	a
	jr	z,1b
	inc	de
	jr	1b

2:	pop	hl
	jp	cret

;=========================================================
; 8018 csv:	csv.asm
; 8024 cret:	
; 802B indir:	
; 802C ncsv:	
;=========================================================		

csv::	
IF	TRACE_CALLS
	call	_PrintPC
ENDIF	
	pop	hl		;return address
	push	iy
	push	ix
	ld	ix,0
	add	ix,sp		;new frame pointer
	jp	(hl)

cret::	ld	sp,ix
	pop	ix
	pop	iy
	ret

indir::	jp	(hl)

; New csv: allocates space for stack based on word following
; call ncsv

ncsv::
IF	TRACE_CALLS
	call	_PrintPC
ENDIF	
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

;=========================================================
;8040 rcsv:	rcsv.asm
;=========================================================

ARG	equ	6		;offset of 1st arg

rcsv::
	ex	(sp),iy		;save iy, get return address
	push	ix
	ld	ix,0
	add	ix,sp		;new frame pointer
	ld	l,(ix+ARG+0)
	ld	h,(ix+ARG+1)
	ld	e,(ix+ARG+2)
	ld	d,(ix+ARG+3)
	ld	c,(ix+ARG+4)
	ld	b,(ix+ARG+5)
	jp	(iy)

IF	CKS

;
;	unsigned char CksData(void)
;	unsigned char CksText(void)
;

_CksData::
	ld	hl,__Hdata
	ld	de,__Ldata
docks:
	call	Cks
	ld	l,a
	ret
;
_CksText::
	ld	hl,__Htext
	ld	de,__Ltext
	jr	docks
;
Cks:
	xor	a
	sbc	hl,de
	ex	de,hl		;HL=from,DE=count
	xor	a
1:
	add	a,(hl)
	ld	c,a
	inc	hl
	dec	de
	ld	a,d
	or	e
	ld	a,c
	jr	nz,1b
	ret
;

ENDIF

IF	PACK_DATA
;
;	Read a signed integer from a compressed array of bytes
;
;	int getnr(char* array, int index)
;
;	each number has 12 bits
;
;	byte0: (first 8 bits of number)
;	byte1: (last 4 bits of number, first 4 bits of next number)
;	byte2: (last 8 bits of next number)
;
;	index par ==> low = byte( 3*(index/2) ), 
;			high = (byte( 3*(index/2) + 1) & 0FH) | 0F0H if bit3=1
;	index impar ==> low = (byte( 3*((index-1)/2) + 1) >> 4) | (byte( 3*((index-1)/2) + 2) & 0FH) << 4
;			high = ((byte( 3*((index-1)/2) + 2) & 0F0H) >> 4) | 0F0H if bit3=1,
;
_getnr::
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE=array pointer
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=index
	srl	b
	rr	c		;BC=index/2, CY=1:odd,0:even
	ld	h,b
	ld	l,c
	jr	c,odd
				;even index
	add	hl,hl
	add	hl,bc		;HL=3*(index/2)
	add	hl,de		;HL=array pointer + 3*(index/2)
	ld	e,(hl)		;low
	inc	hl
	ld	a,(hl)
	and	0FH
	bit	3,a
	jr	z,positive
	or	0F0H		;expand sign
positive:
	ld	d,a		;high
	ex	de,hl
	ret
odd:				;odd index
	add	hl,hl
	add	hl,bc		;HL=3*((index-1)/2)
	inc	hl		;HL=3*((index-1)/2) + 1
	add	hl,de		;HL=array pointer + 3*((index-1)/2) + 1
	ld	e,(hl)
	srl	e
	srl	e
	srl	e
	srl	e
	inc	hl
	ld	a,(hl)
	sla	a
	sla	a
	sla	a
	sla	a
	or	e
	ld	e,a		;low
	ld	a,(hl)
	sra	a
	sra	a
	sra	a
	sra	a
	ld	d,a		;high
	ex	de,hl
	ret			;return HL

ENDIF

; End file libc5.asm


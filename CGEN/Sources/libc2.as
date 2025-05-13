;
;       Parts of LIBC.LIB
;
;       dissasembled by Andrey Nikitin & Ladislau Szilagyi
;

	psect	text

__getargs::
	pop	hl		; return address
	exx
	pop	hl
	pop	hl		; unjunk stack
	ld	a,(80h)		; get buffer length
	inc	a		; allow for null byte
	neg
	ld	l,a
	ld	h,-1
	add	hl,sp
	ld	sp,hl		; allow space for args
	ld	bc,0		; flag end of args
	push	bc
	ld	hl,80h		; address of argument buffer
	ld	c,(hl)
	ld	b,0
	add	hl,bc
	ld	b,c
	ex	de,hl
	ld	hl,(6)
	ld	c,1
	dec	hl
	ld	(hl),0
	inc	b
	jr	3f

2:	ld	a,(de)
	cp	' '
	dec	de
	jr	nz,1f
	push	hl
	inc	c
4:	ld	a,(de)		; remove extra spaces
	cp	' '
	jr	nz,5f
	dec	de
	jr	4b
5:
	xor	a
1:	dec	hl
	ld	(hl),a
3:
	djnz	2b
	ld	(__argc_),bc	; store argcount
	ld	hl,nularg
	push	hl
	ld	hl,0
	add	hl,sp
	exx
	push	de		; junk the stack again
	push	de
	push	hl		; return address
	exx
	ret

;=========================================================
; 7AB0 asallsh:	asallsh.asm
;=========================================================		
;	ASsign Arithmetic Long Left  SHift
;	ASsign Logical Long Left SHift

asallsh::
aslllsh::
	push	bc		; save the count
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl		; save pointer, restore count
	push	bc		; hi word
	ex	(sp),hl
	pop	bc
	call	allsh		; do the shift
	jp	iregstore	; go store the value and return

;=========================================================
; 7AC2 allsh:	allsh.asm
;=========================================================
;	arithmetic long left shift
;	value in HLDE, count in B

allsh::
lllsh::
	ld	a,b		; check for zero shift
	or	a
	ret	z
	cp	33
	jr	c,1f		; limit shift to 32 bits
	ld	b,32
1:
	ex	de,hl
	add	hl,hl
	ex	de,hl 
	adc	hl,hl
	djnz	1b
	ret

;=========================================================
; 7AD3 asalrsh:	asalrsh.asm
;=========================================================
;	ASsign Arithmetic Long Right  SHift

asalrsh::
	push	bc		; save the count
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl		; save pointer, restore count
	push	bc		; hi word
	ex	(sp),hl
	pop	bc
	call	alrsh		; do the shift
	jp	iregstore	; go store the value and return


;=========================================================
; 7AE5 asar:	asar.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment  type operations, when it is in the memory
; location pointed to by HL

asar::
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	hl		; save for the store
	ex	de,hl
	call	shar
	ex	de,hl
	pop	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		; return value in hl
	ret

;=========================================================
; 7AF4 asaladd:	asladd.asm
;=========================================================

asaladd::
aslladd::
	call	iregset
	call	aladd
	jp	iregstore

;=========================================================
; 7AFD asaland:	asland.asm
;=========================================================

asaland::
aslland::
	call	iregset
	call	aland
	jp	iregstore

;=========================================================
; 7B06 asll:	asll.asm
;=========================================================
; Shift operations - the count is always in B, the quantity
; to be shifted is in HL, except for the assignment type
; operations, when it is in the memory location pointed to
; by HL

asll::
asal::
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	hl		; save for the store
	ex	de,hl
	call	shal
	ex	de,hl
	pop	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		; return value in hl
	ret

;=========================================================
; 7B15 asllrsh:	asllrsh.asm
;=========================================================
;	ASsign Logical Long Right  SHift

asllrsh::
	push	bc		; save the count
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl		; save pointer, restore count
	push	bc		; hi word
	ex	(sp),hl
	pop	bc
	call	llrsh		; do the shift
	jp	iregstore	; go store the value and return

;=========================================================
; 7B27 asalmul:	aslmul.asm
;=========================================================

asalmul::
asllmul::
	call	iregset
	call	almul
	jp	iregstore

;=========================================================
; 7B30 asalor:	aslor.asm
;=========================================================

asalor::
asllor::
	call	iregset
	call	alor
	jp	iregstore

;=========================================================
; 7B39 asalsub:	aslsub.asm
;=========================================================

asalsub::
asllsub::
	call	iregset
	call	alsub
	jp	iregstore

;=========================================================
; 7B42 asalxor:	aslxor.asm
;=========================================================		

asalxor::
asllxor::
	call	iregset
	call	alxor
	jp	iregstore

;=========================================================
; 7B4B digit:	
; 7B52 _atoi:	atoi.asm
;=========================================================		

digit:	sub	'0'
	ret	c
	cp	10
	ccf
	ret

_atoi::	pop	bc	; return address
	pop	de
	push	de
	push	bc
	ld	hl,0
1:
	ld	a,(de)
	inc	de
	cp	' '
	jr	z,1b
	cp	9	; tab
	jr	z,1b
	dec	de	; point to 1st non blank char
	cp	'-'
	jr	z,3f
	cp	'+'
	jr	nz,2f
	or	a	; reset zero flag
3:
	inc	de
2:	ex	af,af'
1:
	ld	a,(de)
	inc	de
	call	digit
	jr	c,3f
	add	hl,hl
	ld	c,l
	ld	b,h
	add	hl,hl
	add	hl,hl
	add	hl,bc
	ld	c,a
	ld	b,0
	add	hl,bc
	jr	1b
3:
	ex	af,af'
	ret	nz
	ex	de,hl
	ld	hl,0
	sbc	hl,de
	ret

;=========================================================
;7B8B _atol:	atol.asm
;=========================================================
_atol::
	call ncsv
	defw -5
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	pop	iy
	jr	l2
l3:
	inc	iy
l2:
	ld	a,(iy+0)
	cp	32
	jr	z,l3
	cp	9
	jr	z,l3
	ld	(ix+-4),0
	ld	(ix+-3),0
	ld	(ix+-2),0
	ld	(ix+-1),0
	ld	(ix+-5),0
	cp	45
	jr	nz,l6
	inc	(ix+-5)
	inc	iy
	jr	l6
l7:
	ld	a,(iy+0)
	ld	e,a
	rla
	sbc	a,a
	ld	d,a
	ld	hl,-48
	add	hl,de
	ex	de,hl
	ld	a,d
	rla
	sbc	a,a
	ld	l,a
	ld	h,a
	push	hl
	push	de
	ld	de,10
	ld	hl,0
	push	hl
	push	de
	ld	e,(ix+-4)
	ld	d,(ix+-3)
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	call	almul
	call	aladd
	ld	(ix+-4),e
	ld	(ix+-3),d
	ld	(ix+-2),l
	ld	(ix+-1),h
	inc	iy
l6:
	ld	a,(iy+0)
	ld	e,a
	rla
	sbc	a,a
	ld	d,a
	ld	hl,__ctype_+1
	add	hl,de
	bit	2,(hl)
	jr	nz,l7
	ld	a,(ix+-5)
	or	a
	ld	e,(ix+-4)
	ld	d,(ix+-3)
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	jp	z,cret
	push	hl
	push	de
	ld	hl,0
	pop	bc
	or	a
	sbc	hl,bc
	pop	bc
	ex	de,hl
	ld	hl,0
	sbc	hl,bc
	jp	cret

;=========================================================
;7C37 _blkclr:	blkclr.asm
;=========================================================
;	blkclr(ptr, size)
;	char *	ptr; unsigned short size;
;	Fills memory with size null bytes

_blkclr::
	pop	de	;return address
	pop	hl	;pointer
	pop	bc	;count
	push	bc	;adjust stack
	push	hl
	push	de
	ld	e,0
1:
	ld	a,c	;check for finished
	or	b
	ret	z
	ld	(hl),e
	inc	hl
	dec	bc
	jr	1b

;=========================================================		
; 7C47 iregset:		iregset.asm
; 7C61 iregstore:
;=========================================================		
; routines to support the assignment versions of the long operations

iregset::
	ld	e,(hl)	;pick up arg
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl	;save pointer, get return address
	push	bc	;now put the hi word into hl
	ex	(sp),hl	;saving return address on stack again
	pop	bc	;now return address in bc
	exx
	pop	hl	;the pointer
	pop	bc	;the final return address
	pop	de	;now the low word of arg2
	ex	(sp),hl	;get the hi word, save pointer
	push	bc	;this is the return address again
	ex	(sp),hl	;into hl again
	pop	bc	;hi word now in bc
	ex	(sp),hl	;AHA! got return address on stack, ptr in hl
	push	hl	;now its easy, save pointer
	push	bc	;hi word
	push	de	;low word
	exx
	push	bc	;immediate return address
	ret		;finito

iregstore::
	ex	(sp),hl	;get pointer into hl
	pop	bc	;hi word in bc
	ld	(hl),b
	dec	hl
	ld	(hl),c
	dec	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e	;all done
	push	bc	;restore to hl
	pop	hl
	ret		;and return with value in hl

;=========================================================
; 7C6D aladd:	ladd.asm
;=========================================================

aladd::
lladd::
	exx
	pop	hl
	exx
	pop	bc
	ex	de,hl
	add	hl,bc
	ex	de,hl
	pop	bc
	adc	hl,bc
	exx
	push	hl
	exx
	ret

;=========================================================
; 7C7B aland:	land.asm
;=========================================================

aland::
lland::
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	and	e
	ld	e,a
	ld	a,b
	and	d
	ld	d,a
	pop	bc
	ld	a,c
	and	l
	ld	l,a
	ld	a,b
	and	h
	ld	h,a
	exx
	push	hl
	exx
	ret

;=========================================================
; 7C90 llrsh:	llrsh.asm
;=========================================================
;	logical long right shift
;	value in HLDE, count in B

llrsh::
	ld	a,b	;check for zero shift
	or	a
	ret	z
	cp	33
	jr	c,1f	;limit shift to 32 bits
	ld	b,32
1:
	srl	h
	rr	l
	rr	d
	rr	e
	djnz	1b
	ret

;=========================================================
; 7CA4 alor:	lor.asm
;=========================================================

alor::
llor::
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	or	e
	ld	e,a
	ld	a,b
	or	d
	ld	d,a
	pop	bc
	ld	a,c
	or	l
	ld	l,a
	ld	a,b
	or	h
	ld	h,a
	exx
	push	hl
	exx
	ret

;=========================================================
; 7CB9 arelop:	lrelop.asm
;=========================================================
; long relational operation - returns flags as though
; a long subtract was done.

arelop::
lrelop::
	exx			;select	alternate reg set
	pop	hl		;return	address
	exx			;get other set again
	pop	bc		;low word of 2nd arg
	ex	de,hl		;put hi	word of	1st in de
	ex	(sp),hl		;get hi	word of	2nd in hl
	ex	de,hl		;hi word of 1st	back in	hl
	ld	a,h		;test for differing signs
	xor	d
	jp	p,2f		;the same, so ok
	ld	a,h		;get the sign of the LHS
	or	1		;ensure zero flag is reset, set sign flag
	pop	hl		;unjunk stack
	jr	1f		;return	with sign of LHS
2:
	or	a
	sbc	hl,de		;set the flags
	pop	hl		;low word of 1st into hl again
	jr	nz,1f		;go return if not zero
	sbc	hl,bc		;now set flags on basis	of low word
	jr	z,1f		;if zero, all ok
	ld	a,2		;make non-zero
	rra			;rotate	carry into sign
	or	a		;set minus flag
	rlca			;put carry flag	back

1:
	exx			;get return address
	jp	(hl)		;and return with stack clean

;=========================================================
; 7CDC brelop:	brelop.asm
;=========================================================
; byte relational operation - returns flags correctly for
; comparision of words in a and c

brelop::
	push	de
	ld	e,a
	xor	b	;compare signs
	jp	m,1f	;if different, return sign of lhs
	ld	a,e
	sbc	a,b
	pop	de
	ret
1:
	ld	a,e	;get sign of lhs
	and	80h	;mask out sign flag
	ld	d,a
	ld	a,e
	sbc	a,b	;set carry flag if appropriate
	ld	a,d
	inc	a	;set sign flag as appropriate and reset Z flag
	pop	de
	ret

;=========================================================
;7CF0 wrelop:	wrelop.asm
;=========================================================
; word relational operation - returns flags correctly for
; comparision of words in hl and de

wrelop::
	ld	a,h
	xor	d	;compare signs
	jp	m,1f	;if different, return sign of lhs
	sbc	hl,de	;just set flags as normal
	ret
1:
	ld	a,h	;get sign of lhs
	and	80h	;mask out sign flag
	sbc	hl,de	;set carry flag if appropriate
	inc	a	;set sign flag as appropriate and reset Z flag
	ret

;=========================================================
; 7CFF alsub:	lsub.asm
;=========================================================

alsub::
llsub::
	exx
	pop	hl
	exx
	pop	bc
	ex	de,hl
	or	a
	sbc	hl,bc
	ex	de,hl
	pop	bc
	sbc	hl,bc
	exx
	push	hl
	exx
	ret

;=========================================================
; 7D0F alxor:	lxor.asm
;=========================================================		

alxor::
llxor::
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	xor	e
	ld	e,a
	ld	a,b
	xor	d
	ld	d,a
	pop	bc
	ld	a,c
	xor	l
	ld	l,a
	ld	a,b
	xor	h
	ld	h,a
	exx
	push	hl
	exx
	ret

;=========================================================
; 7D24 amod:	idiv.asm
; 7D29 lmod:	
; 7D33 adiv:	
; 7D80 negif:	
; 7D83 negat:	
;=========================================================		
; 16 bit divide and modulus routines
; called with dividend in hl and divisor in de
; returns with result in hl.
; adiv (amod) is signed divide (modulus), ldiv (lmod) is unsigned

amod::
	call	adiv
	ex	de,hl	;put modulus in hl
	ret

lmod::
	call	ldiv
	ex	de,hl
	ret

ldiv::
	xor	a
	ex	af,af'
	ex	de,hl
	jr	dv1

adiv::
	ld	a,h
	xor	d	;set sign flag for quotient
	ld	a,h	;get sign of dividend
	ex	af,af'
	call	negif
	ex	de,hl
	call	negif

dv1:	ld	b,1
	ld	a,h
	or	l
	ret	z

dv8:	push	hl
	add	hl,hl
	jr	c,dv2
	ld	a,d
	cp	h
	jr	c,dv2
	jr	nz,dv6
	ld	a,e
	cp	l
	jr	c,dv2

dv6:	pop	af
	inc	b
	jr	dv8

dv2:	pop	hl
	ex	de,hl
	push	hl
	ld	hl,0
	ex	(sp),hl

dv4:	ld	a,h
	cp	d
	jr	c,dv3
	jr	nz,dv5

	ld	a,l
	cp	e
	jr	c,dv3

dv5:	sbc	hl,de

dv3:	ex	(sp),hl
	ccf
	adc	hl,hl
	srl	d
	rr	e
	ex	(sp),hl
	djnz	dv4
	pop	de
	ex	de,hl
	ex	af,af'
	call	m,negat
	ex	de,hl
	or	a	;test remainder sign bit
	call	m,negat
	ex	de,hl
	ret

negif:	bit	7,h
	ret	z
negat:	ld	b,h
	ld	c,l
	ld	hl,0
	or	a
	sbc	hl,bc
	ret

; End file libc2.asm



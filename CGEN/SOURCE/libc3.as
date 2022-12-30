;
;	Disassembled by Andrey Nikitin & Ladislau Szilagyi
;
*Title Disassembling CGEN.COM Hi-Tech C v3.09
*Heading Checking recreated code with original

; File - libc3.asm Created 09.03.2019 Last Modified 31.01.2021

;
; For easier obtaining an exact copy of the binary executable
; file, all library functions are located in files libc1.asm,
; libc2.asm, libc3.asm, libc4.asm, libc5.asm.
;
global	ncsv, cret, indir
global	csv
;=========================================================
;7D8C lregset:	ldiv.asm
;7D9B iregset:	
;7DB2 sgndiv:	
;7DC3 asaldiv:	
;7DC9 store:	
;7DD8 aldiv:	
;7DDB dosdiv:	
;7DEB lldiv:	
;7DEE doudiv:	
;7DF7 aslldiv:	
;7DFF almod:	
;7E02 dosrem:	
;7E11 asalmod:	
;7E19 llmod:	
;7E24 asllmod:	
;7E2C negat:	
;7E3B negif:	
;7E50 divide:	
;=========================================================		
;	Long division routines for Z80

	global	lldiv,aldiv,almod,llmod,aslldiv,asaldiv,asllmod,asalmod

	psect	text

; Called with dividend in HLDE, divisor on stack under 2 return
; addresses. Returns with dividend in HL/HL', divisor in DE/DE'
; on return the HIGH words are selected.

lregset:
	pop	bc		;get top return address
	exx			;select other bank
	pop	bc		;return address of call to this module
	pop	de		;get low word of divisor
	exx			;select hi bank
	ex	de,hl		;dividend.low -> hl
	ex	(sp),hl		;divisor.high -> hl
	ex	de,hl		;dividend.high -> hl
	exx			;back to low bank
	push	bc		;put outer r.a. back on stack
	pop	hl		;return address
	ex	(sp),hl		;dividend.low -> hl
	exx
	push	bc		;top return address
	ret

; Much the same as lregset, except that on entry the dividend
; is pointed to by HL.
; The pointer is saved in iy for subsequent updating of memory

iregset:
	pop	de		;immediate return address
	call	lregset		;returns with hi words selected
	push	hl		;save a copy for 'ron
	ex	(sp),iy		;get it in iy, saving old iy
	ld	h,(iy+3)	;high order byte
	ld	l,(iy+2)	;byte 2
	exx			;back to low bank
	push	hl		;return address
	ld	h,(iy+1)	;byte 1
	ld	l,(iy+0)	;and LSB
	exx			;restore hi words
	ret			;now return

; Called with hi words selected, performs division on the absolute
; values of the dividend and divisor. Quotient is positive

sgndiv:
	call	negif		;make dividend positive
	exx
	ex	de,hl		;put divisor in HL/HL'
	exx
	ex	de,hl
	call	negif		;make divisor positive
	ex	de,hl		;restore divisor to DE/DE'
	exx
	ex	de,hl
	exx			;select high words again
	jp	divide		;do division

asaldiv:
	call	iregset
	call	dosdiv
store:
	ld	(iy+0),e
	ld	(iy+1),d
	ld	(iy+2),l
	ld	(iy+3),h
	pop	iy		;restore old iy
	ret

aldiv:	call	lregset		;get args

; Called with high words selected, performs signed division by
; the rule that the quotient is negative iff the signs of the
; dividend and divisor differ returns quotient in HL/DE

dosdiv:
	ld	a,h
	xor	d
	ex	af,af'		;sign bit is now sign of quotient
	call	sgndiv		;do signed division
	ex	af,af'		;get sign flag back
	push	bc		;high word
	exx
	pop	hl
	ld	e,c		;low word of quotient
	ld	d,b
	jp	m,negat		;negate quotient if necessary
	ret

lldiv:	call	lregset

; Called with high words selected, performs unsigned division
; returns with quotient in HL/DE

doudiv:
	call	divide		;unsigned division
	push	bc		;high word of quotien
	exx
	pop	hl
	ld	e,c		;low word
	ld	d,b
	ret

aslldiv:
	call	iregset
	call	doudiv
	jr	store

almod:
	call	lregset

; Called with high words selected, performs signed modulus - the rule
; is that the sign of the remainder is the sign of the dividend

dosrem:
	ld	a,h		;get sign of dividend
	ex	af,af'		;save it
	call	sgndiv		;do signed division
	push	hl		;high word
	exx
	pop	de
	ex	de,hl		;put high word in hl
	ex	af,af'		;get sign bit back
	or	a
	jp	m,negat		;negate if necessary
	ret

asalmod:
	call	iregset
	call	dosrem
	jr	store

llmod:
	call	lregset

; Called with high words selected, perform unsigned modulus

dourem:
	call	divide
	push	hl		;high word of remainder
	exx
	pop	de
	ex	de,hl		;high word in hl
	ret

asllmod:
	call	iregset
	call	dourem
	jr	store

;	Negate the long in HL/DE

negat:	push	hl		;save high word
	ld	hl,0
	or	a
	sbc	hl,de
	ex	de,hl
	pop	bc		;get high word back
	ld	hl,0
	sbc	hl,bc
	ret			;finito

negif:	;called with high word in HL, low word in HL'
	;returns with positive value

	bit	7,h		;check sign
	ret	z		;already positive
	exx			;select low word
	ld	c,l
	ld	b,h
	ld	hl,0
	or	a
	sbc	hl,bc
	exx
	ld	c,l
	ld	b,h
	ld	hl,0
	sbc	hl,bc
	ret			;finito

; Called with dividend in HL/HL', divisor in DE/DE', high words in
; selected register set
; returns with quotient in BC/BC', remainder in HL/HL', high words
; selected

divide:
	ld	bc,0		;initialize quotient
	ld	a,e		;check for zero divisor
	or	d
	exx
	ld	bc,0
	or	e
	or	d
	exx			;restor high words
	ret	z		;return with quotient == 0
	ld	a,1		;loop count
	jr	3f		;enter loop in middle
1:
	push	hl		;save divisor
	exx
	push	hl		;low word
	or	a		;clear carry
	sbc	hl,de		;subtract low word
	exx
	sbc	hl,de		;sbutract hi word
	exx
	pop	hl		;restore dividend
	exx
	pop	hl		;and hi word
	jr	c,2f		;finished - divisor is big enough
	exx
	inc	a		;increment count
	ex	de,hl		;put divisor in hl - still low word
	add	hl,hl		;shift left
	ex	de,hl		;put back in de
	exx			;get hi word
	ex	de,hl
	adc	hl,hl		;shift with carry
	ex	de,hl
3:
	bit	7,d		;test for max divisor
	jr	z,1b		;loop if msb not set

2:	;arrive here with shifted divisor, loop count in a, and low words
	;selected
	
3:
	push	hl		;save dividend
	exx
	push	hl		;low word
	or	a		;clear carry
	sbc	hl,de
	exx
	sbc	hl,de
	exx			;restore low word
	jr	nc,4f
	pop	hl		;restore low word of dividend
	exx
	pop	hl		;hi word
	exx			;restore low word
	jr	5f
4:
	inc	sp		;unjunk stack
	inc	sp
	inc	sp
	inc	sp
5:
	ccf			;complement carry bit
	rl	c		;shift in carry bit
	rl	b		;next byte
	exx			;hi word
	rl	c
	rl	b
	srl	d		;now shift divisor right
	rr	e
	exx			;get low word back
	rr	d
	rr	e
	exx			;select hi word again
	dec	a		;decrement loop count
	jr	nz,3b
	ret			;finished

;=========================================================
;7EAC aslr:	aslr.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the assignment
; type operations, when it is in the memory location pointed to by
; HL

	global	aslr	;assign shift logical right

	psect	text

	global	shlr
aslr:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	hl		;save for the store
	ex	de,hl
	call	shlr
	ex	de,hl
	pop	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

;=========================================================
;7EBB _movmem:	bmove.asm
;=========================================================		
;	bmove(from, to, count)

	global	_bmove, _movmem

	psect	text
_movmem:
_bmove:
	pop	hl		;return address
	exx
	pop	hl		;from
	pop	de		;to
	pop	bc		;count
	ld	a,b
	or	c
	jr	z,1f
	ldir
1:
	push	bc		;stack is as it was
	push	de
	push	hl
	exx
	jp	(hl)

; End file libc3.asm



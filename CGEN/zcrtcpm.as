;
;	Szilagyi Ladislau
;
;	ladislau_szilagyi@euroqst.ro
;
;	December 2022 - March 2023
;
	psect	text

	ld	hl,(6)		; base address of fdos
	ld	sp,hl		; stack grows downwards
	call	crttop
	jp	0

	psect	top

crttop:
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
;
	ld	hl,__Hbss
	ld	(base_pool),hl	; pool start
;
	ld	hl,(6)		; base address of fdos
	ld	bc,512+64	; allow 512+64 bytes for the stack
	sbc	hl,bc
	ld	(top_pool),hl	; pool end
;
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
	call	__getargs
	pop	bc		; unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
	ret

__getargs:
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

	psect	bss

nularg::defs	1

	end

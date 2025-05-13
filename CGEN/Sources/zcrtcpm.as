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
	push	hl
	call	_exit
	jp	0

	psect	bss

nularg::defs	1

	end

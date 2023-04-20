;
;       Parts of LIBC.LIB
;
;       dissasembled by Andrey Nikitin & Ladislau Szilagyi
;

	global	ncsv, cret, indir
	global	csv
;=========================================================
; 7ECB amul:	imul.asm
; 7EDE mult8b:	
;=========================================================		
;	16 bit integer multiply

;	on entry, left operand is in hl, right operand in de

	psect	text

	global	amul,lmul
amul:
lmul:
	ld	a,e
	ld	c,d
	ex	de,hl
	ld	hl,0
	ld	b,8
	call	mult8b
	ex	de,hl
	jr	3f
2:	add	hl,hl
3:
	djnz	2b
	ex	de,hl
1:
	ld	a,c
mult8b:
	srl	a
	jr	nc,1f
	add	hl,de
1:	ex	de,hl
	add	hl,hl
	ex	de,hl
	ret	z
	djnz	mult8b
	ret

;*********************************************************

	psect	data

_allocp:defb	0,0
_allocs:defb	0,0,0,0,0,0
_alloct:defb	0,0
_allocx:defb	0,0,0
F170:	defb	0,0

	psect	text

	global	_malloc

_malloc:

call csv
push hl
ld	hl,(_allocs)
ld	a,l
or	h
jp	nz,l3
ld	hl,_allocs+3
ld	(_allocs),hl
ld	(_alloct),hl
ld	hl,_allocs
ld	(_allocs+3),hl
ld	(_allocp),hl
ld	hl,_allocs+2
set	0,(hl)
ld	hl,_allocs+5
set	0,(hl)
l3:
ld	e,(ix+6)
ld	d,(ix+7)
ld	hl,5
add	hl,de
ld	de,3
call	ldiv
ld	de,3
call	lmul
ld	(ix+6),l
ld	(ix+7),h
ld	iy,(_allocp)
l4:
ld	hl,0
ld	(F170),hl
l7:
bit	0,(iy+2)
jp	nz,l10
jp	l11
l12:
ld	l,(ix+-2)
ld	h,(ix+-1)
ld	c,(hl)
inc	hl
ld	b,(hl)
ld	(iy+0),c
ld	(iy+1),b
l11:
ld	l,(iy+0)
ld	h,(iy+1)
ld	(ix+-2),l
ld	(ix+-1),h
inc	hl
inc	hl
bit	0,(hl)
jp	z,l12
push	iy
pop	de
ld	l,(ix+6)
ld	h,(ix+7)
add	hl,de
ex	de,hl
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
push	iy
jp	c,L5
pop	de
ld	l,(ix+6)
ld	h,(ix+7)
add	hl,de
push	iy
pop	de
call	wrelop
push	iy
jp	c,L5
pop	de
ld	l,(ix+6)
ld	h,(ix+7)
add	hl,de
ld	(_allocp),hl
ld	e,(ix+-2)
ld	d,(ix+-1)
call	wrelop
ld	hl,(_allocp)
jp	nc,L3
ld	de,_allocx
push	hl
ld	bc,3
ldir
pop	hl
ld	e,(iy+0)
ld	d,(iy+1)
ld	hl,(_allocp)
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
ld	(hl),0
dec	hl
dec	hl
L3:
ld	(iy+0),l
ld	(iy+1),h
set	0,(iy+2)
push	iy
pop	hl
inc	hl
inc	hl
inc	hl
jp	cret
l10:
push	iy
L5:
pop	hl
ld	(ix+-2),l
ld	(ix+-1),h
ld	l,(iy+0)
ld	h,(iy+1)
push	hl
pop	iy
push	hl
pop	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	c,l7
ld	de,(_alloct)
ld	l,(ix+-2)
ld	h,(ix+-1)
or	a
sbc	hl,de
jp	nz,L6
ld	de,_allocs
push	iy
pop	hl
or	a
sbc	hl,de
jp	z,l18
L6:
ld	hl,0
jp	cret
l18:
ld	hl,(F170)
inc	hl
ld	(F170),hl
ex	de,hl
ld	hl,1
call	wrelop
jp	nc,l7
ld	de,-259
ld	l,(ix+6)
ld	h,(ix+7)
call	wrelop
jp	c,l21
ld	l,(ix+6)
ld	h,(ix+7)
ld	(F170),hl
jp	l22
l21:
ld	e,(ix+6)
ld	d,(ix+7)
ld	hl,257
add	hl,de
ld	de,255
call	ldiv
ld	de,255
call	lmul
ld	(F170),hl
l22:
ld	hl,(F170)
push	hl
call	_sbrk
pop	bc
ld	(ix+-2),l
ld	(ix+-1),h
ld	de,-1
or	a
sbc	hl,de
jp	z,L6
ld	e,(ix+-2)
ld	d,(ix+-1)
ld	hl,(_alloct)
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
inc	hl
or	a
sbc	hl,de
ld	hl,(_alloct)
inc	hl
inc	hl
jp	z,L4
set	0,(hl)
jp	l25
L4:
res	0,(hl)
l25:
ld	de,(F170)
ld	l,(ix+-2)
ld	h,(ix+-1)
add	hl,de
dec	hl
dec	hl
dec	hl
ex	de,hl
ld	l,(ix+-2)
ld	h,(ix+-1)
ld	(hl),e
inc	hl
ld	(hl),d
ld	(_alloct),de
ld	de,_allocs
ld	hl,(_alloct)
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
set	0,(hl)
ld	l,(ix+-2)
ld	h,(ix+-1)
inc	hl
inc	hl
res	0,(hl)
jp	l4

	global	_free
_free:

call csv
ld	l,(ix+6)
ld	h,(ix+7)
dec	hl
dec	hl
dec	hl
push	hl
pop	iy
ld	(_allocp),iy
res	0,(iy+2)
jp	cret

; End file libc4.asm


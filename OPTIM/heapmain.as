global	_heapmain
global	ncsv, cret, indir
global	_key_f
global	_pr_error
global	wrelop
global	wrelop
global	_freopen
global	__iob
global	wrelop
psect	heap
_heapmain:
global csv
call csv
ld	l,(ix+6)
ld	h,(ix+7)
dec	hl
ld	(ix+6),l
ld	(ix+7),h
ld	l,(ix+8)
ld	h,(ix+9)
inc	hl
inc	hl
ld	(ix+8),l
ld	(ix+9),h
jp	l3
l4:
ld	l,(ix+8)
ld	h,(ix+9)
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
inc	hl
ld	a,(hl)
cp	70
jp	z,l9
cp	102
jp	nz,l10
l9:
ld	a,1
ld	(_key_f),a
l6:
ld	l,(ix+8)
ld	h,(ix+9)
inc	hl
inc	hl
ld	(ix+8),l
ld	(ix+9),h
ld	l,(ix+6)
ld	h,(ix+7)
dec	hl
ld	(ix+6),l
ld	(ix+7),h
l3:
ld	e,(ix+6)
ld	d,(ix+7)
ld	hl,0
call	wrelop
jp	m,L2
jp	l5
l10:
ld	hl,19f
push	hl
call	_pr_error
pop	bc
jp	l6
L2:
ld	l,(ix+8)
ld	h,(ix+9)
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
ld	a,(hl)
cp	45
jp	z,l4
l5:
ld	e,(ix+6)
ld	d,(ix+7)
ld	hl,0
call	wrelop
jp	p,cret
ld	hl,__iob
push	hl
ld	hl,29f
push	hl
ld	l,(ix+8)
ld	h,(ix+9)
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
call	_freopen
pop	bc
pop	bc
pop	bc
ld	a,l
or	h
jp	nz,l12
ld	l,(ix+8)
ld	h,(ix+9)
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
ld	hl,39f
push	hl
call	_pr_error
pop	bc
pop	bc
l12:
ld	e,(ix+6)
ld	d,(ix+7)
ld	hl,1
call	wrelop
jp	p,cret
ld	hl,__iob+8
push	hl
ld	hl,49f
push	hl
ld	l,(ix+8)
ld	h,(ix+9)
inc	hl
inc	hl
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
call	_freopen
pop	bc
pop	bc
pop	bc
ld	a,l
or	h
jp	nz,cret
ld	l,(ix+8)
ld	h,(ix+9)
inc	hl
inc	hl
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
ld	hl,59f
push	hl
call	_pr_error
jp	cret
19:
defb	73,108,108,101,103,97,108,32,115,119,105,116,99,104,0
29:
defb	114,0
39:
defb	67,97,110,39,116,32,111,112,101,110,32,37,115,0
49:
defb	119,0
59:
defb	67,97,110,39,116,32,99,114,101,97,116,101,32,37,115,0

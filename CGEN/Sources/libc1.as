;
;       Parts of LIBC.LIB
;
;       dissasembled by Andrey Nikitin & Ladislau Szilagyi
;

	psect	text

;=========================================================
;	Offsets of things in the _iob structure

	ptr	equ	0	; pointer to next byte
	cnt	equ	2	; number of bytes left
	base	equ	4	; beginning of buffer
	flag	equ	6	; flag bits
	file	equ	7	; file number

;	The bit numbers of the flags in flag
	
	_IOREAD_BIT	equ	0
	_IOWRT_BIT	equ	1
	_IONBF_BIT	equ	2
	_IOMYBUF_BIT	equ	3
	_IOEOF_BIT	equ	4
	_IOERR_BIT	equ	5
	_IOSTRG_BIT	equ	6
	_IOBINARY_BIT	equ	7

;	Various characters

	CPMEOF	equ	032q	; EOF byte 
	NEWLINE	equ	012q	; newline character 
	RETURN	equ	015q	; carriage return 

	EOF	equ	-1	; stdio EOF value

;=========================================================
; sub_70F8 def_fcb Define FCB fields
;=========================================================
;    char def_fcb(register struct fcb * fc, char * fname) {
;	char * cp;
;
;	fc->dr = 0;
;	if(*(fname+1) == ':') {		
;	    fc->dr = *(fname) & 0xf;	
;	    fname += 2;			
;	}					
;	cp = &fc->name;
;	while((*fname != '.')
;	&& ('\t' < *fname)		
;	&& (cp < &fc->ft))  *(cp++) = toupper(*(fname++));
;	while(cp < &fc->ft) *(cp++) = ' ';
;	do {if(*fname == 0) break;} while(*(fname++) != '.');	
;	while(('\t' < *(fname))
;	&& (cp < (char*)&fc->ex)) *(cp++) = toupper(*(++fname));
;	while(cp <= (char*)&fc->ex) *(cp++) = ' ';
;	fc->ex = fc->nr =  0;		
;	return 0;
;    }
;
_def_fcb::
	call 	csv
	push 	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	pop	iy
	ld	(iy+0),0
	ld	l,(ix+8)
	ld	h,(ix+9)
	inc	hl
	ld	a,(hl)
	cp	58
	jr	nz,loc_712D
	dec	hl
	ld	a,(hl)
	ld	l,a
	rla
	ld	a,l
	and	15
	ld	(iy+0),a
	ld	l,(ix+8)
	ld	h,(ix+9)
	inc	hl
	inc	hl
	ld	(ix+8),l
	ld	(ix+9),h
loc_712D:
	push	iy
	pop	hl
	inc	hl
	ld	(ix+-2),l
	ld	(ix+-1),h
	jr	loc_7160
loc_7139:
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
	inc	hl
	ld	(ix+8),l
	ld	(ix+9),h
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	push	hl
	call	_toupper
	pop	bc
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	inc	hl
	ld	(ix+-2),l
	ld	(ix+-1),h
	dec	hl
	ld	(hl),a		;returned by toupper
loc_7160:
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
	cp	46
	jr	z,loc_719E
	ld	a,(hl)
	ld	e,a
	rla
	sbc	a,a
	ld	d,a
	ld	hl,9
	call	wrelop
	push	iy
	pop	de
	ld	hl,9
	jp	p,loc_71A4
	add	hl,de
	ex	de,hl
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	call	wrelop
	jr	c,loc_7139
	jr	loc_719E
loc_718E:
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	inc	hl
	ld	(ix+-2),l
	ld	(ix+-1),h
	dec	hl
	ld	(hl),32
loc_719E:
	push	iy
	pop	de
	ld	hl,9
loc_71A4:
	add	hl,de
	ex	de,hl
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	call	wrelop
	jr	c,loc_718E
loc_71B1:
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
	or	a
	ld	a,(hl)
	jr	z,loc_71F7
	inc	hl
	ld	(ix+8),l
	ld	(ix+9),h
	cp	46
	jr	nz,loc_71B1
	jr	loc_71F0
loc_71C9:
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
	inc	hl
	ld	(ix+8),l
	ld	(ix+9),h
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	push	hl
	call	_toupper
	pop	bc
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	inc	hl
	ld	(ix+-2),l
	ld	(ix+-1),h
	dec	hl
	ld	(hl),a		;returned by toupper
loc_71F0:
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
loc_71F7:
	ld	e,a
	rla
	sbc	a,a
	ld	d,a
	ld	hl,9
	call	wrelop
	push	iy
	pop	de
	ld	hl,12
	jp	p,loc_722F
	add	hl,de
	ex	de,hl
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	call	wrelop
	jr	c,loc_71C9
	jr	loc_7229
loc_7219:
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	inc	hl
	ld	(ix+-2),l
	ld	(ix+-1),h
	dec	hl
	ld	(hl),32
loc_7229:
	push	iy
	pop	de
	ld	hl,12
loc_722F:
	add	hl,de
	ex	de,hl
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	call	wrelop
	jr	c,loc_7219
	xor	a
	ld	(iy+32),a
	ld	(iy+12),a
	ld	l,a
	jp	cret

;=========================================================
; sub_6BF5 freopen OK(no code matching)   Used in: main
;=========================================================
;	FILE * freopen(char * name, char * mode, register FILE * f) {
;	    unsigned char c1, func;
;	    struct fcb * p;
;
;	    fclose(f);
;	    p = _fcb + (f->_file = (f - stdin));
;	    c1 = 0;
;	    func = CPMOPN;
;	    f->_flag &= 0x4F;
;	    switch(*mode) {
;	      case 'w':
;		c1++;
;	        func = CPMMAKE;
;	      case 'r':
;		if(*(mode+1) == 'b') f->_flag = _IOBINARY;
;		break;
;	    }
;	    def_fcb(p, name);
;	    if(func == CPMMAKE) bdos(CPMDEL, p);
;	    if(bdos(func, p) == -1) return 0;
;	    LO_CHAR(p->rwp) = c1 ? 2 : 1;
;	    if(((LO_CHAR(f->_flag) |= (c1+1))&0xC) == 0) {
;		if(f->_base == 0) f->_base = sbrk(512);
;	    }
;	    f->_ptr = f->_base;
;	    if(f->_base == 0) goto m8;
;	    LO_CHAR(f->_cnt) = 0;
;	    if(c1 == 0) goto m9;
;	    HI_CHAR(f->_cnt) = 2;
;	    goto m10;
;	m8: LO_CHAR(f->_cnt) = 0;
;	m9: HI_CHAR(f->_cnt) = 0;
;	m10:
;	    return f;
;	}
;
_freopen::
	call	csv
	push	hl
	push	hl
	ld	l,(ix+10)
	ld	h,(ix+11)
	push	hl
	pop	iy
	push	hl
	call	_fclose
	pop	bc
	ld	de,__iob
	push	iy
	pop	hl
	or	a
	sbc	hl,de
	ld	de,8
	call	adiv
	ld	a,l
	ld	(iy+7),a
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	ld	de,41
	call	amul
	ld	de,__fcb
	add	hl,de
	ld	(ix+-4),l
	ld	(ix+-3),h
	ld	(ix+-1),0
	ld	(ix+-2),15
	ld	a,(iy+6)
	and	79
	ld	(iy+6),a
	ld	l,(ix+8)
	ld	h,(ix+9)
	ld	a,(hl)
	cp	'r'
	jr	z,loc_6C55
	cp	'w'
	jr	nz,loc_6C65
	inc	(ix+-1)
	ld	(ix+-2),22
loc_6C55:
	ld	l,(ix+8)
	ld	h,(ix+9)
	inc	hl
	ld	a,(hl)
	cp	'b'
	jr	nz,loc_6C65
	ld	(iy+6),-128
loc_6C65:
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	ld	l,(ix+-4)
	ld	h,(ix+-3)
	push	hl
	call	_def_fcb
	pop	bc
	pop	bc
	ld	a,(ix+-2)
	cp	22
	ld	l,(ix+-4)
	ld	h,(ix+-3)
	push	hl
	jr	nz,loc_6C95
	ld	hl,19
	push	hl
	call	_bdos
	pop	bc
	ld	l,(ix+-4)
	ld	h,(ix+-3)
	ex	(sp),hl
loc_6C95:
	ld	l,(ix+-2)
	ld	h,0
	push	hl
	call	_bdos
	pop	bc
	pop	bc
	ld	a,l
	cp	255
	jr	nz,loc_6CAB
	ld	hl,0
	jp	cret
loc_6CAB:
	ld	a,(ix+-1)
	or	a
	jr	nz,loc_6CB6
	ld	hl,1
	jr	loc_6CB9
loc_6CB6:
	ld	hl,2
loc_6CB9:
	ld	a,l
	ld	e,(ix+-4)
	ld	d,(ix+-3)
	ld	hl,36
	add	hl,de
	ld	(hl),a
	ld	a,(ix+-1)
	inc	a
	ld	e,a
	ld	a,(iy+6)
	or	e
	ld	(iy+6),a
	and	12
	or	a
	jr	nz,loc_6CEC
	ld	a,(iy+4)
	or	(iy+5)
	jr	nz,loc_6CEC
	ld	hl,512
	push	hl
	call	_sbrk
	pop	bc
	ld	(iy+4),l
	ld	(iy+5),h
loc_6CEC:
	ld	l,(iy+4)
	ld	h,(iy+5)
	ld	(iy+0),l
	ld	(iy+1),h
	ld	a,l
	or	(iy+5)
	jr	z,loc_6D0E
	ld	a,(ix+-1)
	or	a
	ld	(iy+2),0
	jr	z,loc_6D12
	ld	(iy+3),2
	jr	loc_6D16
loc_6D0E:
	ld	(iy+2),0
loc_6D12:
	ld	(iy+3),0
loc_6D16:
	push	iy
	pop	hl
	jp	cret

;=========================================================
; sub_7855 fclose
;=========================================================
;    int fclose(register FILE * f) {
;	struct fcb * p;
;
;	p = _fcb + (f - stdin);
;	if(!(f->_flag & _IORW)) return EOF; 
;	fflush(f);
;	f->_flag &= ~(_IOREAD|_IOWRT|_IONBF);
;	if((char)p->rwp == 2) goto l1;
;	if(!(ISMPM())) goto l2;
;    l1:
;	bdos(CPMCLS, p);	/* close file */
;    l2:
;	LO_CHAR(p->rwp) = 0;
;	return 0;
;    }
;
_fclose::
	call	csv
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	pop	iy
	ld	de,__iob
	push	iy
	pop	hl
	or	a
	sbc	hl,de
	ld	de,8
	call	adiv
	ld	de,41
	call	amul
	ld	de,__fcb
	add	hl,de
	ld	(ix+-2),l
	ld	(ix+-1),h
	ld	a,(iy+6)
	and	3
	or	a
	jr	nz,loc_788F
	ld	hl,-1
	jp	cret
loc_788F:
	push	iy
	call	_fflush
	pop	bc
	ld	a,(iy+6)
	and	248
	ld	(iy+6),a
	ld	e,(ix+-2)
	ld	d,(ix+-1)
	ld	hl,36
	add	hl,de
	ld	a,(hl)
	cp	2
	jr	z,l6
	ld	hl,12
	push	hl
	call	_bdoshl
	pop	bc
	bit	0,h
	jr	z,l8
l6:
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	push	hl
	ld	hl,16	; CPMCLS - close file
	push	hl
	call	_bdos
	pop	bc
	pop	bc
l8:
	ld	e,(ix+-2)
	ld	d,(ix+-1)
	ld	hl,36
	add	hl,de
	ld	(hl),0
	ld	hl,0
	jp	cret

;=========================================================
; 70AC getchar
;=========================================================
;    getchar() {
;	return(fgetc(stdin));
;    }
;
_getchar::
	call	csv
	ld	hl,__iob
	push	hl		;file=stdin
	call	_fgetc
	pop	bc
	jp	cret	

;=========================================================
; 768F fgetc(FILE * f) from libraty
;=========================================================
;    fgetc(f) register FILE * f; {
;	int c;
;	
;	if(f->_flag & _IOEOF || !(f->_flag & _IOREAD)) {
;    reteof:
;	    f->_flag |= _IOEOF;
;	    return EOF;
;	}
;    loop:
;	if(f->_cnt > 0) {
;	    c = (unsigned)*f->_ptr++;
;	    f->_cnt--;
;	} else if(f->_flag & _IOSTRG)
;	    goto reteof;
;	else
;	    c = _filbuf(f);
;	if(f->_flag & _IOBINARY)
;	    return c;
;	if(c == '\r')
;	    goto loop;
;	if(c == CPMEOF) {
;	    f->_cnt++;
;	    f->_ptr--;
;	    goto reteof;
;	}
;	return c;
;    }
_fgetc::
	pop	de			;get return address off stack
	ex	(sp),iy			;save iy and get arguement into iy
	ld	a,(iy+flag)		;get flag bits
	bit	_IOREAD_BIT,a
	jr	z,reteof		;return EOF if not open for read
	bit	_IOEOF_BIT,a		;Already seen EOF?
	jr	nz,reteof		;yes, repeat ourselves

loop:
	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	ld	a,l
	or	h			;any bytes left?
	jr	z,1f			;no, go get some more
	dec	hl
	ld	(iy+cnt),l		;update count
	ld	(iy+cnt+1),h
	ld	l,(iy+ptr)		;get the pointer
	ld	h,(iy+ptr+1)
	ld	a,(hl)
	inc	hl
	ld	(iy+ptr),l		;update pointer
	ld	(iy+ptr+1),h
2:
	bit	_IOBINARY_BIT,(iy+flag)	;Binary mode?
	jr	z,3f			;no, check for EOF etc
retch:
	ld	l,a			;return the character in a
	ld	h,0
	ex	(sp),iy			;restore iy
	push	de			;put return address back
	ret				;with char in hl

3:
	cp	RETURN			;carriage return
	jr	z,loop			;yes, get another instead
	cp	CPMEOF			;end of file?
	jr	nz,retch		;no, return it!

;	ld	a,(iy+base)		;buffered?
;	or	(iy+base+1)
;	jr	z,reteof		;yup, leave count alone

	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	inc	hl			;reset count
	ld	(iy+cnt),l
	ld	(iy+cnt+1),h

	ld	l,(iy+ptr)
	ld	h,(iy+ptr+1)
	dec	hl			;reset pointer
	ld	(iy+ptr),l
	ld	(iy+ptr+1),h
reteof:
	set	_IOEOF_BIT,(iy+flag)	;note EOF
	ld	hl,EOF
	ex	(sp),iy			;restore iy
	push	de
	ret				;return with EOF in hl

1:
	bit	_IOSTRG_BIT,(iy+flag)	;end of string?
	jr	nz,reteof		;yes, return EOF
	push	de			;save de
	push	iy			;pass iy as argument
	call	__filbuf		;refill the buffer
	ld	a,l			;the returned value
	pop	bc
	pop	de			;return address in de again
	bit	7,h
	jr	nz,reteof		;returned EOF
	jr	2b

;=========================================================
; 70B5 putchar
;=========================================================
;    putchar(c) {
;	return(fputc(c, stdout));
;    }
;
_putchar::
	call	csv
	ld	hl,__iob+8
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	call	_fputc
	pop	bc
	pop	bc
	jp	cret

;=========================================================
; 75A2 fputc(uchar c, FILE * f) from library
;=========================================================
;	#include	<stdio.h>
;	
;	fputc(c, f) register FILE * f; uchar c; {
;	    if(!(f->_flag & _IOWRT)) return EOF;
;	    if((f->_flag & _IOBINARY) == 0 && c == '\n')
;	        fputc('\r', f);
;		if(f->_cnt > 0) {
;		    f->_cnt--;
;		    *f->_ptr++ = c;
;		} else
;		    return _flsbuf(c, f);
;		return c;
;	}
;
_fputc::
	pop	de			;return address
	pop	bc			;character argument
	ld	b,0			;so zero the top byte
	ex	(sp),iy			;save iy and get file pointer
	bit	_IOWRT_BIT,(iy+flag)	;are we reading
	jr	z,reteof1
	bit	_IOBINARY_BIT,(iy+flag)	;binary mode?
	jr	nz,2f			;yes, just return
	ld	a,c			;is it a newline?
	cp	NEWLINE
	jr	nz,2f			;no
	push	bc			;save thingos
	push	de
	push	iy			;file argument
	ld	hl,RETURN
	push	hl
	call	_fputc
	pop	hl			;unjunk stack
	pop	bc
	pop	de
	pop	bc
2:
	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	ld	a,l			;check count
	or	h
	jr	z,1f			;no room at the inn
	dec	hl			;update count
	ld	(iy+cnt),l
	ld	(iy+cnt+1),h
	ld	l,(iy+ptr)
	ld	h,(iy+ptr+1)		;get pointer
	ld	(hl),c			;store character
	inc	hl			;bump pointer
	ld	(iy+ptr),l
	ld	(iy+ptr+1),h
3:
	ex	(sp),iy			;restore iy
	push	bc			;fix stack up
	push	de
	ld	l,c
	ld	h,b			;return the character
	ret

1:
	ex	(sp),iy			;restore the stack to what it was
	push	bc
	push	de			;return address and all
	jp	__flsbuf		;let flsbuf handle it

reteof1:
	ld	bc,-1
	jr	3b

;=========================================================
; 708D printf
;=========================================================
;    printf(char * f, int a) {
;
;	return(_doprnt(stdout, f, &a));
;    }
;
_printf::
	call csv
	push	ix
	pop	de
	ld	hl,8
	add	hl,de
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	ld	hl,__iob+8
	push	hl
	call	__doprnt
	pop	bc
	pop	bc
	pop	bc
	jp	cret

;=========================================================
; sub_7247 sprintf
;=========================================================
;    static FILE spf;
; 
;	int sprintf(char * wh, char * f, int a) {
;	spf._cnt = 32767;
;	spf._ptr = wh;
;	spf._flag = _IOWRT|_IOBINARY
;	_doprnt(&spf, f, &a);
;	*spf._ptr = 0;
;	return spf._ptr - wh;
;    }
;
_sprintf::
	call 	csv
	ld	hl,32767
	ld	(_spf+2),hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	ld	(_spf),hl
	ld	hl,_spf+6
	ld	(hl),82h
	push	ix
	pop	de
	ld	hl,10
	add	hl,de
	push	hl
	ld	l,(ix+8)
	ld	h,(ix+9)
	push	hl
	ld	hl,_spf
	push	hl
	call	__doprnt
	pop	bc
	pop	bc
	pop	bc
	ld	hl,(_spf)
	ld	(hl),0
	ld	e,(ix+6)
	ld	d,(ix+7)
	or	a
	sbc	hl,de
	jp	cret

;=========================================================
; sub_6BD3 fprintf
;=========================================================
;    #include <stdio.h>
;
;    fprintf(file, f, a) FILE * file; char * f; int a; {
;	return(_doprnt(file, f, &a));
;    }
;
_fprintf::
	call	csv
	push	ix
	pop	de
	ld	hl,10
	add	hl,de
	push	hl
	ld	l,(ix+8)
	ld	h,(ix+9)
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	call	__doprnt
	pop	bc
	pop	bc
	pop	bc
	jp	cret

;=========================================================
; sub_78F0 - fflush
;=========================================================
;	int fflush(register FILE * f) {
;	    struct fcb * p;
;
;	    p = _fcb + (f - stdin);
;
;	    if(!(f->_flag & _IOWRT)) return -1;
;
;	    if(((unsigned int)f->_cnt & 0x7F) != 0) {
;	        if(p->rwp > 4)  {
;	            *(f->_ptr) = CPMETX;
;	            count--;
;	        }
;	    }
;	    if((f->_ptr = f->_base) == 0) return 0;
;	    f->_cnt = BUFSIZ - f->_cnt;
;	    switch((char)p->rwp) {
;	      case 2:
;		f->_cnt = (f->_cnt+127)/128;
;		for(;;) {
;		    if(f->_cnt-- == 0) break;
;		    bdos(CPMSDMA, f->_ptr);
;		    bdos(CPMWRIT, p);
;		    if(bdos(CPMWRIT, p) != 0) break;
;		    f->_ptr += 128;
;	    	}
;	      case 4:
;		for(;;) {
;		    if(f->_cnt-- == 0) break;
;		    bdos(CPMWCON, *(f->_ptr++));
;	    	}
;	    }
;	    f->_cnt = 0;
;	    f->_ptr = f->_base;
;	    LO_CHAR(f->_cnt) = 0;
;	    HI_CHAR(f->_cnt) = 2;
;	    return 0;
;	}
;
_fflush::
	call	csv
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	pop	iy
	ld	de,__iob
	push	iy
	pop	hl
	or	a
	sbc	hl,de
	ld	de,8
	call	adiv
	ld	de,41
	call	amul
	ld	de,__fcb
	add	hl,de
	ld	(ix+-2),l
	ld	(ix+-1),h
	bit	1,(iy+6)
	jr	nz,loc_7928
	ld	hl,-1
	jp	cret
loc_7928:			; m0:
	ld	a,(iy+2)
	and	127
	ld	l,a
	xor	a
	ld	h,a
	ld	a,l
	or	h
	jr	z,loc_795B
	ld	b,4	
	ld	e,(ix+-2)
	ld	d,(ix+-1)
	ld	hl,36
	add	hl,de	
	ld	a,(hl)	
	call	brelop	
	jr	nc,loc_795B
	ld	l,(iy+0)
	ld	h,(iy+1)
	ld	(hl),26
	ld	l,(iy+2)
	ld	h,(iy+3)
	dec	hl
	ld	(iy+2),l
	ld	(iy+3),h
loc_795B:			; m1:
	ld	l,(iy+4)
	ld	h,(iy+5)
	ld	(iy+0),l
	ld	(iy+1),h
	ld	a,l
	or	h
	jr	nz,loc_7971
loc_796B:			; m2:
	ld	hl,0
	jp	cret
loc_7971:			; m3:
	ld	e,(iy+2)
	ld	d,(iy+3)
	ld	hl,512
	or	a
	sbc	hl,de
	ld	(iy+2),l
	ld	(iy+3),h

	ld	e,(ix+-2)
	ld	d,(ix+-1)
	ld	hl,36
	add	hl,de
	ld	a,(hl)
	cp	2
	jr	z,loc_79E4
	cp	4
	jr	z,loc_79D0
loc_7996:			; m4:
	ld	(iy+2),0
	ld	(iy+3),0
	ld	l,(iy+4)
	ld	h,(iy+5)
	ld	(iy+0),l
	ld	(iy+1),h
	ld	(iy+2),0
	ld	(iy+3),2
	jr	loc_796B
loc_79B4:			; m5:
	ld	l,(iy+0)
	ld	h,(iy+1)
	ld	a,(hl)
	inc	hl
	ld	(iy+0),l
	ld	(iy+1),h
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	push	hl
	ld	hl,2
	push	hl
	call	_bdos
	pop	bc
	pop	bc
loc_79D0:	; case 4:	; m6:
	ld	l,(iy+2)
	ld	h,(iy+3)
	dec	hl
	ld	(iy+2),l
	ld	(iy+3),h
	inc	hl
	ld	a,l
	or	h
	jr	nz,loc_79B4
	jr	loc_7996
loc_79E4:	; case 2:	; m7:
	ld	e,(iy+2)
	ld	d,(iy+3)
	ld	hl,7Fh 	
	add	hl,de	
	ld	de,80h  
	call	adiv    
	ld	(iy+2),l
	ld	(iy+3),h
	jr	loc_7A30
loc_79FC:			; m8:
	ld	l,(iy+0)
	ld	h,(iy+1)
	push	hl
	ld	hl,1Ah ;CPMSDMA
	push	hl
	call	_bdos
	pop	bc

	ld	l,(ix+-2)
	ld	h,(ix+-1)
	ex	(sp),hl
	ld	hl,15h ;CPMWRIT
	push	hl
	call	_bdos
	pop	bc
	pop	bc
	ld	a,l
	or	a
	jp	nz,loc_7996
	ld	de,80h
	ld	l,(iy+0)
	ld	h,(iy+1)
	add	hl,de
	ld	(iy+0),l
	ld	(iy+1),h
loc_7A30:			; m9:
	ld	l,(iy+2)
	ld	h,(iy+3)
	dec	hl
	ld	(iy+2),l
	ld	(iy+3),h
	inc	hl
	ld	a,l
	or	h
	jr	nz,loc_79FC
	jp	loc_7996

;=========================================================
; 75FA flsbuf 		Push next buffer to file
;=========================================================
;	int _flsbuf(int c, register FILE * f) {
;	    struct fcb * p;
;
;	    sub_77F5();	/* Test ^c */
;	    p = _fcb + (f - stdin);
;
;	    if(f->_flag & _IOWRT) {
;		if(f->_base == (char *)NULL) {
;	    	    f->_cnt = 0;
;	    	    if((char)(p->rwp) == 4) {
;			bdos(CPMWCON, c);
;			return c;
;		    }
;	    	    return -1;
;		} else {
;		    fflush(f);
;	    	    *(f->_ptr++) = c;
;	    	    f->_cnt--;
;		}
;		return c;
;	    }
;	    return -1;
;	}
;
__flsbuf::
	call	csv
	push	hl
	ld	l,(ix+8)
	ld	h,(ix+9)
	push	hl
	pop	iy
	call	_sub_77F5
	ld	de,__iob
	push	iy
	pop	hl
	or	a
	sbc	hl,de
	ld	de,8
	call	adiv
	ld	de,41
	call	amul
	ld	de,__fcb
	add	hl,de
	ld	(ix+-2),l
	ld	(ix+-1),h
	bit	1,(iy+6)
	jr	z,loc_7689
	ld	a,(iy+4)
	or	(iy+5)
	jr	nz,loc_7662
	ld	(iy+2),0
	ld	(iy+3),0
	ex	de,hl
	ld	hl,36
	add	hl,de
	ld	a,(hl)
	cp	4
	jr	nz,loc_7689
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	ld	hl,2
	push	hl
	call	_bdos
	pop	bc
	pop	bc
loc_7659:
	ld	l,(ix+6)
	ld	h,(ix+7)
	jp	cret
loc_7662:
	push	iy
	call	_fflush
	pop	bc
	ld	a,(ix+6)
	ld	l,(iy+0)
	ld	h,(iy+1)
	inc	hl
	ld	(iy+0),l
	ld	(iy+1),h
	dec	hl
	ld	(hl),a
	ld	l,(iy+2)
	ld	h,(iy+3)
	dec	hl
	ld	(iy+2),l
	ld	(iy+3),h
	jr	loc_7659
loc_7689:
	ld	hl,-1
	jp	cret

;=========================================================
; sub_770B filbuf			   Used in: fgetc
;=========================================================
;    int filbuf(register FILE * f) {
;	struct fcb * p;
;
;	p = _fcb + (f - stdin);
;	f->_cnt = 0;
;	sub_77F5();
;	if((char)p->rwp == 1) {
;	    for(f->_ptr = f->_base; f->_ptr < f->_base+BUFSIZ; f->_ptr += 0x80) {
;		bdos(CPMSDMA, f->_ptr);
;		if(bdos(CPMREAD, p) != 0) break;
;	    }
;	    f->_cnt = f->_ptr - f->_base;
;	    f->_ptr = f->_base;
;	    if(f->_cnt == 0) return -1;
;	    f->_cnt--;
;	    return (unsigned char)*(f->_ptr++);
;	}
;	return -1;
;    }
;
__filbuf::
	call	csv
	push	hl
	ld	l,(ix+6)
	ld	h,(ix+7)
	push	hl
	pop	iy
	ld	de,__iob
	push	iy
	pop	hl
	or	a
	sbc	hl,de
	ld	de,8
	call	adiv
	ld	de,41
	call	amul
	ld	de,__fcb
	add	hl,de
	ld	(ix+-2),l
	ld	(ix+-1),h

	ld	(iy+2),0
	ld	(iy+3),0

	call	_sub_77F5
	ld	e,(ix+-2)
	ld	d,(ix+-1)
	ld	hl,36
	add	hl,de
	ld	a,(hl)
	cp	1
	jp	nz,loc_77CE

	ld	l,(iy+4)
	ld	h,(iy+5)
	ld	(iy+0),l
	ld	(iy+1),h
	jr	loc_7793

loc_7760:
	ld	l,(iy+0)
	ld	h,(iy+1)
	push	hl
	ld	hl,26 ; Set DMA adr
	push	hl
	call	_bdos
	pop	bc
	ld	l,(ix+-2)
	ld	h,(ix+-1)
	ex	(sp),hl
	ld	hl,20 ; Read next record
	push	hl
	call	_bdos
	pop	bc
	pop	bc
	ld	a,l
	or	a
	jr	nz,loc_77A9

	ld	de,128
	ld	l,(iy+0)
	ld	h,(iy+1)
	add	hl,de
	ld	(iy+0),l
	ld	(iy+1),h

loc_7793:
	ld	e,(iy+4)
	ld	d,(iy+5)
	ld	hl,512
	add	hl,de
	ex	de,hl
	ld	l,(iy+0)
	ld	h,(iy+1)
	call	wrelop
	jr	c,loc_7760

loc_77A9:
	ld	e,(iy+4)
	ld	d,(iy+5)
	ld	l,(iy+0)
	ld	h,(iy+1)
	or	a
	sbc	hl,de
	ld	(iy+2),l
	ld	(iy+3),h
	ld	l,e
	ld	h,d
	ld	(iy+0),l
	ld	(iy+1),h
	ld	a,(iy+2)
	or	(iy+3)
	jr	nz,loc_77D4

loc_77CE:
	ld	hl,-1
	jp	cret

loc_77D4:
	ld	l,(iy+2)
	ld	h,(iy+3)
	dec	hl
	ld	(iy+2),l
	ld	(iy+3),h
	ld	l,(iy+0)
	ld	h,(iy+1)
	inc	hl
	ld	(iy+0),l
	ld	(iy+1),h
	dec	hl
	ld	l,(hl)
	ld	h,0
	jp	cret

;=========================================================
; sub_77F5	Test ^c
;=========================================================
;    #define CPMRCON	1	/* read console */
;    #define	CPMICON	11	/* interrogate console ready */
;
;    char sub_77F5() {	/* Test ^c */
;	char * st;
;	
;	if(bdos(CPMICON) == 0) return;	   /* status console */
;	if(( *st=bdos(CPMRCON)) != 3) return;  /* read console   */
;	exit();
;    }
;
_sub_77F5::
	call	csv
	push 	hl
	ld	hl,11	; CPMICON - interrogate console ready
	push	hl
	call	_bdos
	pop	bc
	ld	a,l
	or	a
	jp	z,cret
	ld	hl,1	; CPMRCON - read console
	push	hl
	call	_bdos
	pop	bc
	ld	e,l
	ld	(ix+-1),e
	ld	a,e
	cp	3
	jp	nz,cret
	call	_exit
	jp	cret

;=========================================================
; 781E exit
;=========================================================
;	#include "stdio.h"
;
;	#define	uchar	unsigned char
;	#define	MAXFILE 5
;	#define WPOKE(addr, word) *((unsigned *)addr) = (unsigned)word
;
;	void exit(int code) {
;	    uchar i;
;	    register FILE * ip;
;
;	    i = MAXFILE;
;	    ip = _iob;
;	    do {
;		fclose(ip);
;		ip++;
;	    } while(--i);
;	    WPOKE(0x80, code);
;	#asm
;	    call   0	/* Exit; */
;	#endasm
;	}
;
_exit::
	call	csv
	push	hl
	ld	(ix+-1),3
	ld	iy,__iob
loc_782A:
	push	iy
	pop	hl
	ld	bc,8
	add	hl,bc
	push	hl
	pop	iy		
	sbc	hl,bc
	push	hl
	call	_fclose	
	pop	bc
	ld	a,(ix+-1)
	add	a,0FFh
	ld	(ix+-1),a
	or	a
	jr	nz,loc_782A
	ld	l,(ix+6)
	ld	h,(ix+7)
	ld	(128),hl
	call	0
	jp	cret

;=========================================================
; 78DA _bdoshl - 
;=========================================================
;	Bdos calls which return values in HL
;	short	bdoshl(fun, arg);

entry	equ	5		; CP/M entry point

arg	equ	8		; argument to call
func	equ	6		; desired function

_bdoshl:
	call	csv
	ld	e,(ix+arg)
	ld	d,(ix+arg+1)
	ld	c,(ix+func)
	push	ix
	call	entry
	pop	ix
	jp	cret		; return value already in hl

;=========================================================
; 7A45 bdos char bdos(func, arg)	from bdos.asm
;=========================================================

_bdos::
	call	csv
	ld	e,(ix+arg)
	ld	d,(ix+arg+1)
	ld	c,(ix+func)
	push	ix
;	push	iy
	call	entry
;	pop	iy
	pop	ix
	ld	l,a
	rla
	sbc	a,a
	ld	h,a
	jp	cret

	psect	bss

__sibuf::	defs	512
__argc_::	defs	2
_spf:		defs	8

; End file libc1.asm


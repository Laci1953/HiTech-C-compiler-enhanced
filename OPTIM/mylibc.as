;
;       Parts of LIBC.LIB
;
;       dissasembled by Andrey Nikitin & Ladislau Szilagyi
;
; set to 1 only one of the following...
M2X64	equ	0
M512	equ	0
Z80ALL	equ	1
ROMWBW	equ	0

HEAP1	equ	6C00H
HEAP1H	equ	80H

IF	M2X64
HEAP2	equ	8A00H
ENDIF

IF	M512
HEAP2	equ	8B00H
ENDIF

IF	Z80ALL
HEAP2	equ	8B00H
ENDIF

IF	ROMWBW
HEAP2	equ	8B00H
ENDIF

;=========================================================
; 8018 csv:     csv.asm
; 8024 cret:
; 802B indir:
; 802C ncsv:
;=========================================================
        global  csv, cret, indir, ncsv

        psect   text

csv:    pop     hl              ;return address
        push    iy
        push    ix
        ld      ix,0
        add     ix,sp           ;new frame pointer
        jp      (hl)

cret:   ld      sp,ix
        pop     ix
        pop     iy
        ret

indir:  jp      (hl)

; New csv: allocates space for stack based on word following
; call ncsv

ncsv:
        pop     hl
        push    iy
        push    ix
        ld      ix,0
        add     ix,sp
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
        ex      de,hl
        add     hl,sp
        ld      sp,hl
        ex      de,hl
        jp      (hl)

;=========================================================
; * 70CB toupper
;=========================================================
;    char toupper(char c) {
;       if(c >= 'a' && c <= 'z') c += 'A' - 'a';
;       return c;
;    }
;
        global  _toupper
        global  brelop
        global  wrelop

        psect   text

_toupper:
        call    csv
        ld      b,'a'
        ld      a,(ix+6)
        call    brelop
        jp      m,loc_70F2
        ld      a,(ix+6)
        ld      e,a
        rla
        sbc     a,a
        ld      d,a
        ld      hl,'z'
        call    wrelop
        jp      m,loc_70F2
        ld      a,(ix+6)
        add     a,224   ; 'A' - 'a'
        ld      l, a
        jp      cret
loc_70F2:
        ld      l,(ix+6)
        jp      cret

;=========================================================
; 7F3D _brk:    sbrk.asm
; 7F45 _sbrk:
; 7F71 _checksp:
;=========================================================
; NB This brk() does not check that the argument is reasonable.

        psect   text

        global  _sbrk, _brk
_brk:
        pop     hl      ;return address
        pop     de      ;argument
        ld      (memtop),de     ;store it
        push    de              ;adjust stack
        jp      (hl)    ;return
_sbrk:
        pop     bc
        pop     de
        push    de
        push    bc
        ld      hl,(memtop)
        add     hl,de
        jr      c,2f            ;if overflow, no room
        ld      bc,512         ;allow 1k bytes stack overhead
        add     hl,bc
        jr      c,2f            ;if overflow, no room
        sbc     hl,sp
        jr      c,1f
2:
        ld      hl,-1           ;no room at the inn
        ret

1:      ld      hl,(memtop)
        push    hl
        add     hl,de
	ld	a,h
	cp	HEAP1H		;if we reached top of HEAP1
	jr	nz,3f
	ld	hl,HEAP2	;...switch to HEAP2
3:
        ld      (memtop),hl
        pop     hl
        ret

        psect   bss

memtop::defs    2

;=========================================================
; 7F90 shll:    shll.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment type operations, when it is in the memory
; location pointed to by HL

        global  shll,shal       ;shift left, arithmetic or logical

        psect   text
shll:
shal:
        ld      a,b             ;check for zero shift
        or      a
        ret     z
        cp      16              ;16 bits is maximum shift
        jr      c,1f            ;is ok
        ld      b,16
1:
        add     hl,hl           ;shift left
        djnz    1b
        ret

;=========================================================
; 7B06 asll:    asll.asm
;=========================================================
; Shift operations - the count is always in B, the quantity
; to be shifted is in HL, except for the assignment type
; operations, when it is in the memory location pointed to
; by HL

        global  asll,asal       ; assign shift left (logical or arithmetic)

        psect   text

        global  shal
asll:
asal:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        push    hl              ; save for the store
        ex      de,hl
        call    shal
        ex      de,hl
        pop     hl
        ld      (hl),d
        dec     hl
        ld      (hl),e
        ex      de,hl           ; return value in hl
        ret

;=========================================================
; 7D24 amod:    idiv.asm
; 7D29 lmod:
; 7D33 adiv:
; 7D80 negif:
; 7D83 negat:
;=========================================================
; 16 bit divide and modulus routines

; called with dividend in hl and divisor in de
; returns with result in hl.

; adiv (amod) is signed divide (modulus), ldiv (lmod) is unsigned

        global  adiv,ldiv,amod,lmod

        psect   text
amod:
        call    adiv
        ex      de,hl   ;put modulus in hl
        ret

lmod:
        call    ldiv
        ex      de,hl
        ret

ldiv:
        xor     a
        ex      af,af'
        ex      de,hl
        jr      dv1

adiv:
        ld      a,h
        xor     d       ;set sign flag for quotient
        ld      a,h     ;get sign of dividend
        ex      af,af'
        call    negif
        ex      de,hl
        call    negif

dv1:    ld      b,1
        ld      a,h
        or      l
        ret     z

dv8:    push    hl
        add     hl,hl
        jr      c,dv2
        ld      a,d
        cp      h
        jr      c,dv2
        jr      nz,dv6
        ld      a,e
        cp      l
        jr      c,dv2

dv6:    pop     af
        inc     b
        jr      dv8

dv2:    pop     hl
        ex      de,hl
        push    hl
        ld      hl,0
        ex      (sp),hl

dv4:    ld      a,h
        cp      d
        jr      c,dv3
        jr      nz,dv5

        ld      a,l
        cp      e
        jr      c,dv3

dv5:    sbc     hl,de

dv3:    ex      (sp),hl
        ccf
        adc     hl,hl
        srl     d
        rr      e
        ex      (sp),hl
        djnz    dv4
        pop     de
        ex      de,hl
        ex      af,af'
        call    m,negat
        ex      de,hl
        or      a       ;test remainder sign bit
        call    m,negat
        ex      de,hl
        ret

negif:  bit     7,h
        ret     z
negat:  ld      b,h
        ld      c,l
        ld      hl,0
        or      a
        sbc     hl,bc
        ret

        global asamod

asamod:
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        push    bc
        ex      (sp),hl
        call    amod
        ex      (sp),hl
        pop     de
        ld      (hl),d
        dec     hl
        ld      (hl),e
        ex      de,hl           ;return value in hl
        ret

        global asadiv

asadiv:
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        push    bc
        ex      (sp),hl
        call    adiv
        ex      (sp),hl
        pop     de
        ld      (hl),d
        dec     hl
        ld      (hl),e
        ex      de,hl           ;return value in hl
        ret

        global  asamul

asamul:
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        push    bc
        ex      (sp),hl
        call    amul
        ex      (sp),hl
        pop     de
        ld      (hl),d
        dec     hl
        ld      (hl),e
        ex      de,hl           ;return value in hl
        ret

;=========================================================
; 7ECB amul:    imul.asm
; 7EDE mult8b:
;=========================================================
;       16 bit integer multiply

;       on entry, left operand is in hl, right operand in de

        psect   text

        global  amul,lmul
amul:
lmul:
        ld      a,e
        ld      c,d
        ex      de,hl
        ld      hl,0
        ld      b,8
        call    mult8b
        ex      de,hl
        jr      3f
2:      add     hl,hl
3:
        djnz    2b
        ex      de,hl
1:
        ld      a,c
mult8b:
        srl     a
        jr      nc,1f
        add     hl,de
1:      ex      de,hl
        add     hl,hl
        ex      de,hl
        ret     z
        djnz    mult8b
        ret

;=========================================================
; 7AE5 asar:    asar.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment  type operations, when it is in the memory
; location pointed to by HL

        global  asar    ; assign shift arithmetic right

        psect   text

        global  shar

asar:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        push    hl              ; save for the store
        ex      de,hl
        call    shar
        ex      de,hl
        pop     hl
        ld      (hl),d
        dec     hl
        ld      (hl),e
        ex      de,hl           ; return value in hl
        ret

;=========================================================
; 7F80 shar:    shar.asm
;=========================================================
; Shift operations - the count is always in B,
; the quantity to be shifted is in HL, except for the
; assignment type operations, when it is in the memory
; location pointed to by HL

        global  shar    ;shift arithmetic right

        psect   text
shar:
        ld      a,b             ;check for zero shift
        or      a
        ret     z
        cp      16              ;16 bits is maximum shift
        jr      c,1f            ;is ok
        ld      b,16
1:
        sra     h
        rr      l
        djnz    1b
        ret

;=========================================================
; 781E exit
;=========================================================
;       #include "stdio.h"
;
;       #define uchar   unsigned char
;       #define MAXFILE 5
;       #define WPOKE(addr, word) *((unsigned *)addr) = (unsigned)word
;
;       void exit(int code) {
;           uchar i;
;           register FILE * ip;
;
;           i = MAXFILE;
;           ip = _iob;
;           do {
;               fclose(ip);
;               ip++;
;           } while(--i);
;           WPOKE(0x80, code);
;       #asm
;           call   0    /* Exit; */
;       #endasm
;       }
;
        global  _exit
        global  __iob
        global  _fclose

        psect   text

_exit:
        call    csv
        push    hl
        ld      (ix+-1),3
        ld      iy,__iob
loc_782A:
        push    iy
        pop     hl
        ld      bc,8
        add     hl,bc
        push    hl
        pop     iy
        sbc     hl,bc
        push    hl
        call    _fclose
        pop     bc
        ld      a,(ix+-1)
        add     a,0FFh
        ld      (ix+-1),a
        or      a
        jr      nz,loc_782A
        ld      l,(ix+6)
        ld      h,(ix+7)
        ld      (128),hl
        call    0
        jp      cret
;
;=========================================================
;       Offsets of things in the _iob structure

        ptr     equ     0       ; pointer to next byte
        cnt     equ     2       ; number of bytes left
        base    equ     4       ; beginning of buffer
        flag    equ     6       ; flag bits
        file    equ     7       ; file number

;       The bit numbers of the flags in flag

        _IOREAD_BIT     equ     0
        _IOWRT_BIT      equ     1
        _IONBF_BIT      equ     2
        _IOMYBUF_BIT    equ     3
        _IOEOF_BIT      equ     4
        _IOERR_BIT      equ     5
        _IOSTRG_BIT     equ     6
        _IOBINARY_BIT   equ     7

;       Various characters

        CPMEOF  equ     032q    ; EOF byte
        NEWLINE equ     012q    ; newline character
        RETURN  equ     015q    ; carriage return

        EOF     equ     -1      ; stdio EOF value
;
; fgets
;
global  _fgets
global  ncsv, cret, indir
global  _fgetc

        psect   text

_fgets:
global csv
call csv
push hl
push hl
ld      l,(ix+10)
ld      h,(ix+11)
push    hl
pop     iy
ld      l,(ix+6)
ld      h,(ix+7)
ld      (ix+-2),l
ld      (ix+-1),h
l4:
ld      l,(ix+8)
ld      h,(ix+9)
dec     hl
ld      (ix+8),l
ld      (ix+9),h
ld      a,l
or      h
jp      z,l5
push    iy
call    _fgetc
pop     bc
ld      (ix+-4),l
ld      (ix+-3),h
ld      de,-1
or      a
sbc     hl,de
jp      z,l5
ld      a,(ix+-4)
ld      l,(ix+6)
ld      h,(ix+7)
inc     hl
ld      (ix+6),l
ld      (ix+7),h
dec     hl
ld      (hl),a
cp      10
jp      nz,l4
l5:
ld      l,(ix+6)
ld      h,(ix+7)
ld      (hl),0
ld      e,(ix+-2)
ld      d,(ix+-1)
or      a
sbc     hl,de
jp      nz,cl6
ld      hl,0
jp      cret
cl6:
ld      l,(ix+-2)
ld      h,(ix+-1)
jp      cret
;
;=========================================================
; 768F fgetc(FILE * f) from libraty
;=========================================================
;    fgetc(f) register FILE * f; {
;       int c;
;
;       if(f->_flag & _IOEOF || !(f->_flag & _IOREAD)) {
;    reteof:
;           f->_flag |= _IOEOF;
;           return EOF;
;       }
;    loop:
;       if(f->_cnt > 0) {
;           c = (unsigned)*f->_ptr++;
;           f->_cnt--;
;       } else if(f->_flag & _IOSTRG)
;           goto reteof;
;       else
;           c = _filbuf(f);
;       if(f->_flag & _IOBINARY)
;           return c;
;       if(c == '\r')
;           goto loop;
;       if(c == CPMEOF) {
;           f->_cnt++;
;           f->_ptr--;
;           goto reteof;
;       }
;       return c;
;    }

;       The assembler version of the above routine

;*Include       stdio.i
        global  _fgetc, __filbuf
        psect   text

_fgetc:
        pop     de                      ;get return address off stack
        ex      (sp),iy                 ;save iy and get arguement into iy
        ld      a,(iy+flag)             ;get flag bits
        bit     _IOREAD_BIT,a
        jr      z,reteof                ;return EOF if not open for read
        bit     _IOEOF_BIT,a            ;Already seen EOF?
        jr      nz,reteof               ;yes, repeat ourselves

loop:
        ld      l,(iy+cnt)
        ld      h,(iy+cnt+1)
        ld      a,l
        or      h                       ;any bytes left?
        jr      z,1f                    ;no, go get some more
        dec     hl
        ld      (iy+cnt),l              ;update count
        ld      (iy+cnt+1),h
        ld      l,(iy+ptr)              ;get the pointer
        ld      h,(iy+ptr+1)
        ld      a,(hl)
        inc     hl
        ld      (iy+ptr),l              ;update pointer
        ld      (iy+ptr+1),h
2:
        bit     _IOBINARY_BIT,(iy+flag) ;Binary mode?
        jr      z,3f                    ;no, check for EOF etc
retch:
        ld      l,a                     ;return the character in a
        ld      h,0
        ex      (sp),iy                 ;restore iy
        push    de                      ;put return address back
        ret                             ;with char in hl

3:
        cp      RETURN                  ;carriage return
        jr      z,loop                  ;yes, get another instead
        cp      CPMEOF                  ;end of file?
        jr      nz,retch                ;no, return it!

;       ld      a,(iy+base)             ;buffered?
;       or      (iy+base+1)
;       jr      z,reteof                ;yup, leave count alone

        ld      l,(iy+cnt)
        ld      h,(iy+cnt+1)
        inc     hl                      ;reset count
        ld      (iy+cnt),l
        ld      (iy+cnt+1),h

        ld      l,(iy+ptr)
        ld      h,(iy+ptr+1)
        dec     hl                      ;reset pointer
        ld      (iy+ptr),l
        ld      (iy+ptr+1),h
reteof:
        set     _IOEOF_BIT,(iy+flag)    ;note EOF
        ld      hl,EOF
        ex      (sp),iy                 ;restore iy
        push    de
        ret                             ;return with EOF in hl

1:
        bit     _IOSTRG_BIT,(iy+flag)   ;end of string?
        jr      nz,reteof               ;yes, return EOF
        push    de                      ;save de
        push    iy                      ;pass iy as argument
        call    __filbuf                ;refill the buffer
        ld      a,l                     ;the returned value
        pop     bc
        pop     de                      ;return address in de again
        bit     7,h
        jr      nz,reteof               ;returned EOF
        jr      2b

;=========================================================
; sub_770B filbuf                          Used in: fgetc
;=========================================================
;    int filbuf(register FILE * f) {
;       struct fcb * p;
;
;       p = _fcb + (f - stdin);
;       f->_cnt = 0;
;       sub_77F5();
;       if((char)p->rwp == 1) {
;           for(f->_ptr = f->_base; f->_ptr < f->_base+BUFSIZ; f->_ptr += 0x80) {
;               bdos(CPMSDMA, f->_ptr);
;               if(bdos(CPMREAD, p) != 0) break;
;           }
;           f->_cnt = f->_ptr - f->_base;
;           f->_ptr = f->_base;
;           if(f->_cnt == 0) return -1;
;           f->_cnt--;
;           return (unsigned char)*(f->_ptr++);
;       }
;       return -1;
;    }
;
        global  __filbuf
        global  __fcb
        global  __iob
        global  adiv
        global  amul
        global  _sub_77F5
        global  _bdos
        global  wrelop

        psect   text

__filbuf:
        call    csv
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        pop     iy
        ld      de,__iob
        push    iy
        pop     hl
        or      a
        sbc     hl,de
        ld      de,8
        call    adiv
        ld      de,41
        call    amul
        ld      de,__fcb
        add     hl,de
        ld      (ix+-2),l
        ld      (ix+-1),h

        ld      (iy+2),0
        ld      (iy+3),0

        call    _sub_77F5
        ld      e,(ix+-2)
        ld      d,(ix+-1)
        ld      hl,36
        add     hl,de
        ld      a,(hl)
        cp      1
        jp      nz,loc_77CE

        ld      l,(iy+4)
        ld      h,(iy+5)
        ld      (iy+0),l
        ld      (iy+1),h
        jr      loc_7793

loc_7760:
        ld      l,(iy+0)
        ld      h,(iy+1)
        push    hl
        ld      hl,26 ; Set DMA adr
        push    hl
        call    _bdos
        pop     bc
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        ex      (sp),hl
        ld      hl,20 ; Read next record
        push    hl
        call    _bdos
        pop     bc
        pop     bc
        ld      a,l
        or      a
        jr      nz,loc_77A9

        ld      de,128
        ld      l,(iy+0)
        ld      h,(iy+1)
        add     hl,de
        ld      (iy+0),l
        ld      (iy+1),h

loc_7793:
        ld      e,(iy+4)
        ld      d,(iy+5)
        ld      hl,512
        add     hl,de
        ex      de,hl
        ld      l,(iy+0)
        ld      h,(iy+1)
        call    wrelop
        jr      c,loc_7760

loc_77A9:
        ld      e,(iy+4)
        ld      d,(iy+5)
        ld      l,(iy+0)
        ld      h,(iy+1)
        or      a
        sbc     hl,de
        ld      (iy+2),l
        ld      (iy+3),h
        ld      l,e
        ld      h,d
        ld      (iy+0),l
        ld      (iy+1),h
        ld      a,(iy+2)
        or      (iy+3)
        jr      nz,loc_77D4

loc_77CE:
        ld      hl,-1
        jp      cret

loc_77D4:
        ld      l,(iy+2)
        ld      h,(iy+3)
        dec     hl
        ld      (iy+2),l
        ld      (iy+3),h
        ld      l,(iy+0)
        ld      h,(iy+1)
        inc     hl
        ld      (iy+0),l
        ld      (iy+1),h
        dec     hl
        ld      l,(hl)
        ld      h,0
        jp      cret

;=========================================================
; sub_7855 fclose
;=========================================================
;    int fclose(register FILE * f) {
;       struct fcb * p;
;
;       p = _fcb + (f - stdin);
;       if(!(f->_flag & _IORW)) return EOF;
;       fflush(f);
;       f->_flag &= ~(_IOREAD|_IOWRT|_IONBF);
;       if((char)p->rwp == 2) goto l1;
;       if(!(ISMPM())) goto l2;
;    l1:
;       bdos(CPMCLS, p);        /* close file */
;    l2:
;       LO_CHAR(p->rwp) = 0;
;       return 0;
;    }
;
        global  _fclose
        global  __fcb
        global  __iob
        global  adiv
        global  amul
        global  _fflush
        global  _bdoshl
        global  _bdos

        psect   text

_fclose:
        call    csv
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        pop     iy
        ld      de,__iob
        push    iy
        pop     hl
        or      a
        sbc     hl,de
        ld      de,8
        call    adiv
        ld      de,41
        call    amul
        ld      de,__fcb
        add     hl,de
        ld      (ix+-2),l
        ld      (ix+-1),h
        ld      a,(iy+6)
        and     3
        or      a
        jr      nz,loc_788F
        ld      hl,-1
        jp      cret
loc_788F:
        push    iy
        call    _fflush
        pop     bc
        ld      a,(iy+6)
        and     248
        ld      (iy+6),a
        ld      e,(ix+-2)
        ld      d,(ix+-1)
        ld      hl,36
        add     hl,de
        ld      a,(hl)
        cp      2
        jr      z,l6
        ld      hl,12
        push    hl
        call    _bdoshl
        pop     bc
        bit     0,h
        jr      z,l8
l6:
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        push    hl
        ld      hl,16   ; CPMCLS - close file
        push    hl
        call    _bdos
        pop     bc
        pop     bc
l8:
        ld      e,(ix+-2)
        ld      d,(ix+-1)
        ld      hl,36
        add     hl,de
        ld      (hl),0
        ld      hl,0
        jp      cret

;=========================================================
; 78DA _bdoshl - from libraty
;=========================================================
;       Bdos calls which return values in HL
;       short   bdoshl(fun, arg);

psect   text

entry   equ     5               ; CP/M entry point

arg     equ     8               ; argument to call
func    equ     6               ; desired function


        global  _bdoshl
        global  csv,cret

        psect   text

_bdoshl:
        call    csv
        ld      e,(ix+arg)
        ld      d,(ix+arg+1)
        ld      c,(ix+func)
        push    ix
        call    entry
        pop     ix
        jp      cret            ; return value already in hl

;=========================================================
; sub_78F0 - fflush
;=========================================================
;       int fflush(register FILE * f) {
;           struct fcb * p;
;
;           p = _fcb + (f - stdin);
;
;           if(!(f->_flag & _IOWRT)) return -1;
;
;           if(((unsigned int)f->_cnt & 0x7F) != 0) {
;               if(p->rwp > 4)  {
;                   *(f->_ptr) = CPMETX;
;                   count--;
;               }
;           }
;           if((f->_ptr = f->_base) == 0) return 0;
;           f->_cnt = BUFSIZ - f->_cnt;
;           switch((char)p->rwp) {
;             case 2:
;               f->_cnt = (f->_cnt+127)/128;
;               for(;;) {
;                   if(f->_cnt-- == 0) break;
;                   bdos(CPMSDMA, f->_ptr);
;                   bdos(CPMWRIT, p);
;                   if(bdos(CPMWRIT, p) != 0) break;
;                   f->_ptr += 128;
;               }
;             case 4:
;               for(;;) {
;                   if(f->_cnt-- == 0) break;
;                   bdos(CPMWCON, *(f->_ptr++));
;               }
;           }
;           f->_cnt = 0;
;           f->_ptr = f->_base;
;           LO_CHAR(f->_cnt) = 0;
;           HI_CHAR(f->_cnt) = 2;
;           return 0;
;       }
;
        global  _fflush
        global  __fcb
        global  __iob
        global  adiv
        global  amul
        global  wrelop
        global  _bdos

        psect   text

_fflush:
        call    csv
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        pop     iy
        ld      de,__iob
        push    iy
        pop     hl
        or      a
        sbc     hl,de
        ld      de,8
        call    adiv
        ld      de,41
        call    amul
        ld      de,__fcb
        add     hl,de
        ld      (ix+-2),l
        ld      (ix+-1),h
        bit     1,(iy+6)
        jr      nz,loc_7928
        ld      hl,-1
        jp      cret
loc_7928:                       ; m0:
        ld      a,(iy+2)
        and     127
        ld      l,a
        xor     a
        ld      h,a
        ld      a,l
        or      h
        jr      z,loc_795B
        ld      b,4
        ld      e,(ix+-2)
        ld      d,(ix+-1)
        ld      hl,36
        add     hl,de
        ld      a,(hl)
        call    brelop
        jr      nc,loc_795B
        ld      l,(iy+0)
        ld      h,(iy+1)
        ld      (hl),26
        ld      l,(iy+2)
        ld      h,(iy+3)
        dec     hl
        ld      (iy+2),l
        ld      (iy+3),h
loc_795B:                       ; m1:
        ld      l,(iy+4)
        ld      h,(iy+5)
        ld      (iy+0),l
        ld      (iy+1),h
        ld      a,l
        or      h
        jr      nz,loc_7971
loc_796B:                       ; m2:
        ld      hl,0
        jp      cret
loc_7971:                       ; m3:
        ld      e,(iy+2)
        ld      d,(iy+3)
        ld      hl,512
        or      a
        sbc     hl,de
        ld      (iy+2),l
        ld      (iy+3),h

        ld      e,(ix+-2)
        ld      d,(ix+-1)
        ld      hl,36
        add     hl,de
        ld      a,(hl)
        cp      2
        jr      z,loc_79E4
        cp      4
        jr      z,loc_79D0
loc_7996:                       ; m4:
        ld      (iy+2),0
        ld      (iy+3),0
        ld      l,(iy+4)
        ld      h,(iy+5)
        ld      (iy+0),l
        ld      (iy+1),h
        ld      (iy+2),0
        ld      (iy+3),2
        jr      loc_796B
loc_79B4:                       ; m5:
        ld      l,(iy+0)
        ld      h,(iy+1)
        ld      a,(hl)
        inc     hl
        ld      (iy+0),l
        ld      (iy+1),h
        ld      l,a
        rla
        sbc     a,a
        ld      h,a
        push    hl
        ld      hl,2
        push    hl
        call    _bdos
        pop     bc
        pop     bc
loc_79D0:       ; case 4:       ; m6:
        ld      l,(iy+2)
        ld      h,(iy+3)
        dec     hl
        ld      (iy+2),l
        ld      (iy+3),h
        inc     hl
        ld      a,l
        or      h
        jr      nz,loc_79B4
        jr      loc_7996
loc_79E4:       ; case 2:       ; m7:
        ld      e,(iy+2)
        ld      d,(iy+3)
        ld      hl,7Fh
        add     hl,de
        ld      de,80h
        call    adiv
        ld      (iy+2),l
        ld      (iy+3),h
        jr      loc_7A30
loc_79FC:                       ; m8:
        ld      l,(iy+0)
        ld      h,(iy+1)
        push    hl
        ld      hl,1Ah ;CPMSDMA
        push    hl
        call    _bdos
        pop     bc

        ld      l,(ix+-2)
        ld      h,(ix+-1)
        ex      (sp),hl
        ld      hl,15h ;CPMWRIT
        push    hl
        call    _bdos
        pop     bc
        pop     bc
        ld      a,l
        or      a
        jp      nz,loc_7996
        ld      de,80h
        ld      l,(iy+0)
        ld      h,(iy+1)
        add     hl,de
        ld      (iy+0),l
        ld      (iy+1),h
loc_7A30:                       ; m9:
        ld      l,(iy+2)
        ld      h,(iy+3)
        dec     hl
        ld      (iy+2),l
        ld      (iy+3),h
        inc     hl
        ld      a,l
        or      h
        jr      nz,loc_79FC
        jp      loc_7996

;=========================================================
; 7A45 bdos char bdos(func, arg)        from bdos.asm
;=========================================================

;entry  equ     5               ; CP/M entry point

;arg    equ     8               ; argument to call
;func   equ     6               ; desired function

        global  _bdos

        psect   text
_bdos:
        call    csv
        ld      e,(ix+arg)
        ld      d,(ix+arg+1)
        ld      c,(ix+func)
        push    ix
;       push    iy
        call    entry
;       pop     iy
        pop     ix
        ld      l,a
        rla
        sbc     a,a
        ld      h,a
        jp      cret

;=========================================================
; 75A2 fputc(uchar c, FILE * f) from library
;=========================================================
;       #include        <stdio.h>
;
;       fputc(c, f) register FILE * f; uchar c; {
;           if(!(f->_flag & _IOWRT)) return EOF;
;           if((f->_flag & _IOBINARY) == 0 && c == '\n')
;               fputc('\r', f);
;               if(f->_cnt > 0) {
;                   f->_cnt--;
;                   *f->_ptr++ = c;
;               } else
;                   return _flsbuf(c, f);
;               return c;
;       }
;
;*Include       stdio.i

        global  _fputc, __flsbuf
        psect   text
_fputc:
        pop     de                      ;return address
        pop     bc                      ;character argument
        ld      b,0                     ;so zero the top byte
        ex      (sp),iy                 ;save iy and get file pointer
        bit     _IOWRT_BIT,(iy+flag)    ;are we reading
        jr      z,reteof1
        bit     _IOBINARY_BIT,(iy+flag) ;binary mode?
        jr      nz,2f                   ;yes, just return
        ld      a,c                     ;is it a newline?
        cp      NEWLINE
        jr      nz,2f                   ;no
        push    bc                      ;save thingos
        push    de
        push    iy                      ;file argument
        ld      hl,RETURN
        push    hl
        call    _fputc
        pop     hl                      ;unjunk stack
        pop     bc
        pop     de
        pop     bc
2:
        ld      l,(iy+cnt)
        ld      h,(iy+cnt+1)
        ld      a,l                     ;check count
        or      h
        jr      z,1f                    ;no room at the inn
        dec     hl                      ;update count
        ld      (iy+cnt),l
        ld      (iy+cnt+1),h
        ld      l,(iy+ptr)
        ld      h,(iy+ptr+1)            ;get pointer
        ld      (hl),c                  ;store character
        inc     hl                      ;bump pointer
        ld      (iy+ptr),l
        ld      (iy+ptr+1),h
3:
        ex      (sp),iy                 ;restore iy
        push    bc                      ;fix stack up
        push    de
        ld      l,c
        ld      h,b                     ;return the character
        ret

1:
        ex      (sp),iy                 ;restore the stack to what it was
        push    bc
        push    de                      ;return address and all
        jp      __flsbuf                ;let flsbuf handle it

reteof1:
        ld      bc,-1
        jr      3b

;=========================================================
; 7CDC brelop:  brelop.asm
;=========================================================
; byte relational operation - returns flags correctly for
; comparision of words in a and c

        psect   text

        global  brelop

brelop:
        push    de
        ld      e,a
        xor     b       ;compare signs
        jp      m,1f    ;if different, return sign of lhs
        ld      a,e
        sbc     a,b
        pop     de
        ret
1:
        ld      a,e     ;get sign of lhs
        and     80h     ;mask out sign flag
        ld      d,a
        ld      a,e
        sbc     a,b     ;set carry flag if appropriate
        ld      a,d
        inc     a       ;set sign flag as appropriate and reset Z flag
        pop     de
        ret

;=========================================================
;7CF0 wrelop:   wrelop.asm
;=========================================================
; word relational operation - returns flags correctly for
; comparision of words in hl and de

        psect   text

        global  wrelop

wrelop:
        ld      a,h
        xor     d       ;compare signs
        jp      m,1f    ;if different, return sign of lhs
        sbc     hl,de   ;just set flags as normal
        ret
1:
        ld      a,h     ;get sign of lhs
        and     80h     ;mask out sign flag
        sbc     hl,de   ;set carry flag if appropriate
        inc     a       ;set sign flag as appropriate and reset Z flag
        ret

        psect   bss

__sibuf::        defs    512
__sobuf::	 defs	 512
__argc_::        defs    2

        psect   text

;=========================================================
; 7A62  start2.asm

;=========================================================
; 708D printf
;=========================================================
;    printf(char * f, int a) {
;
;       return(_doprnt(stdout, f, &a));
;    }
;
        global  _printf
        global  __doprnt
        global  __iob

        psect   text

_printf:
        call csv
        push    ix
        pop     de
        ld      hl,8
        add     hl,de
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        ld      hl,__iob+8
        push    hl
        call    __doprnt
        pop     bc
        pop     bc
        pop     bc
        jp      cret

;------------------------------------------------------

; Called with dividend in HLDE, divisor on stack under 2 return
; addresses. Returns with dividend in HL/HL', divisor in DE/DE'
; on return the HIGH words are selected.

lregset:
        pop     bc              ;get top return address
        exx                     ;select other bank
        pop     bc              ;return address of call to this module
        pop     de              ;get low word of divisor
        exx                     ;select hi bank
        ex      de,hl           ;dividend.low -> hl
        ex      (sp),hl         ;divisor.high -> hl
        ex      de,hl           ;dividend.high -> hl
        exx                     ;back to low bank
        push    bc              ;put outer r.a. back on stack
        pop     hl              ;return address
        ex      (sp),hl         ;dividend.low -> hl
        exx
        push    bc              ;top return address
        ret

        global llmod

llmod:
        call    lregset

;       Called with high words selected, perform unsigned modulus

dourem:
        call    divide
        push    hl                      ;high word of remainder
        exx
        pop     de
        ex      de,hl                           ;high word in hl
        ret

        global lldiv

lldiv:  call    lregset

;       Called with high words selected, performs unsigned division
;       returns with quotient in HL/DE

doudiv:
        call    divide                  ;unsigned division
        push    bc                      ;high word of quotien
        exx
        pop     hl
        ld      e,c                     ;low word
        ld      d,b
        ret

; Called with dividend in HL/HL', divisor in DE/DE', high words in
; selected register set
; returns with quotient in BC/BC', remainder in HL/HL', high words
; selected

divide:
        ld      bc,0            ;initialize quotient
        ld      a,e             ;check for zero divisor
        or      d
        exx
        ld      bc,0
        or      e
        or      d
        exx                     ;restor high words
        ret     z               ;return with quotient == 0
        ld      a,1             ;loop count
        jr      3f              ;enter loop in middle
1:
        push    hl              ;save divisor
        exx
        push    hl              ;low word
        or      a               ;clear carry
        sbc     hl,de           ;subtract low word
        exx
        sbc     hl,de           ;sbutract hi word
        exx
        pop     hl              ;restore dividend
        exx
        pop     hl              ;and hi word
        jr      c,2f            ;finished - divisor is big enough
        exx
        inc     a               ;increment count
        ex      de,hl           ;put divisor in hl - still low word
        add     hl,hl           ;shift left
        ex      de,hl           ;put back in de
        exx                     ;get hi word
        ex      de,hl
        adc     hl,hl           ;shift with carry
        ex      de,hl
3:
        bit     7,d             ;test for max divisor
        jr      z,1b            ;loop if msb not set

2:      ;arrive here with shifted divisor, loop count in a, and low words
        ;selected

3:
        push    hl              ;save dividend
        exx
        push    hl              ;low word
        or      a               ;clear carry
        sbc     hl,de
        exx
        sbc     hl,de
        exx                     ;restore low word
        jr      nc,4f
        pop     hl              ;restore low word of dividend
        exx
        pop     hl              ;hi word
        exx                     ;restore low word
        jr      5f
4:
        inc     sp              ;unjunk stack
        inc     sp
        inc     sp
        inc     sp
5:
        ccf                     ;complement carry bit
        rl      c               ;shift in carry bit
        rl      b               ;next byte
        exx                     ;hi word
        rl      c
        rl      b
        srl     d               ;now shift divisor right
        rr      e
        exx                     ;get low word back
        rr      d
        rr      e
        exx                     ;select hi word again
        dec     a               ;decrement loop count
        jr      nz,3b
        ret                     ;finished

;=========================================================
; 7FC8 _strcmp: strcmp.asm
;=========================================================
        psect   text

        global  _strcmp

_strcmp:
        pop     bc
        pop     de
        pop     hl
        push    hl
        push    de
        push    bc

1:      ld      a,(de)
        cp      (hl)
        jr      nz,2f
        inc     de
        inc     hl
        or      a
        jr      nz,1b
        ld      hl,0
        ret

2:      ld      hl,1
        ret     nc
        dec     hl
        dec     hl
        ret

;=========================================================
; 7FE2 _strcpy: strcpy.asm
;=========================================================
        psect   text

        global  _strcpy

_strcpy:
        pop     bc
        pop     de
        pop     hl
        push    hl
        push    de
        push    bc
        ld      c,e
        ld      b,d             ;save destination pointer

1:      ld      a,(hl)
        ld      (de),a
        inc     de
        inc     hl
        or      a
        jr      nz,1b
        ld      l,c
        ld      h,b
        ret

;=========================================================
; 7FF4 _strlen: strlen.asm
;=========================================================
        psect   text

        global  _strlen
_strlen:        pop     hl
        pop     de
        push    de
        push    hl
        ld      hl,0

1:      ld      a,(de)
        or      a
        ret     z
        inc     hl
        inc     de
        jr      1b

;=========================================================
; sub_6BD3 fprintf
;=========================================================
;    #include <stdio.h>
;
;    fprintf(file, f, a) FILE * file; char * f; int a; {
;       return(_doprnt(file, f, &a));
;    }
;
        psect   text

        global  _fprintf
        global  __doprnt

_fprintf:
        call    csv
        push    ix
        pop     de
        ld      hl,10
        add     hl,de
        push    hl
        ld      l,(ix+8)
        ld      h,(ix+9)
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        call    __doprnt
        pop     bc
        pop     bc
        pop     bc
        jp      cret

;=========================================================
; sub_6BF5 freopen OK(no code matching)   Used in: main
;=========================================================
;       FILE * freopen(char * name, char * mode, register FILE * f) {
;           unsigned char c1, func;
;           struct fcb * p;
;
;           fclose(f);
;           p = _fcb + (f->_file = (f - stdin));
;           c1 = 0;
;           func = CPMOPN;
;           f->_flag &= 0x4F;
;           switch(*mode) {
;             case 'w':
;               c1++;
;               func = CPMMAKE;
;             case 'r':
;               if(*(mode+1) == 'b') f->_flag = _IOBINARY;
;               break;
;           }
;           def_fcb(p, name);
;           if(func == CPMMAKE) bdos(CPMDEL, p);
;           if(bdos(func, p) == -1) return 0;
;           LO_CHAR(p->rwp) = c1 ? 2 : 1;
;           if(((LO_CHAR(f->_flag) |= (c1+1))&0xC) == 0) {
;               if(f->_base == 0) f->_base = sbrk(512);
;           }
;           f->_ptr = f->_base;
;           if(f->_base == 0) goto m8;
;           LO_CHAR(f->_cnt) = 0;
;           if(c1 == 0) goto m9;
;           HI_CHAR(f->_cnt) = 2;
;           goto m10;
;       m8: LO_CHAR(f->_cnt) = 0;
;       m9: HI_CHAR(f->_cnt) = 0;
;       m10:
;           return f;
;       }
;
        global  _freopen
        global  _fclose
        global  __fcb
        global  __iob
        global  adiv
        global  amul
        global  _def_fcb
        global  _bdos
        global  _sbrk

        psect   text

_freopen:
        call    csv
        push    hl
        push    hl
        ld      l,(ix+10)
        ld      h,(ix+11)
        push    hl
        pop     iy
        push    hl
        call    _fclose
        pop     bc
        ld      de,__iob
        push    iy
        pop     hl
        or      a
        sbc     hl,de
        ld      de,8
        call    adiv
        ld      a,l
        ld      (iy+7),a
        ld      l,a
        rla
        sbc     a,a
        ld      h,a
        ld      de,41
        call    amul
        ld      de,__fcb
        add     hl,de
        ld      (ix+-4),l
        ld      (ix+-3),h
        ld      (ix+-1),0
        ld      (ix+-2),15
        ld      a,(iy+6)
        and     79
        ld      (iy+6),a
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
        cp      'r'
        jr      z,loc_6C55
        cp      'w'
        jr      nz,loc_6C65
        inc     (ix+-1)
        ld      (ix+-2),22
loc_6C55:
        ld      l,(ix+8)
        ld      h,(ix+9)
        inc     hl
        ld      a,(hl)
        cp      'b'
        jr      nz,loc_6C65
        ld      (iy+6),-128
loc_6C65:
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        ld      l,(ix+-4)
        ld      h,(ix+-3)
        push    hl
        call    _def_fcb
        pop     bc
        pop     bc
        ld      a,(ix+-2)
        cp      22
        ld      l,(ix+-4)
        ld      h,(ix+-3)
        push    hl
        jr      nz,loc_6C95
        ld      hl,19
        push    hl
        call    _bdos
        pop     bc
        ld      l,(ix+-4)
        ld      h,(ix+-3)
        ex      (sp),hl
loc_6C95:
        ld      l,(ix+-2)
        ld      h,0
        push    hl
        call    _bdos
        pop     bc
        pop     bc
        ld      a,l
        cp      255
        jr      nz,loc_6CAB
        ld      hl,0
        jp      cret
loc_6CAB:
        ld      a,(ix+-1)
        or      a
        jr      nz,loc_6CB6
        ld      hl,1
        jr      loc_6CB9
loc_6CB6:
        ld      hl,2
loc_6CB9:
        ld      a,l
        ld      e,(ix+-4)
        ld      d,(ix+-3)
        ld      hl,36
        add     hl,de
        ld      (hl),a
        ld      a,(ix+-1)
        inc     a
        ld      e,a
        ld      a,(iy+6)
        or      e
        ld      (iy+6),a
        and     12
        or      a
        jr      nz,loc_6CEC
        ld      a,(iy+4)
        or      (iy+5)
        jr      nz,loc_6CEC
        ld      hl,512
        push    hl
        call    _sbrk
        pop     bc
        ld      (iy+4),l
        ld      (iy+5),h
loc_6CEC:
        ld      l,(iy+4)
        ld      h,(iy+5)
        ld      (iy+0),l
        ld      (iy+1),h
        ld      a,l
        or      (iy+5)
        jr      z,loc_6D0E
        ld      a,(ix+-1)
        or      a
        ld      (iy+2),0
        jr      z,loc_6D12
        ld      (iy+3),2
        jr      loc_6D16
loc_6D0E:
        ld      (iy+2),0
loc_6D12:
        ld      (iy+3),0
loc_6D16:
        push    iy
        pop     hl
        jp      cret

;=========================================================
; 75FA flsbuf           Push next buffer to file
;=========================================================
;       int _flsbuf(int c, register FILE * f) {
;           struct fcb * p;
;
;           sub_77F5(); /* Test ^c */
;           p = _fcb + (f - stdin);
;
;           if(f->_flag & _IOWRT) {
;               if(f->_base == (char *)NULL) {
;                   f->_cnt = 0;
;                   if((char)(p->rwp) == 4) {
;                       bdos(CPMWCON, c);
;                       return c;
;                   }
;                   return -1;
;               } else {
;                   fflush(f);
;                   *(f->_ptr++) = c;
;                   f->_cnt--;
;               }
;               return c;
;           }
;           return -1;
;       }
;
        global  __flsbuf
        global  _sub_77F5
        global  __fcb
        global  __iob
        global  adiv
        global  amul
        global  _bdos
        global  _fflush

        psect   text

__flsbuf:
        call    csv
        push    hl
        ld      l,(ix+8)
        ld      h,(ix+9)
        push    hl
        pop     iy
        call    _sub_77F5
        ld      de,__iob
        push    iy
        pop     hl
        or      a
        sbc     hl,de
        ld      de,8
        call    adiv
        ld      de,41
        call    amul
        ld      de,__fcb
        add     hl,de
        ld      (ix+-2),l
        ld      (ix+-1),h
        bit     1,(iy+6)
        jr      z,loc_7689
        ld      a,(iy+4)
        or      (iy+5)
        jr      nz,loc_7662
        ld      (iy+2),0
        ld      (iy+3),0
        ex      de,hl
        ld      hl,36
        add     hl,de
        ld      a,(hl)
        cp      4
        jr      nz,loc_7689
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        ld      hl,2
        push    hl
        call    _bdos
        pop     bc
        pop     bc
loc_7659:
        ld      l,(ix+6)
        ld      h,(ix+7)
        jp      cret
loc_7662:
        push    iy
        call    _fflush
        pop     bc
        ld      a,(ix+6)
        ld      l,(iy+0)
        ld      h,(iy+1)
        inc     hl
        ld      (iy+0),l
        ld      (iy+1),h
        dec     hl
        ld      (hl),a
        ld      l,(iy+2)
        ld      h,(iy+3)
        dec     hl
        ld      (iy+2),l
        ld      (iy+3),h
        jr      loc_7659
loc_7689:
        ld      hl,-1
        jp      cret

;=========================================================
; sub_77F5      Test ^c
;=========================================================
;    #define CPMRCON    1       /* read console */
;    #define    CPMICON 11      /* interrogate console ready */
;
;    char sub_77F5() {  /* Test ^c */
;       char * st;
;
;       if(bdos(CPMICON) == 0) return;     /* status console */
;       if(( *st=bdos(CPMRCON)) != 3) return;  /* read console   */
;       exit();
;    }
;
        global  _sub_77F5
        global  _exit

        psect   text

_sub_77F5:
        call    csv
        push    hl
        ld      hl,11   ; CPMICON - interrogate console ready
        push    hl
        call    _bdos
        pop     bc
        ld      a,l
        or      a
        jp      z,cret
        ld      hl,1    ; CPMRCON - read console
        push    hl
        call    _bdos
        pop     bc
        ld      e,l
        ld      (ix+-1),e
        ld      a,e
        cp      3
        jp      nz,cret
        call    _exit
        jp      cret

;=========================================================
; sub_70F8 def_fcb Define FCB fields
;=========================================================
;    char def_fcb(register struct fcb * fc, char * fname) {
;       char * cp;
;
;       fc->dr = 0;
;       if(*(fname+1) == ':') {
;           fc->dr = *(fname) & 0xf;
;           fname += 2;
;       }
;       cp = &fc->name;
;       while((*fname != '.')
;       && ('\t' < *fname)
;       && (cp < &fc->ft))  *(cp++) = toupper(*(fname++));
;       while(cp < &fc->ft) *(cp++) = ' ';
;       do {if(*fname == 0) break;} while(*(fname++) != '.');
;       while(('\t' < *(fname))
;       && (cp < (char*)&fc->ex)) *(cp++) = toupper(*(++fname));
;       while(cp <= (char*)&fc->ex) *(cp++) = ' ';
;       fc->ex = fc->nr =  0;
;       return 0;
;    }
;
        global  _def_fcb
        global  _toupper
        global  wrelop

        psect   text

_def_fcb:
        call    csv
        push    hl
        ld      l,(ix+6)
        ld      h,(ix+7)
        push    hl
        pop     iy
        ld      (iy+0),0
        ld      l,(ix+8)
        ld      h,(ix+9)
        inc     hl
        ld      a,(hl)
        cp      58
        jr      nz,loc_712D
        dec     hl
        ld      a,(hl)
        ld      l,a
        rla
        ld      a,l
        and     15
        ld      (iy+0),a
        ld      l,(ix+8)
        ld      h,(ix+9)
        inc     hl
        inc     hl
        ld      (ix+8),l
        ld      (ix+9),h
loc_712D:
        push    iy
        pop     hl
        inc     hl
        ld      (ix+-2),l
        ld      (ix+-1),h
        jr      loc_7160
loc_7139:
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
        inc     hl
        ld      (ix+8),l
        ld      (ix+9),h
        ld      l,a
        rla
        sbc     a,a
        ld      h,a
        push    hl
        call    _toupper
        pop     bc
        ld      e,l
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        inc     hl
        ld      (ix+-2),l
        ld      (ix+-1),h
        dec     hl
        ld      (hl),e
loc_7160:
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
        cp      46
        jr      z,loc_719E
        ld      a,(hl)
        ld      e,a
        rla
        sbc     a,a
        ld      d,a
        ld      hl,9
        call    wrelop
        push    iy
        pop     de
        ld      hl,9
        jp      p,loc_71A4
        add     hl,de
        ex      de,hl
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        call    wrelop
        jr      c,loc_7139
        jr      loc_719E
loc_718E:
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        inc     hl
        ld      (ix+-2),l
        ld      (ix+-1),h
        dec     hl
        ld      (hl),32
loc_719E:
        push    iy
        pop     de
        ld      hl,9
loc_71A4:
        add     hl,de
        ex      de,hl
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        call    wrelop
        jr      c,loc_718E
loc_71B1:
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
        or      a
        ld      a,(hl)
        jr      z,loc_71F7
        inc     hl
        ld      (ix+8),l
        ld      (ix+9),h
        cp      46
        jr      nz,loc_71B1
        jr      loc_71F0
loc_71C9:
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
        inc     hl
        ld      (ix+8),l
        ld      (ix+9),h
        ld      l,a
        rla
        sbc     a,a
        ld      h,a
        push    hl
        call    _toupper
        pop     bc
        ld      e,l
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        inc     hl
        ld      (ix+-2),l
        ld      (ix+-1),h
        dec     hl
        ld      (hl),e
loc_71F0:
        ld      l,(ix+8)
        ld      h,(ix+9)
        ld      a,(hl)
loc_71F7:
        ld      e,a
        rla
        sbc     a,a
        ld      d,a
        ld      hl,9
        call    wrelop
        push    iy
        pop     de
        ld      hl,12
        jp      p,loc_722F
        add     hl,de
        ex      de,hl
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        call    wrelop
        jr      c,loc_71C9
        jr      loc_7229
loc_7219:
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        inc     hl
        ld      (ix+-2),l
        ld      (ix+-1),h
        dec     hl
        ld      (hl),32
loc_7229:
        push    iy
        pop     de
        ld      hl,12
loc_722F:
        add     hl,de
        ex      de,hl
        ld      l,(ix+-2)
        ld      h,(ix+-1)
        call    wrelop
        jr      c,loc_7219
        xor     a
        ld      (iy+32),a
        ld      (iy+12),a
        ld      l,a
        jp      cret

;       abs(i) returns the absolute value of i

        global  _abs

_abs:
        pop     de              ;Return address
        pop     hl
        push    hl
        push    de
        bit     7,h             ;Negative?
        ret     z               ;no, leave alone
        ex      de,hl
        ld      hl,0
        or      a               ;Clear carry
        sbc     hl,de
        ret

;       Shift operations - the count is always in B,
;       the quantity to be shifted is in HL, except for the assignment
;       type operations, when it is in the memory location pointed to by
;       HL

        global  shlr    ;shift logical right

shlr:
        ld      a,b             ;check for zero shift
        or      a
        ret     z
        cp      16              ;16 bits is maximum shift
        jr      c,1f            ;is ok
        ld      b,16
1:
        srl     h
        rr      l
        djnz    1b
        ret

        psect   data

;       extern  unsigned char   _ctype_[];      /* in libc.lib */

__ctype_:       ;81h
 defb 00H,20H,20H,20H,20H,20H,20H
 defb 20H,20H,20H,08H,08H,08H,08H,08H,20H,20H,20H,20H,20H,20H,20H,20H
 defb 20H,20H,20H,20H,20H,20H,20H,20H,20H,20H,08H,10H,10H,10H,10H,10H
 defb 10H,10H,10H,10H,10H,10H,10H,10H,10H,10H,04H,04H,04H,04H,04H,04H
 defb 04H,04H,04H,04H,10H,10H,10H,10H,10H,10H,10H,41H,41H,41H,41H,41H
 defb 41H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H,01H
 defb 01H,01H,01H,01H,01H,10H,10H,10H,10H,10H,10H,42H,42H,42H,42H,42H
 defb 42H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H,02H
 defb 02H,02H,02H,02H,02H,10H,10H,10H,10H,20H

;	HEAP1
;
;	Here starts HEAP1
;
	end



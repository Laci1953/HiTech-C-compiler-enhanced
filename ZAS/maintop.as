global  _maintop
global  ncsv, cret, indir
global  _InitDynM
global  _j_opt
global  _n_opt
global  _objFileName
global  _x_opt
global  wrelop
global  _asmFileName
global  _asmFp
global  _fopen
global  _fatalErr
global  _objFp
psect   ttop
_maintop:
global csv
call csv
call    _InitDynM
ld      l,(ix+6)
ld      h,(ix+7)
dec     hl
ld      (ix+6),l
ld      (ix+7),h
ld      l,(ix+8)
ld      h,(ix+9)
inc     hl
inc     hl
ld      (ix+8),l
ld      (ix+9),h
jp      l14
L4:
ld      a,(hl)
inc     hl
ld      h,(hl)
ld      l,a
inc     hl
ld      a,(hl)
cp      74
jp      z,l18
cp      78
jp      z,l20
cp      79
jp      z,l22
cp      88
jp      z,l24
cp      106
jp      z,l18
cp      110
jp      z,l20
cp      111
jp      z,l22
cp      120
jp      z,l24
jp      l13
l18:
ld      a,1
ld      (_j_opt),a
l13:
ld      l,(ix+8)
ld      h,(ix+9)
inc     hl
inc     hl
ld      (ix+8),l
ld      (ix+9),h
ld      l,(ix+6)
ld      h,(ix+7)
dec     hl
ld      (ix+6),l
ld      (ix+7),h
l14:
ld      e,(ix+6)
ld      d,(ix+7)
ld      hl,0
call    wrelop
ld      l,(ix+8)
ld      h,(ix+9)
jp      p,L3
ld      a,(hl)
inc     hl
ld      h,(hl)
ld      l,a
ld      a,(hl)
cp      45
ld      l,(ix+8)
ld      h,(ix+9)
jp      z,L4
L3:
ld      c,(hl)
inc     hl
ld      b,(hl)
ld      (_asmFileName),bc
ld      hl,19f
push    hl
ld      l,c
ld      h,b
push    hl
call    _fopen
pop     bc
pop     bc
ld      (_asmFp),hl
ld      a,l
or      h
jp      nz,l26
ld      hl,(_asmFileName)
push    hl
ld      hl,29f
push    hl
call    _fatalErr
pop     bc
pop     bc
l26:
ld      hl,39f
push    hl
ld      hl,(_objFileName)
push    hl
call    _fopen
pop     bc
pop     bc
ld      (_objFp),hl
ld      a,l
or      h
jp      nz,cret
ld      hl,(_objFileName)
push    hl
ld      hl,49f
push    hl
call    _fatalErr
jp      cret
l20:
ld      a,1
ld      (_n_opt),a
jp      l13
l22:
ld      l,(ix+8)
ld      h,(ix+9)
ld      c,(hl)
inc     hl
ld      b,(hl)
inc     bc
inc     bc
ld      (_objFileName),bc
jp      l13
l24:
ld      a,1
ld      (_x_opt),a
jp      l13
19:
defb    114,0
29:
defb    67,97,110,39,116,32,111,112,101,110,32,102,105,108,101,32
defb    37,115,0
39:
defb    119,98,0
49:
defb    67,97,110,39,116,32,99,114,101,97,116,101,32,102,105,108
defb    101,32,37,115,0
psect   bss
_asmFileName:
defs    2
_objFileName:
defs    2

end

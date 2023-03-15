;
;	Szilagyi Ladislau
;
;	ladislau_szilagyi@euroqst.ro
;
;	December 2022 - March 2023
;
;	Overlays manager for 512KB RAM + 512KB ROM systems
;
;
CHECK_PARS	equ	0		;1 for debug

	GLOBAL _InitOverlays
	GLOBAL _CallOverlay0,_CallOverlay1,_CallOverlay1L,_CallOverlay2,_CallOverlay2L,_CallOverlay3,_CallOverlay4,_CallOverlay5
	GLOBAL _ReturnOverlay0
	GLOBAL _ReturnOverlay1,_ReturnOverlay1L,_ReturnOverlay2,_ReturnOverlay2L,_ReturnOverlay3,_ReturnOverlay4,_ReturnOverlay5
	GLOBAL _ReturnOverlay0_
	GLOBAL _setleds
	GLOBAL _breakpoint
	GLOBAL CrtRamBank
;
;	Return types
;
R0	equ	0
R1	equ	1
R2	equ	2
R3	equ	3
R4	equ	4
R5	equ	5
;
;	Overlay banks
;
Bank_yylex			equ	32+4
Bank_emitSrcInfo		equ	32+4
Bank_peekCh			equ	32+4
Bank_skipStmt			equ	32+4
Bank_skipToSemi			equ	32+4
Bank_sub_3adf			equ	32+4
Bank_sub_3c7e			equ	32+4
Bank_sub_4eed			equ	32+4
Bank_sub_51e7			equ	32+4
Bank_checkScopeExit		equ	32+4
Bank_sub_578d			equ	32+4
Bank_sub_583a			equ	32+4
Bank_sub_58bd			equ	32+4
Bank_sub_591d			equ	32+4
Bank_reduceNodeRef		equ	32+4

Bank_sub_07f5			equ	32+5
Bank_sub_0a83			equ	32+5
Bank_sub_0bfc			equ	32+5
Bank_sub_1441			equ	32+5
Bank_sub_1b4b			equ	32+5
Bank_sub_2105			equ	32+5
Bank_sub_21c7			equ	32+5
Bank_s13ReleaseFreeList		equ	32+5
Bank_allocId			equ	32+5
Bank_allocIConst		equ	32+5
Bank_allocSType			equ	32+5
Bank_sub_2569			equ	32+5
Bank_sub_25f7			equ	32+5

Bank_sub_5be1			equ	32+6
Bank_sub_5c19			equ	32+6
Bank_sub_5dd1			equ	32+6
Bank_sub_69ca			equ	32+6
Bank_sub_7454			equ	32+6
;
;Overlays structure:
;
;Overlays manager
;----------------
;
;RAM MEMORY BANKS
;bank 32	= base at 0000
;bank 36...61	= overlays at 4000H
;
;--------
;
;stack after CallOverlayN: N=0 or 1
;SP -->
;	Return addr
;	Routine
;	(Par1,2,3,4 or none)
;	CALLED RAM bank (+ return type if CHECK_PARS)
;
;stack after entring Routine, before (N)CSV call
;SP -->
;	Return addr
;	(Par1,2,3,4 or none)
;	Routine
;	(Par1,2,3,4 or none)
;	CALLER RAM bank (+ return type if CHECK_PARS)
;
;stack after (N)CSV call
;SP -->
;	local vars
;IX -->
;	old IX
;	old IY
;	Return addr
;	(Par1,2,3,4 or none)
;	Routine
;	(Par1,2,3,4 or none)
;	CALLER RAM bank (+ return type if CHECK_PARS)
;
;stack after executing RET at end of ReturnOverlayN 
;SP -->
;	Routine
;	(Par1,2,3,4 or none)
;	CALLER RAM bank (+ return type if CHECK_PARS)
;

IF	CHECK_PARS

	psect	data

TabBanks:

defb Bank_yylex	
defs 2				
defb Bank_emitSrcInfo		
defs 2
defb Bank_peekCh		
defs 2
defb Bank_skipStmt		
defs 2
defb Bank_skipToSemi		
defs 2
defb Bank_sub_3adf		
defs 2
defb Bank_sub_3c7e		
defs 2
defb Bank_sub_4eed
defs 2
defb Bank_sub_51e7
defs 2
defb Bank_checkScopeExit
defs 2
defb Bank_sub_578d
defs 2
defb Bank_sub_583a
defs 2
defb Bank_sub_58bd
defs 2
defb Bank_sub_591d
defs 2
defb Bank_reduceNodeRef
defs 2
defb Bank_sub_07f5		
defs 2
defb Bank_sub_0a83		
defs 2
defb Bank_sub_0bfc		
defs 2
defb Bank_sub_1441		
defs 2
defb Bank_sub_1b4b		
defs 2
defb Bank_sub_2105		
defs 2
defb Bank_sub_21c7		
defs 2
defb Bank_s13ReleaseFreeList	
defs 2
defb Bank_allocId			
defs 2
defb Bank_allocIConst		
defs 2
defb Bank_allocSType		
defs 2
defb Bank_sub_2569		
defs 2
defb Bank_sub_25f7		
defs 2
defb Bank_sub_5be1		
defs 2
defb Bank_sub_5c19		
defs 2
defb Bank_sub_5dd1		
defs 2
defb Bank_sub_69ca		
defs 2
defb Bank_sub_7454		
	
ENDIF

openf 	equ 15 	; open file
closef 	equ 16 	; close file
readf 	equ 20	; sequential read
getdsk	equ 25  ; get crt disk
setdma	equ 26	; set DMA addr

__bdos	equ	5

	psect	data

fcb:				; fcb
	defs	1		; disk+1
	defm	'P1      '	; file name (P1.OVN, N=1,2,3)
ovrname:defm	'OV '		; 
dfcbz:	defb	0		; EX=0
	defs	2		; S1,S2
	defb	0		; RC=0
	defs	16		; D0,...D15
	defb	0		; CR=0
	defb	0,0,0		; R0,R1,R2
fcb_end:
pointer:defs	2

	psect	text

; system interface subroutines
;
open: 	ld 	c,openf
 	jp 	__bdos
;
close: 	ld 	c,closef
 	jp 	__bdos
;
read: 	ld 	c,readf
 	jp 	__bdos
;
setDMA:	ld	c,setdma
	jp	__bdos
;
GetDisk:ld	c,getdsk
	jp	__bdos
;
_cleanfcb:
	ld	hl,dfcbz
	xor	a
	ld	b,fcb_end - dfcbz
clloop:	ld	(hl),a
	inc	hl
	djnz	clloop
	ret
;
;	Read overlay binary files and store them to RAM banks
;
;	C = RAM bank (36, 37, or 38)
;	B = last char of file extenstion ('1', '2', or '3')
;
;	returns CARRY = 0 : OK, 1 : file not found
; 
ReadOverlay:

	ld	a,c		;RAM bank
	out	(79H),a		;set to 4000H

	ld	a,b		; open P1.OV1
	ld	(ovrname+2),a

	call	_cleanfcb

 	ld 	de,fcb 		; source file
 	call 	open 		; error if 255
 	inc 	a 		; 255 becomes 0
	scf			; CARRY=1
 	ret 	z	 	; done if no file

	ld	de,4000H	; init DMA pointer to 4000H

readloop:

	ld	(pointer),de
	call	setDMA

	ld	de,fcb
 	call 	read 		; read next record
 	or 	a 		; end of file?
 	jp 	nz,eofile

	ld	de,(pointer)	;increment pointer
	ld	hl,128
	add	hl,de
	ex	de,hl

	jr	readloop

eofile:	ld	de,fcb
	call	close
	xor	a		;CARRY=0
	ret
;
;bool InitOverlays(void);

_InitOverlays:
	ld	a,0C3H
	ld	(38H),a
	ld	hl,_breakpoint
	ld	(39H),hl

	call	GetDisk
	inc	a
	ld	(fcb),a

	ld	bc,3124H	;B='1', C=36
	call	ReadOverlay
	jr	c,nofile
	ld	bc,3225H	;B='2', C=37
	call	ReadOverlay
	jr	c,nofile
	ld	bc,3326H	;B='3', C=38
	call	ReadOverlay
	jr	c,nofile

	ld	a,32+1		;reselect RAM bank 1 to 4000H
	out	(79H),a		
	ld	(CrtRamBank),a	;current overlay on 4000H is 33

	ld	l,1
	ret

nofile:	ld	l,0
	ret

;
;void	setleds(char)
;
_setleds:
	ld	hl,2
	add	hl,sp
	ld	a,(hl)
	out	(0),a
	ret
;
_breakpoint:
	ld	(3FFEH),ix	;save IX
	ld	a,33		;set RAM 1 to 4000H
	out	(79H),a
	ld	hl,0C000H	;move stack to 4000H
	ld	de,4000H
	ld	bc,4000H
	ldir
	ld	a,0FFH		;all leds ON
	out	(0),a
	jr	$

IF	CHECK_PARS

WrongRet:
	push	bc		;push return address
WrongBank:
	pop	hl
	ld	a,(CrtRamBank)
	ld	(3FFDH),a	;save crt RAM bank
	ld	(3FFEH),hl	;save return address
	ld	a,0FFH
	out	(0),a		;all leds ON
	jr	$

ENDIF

;
;short CallOverlay0(void* Routine, char CalledRamBank);
;
_CallOverlay0:
	pop	hl		;HL=return addr
	pop	de		;DE=Routine
	push	de		;Routine back on stack
	push	hl		;return address back on stack
	ld	hl,4
IF	CHECK_PARS
	ld	b,R0
ENDIF
save_caller:			;DE=Routine, HL=RAM bank offset on stack
	add	hl,sp
	ld	c,(hl)		;C=called RAM bank
	ld	a,(CrtRamBank)
	ld	(hl),a		;save caller RAM bank on stack
IF	CHECK_PARS
	inc	hl
	ld	(hl),b		;save expected return type
	ld	hl,TabBanks
	ld	a,l		;adjust by adding low(Routine)
	add	a,e
	ld	l,a
	jr	nc,nohinc
	inc	h
nohinc:
	ld	a,(hl)
	cp	c		;should be equal
	jp	nz,WrongBank
ENDIF
	ld	a,c		;C=CalledRamBank
	out	(79H),a		;mapped to 4000H
	ld	(CrtRamBank),a	;set CrtRamBank=CalledRamBank
	ex	de,hl		;HL=Routine
	jp	(hl)
;
;short CallOverlay1(void* Routine, short Par1, char CalledRamBank);
;
_CallOverlay1:
	pop	hl		;HL=return addr
	pop	de		;DE=Routine
	pop	bc		;BC=Par1
	push	bc		;Par1 on stack
	push	de		;Routine on stack
	push	bc		;Par1 on stack
	push	hl		;return address on stack
	ld	hl,8
IF	CHECK_PARS
	ld	b,R1
ENDIF
	jp	save_caller
;
;short CallOverlay1L(void* Routine, long Par1, char CalledRomBank);
;short CallOverlay2(void* Routine, short Par1, short Par2, char CalledRamBank);
;
_CallOverlay1L:
_CallOverlay2:
	pop	de		;DE=return addr
	ld	hl,4
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=Par2 (or Par1 high)
	push	bc		;on stack
	dec	hl
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par1 (or Par1 low)
	push	bc		;on stack
	push	de		;return address on stack
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	e,(hl)		;DE=Routine
	ld	hl,12
IF	CHECK_PARS
	ld	b,R2
ENDIF
	jp	save_caller
;
;short CallOverlay3(void* Routine, short Par1, short Par2, short Par3, char CalledRamBank);
;short CallOverlay2L(void* Routine, long Par1, short Par2, char CalledRamBank);
;
_CallOverlay2L:
_CallOverlay3:
	pop	de		;DE=return addr
	ld	hl,6
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=Par3 (or Par2)
	push	bc		;on stack
	dec	hl
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par2 (or Par1 high)
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par1 (or Par1 low)
	push	bc		;on stack
	push	de		;return address on stack
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	e,(hl)		;DE=Routine
	ld	hl,16
IF	CHECK_PARS
	ld	b,R3
ENDIF
	jp	save_caller
;
;short CallOverlay4(void* Routine, short Par1, short Par2, short Par3, short Par4, char CalledRamBank);
;
_CallOverlay4:
	pop	de		;DE=return addr
	ld	hl,8
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=Par4
	push	bc		;on stack
	dec	hl
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par3
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par2
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par1
	push	bc		;on stack
	push	de		;return address on stack
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	e,(hl)		;DE=Routine
	ld	hl,20
IF	CHECK_PARS
	ld	b,R4
ENDIF
	jp	save_caller
;
;short CallOverlay5(void* Routine, short Par1, short Par2, short Par3, short Par4, short Par5, char CalledRamBank);
;
_CallOverlay5:
	pop	de		;DE=return addr
	ld	hl,10
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC=Par5
	push	bc		;on stack
	dec	hl
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par4
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par3
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par2
	push	bc		;on stack
	dec	hl
	ld	b,(hl)
	dec	hl
	ld	c,(hl)		;BC=Par1
	push	bc		;on stack
	push	de		;return address on stack
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	e,(hl)		;DE=Routine
	ld	hl,24
IF	CHECK_PARS
	ld	b,R5
ENDIF
	jp	save_caller
;
;void ReturnOverlay0(short ret_val);
;	returns HL
;
_ReturnOverlay0:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+9)	;check expected return type
	cp	R0
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+8)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
set_caller_bank:
	ld	(CrtRamBank),a	;set CrtRamBank=CALLER Ram Bank
	out	(79H),a		;mapped to 4000H
	ret 			;return HL
;
;void ReturnOverlay0_(short ret_val);
;(in routines that do not call (N)CSV)
;
_ReturnOverlay0_:
	pop	bc		;drop local return addr
	pop	de		;DE=ret_val
	ld	hl,4
	add	hl,sp
	ld	a,(hl)		;A=CallerRamBank
	ex	de,hl		;HL=ret_val
	jp	set_caller_bank
;
;void ReturnOverlay1(short ret_val);
;	returns HL
;
_ReturnOverlay1:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+13)	;check expected return type
	cp	R1
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+12)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
	pop	de		;DE=return addr
	pop	bc		;drop Par1
	push	de		;return addr on stack
	jp	set_caller_bank
;
;void ReturnOverlay2(short ret_val);
;void ReturnOverlay1L(short ret_val);
;	returns HL
;
_ReturnOverlay1L:
_ReturnOverlay2:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+17)	;check expected return type
	cp	R2
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+16)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
	pop	de		;DE=return addr
	pop	bc		;drop Par1 (or drop low & high Par1)
	pop	bc		;drop Par2
	push	de		;return addr on stack
	jp	set_caller_bank
;
;void ReturnOverlay3(short ret_val);
;void ReturnOverlay2L(short ret_val);
;	returns HL
;
_ReturnOverlay2L:
_ReturnOverlay3:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+21)	;check expected return type
	cp	R3
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+20)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
	pop	de		;DE=return addr
	pop	bc		;drop Par1 (or Par1 low, high & Par2)
	pop	bc		;drop Par2
	pop	bc		;drop Par3
	push	de		;return addr on stack
	jp	set_caller_bank
;
;void ReturnOverlay4(short ret_val);
;	returns HL
;
_ReturnOverlay4:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+25)	;check expected return type
	cp	R4
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+24)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
	pop	de		;DE=return addr
	pop	bc		;drop Par1
	pop	bc		;drop Par2
	pop	bc		;drop Par3
	pop	bc		;drop Par4
	push	de		;return addr on stack
	jp	set_caller_bank
;
;void ReturnOverlay5(short ret_val);
;	returns HL
;
_ReturnOverlay5:
	pop	bc		;drop local return addr
	pop	hl		;HL=ret_val
IF	CHECK_PARS
	ld	a,(ix+29)	;check expected return type
	cp	R5
	jp	nz,WrongRet
ENDIF
	ld	a,(ix+28)	;A=CallerRamBank
	ld	sp,ix		;restore SP
	pop	ix		;restore IX
	pop	iy		;restore IY
	pop	de		;DE=return addr
	pop	bc		;drop Par1
	pop	bc		;drop Par2
	pop	bc		;drop Par3
	pop	bc		;drop Par4
	pop	bc		;drop Par5
	push	de		;return addr on stack
	jp	set_caller_bank
;
	psect	bss

CrtRamBank:	defs	1
;

L_FED0
	CALL L_FED4
	RET NC
L_FED4
	LD A,04
L_FED6
	DEC A
	JR NZ,L_FED6
	AND A
L_FEDA
	INC B
	RET Z
	IN A,($FE)
	XOR C
	AND $40
	JR Z,L_FEDA
	LD A,C

	;DUP 27
	;NOP
	;EDUP

	;BLOCK 500,0   ; define a block of 500 bytes of zero
	; DB and DEFB may be used instead of BYTE

	;XOR $42
	DB $EE
L_FEE5
	DB $42

	LD C,A
	AND $07
	OR $08
	OUT ($FE),A
	SCF
	RET
L_FEEF
	EX AF,AF'
	JR NZ,L_FEF7
	LD (IX+00),L
	JR L_FF00
L_FEF7
	RL C
	XOR L
	RET NZ
	LD A,C
	RRA
	LD C,A
	JR L_FF03
L_FF00	
	INC IX
	DEC DE
L_FF03
	EX AF,AF'
	LD B,$82
L_FF06
	LD L,$01
L_FF08
	CALL L_FED0
	RET NC
	LD A,$8B
	CP B
	RL L
	LD B,$80
	JR NC,L_FF08
	LD A,H
	XOR L
	LD H,A
	LD A,E
	OR D
	JR NZ,L_FEEF
	LD A,H
	CP 01
	RET
L_FF20
	INC D
	EX AF,AF'
	DEC D
	LD A,$08
	OUT ($FE),A
	IN A,($FE)
	AND $40
	LD C,A
	CP A
L_FF2D
	RET NZ
L_FF2E
	CALL L_FED4
	JR NC,L_FF2D
	LD HL,$0100
L_FF36
	DJNZ L_FF36
	DEC HL
	LD A,H
	OR L
	JR NZ,L_FF36
L_FF3D
	CALL L_FED0
	JR NC,L_FF2D
	LD H,00
L_FF44
	LD B,$75
	CALL L_FED0
	JR NC,L_FF2D
	LD A,$B0
	CP B
	JR NC,L_FF2E
	INC H
	JR NZ,L_FF44
L_FF53
	LD B,$B0
	CALL L_FED4
	JR NC,L_FF2D
	LD A,B
	CP $C1
	JR NC,L_FF53
	CALL L_FED4
	RET NC
	LD H,$00
	LD B,$80
	RET

L_FF68
	;LD HL,L_FE00
	;LD DE,L_FE00+1
	;XOR A
	;LD (HL),A
	;LD BC,$01D0
	;LDIR
	;LD IY,$5C3A
	LD HL,$0430
	;EXX
	;IM 1
	;EI
	;LD SP,$626C
	;JP $8000		; jump
L_FF86
	;NOP
	;NOP
	;NOP

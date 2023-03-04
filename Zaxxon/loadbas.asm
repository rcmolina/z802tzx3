	; este cargador solo funciona en memoria non contended >= $8000
        define  tr $ffff &
        ;include define.asm

        output  loadbas.bin
        org     $FE00-(L_FE00-ini) ; muevo a $7F00, y el reubicador por encima
ini	LD 	SP, $fffe	; hace falta, parece
	di
        db      $de, $c0, $37, $0e, $8f, $39, $96
        ld      hl, $5ccb+L_FE00-ini
        ld      de, $FE00   	; muevo dzx7b a $FE00
        ld      bc, fin-L_FE00
        ldir
	jp	$fe00

L_FE00
	LD SP, $fffe
	LD IX, $8000
	LD DE, $0FA5
	LD A, $41	; sync blue
	LD (L_FEE5),A
	LD A, $AA
	SCF
	CALL L_FF20
	LD A,$45	; data cyan, $40 for black and no border
	LD (L_FEE5),A
	LD A,C
	AND $40
	LD C,A
	CALL L_FF06
        ld      hl, $8FA4	; $8FA4=$8000+($0FA5-1) fin del comprimido por arriba, con longcomprimido $0FA5 4005
        ld      de, $5AFF 	; $4000 + $1B00 -1
        call    tr dzx7


	LD IX, $5BF0
	LD DE, $4D1B 	; 
	LD A, $41	; sync blue
	LD (L_FEE5),A
	LD A, $AA	; es el flag byte, el primero del bloque. Por defecto 00 ,se cambia a $AA
	SCF
	CALL L_FF20
	LD A,$44	; data green
	LD (L_FEE5),A
	LD A,C
	AND $40
	LD C,A
	CALL L_FF06
        ld      hl, $A907	; $A917=$5C00+($4D18-1) fin del comprimido por arriba, con longcomprimido $4D18 19736
			 	; empieza en $5BF0, $5BF0 +($4D18-1)=$A907
        ld      de, $FE02 	; $5C00 + $A203 -1
        call    tr dzx7

	
	;EI
	;LD HL, $FF68	; tiene sentido se pone al final y borra el cargador desde $FE00 , yo no lo hago nunca
	;LD DE, $FFD0
	;LD BC, $01E
	;LDIR
	;JP $FFD0

	;LD HL,L_FE00
	;LD DE,L_FE00+1
	;XOR A
	;LD (HL),A
	;LD BC,$01D0
	;LDIR
	;LD IY,$5C3A
	;LD HL,$0430
	;EXX
	;IM 1
	;EI
	;LD SP,$fffe
	;jp $ffd0
	EI
	LD SP,$626C
	jp $8000

LAST
	;BLOCK $FED0-LAST,0   ; define a block of XX bytes of zero for filling gap

dzx7b   include dzx7b_fast.asm

load	include slturbo.asm
fin
	; este cargador solo funciona en memoria non contended >= $8000
        define  tr $ffff &
        ;include define.asm

        output  loaderS7.bin
        org     $FE00

L_FE00
	LD SP, $fffe

	LD IX, $8000
	LD DE, $0FA5
	LD A, $AA
	SCF
	CALL LD_BLOCK

        ld      hl, $8FA4	; $8FA4=$8000+($0FA5-1) fin del comprimido por arriba, con longcomprimido $0FA5 4005
        ld      de, $5AFF 	; $4000 + $1B00 -1
        call    tr dzx7


	LD IX, $5BF0
	LD DE, $4D18 	
	LD A, $AA	; es el flag byte, el primero del bloque. Por defecto 00 ,se cambia a $AA
	SCF
	CALL LD_BLOCK

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

load	include tloadz80rcby.asm
fin
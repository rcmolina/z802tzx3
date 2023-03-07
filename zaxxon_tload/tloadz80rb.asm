; este cargador solo funciona en memoria non contended >= $8000

	   define COMPARE $87 	; variable bd, antes 7
	   define DELAY $3	; variable a, antes 7
	   define COLOUR $41	; antes $0   

LD_BLOCK   CALL LD_BYTES
           RET  C             
           RST  0008	;ERROR_1   
           DEFB $1A ; R Tape Loading Error
LD_BYTES   INC  D               
           EX   AF,AF'         
           DEC  D                
           LD   A,$08            ;(Border BLACK $ MIC off)
           OUT  ($FE),A         
           IN   A,($FE)         
           AND  $40              
           LD   C,A              ;(BLACK/chosen colour)
           CP   A               
LD_BREAK   RET  NZ              
LD_START   CALL LD_EDGE_1       
           JR   NC,LD_BREAK     
           LD   H,$00    	; cambiado a $00 en binario, aunuqe en el desensamblado pone $80      

LD_LEADER  LD   B,$75     	; cambiado a $75 en binario, aunuqe en el desensamblado pone $73        
           CALL LD_EDGE_2       
           JR   NC,LD_BREAK 
           LD   A,$B0  		; cambiado a $B0 en binario, aunuqe en el desensamblado pone $B2    	      
           CP   B          
           JR   NC,LD_START     
           INC  H              
           JR   NZ,LD_LEADER  

LD_SYNC    LD   B,$B0            
           CALL LD_EDGE_1       
           JR   NC,LD_BREAK     
           LD   A,B             
           CP   $C1            
           JR   NC,LD_SYNC      

           CALL LD_EDGE_1       
           RET  NC             
           LD   H,$00          
           LD   B,$80           
           JR   LD_MARKER      

LD_LOOP    EX   AF,AF'        
           JR   NZ,LD_FLAG  
           LD   (IX$00),L      
           JR   LD_NEXT  

LD_FLAG    RL   C       
           XOR  L              
           RET  NZ              
           LD   A,C            
           RRA                  
           LD   C,A             
           JR   LD_FLNEXT     

;LD_NEXT    DEC  IX
LD_NEXT    INC  IX             
           DEC  DE            
LD_FLNEXT  EX   AF,AF'     
           LD   B,$82        
LD_MARKER  LD   L,$01           


LD_8_BITS  CALL LD_EDGE_2        
           RET  NC             
           LD   A,COMPARE       ; 7    (variable 'bd')

           CP   B               
           RL   L         
           LD   B,$80          
           JR   NC,LD_8_BITS     

           LD   A,H            
           XOR  L               
           LD   H,A          
           LD   A,D              
           OR   E              
           JR   NZ,LD_LOOP      
           LD   A,H                     
           CP   $01                    
           RET                         

LD_EDGE_2  CALL LD_EDGE_1       
           RET  NC        

LD_EDGE_1  LD   A,DELAY          ;7    (variable 'a')

LD_DELAY   DEC  A            
           JR   NZ,LD_DELAY    
           AND  A            

LD_SAMPLE  INC  B                ;4    (variable 'b')
           RET  Z             
           IN   A,($FE)         
           XOR  C               
           AND  $40                 
           JR   Z,LD_SAMPLE      
			           ;https://worldofspectrum.org/faq/reference/48kreference.htm#PortFE
			           ;Bit 6 of IN-Port 0xfe is the EAR input bit. 

        		           ;Bit   7   6    5   4    3   2   1   0
            			       ;+-----------------+---------------+
            			       ;|   | EI |   | EO | MO |  Border  |
            			       ;+-----------------+---------------+
           LD      A,C
	   INC     A      	; RAINBOW    
           XOR     $40          ;$41/$00: conmuta borde de 001 azul a 000 negro, y bit 6 activa/desactiva
           AND     $47   
         
           LD   C,A
	   ;ADD  $01		   ; colour 0/1 -> 1/2 (blue/red)
	   ;XOR 	$06		   ; colour 0/7 -> 6/1        
           ;AND  $07            
           ;OR   $08 		   ; MIC OFF, lo quito para compensar ciclos   
           OUT  ($FE),A       
           SCF                  
           RET            



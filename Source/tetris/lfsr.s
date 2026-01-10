
    AREA    asm_functions, CODE, READONLY
    EXPORT  lfsr_step

lfsr_step       PROC
                
                STMFD sp!,{r4-r8,r10-r11,lr}                
                
                MOV r4, r0 
                MOV r5, r1
                
                ;r4=current_state
                ;r5=tap
                
                AND r6, r4, r5      ; Applica maschera Taps
                EOR r8, r8, r8      ; r8 = 0 (Accumulatore parità)
                
loop
                CMP r6, #0
                BEQ end_loop
                
                LSRS r6, r6, #1     ; Shift e aggiorna flag Carry
                BCC loop            ; Se Carry=0, salta XOR
                
                EOR r8, r8, #1      ; Se c'era un bit a 1, inverte parità
                B loop
                
end_loop        
                ; Calcolo nuovo stato
                LSR r4, r4, #1      ; Shift registro a destra
                LSL r8, r8, #7      ; Sposta bit parità in posizione 7 (MSB 8-bit)
                
                AND r4, r4, #0x7F   ; Pulisce bit spazzatura (tiene 7 bit)
                ORR r4, r4, r8      ; Inserisce nuovo MSB
                
                MOV r0, r4          ; Ritorna il nuovo stato in R0
                
                LDMFD sp!,{r4-r8,r10-r11,pc}
                
                ENDP
                
                END
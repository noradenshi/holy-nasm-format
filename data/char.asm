[bits 32]
    label

        ; esp -> [ret]

push 'H'  ; 'H' -> stack


; esp -> ['H' 00 00 00][ret]

call [ebx+1*4]  
add esp, 4       



push 0              ; esp -> [00 00 00 00][ret] awhjahefjk  hfjake hk
call [ebx+0*4]      ; exit(0);

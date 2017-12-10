.section .data 
lista:          .int    1 ,1 ,1 ,1 ,1 ,1 ,1 ,1
                .int    1 ,1 ,1 ,1 ,1 ,1 ,1 ,1
                .int    1 ,1 ,1 ,1 ,1 ,1 ,1 ,1
                .int    1 ,1 ,1 ,1 ,1 ,1 ,1 ,1		

longlista:      .int    (.-lista)/4 
resultado:      .quad   -1       
                                  

.section .text 
_start:	.global _start 

        mov     $lista, %ebx 
        mov     $0, %edx          
        mov     longlista, %ecx 
        call    suma 

 
        mov     %eax, resultado 	
        mov     %edx, resultado+4
	 
        mov     $1, %eax 
        mov     $0, %ebx 
        int     $0x80 
	 
suma: 
        push    %esi           # Usaremos %esi como índice.
        mov     $0, %eax 
        mov     $0, %esi 

bucle: 
        add     (%ebx, %esi, 4), %eax 
        jnc     acarreo        
        inc     %edx

acarreo: 
        inc     %esi 
        cmp     %esi,%ecx 
        jne     bucle 
	 
        pop     %esi 
        ret


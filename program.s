.section .data
dado:
.word  0x4, 0x6, 0x8 


.section .text
.global _start



_start:
    MOV R0, #1
    ldr r4, =dado
    ldr R0, [r4]
    B in
    mov r1, #2
    mov r3, #4
in:
    MOV R2, #3
    B end
end:
    MOV R0, #3
    b .
   
    


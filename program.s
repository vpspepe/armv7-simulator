.section .text
.global _start


in:
    MOV R2, #3
    B end
_start:
    MOV R0, #1
    B in
    mov r1, #2
    mov r3, #4
end:
    MOV R0, #3
   
    


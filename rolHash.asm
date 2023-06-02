section .text
        global _rolHash

; only arg is a pointer to null terminated string, return value - hash value
_rolHash:
                xor rax, rax    ; zeroing hash counter

                push rbx        ; saving rbx val cause it will be used as temp reg
                xor rbx, rbx

loop:
                rol eax, 1              ; rols hash val
                mov bl, byte [rdi]
                add eax, ebx

                inc rdi                 ; next byte
                cmp byte [rdi], 0       ; if zero - break
                jne loop

                pop rbx

                ret
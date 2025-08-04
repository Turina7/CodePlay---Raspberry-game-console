.section ".text.boot"

.global _start

_start:
    // Configurar stack pointer
    ldr sp, =_stack_top
    
    // Limpar BSS
    ldr r0, =_bss_start
    ldr r1, =_bss_end
    mov r2, #0
clear_bss:
    cmp r0, r1
    strlo r2, [r0], #4
    blo clear_bss
    
    // Chamar main
    bl main
    
    // Loop infinito se main retornar
halt:
    wfe
    b halt

.section ".bss"
.align 4
_stack_bottom:
    .space 4096
_stack_top:

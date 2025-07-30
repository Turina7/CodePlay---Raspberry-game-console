ENTRY(_start)

SECTIONS
{
    /* O kernel inicia em 0x8000 para Raspberry Pi 2 */
    . = 0x8000;
    
    /* Seção de código */
    .text : {
        KEEP(*(.text.boot))
        *(.text)
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Seção de dados inicializados */
    .data : {
        *(.data)
    }
    
    /* Seção BSS (dados não inicializados) */
    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }
    
    /* Descartar seções desnecessárias */
    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

/* Definir símbolos para o stack */
_stack_top = 0x8000;
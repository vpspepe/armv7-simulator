SECTIONS {
        .text 0x0: {
                _start = .;
                *(.text)
        }
        .data : {
                *(.data)
        }
        .bss : {
                *(.bss)
        }
}

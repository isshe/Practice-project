;主引导程序
;
;LOADER_BASE_ADDR equ 0xA000
;LOADER_START_SECTOR equ 0x2

SECTION MBR vstart=0x7c00
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov sp, 0x7c00
    mov ax, 0xb800
    mov gs, ax              ;0xb8000!!!!!

;清屏
;----------------------------------
; INT 0x10 功能号 0x06 功能：上卷窗口
;----------------------------------
    mov ax, 0x0600
    mov bx, 0x0700
    mov cx, 0               ; 左上角(0, 0)
    mov dx, 0x184f          ; 右下角(4f=80, 18=25)

    int 0x10

    mov byte [gs:0x00], 'I'
    mov byte [gs:0x01], 0xA4

    mov byte [gs:0x02], 'S'
    mov byte [gs:0x03], 0xA4

    mov byte [gs:0x04], 'S'
    mov byte [gs:0x05], 0xA4

    mov byte [gs:0x06], 'H'
    mov byte [gs:0x07], 0xA4

    mov byte [gs:0x08], 'E'
    mov byte [gs:0x09], 0xA4


    jmp $

    times 510-($-$$) db 0
    db 0x55, 0xaa

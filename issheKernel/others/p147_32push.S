
    %include "boot.inc"
    section push32_test vstart=LOADER_BASE_ADDR
    jmp loader_start

gdt_addr:   ; 构建gdt及其内部描述符
    GDT_BASE:
        dd 0x00000000
        dd 0x00000000
    
    CODE_DESC:
        dd 0x0000FFFF
        dd DESC_CODE_HIGH4

    DATA_STACK_DESC:
        dd 0x0000FFFF
        dd DESC_DATA_HIGH4

    VIDEO_DESC:
        dd 0x80000008
        dd DESC_VIDEO_HIGH4     ; 此时dp1已改为0 ？

    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1
    SELECTOR_CODE equ (0x0001<<3)       ; 0000 0001 -> 0000 1000 = 8
    SELECTOR_DATA equ (0x0002<<3)       ; 0000 0010 -> 0001 0000 = 16
    SELECTOR_VIDEO equ (0x0003<<3)      ; 0000 0011 -> 0001 1000 = 24

gdt_ptr:
        dw GDT_LIMIT
        dd gdt_addr

; -------------准备进入保护模式---------------
;1. 打开A20 (???)
;2. 加载gdt
;3. 将cr0的pe位，置1

loader_start:   
    ;1. 打开A20
    in al, 0x92         ; 读出来，0x92 -> al
    or al, 0000_0010B   ; 2 
    out 0x92, al        ; 写回去，al->0x92

    ;2. 加载gdt
    lgdt [gdt_ptr]      ; 

    ;3. cr0的第0位，置1
    mov eax, cr0        ; 
    or eax, 0x1         ; 0x00000001
    mov cr0, eax        ;

    ; 刷新流水线，避免分支预测的影响。
    ; 这种CUP优化策略，最怕jmp跳转，
    ; 这将导致之前做的预测失效，从而起到刷新的作用！
    jmp SELECTOR_CODE:p_mode_start

[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax 
    mov es, ax 
    mov es, ax 
    mov ss, ax 
    mov esp, LOADER_BASE_ADDR
    push byte 0x7               ; 主要观察一下三个占用的栈空间，感觉会是【2,2,4】
    push word 0x8               ; 实际: 4, 2, 4, word被反转了(0x66)
    push dword 0x9
    jmp $


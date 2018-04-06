    %include "boot.inc"
    section loader vstart=LOADER_BASE_ADDR
    LOADER_STACK_TOP equ LOADER_BASE_ADDR   ; 注意这个是反向生长
    jmp loader_start

; 构建gdt及其内部的描述符
    GDT_BASE:                   ; 没用！
        dd 0x00000000   
        dd 0x00000000
    CODE_DESC:                  ; 代码段描述符
        dd 0x0000FFFF           ; 前面16位是段基址，后面的16位段界限
        dd DESC_CODE_HIGH4      ; 这里段界限还有4位1
    DATA_STACK_DESC:
        dd 0x0000FFFF
        dd DESC_DATA_HIGH4      ;
    VIDEO_DESC:                 ; 这个不是很懂!!!???
        dd 0x80000007           ; 段界限=(0xbffff-0xb8000)/4k = 0x7， 4k = 0x1000
                                ; 0xb8000-0x00或0x01bffff是文本模式显示适配器
        dd DESC_VIDEO_HIGH4     ; 高位中有0x0b

    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1
    times 60 dq 0               ; 预留60个描述符的空位！！！

    ; 以下3个选择子不明白：0x0001代表第1个描述符，也就是CODE_DESC!
    SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0     ; TI_GDT使用全局描述符
    ; 上面一句相当于：   (CODE_DESC - GDT_BASE) / 8 + TI_GDT + RPL0
    SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

    ; 以下是gdt的指针，前2字节是gdt界限，后四字节是gdt起始地址
    gdt_ptr     dw GDT_LIMIT
                dd GDT_BASE

    loadermsg   db '2 loader in real.'

;-------------------------------------------------------------------
; INT 0x10, 功能号:0x13, 功能描述：打印字符串 
;-------------------------------------------------------------------
; AH = 子功能号（0x13）
; AL = 显示输出方式
;   0: 字符串中只含显示字符，其显示属性在BL中，显示后，光标位置不变
;   1: 字符串中只含显示字符，其显示属性在BL中，显示后，光标位置改变
;   2: 字符串中含显示字符和显示属性
;   3: 字符串中含显示字符和显示属性
; BH = 页码
; BL = 属性(若AL = 0x00或0x01)
; (DH, DL) = 坐标(行, 列)
; ES:BP = 字符串地址
; 无返回值
;-------------------------------------------------------------------
loader_start:
    mov sp, LOADER_BASE_ADDR    ; 0x900
    mov bp, loadermsg
    mov cx, 17                  ; 0x11, 字符串长度
    mov ax, 0x1301              ; AH = 0x13（子中断号）, AL = 0x01（显示输出方式）
    mov bx, 0x001f              ; BH = 0(页号0)， BL = 0x1f（蓝底粉红字）
    mov dx, 0x1800              ; (24, 0)
    int 0x10                    ; 0x10号中断 

; 准备进入保护模式
; 1. 打开A20地址线
; 2. 加载GDT
; 3. 将cr0的pe位置1

    ; 1.打开A20地址线
    in al, 0x92
    or al, 0000_0010B
    out 0x92, al 

    ; 加载GDT
    lgdt [gdt_ptr]              ; 

    ; CR0第0位置1
    mov eax, cr0 
    or eax, 0x00000001          ; 
    mov cr0, eax

    ;jmp dword SELECTOR_CODE:p_mode_start ; 代码段的p_mode_start，刷新流水线
    jmp  SELECTOR_CODE:p_mode_start	     ; 刷新流水线，避免分支预测的影响,这种cpu优化策略，最怕jmp跳转，
					     ; 这将导致之前做的预测失效，从而起到了刷新的作用。

[bits 32]
p_mode_start: 
    mov ax, SELECTOR_DATA
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VIDEO
    mov gs, ax                  ; gs，显示内存？

    mov byte [gs:160], 'P'      ; 80*25，每行80个字符，一个字符2字节，故第二行偏移为160

    jmp $
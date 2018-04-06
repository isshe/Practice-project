; 主引导程序
;----------------------------------------------------------

%include "boot.inc"
SECTION MBR vstart=0x7c00
    mov ax, cs      ; 00
    mov ds, ax
    mov es, ax 
    mov ss, ax 
    mov fs, ax 
    mov sp, 0x7c00  ; 
    mov ax, 0xb800  ;
    mov gs, ax      ; 显卡地址

;清屏
;-----------------------------------------------------------
; INT 0x10 功能号：0x06 功能：上卷窗口
;-----------------------------------------------------------
    mov ax, 0x0600
    mov bx, 0x0700
    mov cx, 0x0         ; (0, 0) => (0, 0)
    mov dx, 0x184f      ; (4f, 18) => (80, 25)
    int 0x10

;输出字符串: ISSHE
    mov byte [gs:0x00], '1'
    mov byte [gs:0x01], 0xA4

    mov byte [gs:0x02], ' '
    mov byte [gs:0x03], 0xA4

    mov byte [gs:0x04], 'M'
    mov byte [gs:0x05], 0xA4

    mov byte [gs:0x06], 'B'
    mov byte [gs:0x07], 0xA4

    mov byte [gs:0x08], 'R'
    mov byte [gs:0x09], 0xA4

;---------------------------------------------------------------
    mov eax, LOADER_START_SECTOR    ; 起始扇区LBA地址, eax=LBA扇区号 = 0x2
    mov bx, LOADER_BASE_ADDR        ; 写入的地址, bx=将数据写入的内存地址 = 0x900
    mov cx, 0x4                     ; 待写入的扇区数, 这三个是参数
    call rd_disk_m_16               ; 以下读取程序的起始部分(一个扇区)

    jmp LOADER_BASE_ADDR            ; 读取完以后，就跳过去执行（当前那里还没有内容）

;---------------------------------------------------------------
;功能：读取硬盘n个扇区
;---------------------------------------------------------------
rd_disk_m_16:
    mov esi, eax                    ; 备份eax
    mov di, cx                      ; 备份cx

;读写硬盘
;第1步：设置要读取的扇区数
    mov dx, 0x1f2                   ; Sector count
    mov al, cl                      ; cl = 0x1
    out dx, al                      ; 读取的扇区数

    mov eax, esi                    ; 恢复

;第2步：将LBA地址存入0x1f3-0x1f6， LBA地址是多少？？？->0x2
    ;LBA地址7-0位写入端口0x1f3
    mov dx, 0x1f3                   ; LBA low
    out dx, al                      ; 0x2

    ;LBA地址15-8位写入端口0x1f4
    mov cl, 8       ; 
    shr eax, cl     ; eax是多少? 0x2? 逻辑右移
    mov dx, 0x1f4
    out dx, al      ; 0x0

    ;LBA地址23-16位写入端口0x1f5
    shr eax, cl
    mov dx, 0x1f5
    out dx, al      ; 0x0
    
    shr eax, cl
    and al, 0x0f    ; 24-27位
    or al, 0xe0     ; 设置7-4位 1110， 第6位为1表示LBA模式, 0-3位：0000
    mov dx, 0x1f6
    out dx, al      ; 组合起来 LBA地址是0x2

;第3步：向0x1f7端口写入`读命令`：0x20
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al      ; al -> dx

;第4步：检测硬盘状态
.not_ready:
    ; 统一端口，写时表示写入命令字，读时表示读取硬盘状态
    nop     ; 停一停
    in al, dx           ; dx -> al
    and al, 0x88        ; 10001000，第7位代表是否正忙，第3位代表，数据准备好
    cmp al, 0x08        ; 00001000, 要得到这个，需要al=0xxx1000 
    jnz .not_ready      ; 还没准备好，跳回去继续等待

; 第5步：从0x1f0端口读数据
    mov ax, di          ;要读取的扇区数，一个扇区512字节, di = cx = 1(cx前面备份的)
    mov dx, 256         ;每次读入一个字(2字节)，所以这里是512/2=256
    mul dx              ;di * dx = 总共需要读取的次数
    mov cx, ax          ;结果存在ax,复制到cx，cx用来表示循环计数
    mov dx, 0x1f0       ; 设置数据寄存器地址

.go_on_read:        
    in ax, dx           ; dx -> ax
    mov [bx], ax        ; bx = 0x900
    add bx, 2           ; 加2，两个字节
    loop .go_on_read    ;循环次数由cx指定
    ret

    times 510-($-$$) db 0
    db 0x55, 0xaa

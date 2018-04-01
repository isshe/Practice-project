;------------------------loader and kernel------------------------------
LOADER_BASE_ADDR    equ 0x900
LOADER_START_SECTOR equ 0x2

; -----------------------GDT 描述符属性---------------------------------
DESC_G_4K           equ     1_00000000000000000000000b      ; 23个0, 0x80000
DESC_D_32           equ     1_0000000000000000000000b       ; 22个0，0x40000
DESC_L              equ     0_000000000000000000000b        ; 0x0
;64位代码标记，此处标记0即可，表示32位
DESC_AVL            equ     0_00000000000000000000b
;CPU不用此位，暂清为0
DESC_LIMIT_CODE2    equ     1111_0000000000000000b          ; 16个0, 0xf0000
DESC_LIMIT_DATA2    equ     DESC_LIMIT_CODE2
DESC_LIMIT_VIDEO2   equ     0000_000000000000000b           ; 后面15个0，0x0
DESC_P              equ     1_000000000000000b              ; 后面15个0，0x8000
DESC_DPL_0          equ     00_0000000000000b               ; 后面13个0，0x0
DESC_DPL_1          equ     01_0000000000000b
DESC_DPL_2          equ     10_0000000000000b 
DESC_DPL_3          equ     11_0000000000000b
DESC_S_CODE         equ     1_000000000000b                 ; 后面12个0, 0x1000
DESC_S_DATA         equ     DESC_S_CODE
DESC_S_sys          equ     0_000000000000b                 ; 后面12个0, 0x0
DESC_TYPE_CODE      equ     1000_00000000b                  ; 后面8个0， 0x800
DESC_TYPE_DATA      equ     0010_00000000b                  ; 0x200

DESC_CODE_HIGH4     equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00
DESC_DATA_HIGH4     equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
DESC_VIDEO_HIGH4    equ     (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x0b

;----------------------选择子属性-------------------------------------
RPL0                equ     00b 
RPL1                equ     01b 
RPL2                equ     10b 
RPL3                equ     11b     ; 以上四个"请求特权级"
TI_GDT              equ     000b    ; 全局描述符表
TI_LDT              equ     100b    ; 局部描述符表

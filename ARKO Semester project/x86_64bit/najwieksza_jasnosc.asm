ImgWidth        EQU 0
ImgHeight       EQU 4
ImgLineBytes    EQU 8
ImgData         EQU 16

prefsum_line    EQU 1284
prefsum_max     EQU 309444

BestRX          EQU 8
BestRY          EQU 4
BestRBrightness EQU 0
rectangle_color EQU 0x00DD3322

section .data
prefsum_buf: times prefsum_max db 0	; 321 x 241 x 4 (4 bytes are needed to acuratelly store the prefix sum)

section .text
global mark_greatest_brigthness

; Marks the brightest region of the bitmap with a red recrangle
; Arguments:
; rdi => ImgHeader
; rsi => width
; rdx => height
; Local variables: TODO: maybe this is unnecesary?
; [RBP-12] => Best rectangle struct
;   int x -> RBP-4 (struct + 8)
;   int y -> RBP-8 (sctuct + 4)
;   int max_brightness -> RBP-12 (struct + 0)
mark_greatest_brigthness:
    push RBP
    mov RBP, RSP
    sub RSP, 12 ; Reserve for locals

    mov r12, rdi ; Save ImgHeader
    mov r13, rsi ; Save width
    mov r14, rdx ; Save height
temp:
    lea RAX, [RBP-12] ; default values for best rectangle struct

    mov DWORD[RAX+BestRBrightness], -1
    mov DWORD[RAX+BestRX], -1
    mov DWORD[RAX+BestRY], -1

    mov rdi, r12 ; Load ImgHeader

    call fill_in_prefsum
    ; call find_the_spot
    ; call draw_rect

    ; add RSP, 16 ; Take off arguments
    add RSP, 12 ; Take off locals
    pop RBP
    ret
; ; Puts the pixel 0RGB into RAX
; ; Arguments on stack:
; ; [RBP+16] => ImgHeader
; ; [RBP+12] => X
; ; [RBP+8] => Y
; ; Returns:
; ; RAX => 0RGB at (x, y)
; get_pixel:
;     push RBP
;     mov RBP, RSP

;     mov ECX, DWORD [RBP+16] ; ECX -> imgheader | RAX -> address od the pixel
;     mov RAX, DWORD [ECX + ImgLineBytes]
;     mul DWORD [RBP+8] ; Y offset

;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12] ; X offset

;     add RAX, DWORD [ECX + ImgData]

;     xor ECX, ECX
;     mov cl, BYTE [RAX+2]
;     shl ECX, 8
;     mov cl, BYTE [RAX+1]
;     shl ECX, 8
;     mov cl, BYTE [RAX]

;     mov RAX, ECX

;     pop RBP
;     ret
; ; Sets a selected pixel to a given color
; ; Arguments on stack:
; ; [RBP+20] => 0RGB color
; ; [RBP+16] => ImgHeader
; ; [RBP+12] => X
; ; [RBP+8] => Y
; set_pixel:
;     push RBP
;     mov RBP, RSP
;     push ECX

;     mov ECX, DWORD [RBP+16] ; ECX -> imgheader | RAX -> address od the pixel
;     mov RAX, DWORD [ECX + ImgLineBytes]
;     mul DWORD [RBP+8] ; Y offset

;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12] ; X offset

;     add RAX, DWORD [ECX + ImgData]

;     mov cl, BYTE [RBP+22]
;     mov BYTE [RAX+2], cl
;     mov cl, BYTE [RBP+21]
;     mov BYTE [RAX+1], cl
;     mov cl, BYTE [RBP+20]
;     mov BYTE [RAX], cl

;     pop ECX
;     pop RBP
;     ret
; ; Calculates the 16-bit brightness
; ; Arguments on stack:
; ; [RBP+8] => 0RGB color
; ; Returns:
; ; AX => 16-bit brightness (upper half of RAX zero'ed out)
; ; Local variables:
; ; [RBP-4] => brightness
; calculate_brightness:
;     push RBP
;     mov RBP, RSP
;     sub RSP, 4

;     mov DWORD [RBP-4], 0 ; set brightness initially to zero
;     xor RAX, RAX ; RAX -> current color channel
;     mov AL, BYTE [RBP+8] ; Blue
;     mov ECX, 7
;     mul ECX
;     add DWORD [RBP-4], RAX

;     xor RAX, RAX
;     mov AL, BYTE [RBP+9] ; Green
;     mov ECX, 72
;     mul ECX
;     add DWORD [RBP-4], RAX

;     xor RAX, RAX
;     mov AL, BYTE [RBP+10] ; Red
;     mov ECX, 21
;     mul ECX
;     add DWORD [RBP-4], RAX

;     mov RAX, [RBP-4]

;     add RSP, 4
;     pop RBP
;     ret
; ; Returns value at given X, Y of the prefsum table
; ; Arguments on stack:
; ; [RBP+12] => X
; ; [RBP+8] => Y
; ; Returns:
; ; RAX => 32-bit value of prefsum
; get_prefsum:
;     push RBP
;     mov RBP, RSP

;     mov RAX, DWORD[RBP+8]
;     mov EDX, prefsum_line
;     mul EDX ; Y offset

;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12] ; X offset

;     mov RAX, DWORD[RAX+prefsum_buf]

;     pop RBP
;     ret
; ; Sets the value at (x, y) to the contents of RAX
; ; Arguments on stack:
; ; [RBP+12] => X
; ; [RBP+8] => Y
; ; Register arguments:
; ; RAX => 32-bit value to set the cell to
; set_prefsum:
;     push RBP
;     mov RBP, RSP

;     push RAX ; Save the value

;     mov RAX, DWORD[RBP+8]
;     mov EDX, prefsum_line
;     mul EDX ; Y offset

;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12]
;     add RAX, DWORD [RBP+12] ; X offset

;     pop EDX ; restore the value
;     mov DWORD[RAX+prefsum_buf], EDX

;     pop RBP
;     ret
; Fills in the prefsum table with brightnesses
; Arguments on stack:
; [RBP+8] => ImgHeader
; Local variables:
; [RBP-4] => temporary prefsum value
; [RBP-8] => ImgHeader copy
; [RBP-12] => X coordinate
; [RBP-16] => Y coordinate
fill_in_prefsum:
    push RBP
    mov RBP, RSP
    sub RSP, 16

    mov RAX, DWORD [RBP+8]
    mov DWORD [RBP-8], RAX
    mov RAX, DWORD [RAX+ImgHeight]
    mov DWORD [RBP-16], RAX
    sub DWORD [RBP-16], 1 ; Y is set
fip_loop_x_reset:
    mov RAX, DWORD [RBP+8]
    mov RAX, DWORD [RAX+ImgWidth]
    mov DWORD [RBP-12], RAX
    sub DWORD [RBP-12], 1 ; X is set
fip_loop_count_begin:
    call get_pixel
    push RAX
    call calculate_brightness
    add RSP, 4
    ; RAX -> brightness of pixel (x, y)
    ; Filling the prefsum table
    mov DWORD[RBP-4], RAX

    add DWORD[RBP-12], 1 ; X+=1
    call get_prefsum
    add DWORD[RBP-4], RAX

    add DWORD[RBP-16], 1 ; Y+=1
    call get_prefsum
    sub DWORD[RBP-4], RAX

    sub DWORD[RBP-12], 1 ; X-=1
    call get_prefsum
    add DWORD[RBP-4], RAX

    sub DWORD[RBP-16], 1 ; Y-=1
    mov RAX, DWORD[RBP-4]
    call set_prefsum

    dec DWORD[RBP - 12]
    jge fip_loop_count_begin
    dec DWORD[RBP - 16]
    jge fip_loop_x_reset

    add RSP, 16
    pop RBP
    ret
; ; Finds the bottom left corner of the brightest rectangle
; ; Arguments on stack:
; ; [RBP+20] => Best rectangle struct address
; ; [RBP+16] => height
; ; [RBP+12] => width
; ; [RBP+8] => ImgHeader
; ; Local variables:
; ; [RBP-4] => temp brightness
; ; [RBP-8] => ImgHeader copy
; ; [RBP-12] => X coordinate
; ; [RBP-16] => Y coordinate
; find_the_spot:
;     push RBP
;     mov RBP, RSP
;     sub RSP, 16

;     mov RAX, DWORD [RBP+8]
;     mov DWORD [RBP-8], RAX ; save image header copy

;     mov RAX, DWORD [RBP+8]
;     mov DWORD [RBP-8], RAX
;     mov RAX, DWORD [RAX+ImgHeight]
;     sub RAX, DWORD [RBP+16] ; Subtract height to limit search
;     mov DWORD [RBP-16], RAX ; Y is set
; fts_loop_x_reset:
;     mov RAX, DWORD [RBP+8]
;     mov RAX, DWORD [RAX+ImgWidth]
;     sub RAX, DWORD [RBP+12] ; Subtract width to limit search
;     mov DWORD [RBP-12], RAX ; X is set
; fts_loop_search_begin:
;     ; Get the total brightness
;     call get_prefsum
;     mov DWORD[RBP-4], RAX ; The cell at (x, y)

;     mov ECX, DWORD [RBP+12]
;     add DWORD[RBP-12], ECX ; X+=width
;     call get_prefsum
;     sub DWORD[RBP-4], RAX ; Cell to the right

;     mov ECX, DWORD [RBP+16]
;     add DWORD[RBP-16], ECX ; Y+=height
;     call get_prefsum
;     add DWORD[RBP-4], RAX ; Cell on the diagonal

;     mov ECX, DWORD [RBP+12]
;     sub DWORD[RBP-12], ECX ; X-=width
;     call get_prefsum
;     sub DWORD[RBP-4], RAX ; Cell above

;     mov ECX, DWORD [RBP+16]
;     sub DWORD[RBP-16], ECX ; Y-=height

;     mov ECX, [RBP+20] ; Best rect struct
;     mov RAX, [ECX+BestRBrightness]
;     cmp [RBP-4], RAX

;     jle fts_no_improvement
;     ; A new better rectangle was found! Store in in the structure
;     mov RAX, [RBP-4]
;     mov [ECX+BestRBrightness], RAX
;     mov RAX, [RBP-12]
;     mov [ECX+BestRX], RAX
;     mov RAX, [RBP-16]
;     mov [ECX+BestRY], RAX
; fts_no_improvement:
;     dec DWORD[RBP - 12]
;     jge fts_loop_search_begin
;     dec DWORD[RBP - 16]
;     jge fts_loop_x_reset

;     add RSP, 16
;     pop RBP
;     ret
; ; Draws a rectangle at a given position
; ; Arguments on stack:
; ; [RBP+20] => Best rectangle struct address
; ; [RBP+16] => height
; ; [RBP+12] => width
; ; [RBP+8] => ImgHeader
; draw_rect:
;     push RBP
;     mov RBP, RSP

;     mov ECX, [RBP+20]
;     ; Put arguments on stack:
;     ;   RSP+12 -> color
;     ;   RSP+8 -> ImgHeader
;     ;   RSP+4 -> X
;     ;   RSP+0 -> Y
;     push DWORD rectangle_color
;     push DWORD [RBP+8]
;     push DWORD [ECX+BestRX]
;     push DWORD [ECX+BestRY]

;     mov ECX, DWORD [RBP+16]
; drt_horizontal_loop: ; Draw horizontal lines
;     call set_pixel

;     mov RAX, DWORD [RBP+12]
;     add DWORD[RSP+4], RAX
;     sub DWORD[RSP+4], 1
;     call set_pixel  ; Draw the other pixel offsetted by width

;     mov RAX, DWORD [RBP+12]
;     sub DWORD[RSP+4], RAX
;     add DWORD[RSP+4], 1 ; Come back to the original position

;     inc DWORD[RSP+0] ; increase Y coordinate
;     dec ECX
;     jg drt_horizontal_loop

;     mov ECX, DWORD [RBP+12]
;     dec DWORD[RSP+0]
; drt_vertical_loop: ; Draw horizontal lines
;     call set_pixel

;     mov RAX, DWORD [RBP+16]
;     sub DWORD[RSP+0], RAX
;     add DWORD[RSP+0], 1
;     call set_pixel  ; Draw the other pixel offsetted

;     mov RAX, DWORD [RBP+16]
;     add DWORD[RSP+0], RAX
;     sub DWORD[RSP+0], 1 ; Come back to the original position

;     inc DWORD[RSP+4] ; increase X coordinate
;     dec ECX
;     jg drt_vertical_loop

;     add RSP, 16
;     pop RBP
;     ret
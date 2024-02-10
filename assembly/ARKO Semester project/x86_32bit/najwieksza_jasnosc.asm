ImgWidth        EQU 0
ImgHeight       EQU 4
ImgLineBytes    EQU 8
ImgData         EQU 12

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
; Arguments on stack:
; [ebp+16] => height
; [ebp+12] => width
; [ebp+8] => ImgHeader
; Local variables:
; [ebp-12] => Best rectangle struct
;   int x -> ebp-4 (struct + 8)
;   int y -> ebp-8 (sctuct + 4)
;   int max_brightness -> ebp-12 (struct + 0)
mark_greatest_brigthness:
    push ebp
    mov ebp, esp
    sub esp, 12 ; Reserve for locals

    lea eax, [EBP-12] ; default values for best rectangle struct

    mov DWORD[eax+BestRBrightness], -1
    mov DWORD[eax+BestRX], -1
    mov DWORD[eax+BestRY], -1

    push DWORD eax      ; push struct
    push DWORD [ebp+16] ; push height
    push DWORD [ebp+12] ; push width
    push DWORD [ebp+8]  ; push img header
    call fill_in_prefsum
    call find_the_spot
    call draw_rect

    add esp, 16 ; Take off arguments
    add esp, 12 ; Take off locals
    pop ebp
    ret
; Puts the pixel 0RGB into EAX
; Arguments on stack:
; [ebp+16] => ImgHeader
; [ebp+12] => X
; [ebp+8] => Y
; Returns:
; EAX => 0RGB at (x, y)
get_pixel:
    push ebp
    mov ebp, esp

    mov ECX, DWORD [ebp+16] ; ECX -> imgheader | EAX -> address od the pixel
    mov EAX, DWORD [ECX + ImgLineBytes]
    mul DWORD [ebp+8] ; Y offset

    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12] ; X offset

    add EAX, DWORD [ECX + ImgData]

    xor ECX, ECX
    mov cl, BYTE [EAX+2]
    shl ECX, 8
    mov cl, BYTE [EAX+1]
    shl ECX, 8
    mov cl, BYTE [EAX]

    mov EAX, ECX

    pop ebp
    ret
; Sets a selected pixel to a given color
; Arguments on stack:
; [ebp+20] => 0RGB color
; [ebp+16] => ImgHeader
; [ebp+12] => X
; [ebp+8] => Y
set_pixel:
    push ebp
    mov ebp, esp
    push ECX

    mov ECX, DWORD [ebp+16] ; ECX -> imgheader | EAX -> address od the pixel
    mov EAX, DWORD [ECX + ImgLineBytes]
    mul DWORD [ebp+8] ; Y offset

    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12] ; X offset

    add EAX, DWORD [ECX + ImgData]

    mov cl, BYTE [ebp+22]
    mov BYTE [EAX+2], cl
    mov cl, BYTE [ebp+21]
    mov BYTE [EAX+1], cl
    mov cl, BYTE [ebp+20]
    mov BYTE [EAX], cl

    pop ECX
    pop ebp
    ret
; Calculates the 16-bit brightness
; Arguments on stack:
; [ebp+8] => 0RGB color
; Returns:
; AX => 16-bit brightness (upper half of EAX zero'ed out)
; Local variables:
; [ebp-4] => brightness
calculate_brightness:
    push ebp
    mov ebp, esp
    sub esp, 4

    mov DWORD [ebp-4], 0 ; set brightness initially to zero
    xor EAX, EAX ; EAX -> current color channel
    mov AL, BYTE [ebp+8] ; Blue
    mov ECX, 7
    mul ECX
    add DWORD [ebp-4], EAX

    xor EAX, EAX
    mov AL, BYTE [ebp+9] ; Green
    mov ECX, 72
    mul ECX
    add DWORD [ebp-4], EAX

    xor EAX, EAX
    mov AL, BYTE [ebp+10] ; Red
    mov ECX, 21
    mul ECX
    add DWORD [ebp-4], EAX

    mov EAX, [ebp-4]

    add esp, 4
    pop ebp
    ret
; Returns value at given X, Y of the prefsum table
; Arguments on stack:
; [ebp+12] => X
; [ebp+8] => Y
; Returns:
; EAX => 32-bit value of prefsum
get_prefsum:
    push ebp
    mov ebp, esp

    mov EAX, DWORD[ebp+8]
    mov EDX, prefsum_line
    mul EDX ; Y offset

    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12] ; X offset

    mov EAX, DWORD[EAX+prefsum_buf]

    pop ebp
    ret
; Sets the value at (x, y) to the contents of EAX
; Arguments on stack:
; [ebp+12] => X
; [ebp+8] => Y
; Register arguments:
; EAX => 32-bit value to set the cell to
set_prefsum:
    push ebp
    mov ebp, esp

    push EAX ; Save the value

    mov EAX, DWORD[ebp+8]
    mov EDX, prefsum_line
    mul EDX ; Y offset

    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12]
    add EAX, DWORD [ebp+12] ; X offset

    pop EDX ; restore the value
    mov DWORD[EAX+prefsum_buf], EDX

    pop ebp
    ret
; Fills in the prefsum table with brightnesses
; Arguments on stack:
; [ebp+8] => ImgHeader
; Local variables:
; [ebp-4] => temporary prefsum value
; [ebp-8] => ImgHeader copy
; [ebp-12] => X coordinate
; [ebp-16] => Y coordinate
fill_in_prefsum:
    push ebp
    mov ebp, esp
    sub esp, 16

    mov EAX, DWORD [ebp+8]
    mov DWORD [ebp-8], EAX
    mov EAX, DWORD [EAX+ImgHeight]
    mov DWORD [ebp-16], EAX
    sub DWORD [ebp-16], 1 ; Y is set
fip_loop_x_reset:
    mov EAX, DWORD [ebp+8]
    mov EAX, DWORD [EAX+ImgWidth]
    mov DWORD [ebp-12], EAX
    sub DWORD [ebp-12], 1 ; X is set
fip_loop_count_begin:
    call get_pixel
    push EAX
    call calculate_brightness
    add esp, 4
    ; EAX -> brightness of pixel (x, y)
    ; Filling the prefsum table
    mov DWORD[ebp-4], EAX

    add DWORD[ebp-12], 1 ; X+=1
    call get_prefsum
    add DWORD[ebp-4], EAX

    add DWORD[ebp-16], 1 ; Y+=1
    call get_prefsum
    sub DWORD[ebp-4], EAX

    sub DWORD[ebp-12], 1 ; X-=1
    call get_prefsum
    add DWORD[ebp-4], EAX

    sub DWORD[ebp-16], 1 ; Y-=1
    mov EAX, DWORD[ebp-4]
    call set_prefsum

    dec DWORD[ebp - 12]
    jge fip_loop_count_begin
    dec DWORD[ebp - 16]
    jge fip_loop_x_reset

    add esp, 16
    pop ebp
    ret
; Finds the bottom left corner of the brightest rectangle
; Arguments on stack:
; [ebp+20] => Best rectangle struct address
; [ebp+16] => height
; [ebp+12] => width
; [ebp+8] => ImgHeader
; Local variables:
; [ebp-4] => temp brightness
; [ebp-8] => ImgHeader copy
; [ebp-12] => X coordinate
; [ebp-16] => Y coordinate
find_the_spot:
    push ebp
    mov ebp, esp
    sub esp, 16

    mov EAX, DWORD [ebp+8]
    mov DWORD [ebp-8], EAX ; save image header copy

    mov EAX, DWORD [ebp+8]
    mov DWORD [ebp-8], EAX
    mov EAX, DWORD [EAX+ImgHeight]
    sub EAX, DWORD [ebp+16] ; Subtract height to limit search
    mov DWORD [ebp-16], EAX ; Y is set
fts_loop_x_reset:
    mov EAX, DWORD [ebp+8]
    mov EAX, DWORD [EAX+ImgWidth]
    sub EAX, DWORD [ebp+12] ; Subtract width to limit search
    mov DWORD [ebp-12], EAX ; X is set
fts_loop_search_begin:
    ; Get the total brightness
    call get_prefsum
    mov DWORD[ebp-4], EAX ; The cell at (x, y)

    mov ECX, DWORD [ebp+12]
    add DWORD[ebp-12], ECX ; X+=width
    call get_prefsum
    sub DWORD[ebp-4], EAX ; Cell to the right

    mov ECX, DWORD [ebp+16]
    add DWORD[ebp-16], ECX ; Y+=height
    call get_prefsum
    add DWORD[ebp-4], EAX ; Cell on the diagonal

    mov ECX, DWORD [ebp+12]
    sub DWORD[ebp-12], ECX ; X-=width
    call get_prefsum
    sub DWORD[ebp-4], EAX ; Cell above

    mov ECX, DWORD [ebp+16]
    sub DWORD[ebp-16], ECX ; Y-=height

    mov ECX, [ebp+20] ; Best rect struct
    mov EAX, [ECX+BestRBrightness]
    cmp [ebp-4], EAX

    jle fts_no_improvement
    ; A new better rectangle was found! Store in in the structure
    mov EAX, [ebp-4]
    mov [ECX+BestRBrightness], EAX
    mov EAX, [ebp-12]
    mov [ECX+BestRX], EAX
    mov EAX, [ebp-16]
    mov [ECX+BestRY], EAX
fts_no_improvement:
    dec DWORD[ebp - 12]
    jge fts_loop_search_begin
    dec DWORD[ebp - 16]
    jge fts_loop_x_reset

    add esp, 16
    pop ebp
    ret
; Draws a rectangle at a given position
; Arguments on stack:
; [ebp+20] => Best rectangle struct address
; [ebp+16] => height
; [ebp+12] => width
; [ebp+8] => ImgHeader
draw_rect:
    push ebp
    mov ebp, esp

    mov ECX, [ebp+20]
    ; Put arguments on stack:
    ;   esp+12 -> color
    ;   esp+8 -> ImgHeader
    ;   esp+4 -> X
    ;   esp+0 -> Y
    push DWORD rectangle_color
    push DWORD [ebp+8]
    push DWORD [ECX+BestRX]
    push DWORD [ECX+BestRY]

    mov ECX, DWORD [ebp+16]
drt_horizontal_loop: ; Draw horizontal lines
    call set_pixel

    mov EAX, DWORD [ebp+12]
    add DWORD[esp+4], EAX
    sub DWORD[esp+4], 1
    call set_pixel  ; Draw the other pixel offsetted by width

    mov EAX, DWORD [ebp+12]
    sub DWORD[esp+4], EAX
    add DWORD[esp+4], 1 ; Come back to the original position

    inc DWORD[esp+0] ; increase Y coordinate
    dec ECX
    jg drt_horizontal_loop

    mov ECX, DWORD [ebp+12]
    dec DWORD[esp+0]
drt_vertical_loop: ; Draw horizontal lines
    call set_pixel

    mov EAX, DWORD [ebp+16]
    sub DWORD[esp+0], EAX
    add DWORD[esp+0], 1
    call set_pixel  ; Draw the other pixel offsetted

    mov EAX, DWORD [ebp+16]
    add DWORD[esp+0], EAX
    sub DWORD[esp+0], 1 ; Come back to the original position

    inc DWORD[esp+4] ; increase X coordinate
    dec ECX
    jg drt_vertical_loop

    add esp, 16
    pop ebp
    ret
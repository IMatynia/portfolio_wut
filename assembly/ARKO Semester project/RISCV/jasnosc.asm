#-------------------------------------------------------------------------------
#author: Igor Matynia
#date : 2022.05.27
#description : The program marks with a red rectangle a region of defined dimentions with the greatest average brightness
#-------------------------------------------------------------------------------

.eqv ImgInfo_fname	0
.eqv ImgInfo_hdrdat 	4
.eqv ImgInfo_imdat	8
.eqv ImgInfo_width	12
.eqv ImgInfo_height	16
.eqv ImgInfo_lbytes	20

.eqv MAX_IMG_SIZE 	230400 	# 320 x 240 x 3 (piksele)
.eqv MAX_TABLE_SIZE	309444	# 321 x 241 x 4 (4 bytes are needed to acuratelly store the prefix sum)
.eqv TABLE_ROW		1284

# more information about bmp format: https://en.wikipedia.org/wiki/BMP_file_format
.eqv BMPHeader_Size 54
.eqv BMPHeader_width 18
.eqv BMPHeader_height 22

.eqv system_OpenFile	1024
.eqv system_ReadFile	63
.eqv system_WriteFile	64
.eqv system_CloseFile	57
.eqv system_ReadInt		5
.eqv system_PrintString	4
.eqv system_PrintInt	1
.eqv system_PrintHex	34

.macro endl
	la, a0, endl_std
	li a7, system_PrintString
	ecall
.end_macro

.macro printRdec %reg
	mv a0, %reg
	li a7, system_PrintInt
	ecall
	endl
.end_macro

.macro printRhex %reg
	mv a0, %reg
	li a7, system_PrintHex
	ecall
	endl
.end_macro

.macro printStr %addr
	la a0, %addr
	li a7, system_PrintString
	ecall
.end_macro

.macro printLine %addr
	printStr %addr
	endl
.end_macro

.macro readIntToR %reg
	li a7, system_ReadInt
	ecall
	mv %reg, a0
.end_macro

	.data
imgInfo: .space	24	# deskryptor obrazu

	.align 2		# wyrównanie do granicy słowa
dummy:		.space 2
bmpHeader:	.space	BMPHeader_Size

	.align 2
imgData: 	.space	MAX_IMG_SIZE

ifname:	.asciz "source.bmp"
ofname: .asciz "result.bmp"

	.align 3
brightSum: 	.space MAX_TABLE_SIZE	# a 321 x 241 table to store prefix sum of brightness
prompt1:	.asciz "Type desired X and Y scale of the rectangle:\n"
error_prompt:	.asciz "Given size is invalid!\n"
endl_std:	.asciz	"\n"
	.text
main:
	# wypełnienie deskryptora obrazu
	la a0, imgInfo
	la t0, ifname
	sw t0, ImgInfo_fname(a0)
	la t0, bmpHeader
	sw t0, ImgInfo_hdrdat(a0)
	la t0, imgData
	sw t0, ImgInfo_imdat(a0)
	jal	read_bmp
	bnez a0, main_failure
	# bmp is in the memory

	printStr prompt1
	readIntToR a1
	readIntToR a2
	la a0, imgInfo
	jal validate_input
	jal mark_greatest_brigthness

	# save the modified BMP:
	la a0, imgInfo
	la t0, ofname
	sw t0, ImgInfo_fname(a0)
	jal save_bmp
main_failure:
	li a7, 10
	ecall

#============================================================================
# read_bmp:
#	reads the content of a bmp file into memory
# arguments:
#	a0 - address of image descriptor structure
#		input filename pointer, header and image buffers should be set
# return value:
#	a0 - 0 if successful, error code in other cases
read_bmp:
	mv t0, a0	# preserve imgInfo structure pointer

#open file
	li a7, system_OpenFile
    lw a0, ImgInfo_fname(t0)	#file name
    li a1, 0					#flags: 0-read file
    ecall

	blt a0, zero, rb_error
	mv t1, a0					# save file handle for the future

#read header
	li a7, system_ReadFile
	lw a1, ImgInfo_hdrdat(t0)
	li a2, BMPHeader_Size
	ecall

#extract image information from header
	lw a0, BMPHeader_width(a1)
	sw a0, ImgInfo_width(t0)

	# compute line size in bytes - bmp line has to be multiple of 4
	add a2, a0, a0
	add a0, a2, a0	# pixelbytes = width * 3
	addi a0, a0, 3
	srai a0, a0, 2
	slli a0, a0, 2	# linebytes = ((pixelbytes + 3) / 4 ) * 4
	sw a0, ImgInfo_lbytes(t0)

	lw a0, BMPHeader_height(a1)
	sw a0, ImgInfo_height(t0)

#read image data
	li a7, system_ReadFile
	mv a0, t1
	lw a1, ImgInfo_imdat(t0)
	li a2, MAX_IMG_SIZE
	ecall

#close file
	li a7, system_CloseFile
	mv a0, t1
    ecall

	mv a0, zero
	jr ra

rb_error:
	li a0, 1	# error opening file
	jr ra

# ============================================================================
# save_bmp - saves bmp file stored in memory to a file
# arguments:
#	a0 - address of ImgInfo structure containing description of the image`
# return value:
#	a0 - zero if successful, error code in other cases

save_bmp:
	mv t0, a0	# preserve imgInfo structure pointer

#open file
	li a7, system_OpenFile
    lw a0, ImgInfo_fname(t0)	#file name
    li a1, 1					#flags: 1-write file
    ecall

	blt a0, zero, wb_error
	mv t1, a0					# save file handle for the future

#write header
	li a7, system_WriteFile
	lw a1, ImgInfo_hdrdat(t0)
	li a2, BMPHeader_Size
	ecall

#write image data
	li a7, system_WriteFile
	mv a0, t1
	# compute image size (linebytes * height)
	lw a2, ImgInfo_lbytes(t0)
	lw a1, ImgInfo_height(t0)
	mul a2, a2, a1
	lw a1, ImgInfo_imdat(t0)
	ecall

#close file
	li a7, system_CloseFile
	mv a0, t1
    ecall

	mv a0, zero
	jr ra

wb_error:
	li a0, 2 # error writing file
	jr ra


# ============================================================================
# set_pixel - sets the color of specified pixel
#arguments:
#	a0 - address of ImgInfo image descriptor
#	a1 - x coordinate
#	a2 - y coordinate - (0,0) - bottom left corner
#	a3 - 0RGB - pixel color
#return value: none
#remarks - a0, a1, a2 values are left unchanged

set_pixel:
	lw t1, ImgInfo_lbytes(a0)
	mul t1, t1, a2  # t1 = y * linebytes
	add t0, a1, a1
	add t0, t0, a1 	# t0 = x * 3
	add t0, t0, t1  # t0 is offset of the pixel

	lw t1, ImgInfo_imdat(a0) # address of image data
	add t0, t0, t1 	# t0 is address of the pixel

	#set new color
	sb   a3,(t0)		#store B
	srli a3, a3, 8
	sb   a3, 1(t0)		#store G
	srli a3, a3, 8
	sb   a3, 2(t0)		#store R

	jr ra

# ============================================================================
# get_pixel- returns color of specified pixel
#arguments:
#	a0 - address of ImgInfo image descriptor
#	a1 - x coordinate
#	a2 - y coordinate - (0,0) - bottom left corner
#return value:
#	a0 - 0RGB - pixel color
#remarks: a1, a2 are preserved

get_pixel:
	lw t1, ImgInfo_lbytes(a0)
	mul t1, t1, a2  # t1 = y * linebytes
	add t0, a1, a1
	add t0, t0, a1 	# t0 = x * 3
	add t0, t0, t1  # t0 is offset of the pixel

	lw t1, ImgInfo_imdat(a0) # address of image data
	add t0, t0, t1 	# t0 is address of the pixel

	#get color
	lbu a0,(t0)		#load B
	lbu t1,1(t0)		#load G
	slli t1,t1,8
	or a0, a0, t1
	lbu t1,2(t0)		#load R
    slli t1,t1,16
	or a0, a0, t1

	jr ra
# ============================================================================

# ============================================================================
# mark_greatest_brigthness - puts the brigthtest region of the image in a red box
#arguments:
#	a0 - address of ImgInfo image descriptor
#	a1 - x scale of the rectangle
#	a2 - y scale of the rectangle
#return value:
#	none

mark_greatest_brigthness:
	addi sp, sp, -20
	sw ra, 16(sp)		#push ra
	sw s3, 12(sp)		#push s3
	sw s2, 8(sp)		#push s2
	sw s1, 4(sp)		#push s1
	sw s0, 0(sp)		#push s0

	mv s0, a0 		#preserve imgInfo for further use
	mv s1, a1		#rect x
	mv s2, a2		#rect y

	mv a0, s0
	jal fill_in_prefsum

	mv a0, s0
	mv a1, s1
	mv a2, s2
	jal find_the_spot

	mv a0, s0
	mv a3, s1
	mv a4, s2
	jal draw_a_box

	lw ra, 16(sp)		#push ra
	lw s3, 12(sp)		#push s3
	lw s2, 8(sp)		#push s3
	lw s1, 4(sp)		#push s1
	lw s0, 0(sp)		#push s0
	addi sp, sp, 16
	jr ra

# ============================================================================
# draw_a_box - draws the box onto the bitmap
#arguments:
#	a0 - address of ImgInfo image descriptor
# 	a1 - X corner
# 	a2 - Y corner
#	a3 - X scale
#	a4 - Y scale
#return value:
#	none
.eqv MarkerColor 0x00FF0000

draw_a_box:
	addi sp, sp, -20
	sw ra, 16(sp)		#push ra
	sw s3, 12(sp)		#push s3
	sw s2, 8(sp)		#push s2
	sw s1, 4(sp)		#push s1
	sw s0, 0(sp)		#push s0

	mv s0, a0 		#preserve imgInfo for further use
	mv s1, a3		# save box scales
	mv s2, a4
	addi s1, s1, -1
	addi s2, s2, -1

	# Horizontal lines
	mv s3, s1
dab_hotrizontal_loop:
	mv a0, s0
	li a3, MarkerColor
	jal set_pixel

	add a2, a2, s2
	mv a0, s0
	li a3, MarkerColor
	jal set_pixel
	sub a2, a2, s2

	addi a1, a1, 1
	addi s3, s3, -1
	bgt s3, zero, dab_hotrizontal_loop

	# Vertical lines
	mv s3, s2
dab_vertical_loop:
	mv a0, s0
	li a3, MarkerColor
	jal set_pixel

	sub a1, a1, s1
	mv a0, s0
	li a3, MarkerColor
	jal set_pixel
	add a1, a1, s1

	addi a2, a2, 1
	addi s3, s3, -1
	bgt s3, zero, dab_vertical_loop

	mv a0, s0
	li a3, MarkerColor
	jal set_pixel # one last dot

	lw ra, 16(sp)		#pop ra
	sw s3, 12(sp)		#pop s3
	lw s2, 8(sp)		#pop s2
	lw s1, 4(sp)		#pop s1
	lw s0, 0(sp)		#pop s0
	addi sp, sp, 20
	jr ra

# ============================================================================
# find_the_spot - finds a corner of the best brighness
#arguments:
#	a0 - address of ImgInfo image descriptor
# 	a1 - x scale of rectangle
# 	a2 - y scale of rectangle
#return value:
#	a1 - x pos of the rectangle
#	a2 - y pos of the rectangle

find_the_spot:
	addi sp, sp, -24
	sw ra, 20(sp)		#push ra
	sw s4, 16(sp)		#push s4
	sw s3, 12(sp)		#push s3
	sw s2, 8(sp)		#push s2
	sw s1, 4(sp)		#push s1
	sw s0, 0(sp)		#push s0

	mv s0, a0 		#preserve imgInfo for further use
	mv s1, a1		# save rect X scale
	mv s2, a2		# save rect Y scale

	xor s4, s4, s4		# store max brightness for comparions

# Iterate through the image from (width-X_scale, height-Y_scale) -> (0, 0)
	lw a2, ImgInfo_height(s0)
	sub a2, a2, s2	# Offset to fit the rectangle

fts_loop_line_1:
	lw a1, ImgInfo_width(s0)
	sub a1, a1, s1	# Offset to fit the rectangle

fts_loop_find_max_prefsum_beign:
	jal get_prefsum # a0 -> prefsum brigthness at (a1, a2)
	mv s3, a0 # s3 -> total brightness in this rectangle
	add a2, a2, s2
	jal get_prefsum
	sub s3, s3, a0 # a0 -> prefsum of upper bound
	add a1, a1, s1
	jal get_prefsum
	add s3, s3, a0 # a0 -> prefsum of diagonal
	sub a2, a2, s2
	jal get_prefsum
	sub s3, s3, a0 # a0 -> prefsum of rightmost bound

	sub a1, a1, s1 # Return to original position (a1, a2)

	ble s3, s4, fts_no_improvement # if s3 <= s4 then fts_no_improvement
	mv s4, s3
	sw a1, -4(sp)	# Store X of maximum
	sw a2, -8(sp)	# Store Y of maximum
fts_no_improvement:
	addi a1, a1, -1
	bge a1, zero, fts_loop_find_max_prefsum_beign
	addi a2, a2, -1
	bge a2, zero, fts_loop_line_1

	# Load the X;Y of the best rectangle from memory
	lw a1, -4(sp)	# get X of maximum
	lw a2, -8(sp)	# get Y of maximum

	lw ra, 20(sp)		#pop ra
	lw s4, 16(sp)		#pop s4
	lw s3, 12(sp)		#pop s3
	lw s2, 8(sp)		#pop s2
	lw s1, 4(sp)		#pop s1
	lw s0, 0(sp)		#pop s0
	addi sp, sp, 24
	jr ra

# ============================================================================
# fill_in_prefsum - calculates the prefsum table
#arguments:
#	a0 - address of ImgInfo image descriptor
#return value:
#	none

fill_in_prefsum:
	addi sp, sp, -12
	sw ra, 8(sp)		#push ra
	sw s3, 4(sp)		#push s3
	sw s0, 0(sp)		#push s0

	mv s0, a0 		#preserve imgInfo for further use

# Iterate through the image
	lw a2, ImgInfo_height(s0)
	addi a2, a2, -1

fip_loop_line_1:
	lw a1, ImgInfo_width(s0)
	addi a1, a1, -1

fip_loop_count_prefsum_beign:
	mv a0, s0
	jal get_pixel
	jal calculate_brightness
	# a0 -> brightness of pixel at (a1, a2)
	mv s3, a0 # s3 -> new value for the prefsum table at (a1, a2)

	addi a1, a1, 1
	jal get_prefsum
	add s3, s3, a0	# Cell to the right
	addi a2, a2, 1
	addi a1, a1, -1
	jal get_prefsum
	add s3, s3, a0	# Cell to the bottom
	addi a1, a1, 1
	jal get_prefsum
	sub s3, s3, a0	# Cell on a diagonal

	addi a1, a1, -1
	addi, a2, a2, -1
	mv a0, s3
	jal set_prefsum

	addi a1, a1, -1
	bge a1, zero, fip_loop_count_prefsum_beign
	addi a2, a2, -1
	bge a2, zero, fip_loop_line_1

	lw ra, 8(sp)		#pop ra
	lw s3, 4(sp)		#pop s3
	lw s0, 0(sp)		#pop s0
	addi sp, sp, 12
	jr ra

# ============================================================================
# calculate_brightness - puts the brigthtest region of the image in a red box
#arguments:
#	a0 - 0RGB color
#return value:
#	a0 - 16-bit of precise brightness

calculate_brightness:
	xor t1, t1, t1 	# Brightness

	mv t0, a0	# Red
	slli t0, t0, 8
	srli t0, t0, 24
	li t2, 21
	mul t0, t0, t2
	add t1, t1, t0

	mv t0, a0	# Green
	slli t0, t0, 16
	srli t0, t0, 24
	li t2, 72
	mul t0, t0, t2
	add t1, t1, t0

	mv t0, a0	# Blue
	slli t0, t0, 24
	srli t0, t0, 24
	li t2, 7
	mul t0, t0, t2
	add t1, t1, t0

	mv a0, t1
	jr ra

# ============================================================================
# get_prefsum - get a value from the prefsum table
#arguments:
#	a1 - X
#	a2 - Y
#return value:
#	a0 - 32-bit prefsum value
#remarks - a1, a2 values are left unchanged

get_prefsum:
	li t0, 4
	mv t1, a1
	mv t2, a2
	mul t1, t1, t0 		# 4 bytes per element

	li t3, TABLE_ROW	# t3 -> memory address
	mul t3, t3, t2
	add t3, t3, t1
	la t0, brightSum
	add t3, t3, t0
	lw a0, (t3)
	jr ra

# ============================================================================
# set_prefsum - get a value from the prefsum table
#arguments:
# 	a0 - 32-bit value to store
#	a1 - X
#	a2 - Y
#return value:
#	none
#remarks - a1, a2 values are left unchanged

set_prefsum:
	li t0, 4
	mv t1, a1
	mv t2, a2
	mul t1, t1, t0 		# 4 bytes per element

	li t3, TABLE_ROW	# t3 -> memory address
	mul t3, t3, t2
	add t3, t3, t1
	la t0, brightSum
	add t3, t3, t0
	sw a0, (t3)
	jr ra

# ============================================================================
# validate_input - validates if the rectangle given by the user fits on the image
#arguments:
# 	a0 - address of ImgInfo image descriptor
#	a1 - X scale
#	a2 - Y scale
#return value:
#	Prints a message to the console and exits program
#remarks - a0, a1, a2 values are left unchanged

validate_input:
	lw t0, ImgInfo_height(a0)
	blt t0, a2, vi_error # if t0 < a2 then vi_error
	lw t0, ImgInfo_width(a0)
	blt t0, a1, vi_error # if t0 < a1 then vi_error
	ble a2, zero, vi_error
	ble a1, zero, vi_error
	jr ra
vi_error:
	printStr error_prompt
	j main_failure

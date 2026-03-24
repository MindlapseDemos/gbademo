	.section .iwram
	.arm
	.cpu arm7tdmi

	@ r0: vector ptr
	@ r1: matrix ptr
	.globl g3d_xform
g3d_xform:
	stmfd sp!, {r4-r10}

	@ load vector in r4-r7
	@ load matrix rows in r8,r9,r10,r12
	@ accumulate in r2 (low)/r3 (high), 
	ldmia r0, {r4-r7}

	.macro mulrow
	ldmia r1!, {r8-r10,r12}
	smull r2,r3, r8,r4
	smlal r2,r3, r9,r5
	smlal r2,r3, r10,r6
	smlal r2,r3, r12,r7
	mov r8, r2, LSR #16
	orr r8, r3, LSL #16
	str r8, [r0], #4
	.endm

	mulrow
	mulrow
	mulrow
	mulrow

	ldmfd sp!, {r4-r10}
	bx lr


	@ r0: vector ptr
	@ r1: matrix ptr
	.globl g3d_xform3
g3d_xform3:
	stmfd sp!, {r4-r8}

	@ load vector3 in r2-r4
	@ load matrix rows in r5-r8
	@ accumulate in r12 (low)/r8 (high), 
	ldmia r0, {r2-r4}

	.macro mulrow3
	ldmia r1!, {r5-r8}
	mov r12, r8, LSL #16	@ r12 <- fractional part
	mov r8, r8, LSR #16	@ r8 <- integer part
	smlal r12,r8, r7,r4
	smlal r12,r8, r6,r3
	smlal r12,r8, r5,r2
	mov r12, r12, LSR #16
	orr r12, r8, LSL #16
	str r12, [r0], #4
	.endm

	mulrow3
	mulrow3
	mulrow3

	ldmfd sp!, {r4-r8}
	bx lr


	@ r0: matrix a ptr (dest)
	@ r1: matrix b ptr
	.globl mat_mult_pre
mat_mult_pre:
	stmfd sp!, {r4-r11}

	@ transpose matrix b to a temp buffer, then adjust r1 to point there
	sub r2, sp, #64

	.macro transpose_column
	ldr r6, [r1, #48]; ldr r5, [r1, #32]; ldr r4, [r1, #16]; ldr r3, [r1], #4
	stmia r2!, {r3-r6}
	.endm

	transpose_column
	transpose_column
	transpose_column
	transpose_column

	mov r12, #4
0:	sub r1, sp, #64		@ bring r1 back to the top of B for the loop
	ldmia r0, {r2-r5}	@ row from A in r2-r5

	.macro matmulrow_pre
	ldmia r1!, {r6-r9}	@ column from B in r6-r9
	smull r10,r11, r2,r6
	smlal r10,r11, r3,r7
	smlal r10,r11, r4,r8
	smlal r10,r11, r5,r9
	mov r10, r10, LSR #16
	orr r10, r11, LSL #16
	str r10, [r0], #4
	.endm

	matmulrow_pre
	matmulrow_pre
	matmulrow_pre
	matmulrow_pre

	subs r12, r12, #1
	bne 0b

	ldmfd sp!, {r4-r11}
	bx lr


	@ r0: matrix a ptr (dest)
	@ r1: matrix b ptr
	.globl mat_mult
mat_mult:
	stmfd sp!, {r4-r11,lr}
	mov r14, r0

	mov r12, #4
	@ column from A into r2-r5
0:	ldr r5, [r0, #48]; ldr r4, [r0, #32]; ldr r3, [r0, #16]; ldr r2, [r0]

	.macro matmulrow
	@ row from B into r6-r9
	ldmia r1!, {r6-r9}
	smull r10,r11, r2,r6
	smlal r10,r11, r3,r7
	smlal r10,r11, r4,r8
	smlal r10,r11, r5,r9
	mov r10, r10, LSR #16
	orr r10, r11, LSL #16
	.endm

	matmulrow
	str r10, [r0], #4
	matmulrow
	str r10, [r0, #16-4]
	matmulrow
	str r10, [r0, #32-4]
	matmulrow
	str r10, [r0, #48-4]

	subs r12, r12, #1
	subne r1, r1, #64	@ bring r1 back for the next loop
	bne 0b

	ldmfd sp!, {r4-r11,lr}
	bx lr


@ vim:ft=arm:

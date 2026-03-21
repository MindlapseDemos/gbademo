	.section .iwram
	.arm
	.cpu arm7tdmi

	@ r0: vector ptr
	@ r1: matrix ptr
	.globl g3d_xform
g3d_xform:
	stmfd sp!, {r4-r11}

	@ load vector in r4-r7
	@ load matrix rows in r8-r11
	@ accumulate in r0 (low)/r2 (high), 
	ldmia r0, {r4-r7}

	.macro mulrow
	ldmia r1!, {r8-r11}
	smull r2,r3, r8,r4
	smlal r2,r3, r9,r5
	smlal r2,r3, r10,r6
	smlal r2,r3, r11,r7
	mov r8, r2, LSR #16
	orr r8, r3, LSL #16
	str r8, [r0], #4
	.endm

	mulrow
	mulrow
	mulrow
	mulrow

	ldmfd sp!, {r4-r11}
	bx lr

@ vim:ft=arm:

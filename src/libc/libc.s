	.text
	.thumb

	@ r0: dest
	@ r1: src
	@ r2: size
	.globl memcpy_asm
memcpy_asm:
	push {r4-r7}

	cmp r2, #4
	blt memcpy_bytes
	mov r7, #3
	mov r3, r0
	mov r4, r1
	and r3, r7
	and r4, r7
	cmp r3, r4
	bne memcpy_bytes

	@ same alignment, copy bytes until pointers are 32bit aligned
	cmp r3, #0
	beq memcpy_aligned
	ldr r5, [r1, r3]
	str r5, [r0, r3]
	sub r3, #1
	beq memcpy_aligned
	ldr r5, [r1, r3]
	str r5, [r0, r3]
	sub r3, #1
	beq memcpy_aligned
	ldr r5, [r1, r3]
	str r5, [r0, r3]
memcpy_aligned:
	add r0, r4	@ add 0 to 3, to advance previously unaligned pointers
	add r1, r4
	sub r2, r4	@ also subtract from the byte count

	@ properly aligned pointers, proceed to copy blocks of 16bytes
	b 1f	@ start from the >= 16byte check
0:	ldmia r1!, {r3-r6}
	stmia r0!, {r3-r6}
	sub r2, #16
1:	cmp r2, #16
	bge 0b

	@ find out what the trailing bytes are after we copy words
	mov r5, #3	@ r5: 00000003
	mvn r6, r5	@ r6: fffffffc
	mov r4, r2
	and r4, r6	@ r4: remaining words
	and r2, r5	@ r2: trailing bytes

	@ copy the remaining words
	cmp r4, #0
	beq memcpy_bytes
0:	ldr r3, [r1, r4]
	str r3, [r0, r4]
	sub r4, #4
	bne 0b

memcpy_bytes:
	cmp r2, #0
	beq memcpy_end
	b 1f
0:	ldrb r3, [r1, r2]
	strb r3, [r0, r2]
1:	sub r2, #1
	bge 0b

memcpy_end:
	pop {r4-r7}
	bx lr
	

@ vim:ft=arm:

	.syntax unified
	.arch armv7-a
	.eabi_attribute 27, 3
	.eabi_attribute 28, 1
	.fpu neon
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.thumb
	.file	"sample.c"
	.global	__aeabi_ldivmod
	.text
	.align	2
	.global	int_average
	.thumb
	.thumb_func
	.type	int_average, %function
int_average:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	subs	r2, r1, #0
	ble	.L5
	push	{r4, r5, r6, lr}
	add	r6, r0, r2, lsl #2
	movs	r4, #0
	movs	r5, #0
.L4:
	ldr	r3, [r0], #4
	adds	r4, r4, r3
	adc	r5, r5, r3, asr #31
	cmp	r0, r6
	bne	.L4
	mov	r0, r4
	mov	r1, r5
	asrs	r3, r2, #31
	bl	__aeabi_ldivmod
	pop	{r4, r5, r6, pc}
.L5:
	movs	r0, #0
	bx	lr
	.size	int_average, .-int_average
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.8.5-4ubuntu1) 4.8.5"
	.section	.note.GNU-stack,"",%progbits

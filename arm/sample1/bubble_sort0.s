	.arch armv7-a
	.eabi_attribute 27, 3
	.eabi_attribute 28, 1
	.fpu vfpv3-d16
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"bubble_sort0.c"
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r0, .L8
	movw	ip, #:lower16:data
	str	lr, [sp, #-4]!
	movt	ip, #:upper16:data
	movw	lr, #32767
.L2:
	ldr	r1, [ip]
	movw	ip, #:lower16:data
	movt	ip, #:upper16:data
	mov	r3, ip
.L3:
	ldr	r2, [r3, #4]!
	cmp	r3, r0
	str	r2, [r3, #-4]
	str	r1, [r3]
	bne	.L3
	subs	lr, lr, #1
	bne	.L2
	mov	r0, #1
	ldr	pc, [sp], #4
.L9:
	.align	2
.L8:
	.word	data+131068
	.size	main, .-main
	.comm	data,131072,4
	.ident	"GCC: (Ubuntu/Linaro 4.9.3-13ubuntu2) 4.9.3"
	.section	.note.GNU-stack,"",%progbits

section .data
newline_char: db 10
codes: db '0123456890ABCDEF'
hex_head: db '0x'	
section .text

global exit
exit:
	mov rax, 60
	syscall
	ret

global string_length
string_length:
.loop0:
	cmp byte [rdi + rax], 0
	ja .end0
	inc rax
	jmp .loop0
.end0:
	ret

global print_string
print_string:
	call string_length
	mov rsi, rdi
	mov rdx, rax
	mov rax, 1
	mov rdi, 1
	syscall
	ret


global print_newline
print_newline:
	mov rax, 1
	mov rdi, 1
	mov rsi, newline_char
	mov rdx, 1,
	syscall
	ret
	
global print_hex
print_hex:
	mov rax, rdi
	mov rdi, 1
	mov rdx, 1
	mov rcx, 64
	mov rbx, 0xf		;mask

	test rax, 0		;0だった場合の判定
	jz .zero
.iterate1:
	push rax
	sub rcx, 4
	sar rax, cl

	
	and rax, 0xf
	test rbx, rax  		; 前0判定
	jz .iterate2

	lea rsi, [codes + rax]	
	mov rbx, 0xf
	
	mov rax, 1
	push rcx
	syscall
	pop rcx

	pop rax
	test rcx, rcx
	jnz .iterate1
.end1:
	ret
.iterate2:  			; 前0を排除する
	pop rax
	jmp .iterate1
	
.zero:
	lea rsi, [codes]
	mov rax, 1
	syscall
	ret
	

section .data
newline_char: db 0xa
codes: db '01234567890ABCDEF'
hex_head: db '0x'	
section .text

global exit
exit:
	mov rax, 60
	syscall
	ret

global string_length
string_length:
	xor rax, rax
.loop0:
	cmp byte [rdi + rax], 0
	je .end0
	inc rax
	jmp .loop0
.end0:
	ret

	
global print_string
print_string:
	push rdi
	call string_length
	pop rsi
	mov rdx, rax
	mov rax, 1
	mov rdi, 1
	syscall
	ret

global print_char
print_char:
	push rdi
	mov rdi, rsp
	call print_string
	pop rdi
	ret
	
global print_newline
print_newline:
	mov rdi, 10
	jmp print_char

global print_uint
print_uint:
	mov rax, rdi
	mov rdi, rsp		;
	push 0  			; null終端
	dec rdi				; nullのpointerをずらす
	sub rsp, 16			; rsp 
	mov r8, 10			; mod 10するときの基数を設定

.loop:
	xor rdx, rdx		; 初期化 divには商がrax,余がrdxに代入される
	div r8			; 割り算
	or dl, 0x30		; 数値を文字(0-9)に変換
	dec rdi			; pointerを減らす
	mov [rdi], dl		; 文字をメモリにいれる
	test rax, rax		; 商が0でないかを確認。0なら終了
	jnz .loop
	
	call print_string	; 文字列を表示
	add rsp, 24		;
	ret
	
; global print_newline
; print_newline:
; 	mov rsi, newline_char
; 	mov rax, 1
; 	mov rdi, 1
; 	mov rdx, 1
; 	syscall
; 	ret
	
; global print_hex
; print_hex:
; 	mov rax, rdi
; 	mov rdi, 1
; 	mov rdx, 1
; 	mov rcx, 64
; 	mov rbx, 0xf		;mask

; 	test rax, 0		;0だった場合の判定
; 	jz .zero
; .iterate1:
; 	push rax
; 	sub rcx, 4
; 	sar rax, cl

	
; 	and rax, 0xf
; 	test rbx, rax  		; 前0判定
; 	jz .iterate2

; 	lea rsi, [codes + rax]	
; 	mov rbx, 0xf
	
; 	mov rax, 1
; 	push rcx
; 	syscall
; 	pop rcx

; 	pop rax
; 	test rcx, rcx
; 	jnz .iterate1
; .end1:
; 	ret
; .iterate2:  			; 前0を排除する
; 	pop rax
; 	jmp .iterate1
	
; .zero:
; 	lea rsi, [codes]
; 	mov rax, 1
; 	syscall
; 	ret
	

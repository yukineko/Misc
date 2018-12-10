global _start
section .data
test_string: db 'abcde'
section .text

extern exit
extern print_newline
extern print_hex
_start:
	;	mov rdi, test_string
	mov rdi, 0x0
	call print_hex
	call print_newline	
	call exit
	

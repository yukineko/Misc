global _start
section .data
test_string: db 'abcde'
section .text

extern exit
extern print_newline
extern print_string
extern print_uint
	
_start:
	call print_newline
	mov rdi, test_string
	call print_string
	mov rdi, 33
	call print_uint
	call exit
	

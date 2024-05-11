	global _start
	section .text
_start:
	push 12
	push 12
	mov rax, 60
	mov rdi, 0
	syscall

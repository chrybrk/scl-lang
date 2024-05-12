	global _start
	section .text
_start:
	push 10
	push qword [rsp + (0 - 0) * 8]
	mov rax, 60
	mov rdi, qword [rsp + (1 - 0) * 8]
	syscall

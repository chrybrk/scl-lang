format ELF64
section '.text' executable
public _start
_start:
	push rbp
	mov rbp, rsp
	mov rax, 60
	mov rdi, 0
	syscall
	pop rbp

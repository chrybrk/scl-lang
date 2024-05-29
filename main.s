format ELF64
section '.text' executable
public _start
extrn atoi
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov rax, 2
	push rax
	mov rax, 2
	pop rdx
	add rax, rdx
	push rax
	mov rax, 4
	pop rdx
	sub rax, rdx
	mov qword [rbp - 8], rax
	mov rax, [rbp - 8]
	mov rdi, rax
	mov rax, 60
	syscall
	add rsp, 8
	pop rbp

section '.data' writeable

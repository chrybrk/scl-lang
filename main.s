format ELF64
section '.text' executable
public _start
extrn atoi
extrn print_int
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 0
	mov rax, 2
	push rax
	mov rax, 4
	push rax
	pop rdx
	pop rax
	sub rax, rdx
	push rax
	push rax
	mov rax, 3
	push rax
	pop rdx
	pop rax
	add rax, rdx
	push rax
	push rax
	mov rax, 100
	push rax
	pop rdx
	pop rax
	add rax, rdx
	push rax
	push rax
	mov rax, 20
	push rax
	pop rdx
	pop rax
	sub rax, rdx
	push rax
	mov rdi, rax
	call print_int
	mov rax, 0
	mov rdi, rax
	mov rax, 60
	syscall
	add rsp, 0
	pop rbp

section '.data' writeable

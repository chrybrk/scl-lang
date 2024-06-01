format ELF64
section '.text' executable
public _start
extrn print_int
extrn print
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 0
	mov rax, string_0
	push rax
	pop rdi
	call print
	push 10
	push 20
	push 10
	pop rdx
	pop rax
	mul rdx
	push rax
	push 2
	xor rdx, rdx
	pop rcx
	pop rax
	div rcx
	push rax
	pop rdx
	pop rax
	add rax, rdx
	push rax
	pop rdi
	call print_int
	push 0
	mov rdi, rax
	mov rax, 60
	syscall
	add rsp, 0
	pop rbp

section '.data' writeable
string_0: db "Hello, World!", 0

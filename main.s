format ELF64
section '.text' executable
public _start
extrn print
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	lea rax, [string_0]
	push rax
	pop rax
	mov qword [rbp - 8], rax
	mov rax, [rbp - 8]
	push rax
	pop rdi
	call print
	push 0
	pop rdi
	mov rax, 60
	syscall
	add rsp, 8
	pop rbp

section '.data' writeable
string_0: db "Hello, World!", 0

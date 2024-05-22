format ELF64
section '.text' executable
public _start
extrn atoi
extrn _exit
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov rax, 10
	mov qword [rbp - 8], rax
	mov rax, string_0
	mov rdi, rax
	call atoi
	mov rdi, rax
	call _exit
	add rsp, 8
	pop rbp

section '.data' writeable
string_0: db "100", 0

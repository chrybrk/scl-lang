format ELF64
section '.text' executable
public _start
extrn puts
extrn _exit
extrn atoi
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov rax, string_0
	mov rdi, rax
	call atoi
	mov r8d, eax
	mov dword [rbp - 4], r8d
	mov rax, [rbp - 4]
	mov rdi, rax
	call _exit
	mov r8d, eax
	mov dword [rbp - 8], r8d
	mov rax, string_1
	mov rdi, rax
	call puts
	mov rax, [rbp - 4]
	mov rdi, rax
	call _exit
	add rsp, 8
	pop rbp

section '.data' writeable
string_0: db "10", 0
string_1: db "Hello, World!", 0

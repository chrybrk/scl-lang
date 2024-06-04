format ELF64
section '.text' executable
public _start
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push 10
	pop rax
	mov dword [rbp - 4], eax
	mov rax, [rbp - 4]
	push rax
	pop rax
	mov r8d, eax
	mov dword [rbp - 8], r8d
	mov rax, [rbp - 8]
	push rax
	push 20
	pop rdx
	pop rax
	mul rdx
	push rax
	pop rax
	mov dword [rbp - 8], eax
	mov rax, [rbp - 8]
	push rax
	pop rax
	mov r8d, eax
	mov dword [rbp - 16], r8d
	mov rax, [rbp - 16]
	push rax
	pop rdi
	mov rax, 60
	syscall
	mov rax, [rbp - 4]
	push rax
	pop rdi
	mov rax, 60
	syscall
	add rsp, 16
	pop rbp

section '.data' writeable

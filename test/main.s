format ELF64
section '.text' executable
public _start

_start:
	push rbp
	mov rbp, rsp

    ; 4 - 2 + 3 + 4
    mov rax, 4
    push rax

    mov rax, 2
    push rax

    mov rax, 3
    push rax

    mov rax, 4
    push rax

    mov rdx, [rsp + 8 * 3]
    mov rax, [rsp + 8 * 2]
    sub rdx, rax
    mov [rsp + 8 * 2], rdx

    mov rdx, [rsp + 8 * 2]
    mov rax, [rsp + 8 * 1]
    add rdx, rax
    mov [rsp + 8 * 1], rdx

    mov rdx, [rsp + 8 * 1]
    mov rax, [rsp + 8 * 0]
    add rdx, rax

    push rdx

	mov rax, 60
    mov rdi, [rsp + 8 * 0]
	syscall

	pop rbp

section '.data' writeable

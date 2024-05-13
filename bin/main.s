format ELF64 executable
segment readable executable
entry _start
_start:
	push rbp
	mov rbp, rsp
	mov byte [rbp - 1], 10
	mov r8w, [rbp - 1]
	mov word [rbp - 3], r8w
	mov qword [rbp - 11], 1240
	mov r8b, [rbp - 11]
	mov byte [rbp - 12], r8b
	mov rax, 60
	mov rdi, [rbp - 12]
	syscall
	pop rbp

format ELF64 executable
segment readable executable
entry _start
_start:
	push rbp
	mov rbp, rsp
	mov qword [rbp - 8], 5
	mov r8w, [rbp - 8]
	mov word [rbp - 10], r8w
	mov rax, 60
	mov rdi, [rbp - 10]
	syscall
	pop rbp

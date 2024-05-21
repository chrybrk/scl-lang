format ELF64
section '.text' executable
public _start
extrn puts
extrn printf
extrn strlen
extrn _exit
_start:
	push rbp
	mov rbp, rsp
	mov qword [rbp - 8], string_0
	mov rdi, [rbp - 8]
	call puts
	mov rdi, [rbp - 8]
	call strlen
	mov rax, 60
	mov rdi, 0
	syscall
	pop rbp
section '.data' writeable
string_0: db "Hello, World", 0

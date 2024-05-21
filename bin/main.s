format ELF64
section '.text' executable
public _start
extrn puts
extrn _exit
_start:
	push rbp
	mov rbp, rsp
	mov rdi, string_0
	call puts
	mov rdi, 0
	call _exit
	pop rbp
section '.data' writeable
string_0: db "Hello, World!\n", 0

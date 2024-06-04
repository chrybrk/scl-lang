format ELF64
section '.text' executable
public _start
extrn printI
extrn print
extrn puts
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 40
	push 10
	pop rax
	mov dword [rbp - 4], eax
	push 12
	pop rax
	mov dword [rbp - 8], eax
	lea rax, [string_0]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_1]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 8]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_2]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; GT condition
	pop rax
	pop rdx
	cmp rax, rdx
	setl al
	movzx eax, al
	push rax
	; GT condition - ends

	pop rdi
	call printI
	lea rax, [string_3]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; LT condition
	pop rax
	pop rdx
	cmp rax, rdx
	setg al
	movzx eax, al
	push rax
	; LT condition - ends

	pop rdi
	call printI
	lea rax, [string_4]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; GTE condition
	pop rax
	pop rdx
	cmp rax, rdx
	setle al
	movzx eax, al
	push rax
	; GTE condition - ends

	pop rdi
	call printI
	lea rax, [string_5]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; LTE condition
	pop rax
	pop rdx
	cmp rax, rdx
	setge al
	movzx eax, al
	push rax
	; LTE condition - ends

	pop rdi
	call printI
	lea rax, [string_6]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; EQ condition
	pop rax
	pop rdx
	cmp rax, rdx
	sete al
	movzx eax, al
	push rax
	; EQ condition - ends

	pop rdi
	call printI
	lea rax, [string_7]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; NEQ condition
	pop rax
	pop rdx
	cmp rax, rdx
	setne al
	movzx eax, al
	push rax
	; NEQ condition - ends

	pop rdi
	call printI
	lea rax, [string_8]
	push rax
	pop rdi
	call puts
	lea rax, [string_9]
	push rax
	pop rdi
	call puts
	mov eax, dword [rbp - 4]
	cdq
	push rax
	pop rax
	mov r8d, eax
	mov dword [rbp - 12], r8d
	mov eax, dword [rbp - 8]
	cdq
	push rax
	pop rax
	mov r8d, eax
	mov dword [rbp - 4], r8d
	mov eax, dword [rbp - 12]
	cdq
	push rax
	pop rax
	mov r8d, eax
	mov dword [rbp - 8], r8d
	lea rax, [string_10]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_11]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 8]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_12]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; GT condition
	pop rax
	pop rdx
	cmp rax, rdx
	setl al
	movzx eax, al
	push rax
	; GT condition - ends

	pop rdi
	call printI
	lea rax, [string_13]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; LT condition
	pop rax
	pop rdx
	cmp rax, rdx
	setg al
	movzx eax, al
	push rax
	; LT condition - ends

	pop rdi
	call printI
	lea rax, [string_14]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; GTE condition
	pop rax
	pop rdx
	cmp rax, rdx
	setle al
	movzx eax, al
	push rax
	; GTE condition - ends

	pop rdi
	call printI
	lea rax, [string_15]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; LTE condition
	pop rax
	pop rdx
	cmp rax, rdx
	setge al
	movzx eax, al
	push rax
	; LTE condition - ends

	pop rdi
	call printI
	lea rax, [string_16]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; EQ condition
	pop rax
	pop rdx
	cmp rax, rdx
	sete al
	movzx eax, al
	push rax
	; EQ condition - ends

	pop rdi
	call printI
	lea rax, [string_17]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 4]
	cdq
	push rax
	mov eax, dword [rbp - 8]
	cdq
	push rax

	; NEQ condition
	pop rax
	pop rdx
	cmp rax, rdx
	setne al
	movzx eax, al
	push rax
	; NEQ condition - ends

	pop rdi
	call printI
	lea rax, [string_18]
	push rax
	pop rdi
	call puts
	lea rax, [string_19]
	push rax
	pop rdi
	call puts
	push 10
	pop rax
	mov dword [rbp - 24], eax
	push 20
	pop rax
	mov dword [rbp - 28], eax
	push 30
	pop rax
	mov dword [rbp - 32], eax
	push 0
	pop rax
	mov dword [rbp - 36], eax
	lea rax, [string_20]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 24]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_21]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 28]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_22]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 32]
	cdq
	push rax
	pop rdi
	call printI
	lea rax, [string_23]
	push rax
	pop rdi
	call print
	mov eax, dword [rbp - 24]
	cdq
	push rax
	mov eax, dword [rbp - 28]
	cdq
	push rax

	; GT condition
	pop rax
	pop rdx
	cmp rax, rdx
	setl al
	movzx eax, al
	push rax
	; GT condition - ends

	pop rax
	mov dword [rbp - 36], eax
	pop rdi
	call print
	push 0
	pop rdi
	mov rax, 60
	syscall
	add rsp, 40
	pop rbp

section '.data' writeable
string_0: db "x: ", 0
string_1: db "y: ", 0
string_2: db "x > y: ", 0
string_3: db "x < y: ", 0
string_4: db "x >= y: ", 0
string_5: db "x <= y: ", 0
string_6: db "x == y: ", 0
string_7: db "x != y: ", 0
string_8: db "", 0
string_9: db "Lets swap x and y.", 0
string_10: db "x: ", 0
string_11: db "y: ", 0
string_12: db "x > y: ", 0
string_13: db "x < y: ", 0
string_14: db "x >= y: ", 0
string_15: db "x <= y: ", 0
string_16: db "x == y: ", 0
string_17: db "x != y: ", 0
string_18: db "", 0
string_19: db "Lets try 3 variables.", 0
string_20: db "a: ", 0
string_21: db "b: ", 0
string_22: db "c: ", 0
string_23: db "d = a > b", 0

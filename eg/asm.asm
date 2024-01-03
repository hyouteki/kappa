global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 69
	mov DWORD [rbp-8], 12
	mov QWORD [rbp-16], L0
	cmp DWORD [rbp-4], 69
	je L1
	mov rax, 60
	mov edi, 1
	syscall
	jmp L2
L1:
	mov rax, 60
	mov edi, 2
	syscall
	jmp L2
L2:
	mov rax, 1
	mov rdi, 1
	mov rsi, QWORD [rbp-16]
	mov edx, DWORD [rbp-8]
	syscall
	mov rax, 60
	mov edi, DWORD [rbp-4]
	syscall
section .data
L0:
	db `Hello moto\n`

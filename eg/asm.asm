global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 69
	mov DWORD [rbp-8], 12
	mov QWORD [rbp-16], L0
	mov rax, 1
	mov rdi, 1
	mov rsi, L1
	mov rdx, 10
	syscall
	cmp DWORD [rbp-4], 69
	jne L2
	mov DWORD [rbp-20], 19
	mov QWORD [rbp-28], L4
	mov rax, 1
	mov rdi, 1
	mov rsi, QWORD [rbp-28]
	mov edx, DWORD [rbp-20]
	syscall
	jmp L3
L2:
	mov rax, 1
	mov rdi, 1
	mov rsi, L5
	mov rdx, 19
	syscall
	jmp L3
L3:
	mov DWORD [rbp-20], 8
	mov QWORD [rbp-28], L6
	mov rax, 1
	mov rdi, 1
	mov rsi, QWORD [rbp-28]
	mov edx, DWORD [rbp-20]
	syscall
	mov rax, 60
	mov edi, DWORD [rbp-4]
	syscall
section .data
L0:
	db `Hello moto\n`, 0
L1:
	db `If Start\n`, 0
L4:
	db `Inside then block\n`, 0
L5:
	db `Inside else block\n`, 0
L6:
	db `If end\n`, 0

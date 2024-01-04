global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 123
	mov ecx, DWORD [rbp-4]
	push rcx
	mov rcx, 10
	push rcx
	mov rcx, 2
	push rcx
	pop rbx
	pop rax
	mul rbx
	mov rcx, rax
	push rcx
	mov rcx, 3
	push rcx
	pop rbx
	pop rax
	add rax, rbx
	mov rcx, rax
	push rcx
	pop rbx
	pop rax
	sub rax, rbx
	mov rbx, rax
	cmp rbx, 100
	jne L0
	mov rax, 1
	mov rdi, 1
	mov rsi, L2
	mov rdx, 12
	syscall
	jmp L1
L0:
	jmp L1
L1:
	mov rbx, 69
	mov rax, 60
	mov rdi, rbx
	syscall
section .data
L2:
	db `Hello moto\n`, 0

global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 69
	mov rax, 1
	mov rdi, 1
	mov rsi, L0
	mov rdx, 11
	syscall
	mov rax, 60
	mov edi, DWORD [rbp-4]
	syscall
section .data
L0:
	dw 'Hello moto'

global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov rax, 1
	mov rdi, 1
	mov rsi, L0
	mov rdx, 12
	syscall
section .data
L0:
	db `Hello moto\n`, 0

global _start
section .text
_start:
	mov rax, 1
	mov rdi, 1
	mov rsi, L0
	mov rdx, 11
	syscall
	mov rax, 60
	mov rdi, 69
	syscall
section .data
L0:
	dw 'Hello moto'

global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 69
	mov rbx, 69
	mov rax, 60
	mov rdi, rbx
	syscall
section .data

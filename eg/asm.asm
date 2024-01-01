global _start
section .text
_start:
kappa_data_exit:
	mov rax, 60
	mov rdi, 69
	syscall

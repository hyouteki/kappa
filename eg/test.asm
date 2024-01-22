global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	mov DWORD [rbp-4], 0
	mov DWORD [rbp-8], 0
	mov DWORD [rbp-12], 1
	mov QWORD [rbp-20], L0
	mov ebx, DWORD [rbp-8]
	mov rax, 60
	mov rdi, rbx
	syscall
section .data
L0:
	db ``, 0

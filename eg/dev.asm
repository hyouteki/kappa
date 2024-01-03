global _start
section .text
_start:
	push rbp
	mov rbp, rsp
	push 1
	push 2
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	push 5
	push 7
	pop rbx
	pop rax
	mul rbx
	push rax
	pop rbx
	pop rax
	add rax, rbx
	mov DWORD [rbp-4], eax
	mov rax, 60
	mov edi, DWORD [rbp-4]
	syscall
section .data

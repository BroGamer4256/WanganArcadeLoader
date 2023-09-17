extern ratio

extern implOfRenderShape
extern whereRenderShape
extern realRenderShape

extern implOfRenderText
extern whereRenderText

%macro pushaq 0
	push rax
	push rcx
	push rdx
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popaq 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdx
	pop rcx
	pop rax
%endmacro

section .text
implOfRenderShape:
	pushaq
	call realRenderShape
	popaq

	movss xmm11, xmm0
	movss dword [rbp+0x8], xmm11

	mov rax, [rel whereRenderShape]
	add rax, 15
	jmp rax

implOfRenderText:
	movss xmm14, [rel ratio]
	movss dword [rbp - 0x28], xmm14
	movss dword [rbp - 0x18], xmm12

	mov rax, [rel whereRenderText]
	add rax, 12
	jmp rax

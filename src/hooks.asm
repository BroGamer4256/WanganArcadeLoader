extern whereRenderShape
extern whereRenderText
extern ratio

extern implOfRenderShape
extern implOfRenderText

section .text
implOfRenderShape:
	movss xmm11, [rel ratio]
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

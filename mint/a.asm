.386
.model flat, stdcall
option casemap :none

includelib msvcrt.lib
printf  PROTO C:ptr sbyte,:vararg
scanf  PROTO C:ptr sbyte,:vararg

.data
__str0	DB	"the bigger one is:",0
_CHAR	DB	"%c",0
_INT	DB	"%d",0

.code
_max	proc
	push	ebp
	mov	ebp,	esp
	sub	esp,	0
	push	ebx
	push	edi
	push	esi
	sub	esp,	0
	mov	eax,	dword ptr [ebp+8]
	mov	ecx,	dword ptr [ebp+12]
	cmp	eax,	ecx
	jl	label_0
	mov	eax,	dword ptr [ebp+8]
	mov	esp,	ebp
	sub	esp,	16
	pop	esi
	pop	edi
	pop	ebx
	mov	esp,	ebp
	pop	ebp
	ret
	jmp	label_1
label_0:
	mov	eax,	dword ptr [ebp+12]
	mov	esp,	ebp
	sub	esp,	16
	pop	esi
	pop	edi
	pop	ebx
	mov	esp,	ebp
	pop	ebp
	ret
label_1:
	mov	esp,	ebp
	sub	esp,	12
	pop	esi
	pop	edi
	pop	ebx
	mov	esp,	ebp
	pop	ebp
	ret
_max	endp
main	proc
	push	ebp
	mov	ebp,	esp
	sub	esp,	12
	sub	esp,	4
	lea	eax,	dword ptr [ebp-4]
	push	eax
	lea	eax,	_INT
	push	eax
	call	scanf
	add	esp,	8
	lea	eax,	dword ptr [ebp-8]
	push	eax
	lea	eax,	_INT
	push	eax
	call	scanf
	add	esp,	8
	push	dword ptr [ebp-8]
	push	dword ptr [ebp-4]
	call	_max
	add	esp,	8
	mov	dword ptr [ebp-16],	eax
	mov	eax,	dword ptr [ebp-16]
	mov	dword ptr [ebp-12],	eax
	lea	eax,	__str0
	push	eax
	call	printf
	add	esp,	4
	mov	eax,	dword ptr [ebp-12]
	push	eax
	lea	eax,	_INT
	push	eax
	call	printf
	add	esp,	8
	mov	esp,	ebp
	pop	ebp
	ret
main	endp
end main

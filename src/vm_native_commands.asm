
;.686
;.model flat, stdcall
;option casemap :none

_text SEGMENT

.code

;-------------------------------------------------------------------------------
fnAND8 PROC

mov EAX, 0
mov AL, BYTE PTR[ECX]
mov BL, BYTE PTR[EDX]
and AL, BL
mov BYTE PTR[R8], AL
lahf
SAR AX, 8

ret
fnAND8 ENDP

fnAND16 PROC

mov AX, WORD PTR[ECX]
mov BX, WORD PTR[ECX]
and AX, BX
pushf
mov WORD PTR[EAX], AX
pop AX 

ret
fnAND16 ENDP

fnAND32 PROC

mov EAX, DWORD PTR[R11]
mov EBX, DWORD PTR[R12]
and EAX, EBX
pushf
mov DWORD PTR[R13], EAX
pop AX 

ret
fnAND32 ENDP

fnAND64 PROC, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

mov R11, Arg1
mov R12, Arg2
mov R13, Arg3
mov RAX, QWORD PTR[R11]
mov RBX, QWORD PTR[R12]
and RAX, RBX
pushf
mov QWORD PTR[R13], RAX
pop AX 

ret
fnAND64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------

fnOR8 PROC, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

mov rax, Arg1
add rax, Arg2
add rax, Arg3

ret
fnOR8 ENDP

fnOR16 PROC, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

mov rax, Arg1
add rax, Arg2
add rax, Arg3

ret
fnOR16 ENDP

fnOR32 PROC, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

mov rax, Arg1
add rax, Arg2
add rax, Arg3

ret
fnOR32 ENDP

fnOR64 PROC, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

mov rax, Arg1
add rax, Arg2
add rax, Arg3

ret
fnOR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnXOR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnXOR8 ENDP

fnXOR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnXOR16 ENDP

fnXOR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnXOR32 ENDP

fnXOR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnXOR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnNOT8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnNOT8 ENDP

fnNOT16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOT16 ENDP

fnNOT32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOT32 ENDP

fnNOT64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOT64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnNAND8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnNAND8 ENDP

fnNAND16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNAND16 ENDP

fnNAND32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNAND32 ENDP

fnNAND64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNAND64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnNOR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnNOR8 ENDP

fnNOR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOR16 ENDP

fnNOR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOR32 ENDP

fnNOR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNOR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnADD8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnADD8 ENDP

fnADD16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADD16 ENDP

fnADD32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADD32 ENDP

fnADD64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADD64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSUB8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSUB8 ENDP

fnSUB16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSUB16 ENDP

fnSUB32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSUB32 ENDP

fnSUB64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSUB64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnMUL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnMUL8 ENDP

fnMUL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnMUL16 ENDP

fnMUL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnMUL32 ENDP

fnMUL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnMUL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnDIV8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnDIV8 ENDP

fnDIV16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDIV16 ENDP

fnDIV32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDIV32 ENDP

fnDIV64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDIV64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnINC8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnINC8 ENDP

fnINC16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnINC16 ENDP

fnINC32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnINC32 ENDP

fnINC64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnINC64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnDEC8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnDEC8 ENDP

fnDEC16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDEC16 ENDP

fnDEC32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDEC32 ENDP

fnDEC64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnDEC64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnADC8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnADC8 ENDP

fnADC16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADC16 ENDP

fnADC32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADC32 ENDP

fnADC64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnADC64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSBB8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSBB8 ENDP

fnSBB16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSBB16 ENDP

fnSBB32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSBB32 ENDP

fnSBB64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSBB64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnIMUL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnIMUL8 ENDP

fnIMUL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIMUL16 ENDP

fnIMUL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIMUL32 ENDP

fnIMUL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIMUL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnIDIV8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnIDIV8 ENDP

fnIDIV16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIDIV16 ENDP

fnIDIV32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIDIV32 ENDP

fnIDIV64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnIDIV64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnNEG8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnNEG8 ENDP

fnNEG16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNEG16 ENDP

fnNEG32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNEG32 ENDP

fnNEG64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnNEG64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSHR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSHR8 ENDP

fnSHR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHR16 ENDP

fnSHR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHR32 ENDP

fnSHR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSHL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSHL8 ENDP

fnSHL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHL16 ENDP

fnSHL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHL32 ENDP

fnSHL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSHL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnROR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnROR8 ENDP

fnROR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROR16 ENDP

fnROR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROR32 ENDP

fnROR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnROL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnROL8 ENDP

fnROL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROL16 ENDP

fnROL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROL32 ENDP

fnROL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnROL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSAR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSAR8 ENDP

fnSAR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAR16 ENDP

fnSAR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAR32 ENDP

fnSAR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnSAL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnSAL8 ENDP

fnSAL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAL16 ENDP

fnSAL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAL32 ENDP

fnSAL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnSAL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnRCR8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnRCR8 ENDP

fnRCR16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCR16 ENDP

fnRCR32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCR32 ENDP

fnRCR64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCR64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnRCL8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnRCL8 ENDP

fnRCL16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCL16 ENDP

fnRCL32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCL32 ENDP

fnRCL64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnRCL64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnTEST8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnTEST8 ENDP

fnTEST16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnTEST16 ENDP

fnTEST32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnTEST32 ENDP

fnTEST64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnTEST64 ENDP
;-------------------------------------------------------------------------------


;-------------------------------------------------------------------------------
fnCMP8 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

	ret
fnCMP8 ENDP

fnCMP16 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnCMP16 ENDP

fnCMP32 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnCMP32 ENDP

fnCMP64 PROC ;, Arg1:QWORD, Arg2:QWORD, Arg3:QWORD

	mov rax, 2341
	mov rcx, 2341
	mov rax, 2343

ret
fnCMP64 ENDP
;-------------------------------------------------------------------------------


_text ENDS

;----------------------------------------------
END


#include "base_definitions.h"
#include "base_exception.h"

extern "C" uint16 __stdcall fnAND8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		mov EDX, DWORD PTR[pTarget]	// Read sddress of the destination
		mov AL, BYTE PTR[EBX]		// Load first operand into the AX register
		mov BL, BYTE PTR[ECX]		// Load second operand into the BX register
		and AL, BL					// Do the operation
		pushf						// Save flags 
		mov BYTE PTR[EDX], AL		// Store the result into the destination
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnAND16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		and AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnAND32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		and EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnAND64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		
		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		and EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		and EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}


extern "C" uint16 __stdcall fnOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		or AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}


extern "C" uint16 __stdcall fnOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		or AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}


extern "C" uint16 __stdcall fnOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		or EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		or EAX, EBX					// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		or EDX, ECX					// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnXOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		xor AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnXOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		xor AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnXOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		xor EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnXOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[ESI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[ESI + 4]	// Load upper half of the second operand into the DX register

		xor EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		xor EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnNOT8(uint8* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EDX]
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnNOT16(uint16* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EDX]
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnNOT32(uint32* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EDX]
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnNOT64(uint64* pTarget)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]	// Read sddress of the only operand

		mov EAX, DWORD PTR[EDI]		// Load lower half of the operand into the AX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the operand into the DX register

		not EAX						// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		not EDX						// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnNAND8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		and AL, BL
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNAND16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		and AX, BX
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNAND32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		and EAX, EBX
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNAND64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		and EAX, EBX				// Do the operation for the lower half
		not EAX
		pushf						// Save first intermediatre flags
		and EDX, ECX				// Do the operation for the upper half
		not EDX
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		xor AX, 0x00C0				// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		or AL, BL
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}


extern "C" uint16 __stdcall fnNOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		or AX, BX
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		or EAX, EBX
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		xor AX, 0x00C0	// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnNOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		or EAX, EBX					// Do the operation for the lower half
		not EAX
		pushf						// Save first intermediatre flags
		or EDX, ECX					// Do the operation for the upper half
		not EDX
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		xor AX, 0x00C0				// toggle ZF & SF
	}
}

extern "C" uint16 __stdcall fnADD8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		add AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnADD16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		add AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnADD32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		add EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnADD64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		add EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		adc EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnSUB8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		sub AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnSUB16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		sub AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSUB32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		sub EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSUB64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		sub EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		sbb EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnMUL8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		mov AL, BYTE PTR[EBX]		// Load first operand into the AX register
		mov BL, BYTE PTR[ECX]		// Load second operand into the BX register
		mul BL						// AX <- AL * BL
		pushf						// Save flags 
		mov EDX, DWORD PTR[pTarget]	// Read address of the destination
		mov BYTE PTR[EDX], AL		// Store the result into the destination
		CMP AX, 0					// Set zero flags since mul doesn't sets it
		pushf
		pop BX
		pop AX
		and AX, 0xFF3F
		and BX, 0x00C0
		or AX, BX					// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		mul BX						// DX:AX <- AX * BX
		pushf
		mov EDX, DWORD PTR[pTarget]
		mov WORD PTR[EDX], AX
		CMP AX, 0
		pushf
		pop BX
		pop AX
		and AX, 0xFF3F
		and BX, 0x00C0
		or AX, BX
	}
}

extern "C" uint16 __stdcall fnMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		mul EBX						// EDX:EAX <- EAX * EBX
		pushf
		mov ECX, DWORD PTR[pTarget]
		mov DWORD PTR[ECX], EAX
		CMP EAX, 0
		pushf
		pop BX
		pop AX
		and AX, 0xFF3F
		and BX, 0x00C0
		or AX, BX
	}
}

extern "C" uint16 __stdcall fnMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval * rval;
	__asm pushf	// Doesn't makes sense, dummy flags
	*pTarget = res;
	__asm pop AX
}

extern "C" uint16 __stdcall fnDIV8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		xor AX, AX					// Reset AX to 0
		mov AL, BYTE PTR[EBX]		// Load first operand into the AX register (dividend)
		mov BL, BYTE PTR[ECX]		// Load second operand into the BX register (divisor)
		div BL						// AH (reminder) : AL (quotient) <- AL / BL
		pushf						// Save flags
		mov EDX, DWORD PTR[pTarget]	// Read address of the destination 
		mov WORD PTR[EDX], AX		// Store the result into the destination (Reminder : Quotient)
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		xor DX, DX					// Reset AX to 0
		mov AX, WORD PTR[EBX]		// Load first operand into the AX register (dividend)
		mov BX, WORD PTR[ECX]		// Load second operand into the BX register (divisor)
		div BX						// DX (reminder) : AX (quotient) <- AX / BX
		pushf						// Save flags
		mov EBX, DWORD PTR[pTarget]	// Read address of the destination 
		mov WORD PTR[EDX], AX		// Store the result into the destination (Quotient)
		mov WORD PTR[EDX+2], DX		// Store the result into the destination (Reminder)
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EDX, EDX				// Reset AX to 0
		mov EAX, DWORD PTR[EBX]		// Load first operand into the AX register (dividend)
		mov EBX, DWORD PTR[ECX]		// Load second operand into the BX register (divisor)
		div BX						// EDX (reminder) : EAX (quotient) <- EAX / EBX
		pushf						// Save flags
		mov EBX, DWORD PTR[pTarget]	// Read address of the destination 
		mov DWORD PTR[EDX], EAX		// Store the result into the destination (Quotient)
		mov DWORD PTR[EDX + 4], EDX	// Store the result into the destination (Reminder)
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval / rval;
	__asm pushf // Doesn't makes sense, dummy flags
	*pTarget = res;
	__asm pop AX
}

extern "C" uint16 __stdcall fnINC8(uint8* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]	// Read sddress of the operand
		mov AL, BYTE PTR[EDX]		// Load operand into the AX register
		inc AL						// Increment by 1
		pushf						// Save flags
		mov BYTE PTR[EDX], AL		// Store the result into the destination
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnINC16(uint16* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EDX]
		inc AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnINC32(uint32* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EDX]
		inc EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnINC64(uint64* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]	// Read sddress of the operand
		mov EAX, DWORD PTR[EDX]		// Load lower half of the operand into the AX register
		mov EBX, DWORD PTR[EDX + 4]	// Load upper half of the operand into the BX register
		
		inc EAX						// Increment lower half
		pushf
		adc EBX, 0					// Add carry bit to upper half
		pushf

		mov DWORD PTR[EDX], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDX + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnDEC8(uint8* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EDX]
		dec AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnDEC16(uint16* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EDX]
		dec AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnDEC32(uint32* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EDX]
		dec EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnDEC64(uint64* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]	// Read sddress of the operand
		mov EAX, DWORD PTR[EDX]		// Load lower half of the operand into the AX register
		mov EBX, DWORD PTR[EDX + 4]	// Load upper half of the operand into the BX register

		dec EAX						// Increment lower half
		pushf
		sbb EBX, 0					// Add carry bit to upper half
		pushf

		mov DWORD PTR[EDX], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDX + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnADC8(uint8* pLeft, uint8* pRight, uint8* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		adc AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnADC16(uint16* pLeft, uint16* pRight, uint16* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		adc AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnADC32(uint32* pLeft, uint32* pRight, uint32* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		adc EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnADC64(uint64* pLeft, uint64* pRight, uint64* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		adc EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		adc EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnSBB8(uint8* pLeft, uint8* pRight, uint8* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		sbb AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnSBB16(uint16* pLeft, uint16* pRight, uint16* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		sbb AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}


extern "C" uint16 __stdcall fnSBB32(uint32* pLeft, uint32* pRight, uint32* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		sbb EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}


extern "C" uint16 __stdcall fnSBB64(uint64* pLeft, uint64* pRight, uint64* pTarget, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		sbb EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		sbb EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pTarget]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EDX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}


extern "C" uint16 __stdcall fnIMUL8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		imul BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnIMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		mul BX						// DX:AX <- AX * BX
		pushf
		mov EDX, DWORD PTR[pTarget]
		mov WORD PTR[EDX], AX
		CMP AX, 0
		pushf
		pop BX
		pop AX
		and AX, 0xFF3F
		and BX, 0x00C0
		or AX, BX
	}
}

extern "C" uint16 __stdcall fnIMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		mul EBX						// EDX:EAX <- EAX * EBX
		pushf
		mov EDX, DWORD PTR[pTarget]
		mov DWORD PTR[EDX], EAX
		CMP EAX, 0
		pushf
		pop BX
		pop AX
		and AX, 0xFF3F
		and BX, 0x00C0
		or AX, BX
	}
}

extern "C" uint16 __stdcall fnIMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
}

extern "C" uint16 __stdcall fnIDIV8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		xor AX, AX
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		idiv BL
		pushf	// dummy flags
		mov EDX, DWORD PTR[pTarget]
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnIDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		xor DX, DX					// Reset AX to 0
		mov AX, WORD PTR[EBX]		// Load first operand into the AX register (dividend)
		mov BX, WORD PTR[ECX]		// Load second operand into the BX register (divisor)
		idiv BX						// DX (reminder) : AX (quotient) <- AX / BX
		pushf						// Save flags
		mov EBX, DWORD PTR[pTarget]	// Read address of the destination 
		mov WORD PTR[EDX], AX		// Store the result into the destination (Quotient)
		mov WORD PTR[EDX + 2], DX		// Store the result into the destination (Reminder)
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnIDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov ECX, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EDX, EDX				// Reset AX to 0
		mov EAX, DWORD PTR[EBX]		// Load first operand into the AX register (dividend)
		mov EBX, DWORD PTR[ECX]		// Load second operand into the BX register (divisor)
		idiv BX						// EDX (reminder) : EAX (quotient) <- EAX / EBX
		pushf						// Save flags
		mov EBX, DWORD PTR[pTarget]	// Read address of the destination 
		mov DWORD PTR[EDX], EAX		// Store the result into the destination (Quotient)
		mov DWORD PTR[EDX + 4], EDX	// Store the result into the destination (Reminder)
		pop AX						// Return flags in AX
	}
}

extern "C" uint16 __stdcall fnIDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval / rval;
	__asm pushf	// dummy flags
	*pTarget = res;
	__asm pop AX
}

extern "C" uint16 __stdcall fnNEG8(uint8* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AL, BYTE PTR[EDX]
		neg AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnNEG16(uint16* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov AX, WORD PTR[EDX]
		neg AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnNEG32(uint32* pTarget)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EAX, DWORD PTR[EDX]
		neg EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnNEG64(uint64* pTarget)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]	// Read sddress of the operand
		mov EBX, DWORD PTR[EDI]		// Load lower half of the first operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the first operand into the BX register

		test EBX, EBX				// iz lower part is zero?
		jz lower_part_is_zero

		not ECX						// negate lower part only but take care also for the rest
		neg EBX
		pushf						// save flags
		pop AX						// get flags into AX to return later
		and AX, 0x3F				// clear ZF & SF (to be carried from higher part comparison)
		test ECX, ECX				// compare higher part with zero to take ZF
		mov DH, AH					// temporary save high byte of flags
		lahf
		and AH, 0x80
		or AL, AH					// carry ZF into AL
		mov AH, DH					// restore high byte of flags
		jmp finilize

lower_part_is_zero:
		neg ECX						// since lower part is 0 negate only higher part
		pushf						// save flags
		pop AX						// get flags into AX to return later
		or AX, 0x04					// set PF flag since lower part is 0 thus contains even number of 1s

finilize:
		mov DWORD PTR[EDI], EBX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], ECX	// Store upper half of the result into the destination
	}
}

extern "C" uint16 __stdcall fnSHR8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shr AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHR16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shr AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHR32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shr EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHR64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		mov CL, BYTE PTR[EBX]
		shrd EAX, EDX, CL
		pushf
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHL8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shl AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHL16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shl AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHL32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		shl EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSHL64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		mov CL, BYTE PTR[EBX]
		shld EDX, EAX, CL
		pushf
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROR8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		ror AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnROR16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		ror AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROR32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		ror EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROR64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[EBX]
		and CL, 0x1F
		pushf
while_not_0:	// Do rotate via looping by putting CF flag
		test ECX, ECX
		jz finilize
		dec ECX
		pop AX
		shrd EBX, EDX, 1
		pushf
		lahf
		and EAX, 0x00000100
		shl EAX, 23
		and EDX, 0x80000000
		or EDX, EAX
		jmp while_not_0
finilize:
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROL8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rol AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnROL16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rol AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROL32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rol EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnROL64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[EBX]
		and CL, 0x1F
		pushf
while_not_0:	// Do rotate via looping by putting CF flag
		test ECX, ECX
		jz finilize
		dec ECX
		pop AX
		shld EDX, EBX, 1
		pushf
		lahf
		mov al, ah
		and EAX, 0x00000001
		and EBX, 0xFFFFFFFE
		or EBX, EAX
		jmp while_not_0
finilize :
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAR8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sar AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAR16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sar AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAR32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sar EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAR64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[EBX]
		and CL, 0x1F
		mov EAX, EDX
		and EAX, 0x80000000
		pushf
while_not_0:	// Do shift via looping by putting CF flag
		test ECX, ECX
		jz finilize
		dec ECX
		pop SI
		shrd EBX, EDX, 1
		pushf
		or EDX, EAX
		jmp while_not_0
finilize:
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAL8(uint8* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sal AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}


extern "C" uint16 __stdcall fnSAL16(uint16* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sal AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAL32(uint32* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		sal EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnSAL64(uint64* pTarget, uint8* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		mov CL, BYTE PTR[EBX]
		shld EDX, EAX, CL
		pushf
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCR8(uint8* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR [bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcr AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}


extern "C" uint16 __stdcall fnRCR16(uint16* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcr AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCR32(uint32* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcr EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCR64(uint64* pTarget, uint8* pOperand, bool bCarryFlag)
{
		__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]

		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[EBX]
		and CL, 0x1F
		BT EAX, 1	// Load carry bit into CF
		pushf
while_not_0:	// Do rotate via looping by putting CF flag
		test ECX, ECX
		jz finilize
		dec ECX
		pop SI
		BT EAX, 1	// Load carry bit into CF
		rcr EDX, 1
		rcr EBX, 1
		pushf
		lahf		// Save CF into AL
		mov AL, AH
		jmp while_not_0
finilize:
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCL8(uint8* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcl AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
	}
}


extern "C" uint16 __stdcall fnRCL16(uint16* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcl AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCL32(uint32* pTarget, uint8* pOperand, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		BT EAX, 1	// Load carry bit into CF

		mov EDX, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[EBX]
		rcl EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
	}
}

extern "C" uint16 __stdcall fnRCL64(uint64* pTarget, uint8* pOperand, bool bCarryFlag)
{
		__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]

		mov EDI, DWORD PTR[pTarget]
		mov EBX, DWORD PTR[pOperand]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[EBX]
		and CL, 0x1F
		BT EAX, 1	// Load carry bit into CF
		pushf
while_not_0:	// Do rotate via looping by putting CF flag
		test ECX, ECX
		jz finilize
		dec ECX
		pop SI
		BT EAX, 1	// Load carry bit into CF
		rcl EBX, 1
		rcl EDX, 1
		pushf
		lahf		// Save CF into AL
		mov AL, AH
		jmp while_not_0
finilize:
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
	}
}

extern "C" uint16 __stdcall fnTEST8(uint8* pLeft, uint8* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		test AL, BL
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnTEST16(uint16* pLeft, uint16* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		test AX, BX
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnTEST32(uint32* pLeft, uint32* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		test EAX, EBX
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnTEST64(uint64* pLeft, uint64* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		test EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		test EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}

extern "C" uint16 __stdcall fnCMP8(uint8* pLeft, uint8* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AL, BYTE PTR[EBX]
		mov BL, BYTE PTR[ECX]
		cmp AL, BL
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnCMP16(uint16* pLeft, uint16* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov AX, WORD PTR[EBX]
		mov BX, WORD PTR[ECX]
		cmp AX, BX
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnCMP32(uint32* pLeft, uint32* pRight)
{
	__asm
	{
		mov EBX, DWORD PTR[pLeft]
		mov ECX, DWORD PTR[pRight]
		mov EAX, DWORD PTR[EBX]
		mov EBX, DWORD PTR[ECX]
		cmp EAX, EBX
		pushf
		pop AX
	}
}

extern "C" uint16 __stdcall fnCMP64(uint64* pLeft, uint64* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EDX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov EBX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov ECX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		sub EAX, EBX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		sbb EDX, ECX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
	}
}


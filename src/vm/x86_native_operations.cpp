#include <stdint.h>
#include <cstdlib>

// This function converts IA32 Flags located in the AX register to VASM Flags
// Returns converted flags in the AX register
extern "C" uint16_t __stdcall ConvertFlags()
{
	__asm
	{
		xor EBX, EBX	// EBX = 0
		mov BX, AX		// Copy IA32 Flags into EBX
		xor EAX, EAX	// EAX = 0

		// Find out Carry Flag
		mov ECX, 00000001H
		and ECX, EBX
		or  EAX, ECX	// Keep CF in AX bit-0

		// Find out Zero Flag
		mov ECX, 00000040H
		and ECX, EBX
		shr ECX, 5
		or  EAX, ECX	// Keep ZF in AX bit-1

		// Find out Sign Flag
		mov ECX, 00000080H
		and ECX, EBX
		shr ECX, 5
		or  EAX, ECX	// Keep SF in AX bit-2

		// Find out Overflow Flag
		mov ECX, 00000800H
		and ECX, EBX
		shr ECX, 8
		or  EAX, ECX	// Keep OF in AX bit-3
	}
}


//
// Comparison operations
//

extern "C" uint16_t __stdcall fnTEST8(uint8_t* pLeft, uint8_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand into EDI register
		mov AL, BYTE PTR[ESI]		// Load first operand into 8 bit AL register
		mov BL, BYTE PTR[EDI]		// Load second operand into 8 bit BL register
		test AL, BL					// Test operands
		pushf						// Save 16 bit Flags
		pop AX						// Read x86 Flags into AX register (return flags in AX)
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnTEST16(uint16_t* pLeft, uint16_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand into EDI register
		mov AX, WORD PTR[ESI]		// Load first operand into 16 bit AX register
		mov BX, WORD PTR[EDI]		// Load second operand into 16 bit BX register
		test AX, BX					// Test operands
		pushf						// Save 16 bit Flags
		pop AX						// Read x86 Flags into AX register (return flags in AX)
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnTEST32(uint32_t* pLeft, uint32_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand into EDI register
		mov EAX, DWORD PTR[ESI]		// Load first operand into 32 bit EAX register
		mov EBX, DWORD PTR[EDI]		// Load second operand into 32 bit EBX register
		test EAX, EBX				// Test operands
		pushf						// Save 16 bit Flags
		pop AX						// Read x86 Flags into AX register (return flags in AX)
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnTEST64(uint64_t* pLeft, uint64_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		test EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediate flags
		test EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediate flags

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnCMP8(uint8_t* pLeft, uint8_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		cmp AL, BL
		pushf
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnCMP16(uint16_t* pLeft, uint16_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		cmp AX, BX
		pushf
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnCMP32(uint32_t* pLeft, uint32_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		cmp EAX, EBX
		pushf
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnCMP64(uint64_t* pLeft, uint64_t* pRight)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		sub EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediate flags
		sbb EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediate flags

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}


//
// Logical operations
//

extern "C" uint16_t __stdcall fnAND8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first (source) operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second (source) operand into EDI register
		mov EDX, DWORD PTR[pResult]	// Read address of the result (destination) into EDX register
		mov AL, BYTE PTR[ESI]		// Load first operand into the 8 bit AL register
		mov BL, BYTE PTR[EDI]		// Load second operand into the 8 bit BL register
		and AL, BL					// Do the operation
		pushf						// Save 16 bit flags 
		mov BYTE PTR[EDX], AL		// Store the result into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first (source) operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second (source) operand into EDI register
		mov EDX, DWORD PTR[pResult]	// Read address of the result (destination) into EDX register
		mov AX, WORD PTR[ESI]		// Load first operand into the 16 bit AX register
		mov BX, WORD PTR[EDI]		// Load second operand into the 16 bit BX register
		and AX, BX					// Do the operation
		pushf						// Save 16 bit flags 
		mov WORD PTR[EDX], AX		// Store the result into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first (source) operand into ESI register
		mov EDI, DWORD PTR[pRight]	// Read address of the second (source) operand into EDI register
		mov EDX, DWORD PTR[pResult]	// Read address of the result (destination) into EDX register
		mov EAX, DWORD PTR[ESI]		// Load first operand into the 32 bit EAX register
		mov EBX, DWORD PTR[EDI]		// Load second operand into the 32 bit EBX register
		and EAX, EBX				// Do the operation
		pushf						// Save 16 bit flags 
		mov DWORD PTR[EDX], EAX		// Store the result into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first (source) operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second 9source) operand
		
		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		and EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediate flags
		and EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediate flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the result (destination)
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}


extern "C" uint16_t __stdcall fnOR8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		or AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		or AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		or EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		or EAX, ECX					// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		or EBX, EDX					// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnXOR8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		xor AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnXOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		xor AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnXOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		xor EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnXOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		xor EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		xor EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnNAND8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		and AL, BL
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		and AX, BX
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		and EAX, EBX
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		and EAX, ECX				// Do the operation for the lower half
		not EAX
		pushf						// Save first intermediatre flags
		and EBX, EDX				// Do the operation for the upper half
		not EBX
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnNOR8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		or AL, BL
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnNOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		or AX, BX
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		or EAX, EBX
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read address of the first operand
		mov EDI, DWORD PTR[pRight]	// Read address of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the EAX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the EBX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the ECX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the EDX register

		or EAX, ECX					// Do the operation for the lower half
		not EAX
		pushf						// Save first intermediatre flags
		or EBX, EDX					// Do the operation for the upper half
		not EBX
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		xor AX, 0x00C0				// Toggle ZF & SF
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnNOT8(uint8_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		not AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNOT16(uint16_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		not AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNOT32(uint32_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		not EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNOT64(uint64_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]// Read address of the only operand

		mov EAX, DWORD PTR[EDX]		// Load lower half of the operand into the EAX register
		mov EBX, DWORD PTR[EDX + 4]	// Load upper half of the operand into the EBX register

		not EAX						// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		not EBX						// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

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
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}


//
// Shift & Rotate operations
//

extern "C" uint16_t __stdcall fnSHL8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]// Read address of the only operand into EDX register
		mov ESI, DWORD PTR[pCount]	// Read address of the count into ESI register
		mov AL, BYTE PTR[EDX]		// Load operand into 8 bit AL register
		mov CL, BYTE PTR[ESI]		// Load count into 8 bit CL register
		and CL, 0x1F				// Count should be less than 32, Count >= 32 is not supported
		shl AL, CL					// Do the operation
		pushf						// Save 16 bit flags
		mov BYTE PTR[EDX], AL		// Store result into the destination (operand)
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSHL16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]// Read address of the only operand into EDX register
		mov ESI, DWORD PTR[pCount]	// Read address of the count into ESI register
		mov AX, WORD PTR[EDX]		// Load operand into 16 bit AX register
		mov CL, BYTE PTR[ESI]		// Load count into 8 bit CL register
		and CL, 0x1F				// Count should be less than 32, Count >= 32 is not supported
		shl AX, CL					// Do the operation
		pushf						// Save 16 bit flags
		mov WORD PTR[EDX], AX		// Store result into the destination (operand)
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSHL32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]// Read address of the only operand into EDX register
		mov ESI, DWORD PTR[pCount]	// Read address of the count into ESI register
		mov EAX, DWORD PTR[EDX]		// Load operand into 32 bit EAX register
		mov CL, BYTE PTR[ESI]		// Load count into 8 bit CL register
		and CL, 0x1F				// Count should be less than 32, Count >= 32 is not supported
		shl EAX, CL					// Do the operation
		pushf						// Save 16 bit flags
		mov DWORD PTR[EDX], EAX		// Store result into the destination (operand)
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSHL64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]// Read address of the only operand into EDX register
		mov ESI, DWORD PTR[pCount]	// Read address of the count into ESI register
		mov EAX, DWORD PTR[EDX]		// Load lower half of the operand into the EAX register
		mov EBX, DWORD PTR[EDX + 4]	// Load upper half of the operand into the EBX register
		mov CL, BYTE PTR[ESI]		// Load count into 8 bit CL register
		and CL, 0x1F				// Count should be less than 32, Count >= 32 is not supported
		shld EBX, EAX, CL			// Do the operation for the upper half
		pushf						// Save 16 bit flags
		shl EAX, CL					// Do the operation for the lower half
		mov DWORD PTR[EDX], EAX		// Store lower half of the result into the destination (operand)
		mov DWORD PTR[EDX + 4], EBX	// Store upper half of the result into the destination (operand)
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSHR8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shr AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSHR16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shr AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSHR32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shr EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSHR64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov EBX, DWORD PTR[EDX + 4]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shrd EAX, EBX, CL			// Do the operation for the lower half
		pushf
		shr EBX, CL					// Do the operation for the upper half
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EBX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAL8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sal AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnSAL16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sal AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAL32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sal EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAL64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov EBX, DWORD PTR[EDX + 4]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shld EBX, EAX, CL
		pushf
		sal EAX, CL
		mov DWORD PTR[EDX], EAX
		mov DWORD PTR[EDX + 4], EBX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAR8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sar AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAR16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sar AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAR32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		sar EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSAR64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov EBX, DWORD PTR[EDX + 4]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		shrd EAX, EBX, CL
		pushf
		sar EBX, CL
		mov DWORD PTR[EDX], EAX
		mov DWORD PTR[EDX + 4], EBX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROL8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		rol AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROL16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		rol AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROL32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		rol EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROL64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDI, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[ESI]
		and CL, 0x1F
		pushf
	while_not_0 :	// Do rotate via looping by putting CF flag
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
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROR8(uint8_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		ror AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROR16(uint16_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		ror AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROR32(uint32_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		ror EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnROR64(uint64_t* pOperand, uint8_t* pCount)
{
	__asm
	{
		mov EDI, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[ESI]
		and CL, 0x1F
		pushf
	while_not_0 :	// Do rotate via looping by putting CF flag
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
	finilize :
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCL8(uint8_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcl AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnRCL16(uint16_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcl AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCL32(uint32_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcl EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCL64(uint64_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		mov EDI, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[ESI]
		and CL, 0x1F
		BT EAX, 1	// Load carry bit into CF
		pushf
	while_not_0 :	// Do rotate via looping by putting CF flag
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
	finilize :
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCR8(uint8_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AL, BYTE PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcr AL, CL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCR16(uint16_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov AX, WORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcr AX, CL
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCR32(uint32_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EBX, BYTE PTR[bCarryFlag]
		mov EDX, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EAX, DWORD PTR[EDX]
		mov CL, BYTE PTR[ESI]
		and CL, 0x1F
		BT EBX, 1	// Load carry bit into CF
		rcr EAX, CL
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnRCR64(uint64_t* pOperand, uint8_t* pCount, bool bCarryFlag)
{
	__asm
	{
		movzx EAX, BYTE PTR[bCarryFlag]
		mov EDI, DWORD PTR[pOperand]
		mov ESI, DWORD PTR[pCount]
		mov EBX, DWORD PTR[EDI]
		mov EDX, DWORD PTR[EDI + 4]
		movzx ECX, BYTE PTR[ESI]
		and CL, 0x1F
		BT EAX, 1	// Load carry bit into CF
		pushf
		while_not_0 :	// Do rotate via looping by putting CF flag
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
	finilize :
		mov DWORD PTR[EDI], EBX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
		call ConvertFlags
	}
}


//
// Integral arithmetic operations
//

extern "C" uint16_t __stdcall fnADD8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		add AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADD16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		add AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADD32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		add EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADD64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		add EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		adc EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnADC8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		adc AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADC16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		adc AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADC32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		adc EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnADC64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult, bool bCarryFlag)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		movzx ESI, BYTE PTR[bCarryFlag]
		BT ESI, 1	// Load carry bit into CF

		adc EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		adc EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSUB8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		sub AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSUB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		sub AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSUB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		sub EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSUB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		sub EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		sbb EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnSBB8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		sbb AL, BL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnSBB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		sbb AX, BX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnSBB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult, bool bCarryFlag)
{
	__asm
	{
		movzx ECX, BYTE PTR[bCarryFlag]
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EDX, DWORD PTR[pResult]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		BT ECX, 1	// Load carry bit into CF
		sbb EAX, EBX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}


extern "C" uint16_t __stdcall fnSBB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult, bool bCarryFlag)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand

		mov EAX, DWORD PTR[ESI]		// Load lower half of the first operand into the AX register
		mov EBX, DWORD PTR[ESI + 4]	// Load upper half of the first operand into the DX register

		mov ECX, DWORD PTR[EDI]		// Load lower half of the second operand into the AX register
		mov EDX, DWORD PTR[EDI + 4]	// Load upper half of the second operand into the DX register

		movzx ESI, BYTE PTR[bCarryFlag]
		BT ESI, 1	// Load carry bit into CF

		sbb EAX, ECX				// Do the operation for the lower half
		pushf						// Save first intermediatre flags
		sbb EBX, EDX				// Do the operation for the upper half
		pushf						// Save the second intermediatre flags

		mov EDI, DWORD PTR[pResult]	// Read sddress of the destination
		mov DWORD PTR[EDI], EAX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], EBX	// Store upper half of the result into the destination

		pop BX						// Combine flags into single flags
		pop AX						// CF=CF2 PF=PF1 ZF=ZF1&ZF2, SF=SF2 OF=OF2

		mov CX, BX					// PF = PF1
		and CX, 0x0004
		and AX, 0xFFFD
		or AX, CX

		or BX, 0xFFBF				// ZF=ZF1&ZF2
		and AX, BX					// and also return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnNEG8(uint8_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov AL, BYTE PTR[EDX]
		neg AL
		pushf
		mov BYTE PTR[EDX], AL
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNEG16(uint16_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov AX, WORD PTR[EDX]
		neg AX
		pushf
		mov WORD PTR[EDX], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNEG32(uint32_t* pOperand)
{
	__asm
	{
		mov EDX, DWORD PTR[pOperand]
		mov EAX, DWORD PTR[EDX]
		neg EAX
		pushf
		mov DWORD PTR[EDX], EAX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnNEG64(uint64_t* pOperand)
{
	__asm
	{
		mov EDI, DWORD PTR[pOperand]	// Read sddress of the operand
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

	lower_part_is_zero :
		neg ECX						// since lower part is 0 negate only higher part
		pushf						// save flags
		pop AX						// get flags into AX to return later
		or AX, 0x04					// set PF flag since lower part is 0 thus contains even number of 1s

	finilize :
		mov DWORD PTR[EDI], EBX		// Store lower half of the result into the destination
		mov DWORD PTR[EDI + 4], ECX	// Store upper half of the result into the destination
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}


//
// Multiplication and Division
//

extern "C" uint16_t __stdcall fnMUL8(uint8_t* pLeft, uint8_t* pRight, uint16_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		mov AL, BYTE PTR[ESI]		// Load first operand into the AL register
		mov BL, BYTE PTR[EDI]		// Load second operand into the BL register
		mul BL						// AX <- AL * BL
		pushf						// Save flags 
		mov EDI, DWORD PTR[pProduct]// Read address of the destination
		mov WORD PTR[EDI], AX
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnMUL16(uint16_t* pLeft, uint16_t* pRight, uint32_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		mul BX						// DX:AX <- AX * BX
		pushf
		mov EDI, DWORD PTR[pProduct]
		mov WORD PTR[EDI], AX
		mov WORD PTR[EDI + 2], DX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnMUL32(uint32_t* pLeft, uint32_t* pRight, uint64_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		mul EBX						// EDX:EAX <- EAX * EBX
		pushf
		mov EDI, DWORD PTR[pProduct]
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pProduct)
{
	// No ASM code this time ;)
	// AB * CD = (A*2^32 + B) * (C*2^32 + D) = A*C*2^64 + A*D*2^32 + B*C*2^32 + B*D
	uint32_t const uLo1 = static_cast<uint32_t>(*pLeft);		// B
	uint32_t const uHi1 = static_cast<uint32_t>(*pLeft >> 32);	// A

	uint32_t const uLo2 = static_cast<uint32_t>(*pRight);		// D
	uint32_t const uHi2 = static_cast<uint32_t>(*pRight >> 32);	// C

	uint64_t const uL1L2 = static_cast<uint64_t>(uLo1) * static_cast<uint64_t>(uLo2);	// B*D
	uint64_t const uL1H2 = static_cast<uint64_t>(uLo1) * static_cast<uint64_t>(uHi2);	// B*C
	uint64_t const uH1L2 = static_cast<uint64_t>(uHi1) * static_cast<uint64_t>(uLo2);	// A*D
	uint64_t const uH1H2 = static_cast<uint64_t>(uHi1) * static_cast<uint64_t>(uHi2);	// A*C

	uint64_t uResLo = uL1L2;
	uint64_t uResHi = uH1H2;

	uint64_t uTemp = uResLo;
	uResLo += (uL1H2 << 32);
	if (uResLo < uTemp)
		++uResHi;	// Overflow occured

	uTemp = uResLo;
	uResLo += (uH1L2 << 32);
	if (uResLo < uTemp)
		++uResHi;	// Overflow occured

	uResHi += (uH1L2 >> 32);
	uResHi += (uL1H2 >> 32);

	bool const isOverflow = (uResHi != 0);

	// Store the result
	pProduct[0] = uResLo;
	pProduct[1] = uResHi;

	return isOverflow ? 9 : 0;
}

extern "C" uint16_t __stdcall fnDIV8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pQuotient, uint8_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EAX, EAX				// Reset EAX to 0
		xor EBX, EBX				// Reset EBX to 0
		mov AL, BYTE PTR[ESI]		// Load first operand into the AL register (dividend)
		mov BL, BYTE PTR[EDI]		// Load second operand into the BL register (divisor)
		div BL						// AX / BL : AL <- Quotient, AH <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov BYTE PTR[ESI], AL		// Store the quotient into the destination
		mov BYTE PTR[EDI], AH		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnDIV16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pQuotient, uint16_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EAX, EAX				// Reset EAX to 0
		xor EBX, EBX				// Reset EBX to 0
		xor EDX, EDX				// Reset EDX to 0
		mov AX, WORD PTR[ESI]		// Load first operand into the AX register (dividend)
		mov BX, WORD PTR[EDI]		// Load second operand into the BX register (divisor)
		div BX						// DX:AX / BX : AX <- Quotient, DX <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov WORD PTR[ESI], AX		// Store the quotient into the destination
		mov WORD PTR[EDI], DX		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnDIV32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pQuotient, uint32_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EDX, EDX				// Reset EDX to 0
		mov EAX, DWORD PTR[ESI]		// Load first operand into the EAX register (dividend)
		mov EBX, DWORD PTR[EDI]		// Load second operand into the EBX register (divisor)
		div EBX						// EDX:EAX / EBX : EAX <- Quotient, EDX <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov DWORD PTR[ESI], EAX		// Store the quotient into the destination
		mov DWORD PTR[EDI], EDX		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pQuotient, uint64_t* pRemainder)
{
	// This time is simpler ;)
	*pQuotient = *pLeft / *pRight;
	*pRemainder = *pLeft % *pRight;
	return 0;
}

extern "C" uint16_t __stdcall fnIMUL8(uint8_t* pLeft, uint8_t* pRight, uint16_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov AL, BYTE PTR[ESI]
		mov BL, BYTE PTR[EDI]
		imul BL						// AX <- AL * BL
		pushf
		mov EDI, DWORD PTR[pProduct]
		mov WORD PTR[EDI], AX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnIMUL16(uint16_t* pLeft, uint16_t* pRight, uint32_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov AX, WORD PTR[ESI]
		mov BX, WORD PTR[EDI]
		mul BX						// DX:AX <- AX * BX
		pushf
		mov EDI, DWORD PTR[pProduct]
		mov WORD PTR[EDI], AX
		mov WORD PTR[EDI + 2], DX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnIMUL32(uint32_t* pLeft, uint32_t* pRight, uint64_t* pProduct)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]
		mov EDI, DWORD PTR[pRight]
		mov EAX, DWORD PTR[ESI]
		mov EBX, DWORD PTR[EDI]
		mul EBX						// EDX:EAX <- EAX * EBX
		pushf
		mov EDI, DWORD PTR[pProduct]
		mov DWORD PTR[EDI], EAX
		mov DWORD PTR[EDI + 4], EDX
		pop AX
		call ConvertFlags
	}
}

extern "C" uint16_t __stdcall fnIMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pProduct)
{
	// No ASM code this time ;)
	// AB * CD = (A*2^32 + B) * (C*2^32 + D) = A*C*2^64 + A*D*2^32 + B*C*2^32 + B*D
	int64_t const iLeft = *reinterpret_cast<int64_t*>(pLeft);
	int64_t const iRight = *reinterpret_cast<int64_t*>(pRight);
	bool const isNegative = (iLeft < 0) || (iRight < 0);		// Determine results sign

	uint64_t const uLeft = static_cast<uint64_t>(std::abs(iLeft));
	uint64_t const uRight = static_cast<uint64_t>(std::abs(iRight));

	uint32_t const uLo1 = static_cast<uint32_t>(uLeft);			// B
	uint32_t const uHi1 = static_cast<uint32_t>(uLeft >> 32);	// A

	uint32_t const uLo2 = static_cast<uint32_t>(uRight);		// D
	uint32_t const uHi2 = static_cast<uint32_t>(uRight >> 32);	// C

	uint64_t const uL1L2 = static_cast<uint64_t>(uLo1) * static_cast<uint64_t>(uLo2);	// B*D
	uint64_t const uL1H2 = static_cast<uint64_t>(uLo1) * static_cast<uint64_t>(uHi2);	// B*C
	uint64_t const uH1L2 = static_cast<uint64_t>(uHi1) * static_cast<uint64_t>(uLo2);	// A*D
	uint64_t const uH1H2 = static_cast<uint64_t>(uHi1) * static_cast<uint64_t>(uHi2);	// A*C

	uint64_t uResLo = uL1L2;
	uint64_t uResHi = uH1H2;

	uint64_t uTemp = uResLo;
	uResLo += (uL1H2 << 32);
	if (uResLo < uTemp)
		++uResHi;	// Overflow occured

	uTemp = uResLo;
	uResLo += (uH1L2 << 32);
	if (uResLo < uTemp)
		++uResHi;	// Overflow occured

	uResHi += (uH1L2 >> 32);
	uResHi += (uL1H2 >> 32);

	bool const isOverflow = (uResHi != 0);

	// Apply sign
	if (isNegative)
	{
		if (uResLo == 0)
		{
			uResHi = ~uResHi;
			uResHi += 1;
		}
		else
		{
			uResHi = ~uResHi;
			uResLo = ~uResLo;
			uResLo += 1;
		}
	}

	// Store the result
	pProduct[0] = uResLo;
	pProduct[1] = uResHi;

	return isOverflow ? 9 : 0;
}

extern "C" uint16_t __stdcall fnIDIV8(uint8_t* pLeft, uint8_t* pRight, uint8_t* pQuotient, uint8_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EAX, EAX				// Reset EAX to 0
		xor EBX, EBX				// Reset EBX to 0
		mov AL, BYTE PTR[ESI]		// Load first operand into the AL register (dividend)
		mov BL, BYTE PTR[EDI]		// Load second operand into the BL register (divisor)
		idiv BL						// AX / BL : AL <- Quotient, AH <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov BYTE PTR[ESI], AL		// Store the quotient into the destination
		mov BYTE PTR[EDI], AH		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnIDIV16(uint16_t* pLeft, uint16_t* pRight, uint8_t* pQuotient, uint8_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EAX, EAX				// Reset EAX to 0
		xor EBX, EBX				// Reset EBX to 0
		xor EDX, EDX				// Reset EDX to 0
		mov AX, WORD PTR[ESI]		// Load first operand into the AX register (dividend)
		mov BX, WORD PTR[EDI]		// Load second operand into the BX register (divisor)
		idiv BX						// DX:AX / BX : AX <- Quotient, DX <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov WORD PTR[ESI], AX		// Store the quotient into the destination
		mov WORD PTR[EDI], DX		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnIDIV32(uint32_t* pLeft, uint32_t* pRight, uint16_t* pQuotient, uint16_t* pRemainder)
{
	__asm
	{
		mov ESI, DWORD PTR[pLeft]	// Read sddress of the first operand
		mov EDI, DWORD PTR[pRight]	// Read sddress of the second operand
		xor EDX, EDX				// Reset EDX to 0
		mov EAX, DWORD PTR[ESI]		// Load first operand into the EAX register (dividend)
		mov EBX, DWORD PTR[EDI]		// Load second operand into the EBX register (divisor)
		idiv EBX					// EDX:EAX / EBX : EAX <- Quotient, EDX <- Remainder
		pushf						// Save flags
		mov ESI, DWORD PTR[pQuotient]	// Read address of the destination (quotient)
		mov EDI, DWORD PTR[pRemainder]	// Read address of the destination (remainder)
		mov DWORD PTR[ESI], EAX		// Store the quotient into the destination
		mov DWORD PTR[EDI], EDX		// Store the remainder into the destination
		pop AX						// Return flags in AX
		call ConvertFlags			// Convert x86 Flags into VASM flags
	}
}

extern "C" uint16_t __stdcall fnIDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pQuotient, uint64_t* pRemainder)
{
	// This time also is simpler ;)
	*reinterpret_cast<int64_t*>(pQuotient) = *reinterpret_cast<int64_t*>(pLeft) / *reinterpret_cast<int64_t*>(pRight);
	*reinterpret_cast<int64_t*>(pRemainder) = *reinterpret_cast<int64_t*>(pLeft) / *reinterpret_cast<int64_t*>(pRight);
	return 0;
}

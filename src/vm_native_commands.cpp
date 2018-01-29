#include "base_definitions.h"

extern "C" uint16 __stdcall fnAND8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval & rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnAND16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval & rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnAND32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval & rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnAND64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval & rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnXOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval ^ rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnXOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnXOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnXOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval | rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOT8(uint8* pTarget)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	res = ~res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOT16(uint16* pTarget)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	res = ~res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOT32(uint32* pTarget)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	res = ~res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOT64(uint64* pTarget)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	res = ~res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNAND8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = ~(lval & rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNAND16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = (lval & rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNAND32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = (lval & rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNAND64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = (lval & rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOR8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = ~(lval | rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnNOR16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = ~(lval | rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOR32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = ~(lval | rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNOR64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = ~(lval | rval);
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADD8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADD16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADD32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADD64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSUB8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSUB16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSUB32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSUB64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnMUL8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDIV8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	uint32 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnINC8(uint8* pTarget)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	++res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnINC16(uint16* pTarget)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	++res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnINC32(uint32* pTarget)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	++res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnINC64(uint64* pTarget)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	++res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDEC8(uint8* pTarget)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	--res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDEC16(uint16* pTarget)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	--res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDEC32(uint32* pTarget)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	--res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnDEC64(uint64* pTarget)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	--res;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADC8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	int8 lval = (int8) *pLeft;
	int8 rval = (int8) *pRight;
	int8 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADC16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	int16 lval = (int16) *pLeft;
	int16 rval = (int16) *pRight;
	int16 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADC32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	int32 lval = (int32) *pLeft;
	int32 rval = (int32) *pRight;
	int32 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnADC64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval + rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSBB8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	int8 lval = (int8) *pLeft;
	int8 rval = (int8) *pRight;
	int8 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSBB16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	int16 lval = (int16) *pLeft;
	int16 rval = (int16) *pRight;
	int16 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnSBB32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	int32 lval = (int32) *pLeft;
	int32 rval = (int32) *pRight;
	int32 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnSBB64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval - rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnIMUL8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	int8 lval = (int8) *pLeft;
	int8 rval = (int8) *pRight;
	int8 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	int16 lval = (int16) *pLeft;
	int16 rval = (int16) *pRight;
	int16 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	int32 lval = (int32) *pLeft;
	int32 rval = (int32) *pRight;
	int32 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval * rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIDIV8(uint8* pLeft, uint8* pRight, uint8* pTarget)
{
	uint16 flags = 0;
	int8 lval = (int8) *pLeft;
	int8 rval = (int8) *pRight;
	int8 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget)
{
	uint16 flags = 0;
	int16 lval = (int16) *pLeft;
	int16 rval = (int16) *pRight;
	int16 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget)
{
	uint16 flags = 0;
	int32 lval = (int32) *pLeft;
	int32 rval = (int32) *pRight;
	int32 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnIDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget)
{
	uint16 flags = 0;
	int64 lval = (int64) *pLeft;
	int64 rval = (int64) *pRight;
	int64 res = lval / rval;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNEG8(uint8* pTarget)
{
	uint16 flags = 0;
	int8 res = *pTarget;
	res *= -1;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNEG16(uint16* pTarget)
{
	uint16 flags = 0;
	int16 res = *pTarget;
	res *= -1;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNEG32(uint32* pTarget)
{
	uint16 flags = 0;
	int32 res = *pTarget;
	res *= -1;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnNEG64(uint64* pTarget)
{
	uint16 flags = 0;
	int64 res = *pTarget;
	res *= -1;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHR8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	uint8 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHR16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	uint16 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHR32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	uint32 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHR64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	uint64 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHL8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	uint8 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHL16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	uint16 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHL32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	uint32 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSHL64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	uint64 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROR8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	uint8 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROR16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	uint16 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROR32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	uint32 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROR64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	uint64 val = *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROL8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	uint8 res = *pTarget;
	uint8 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROL16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	uint16 res = *pTarget;
	uint16 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROL32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	uint32 res = *pTarget;
	uint32 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnROL64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	uint64 res = *pTarget;
	uint64 val = *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAR8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	int8 res = (int8) *pTarget;
	int8 val = (int8) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAR16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	int16 res = (int16) *pTarget;
	int16 val = (int16) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAR32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	int32 res = (int32) *pTarget;
	int32 val = (int32) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAR64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	int64 res = (int64) *pTarget;
	int64 val = (int64) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAL8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	int8 res = (int8) *pTarget;
	int8 val = (int8) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnSAL16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	int16 res = (int16) *pTarget;
	int16 val = (int16) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAL32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	int32 res = (int32) *pTarget;
	int32 val = (int32) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnSAL64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	int64 res = (int64) *pTarget;
	int64 val = (int64) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCR8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	int8 res = (int8) *pTarget;
	int8 val = (int8) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnRCR16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	int16 res = (int16) *pTarget;
	int16 val = (int16) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCR32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	int32 res = (int32) *pTarget;
	int32 val = (int32) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCR64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	int64 res = (int64) *pTarget;
	int64 val = (int64) *pOperand;
	res = res >> val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCL8(uint8* pTarget, uint8* pOperand)
{
	uint16 flags = 0;
	int8 res = (int8) *pTarget;
	int8 val = (int8) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


extern "C" uint16 __stdcall fnRCL16(uint16* pTarget, uint16* pOperand)
{
	uint16 flags = 0;
	int16 res = (int16) *pTarget;
	int16 val = (int16) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCL32(uint32* pTarget, uint32* pOperand)
{
	uint16 flags = 0;
	int32 res = (int32) *pTarget;
	int32 val = (int32) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnRCL64(uint64* pTarget, uint64* pOperand)
{
	uint16 flags = 0;
	int64 res = (int64) *pTarget;
	int64 val = (int64) *pOperand;
	res = res << val;
	__asm pushf
	*pTarget = res;
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnTEST8(uint8* pLeft, uint8* pRight)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	//uint8 res = lval & rval;
	__asm mov AH, lval
	__asm mov AL, rval
	__asm TEST AH, AL
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnTEST16(uint16* pLeft, uint16* pRight)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	//uint16 res = lval & rval;
	__asm mov AX, lval
	__asm mov BX, rval
	__asm TEST AX, BX
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnTEST32(uint32* pLeft, uint32* pRight)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	//uint32 res = lval & rval;
	__asm mov EAX, lval
	__asm mov EBX, rval
	__asm TEST EAX, EBX
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnTEST64(uint64* pLeft, uint64* pRight)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval & rval;
	(void) res;
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnCMP8(uint8* pLeft, uint8* pRight)
{
	uint16 flags = 0;
	uint8 lval = *pLeft;
	uint8 rval = *pRight;
	uint8 res = lval - rval;
	(void) res;
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnCMP16(uint16* pLeft, uint16* pRight)
{
	uint16 flags = 0;
	uint16 lval = *pLeft;
	uint16 rval = *pRight;
	uint16 res = lval - rval;
	(void) res;
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnCMP32(uint32* pLeft, uint32* pRight)
{
	uint16 flags = 0;
	uint32 lval = *pLeft;
	uint32 rval = *pRight;
	__asm mov EAX, lval
	__asm mov EBX, rval
	__asm cmp EAX, EBX
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}

extern "C" uint16 __stdcall fnCMP64(uint64* pLeft, uint64* pRight)
{
	uint16 flags = 0;
	uint64 lval = *pLeft;
	uint64 rval = *pRight;
	uint64 res = lval - rval;
	(void) res;
	__asm pushf
	__asm pop AX
	__asm mov flags, AX
	return flags;
}


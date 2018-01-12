//
//	Includes
//
#include "vm_executor.h"
#include "vm_definitions.h"

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Assembly functions forward declarations
//	All functions return Flags
//
extern "C" uint16 __stdcall fnAND8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnAND16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnAND32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnAND64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnOR8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnOR16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnOR32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnOR64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnXOR8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnXOR16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnXOR32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnXOR64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnNOT8 (uint8* pTarget);
extern "C" uint16 __stdcall fnNOT16(uint16* pTarget);
extern "C" uint16 __stdcall fnNOT32(uint32* pTarget);
extern "C" uint16 __stdcall fnNOT64(uint64* pTarget);

extern "C" uint16 __stdcall fnNAND8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnNAND16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnNAND32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnNAND64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnNOR8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnNOR16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnNOR32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnNOR64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnADD8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnADD16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnADD32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnADD64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnSUB8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnSUB16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnSUB32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnSUB64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnMUL8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnDIV8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnINC8 (uint8* pTarget);
extern "C" uint16 __stdcall fnINC16(uint16* pTarget);
extern "C" uint16 __stdcall fnINC32(uint32* pTarget);
extern "C" uint16 __stdcall fnINC64(uint64* pTarget);

extern "C" uint16 __stdcall fnDEC8 (uint8* pTarget);
extern "C" uint16 __stdcall fnDEC16(uint16* pTarget);
extern "C" uint16 __stdcall fnDEC32(uint32* pTarget);
extern "C" uint16 __stdcall fnDEC64(uint64* pTarget);

extern "C" uint16 __stdcall fnADC8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnADC16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnADC32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnADC64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnSBB8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnSBB16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnSBB32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnSBB64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnIMUL8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnIMUL16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnIMUL32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnIMUL64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnIDIV8 (uint8* pLeft, uint8* pRight, uint8* pTarget);
extern "C" uint16 __stdcall fnIDIV16(uint16* pLeft, uint16* pRight, uint16* pTarget);
extern "C" uint16 __stdcall fnIDIV32(uint32* pLeft, uint32* pRight, uint32* pTarget);
extern "C" uint16 __stdcall fnIDIV64(uint64* pLeft, uint64* pRight, uint64* pTarget);

extern "C" uint16 __stdcall fnNEG8 (uint8* pTarget);
extern "C" uint16 __stdcall fnNEG16(uint16* pTarget);
extern "C" uint16 __stdcall fnNEG32(uint32* pTarget);
extern "C" uint16 __stdcall fnNEG64(uint64* pTarget);

extern "C" uint16 __stdcall fnSHR8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnSHR16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnSHR32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnSHR64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnSHL8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnSHL16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnSHL32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnSHL64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnROR8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnROR16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnROR32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnROR64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnROL8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnROL16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnROL32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnROL64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnSAR8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnSAR16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnSAR32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnSAR64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnSAL8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnSAL16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnSAL32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnSAL64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnRCR8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnRCR16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnRCR32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnRCR64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnRCL8 (uint8* pTarget, uint8* pOperand);
extern "C" uint16 __stdcall fnRCL16(uint16* pTarget, uint16* pOperand);
extern "C" uint16 __stdcall fnRCL32(uint32* pTarget, uint32* pOperand);
extern "C" uint16 __stdcall fnRCL64(uint64* pTarget, uint64* pOperand);

extern "C" uint16 __stdcall fnTEST8 (uint8* pLeft, uint8* pRight);
extern "C" uint16 __stdcall fnTEST16(uint16* pLeft, uint16* pRight);
extern "C" uint16 __stdcall fnTEST32(uint32* pLeft, uint32* pRight);
extern "C" uint16 __stdcall fnTEST64(uint64* pLeft, uint64* pRight);

extern "C" uint16 __stdcall fnCMP8 (uint8* pLeft, uint8* pRight);
extern "C" uint16 __stdcall fnCMP16(uint16* pLeft, uint16* pRight);
extern "C" uint16 __stdcall fnCMP32(uint32* pLeft, uint32* pRight);
extern "C" uint16 __stdcall fnCMP64(uint64* pLeft, uint64* pRight);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CExecutor Implementation
//
////////////////////////////////////////////////////////////////////////////////
CExecutor::CExecutor(CMemoryPtr pMemory, IPortsPtr pPorts)
	: m_pMemory(pMemory), m_pPorts(pPorts)
{
	if (m_pMemory == nullptr || m_pPorts == nullptr)
		throw base::CException("Failed to initialize IA32 Executor: Invalid argument.");
}

CExecutor::~CExecutor() = default;

//
// Internal routines
//
CExecutor::t_parArgument CExecutor::GetArgument(
	SMachineState& tMachineState,
	SCommandContext& tCommand,
	EArgIndex eArgIdx)
{
	CProcessor::SArgument* pArg = nullptr;
	switch (eArgIdx)
	{
	case EArgIndex::First:
		pArg = &tCommand.tArg1;
		break;
	case EArgIndex::Second:
		pArg = &tCommand.tArg2;
		break;
	case EArgIndex::Third:
		pArg = &tCommand.tArg3;
		break;
	default:
		throw base::CException("Runtime error: invalid argument index.");
	}

	t_parArgument parArg(nullptr, CProcessor::EOpSize::DWord);
	switch (pArg->eType)
	{
	case CProcessor::SArgument::AR:
	{
		if (pArg->nValue < 0 || pArg->nValue >= CProcessor::eAddressRegistersPoolSize)
			throw base::CException("Runtime error: invalid address register index.");
		parArg.first = (void*) &tMachineState.aui32ARPool[pArg->nValue];
		parArg.second = CProcessor::EOpSize::DWord;
		break;
	}
	case CProcessor::SArgument::GPR:
	{
		if (pArg->nValue < 0 || (pArg->nValue + int(tCommand.eOpSize)) > CProcessor::eGeneralPurposeRegisterPoolSize)
			throw base::CException("Runtime error: invalid generic register index.");
		parArg.first = (void*) &tMachineState.aui8GPRPool[pArg->nValue];
		parArg.second = tCommand.eOpSize;
		break;
	}
	case CProcessor::SArgument::Number:
	{
		parArg.first = (void*) &pArg->nValue;
		parArg.second = tCommand.eOpSize;
		parArg.second = CProcessor::EOpSize::DWord;
		break;
	}
	case CProcessor::SArgument::Offset:
	{
		parArg.first = (void*) &pArg->nValue;
		parArg.second = CProcessor::EOpSize::DWord;
		break;
	}
	default:
		throw base::CException("Runtime error: argument is invalid.");
	}

	return parArg;
}

//
// Command handlers
//

void CExecutor::Nop(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// No operation
	(void) tMachineState;	// warning C4100
	(void) tCommand;		// warning C4100
}

void CExecutor::Break(SMachineState& tMachineState, SCommandContext& tCommand)
{
	tMachineState.oFlags.setTrap(true);
	(void) tCommand;	// warning C4100
}


// Execution control
void CExecutor::Jump(SMachineState& tMachineState, SCommandContext& tCommand)
{
	if (tCommand.tArg1.eType == CProcessor::SArgument::Offset)
		tMachineState.nPC += tCommand.tArg1.Number;
	else if (tCommand.tArg1.eType == CProcessor::SArgument::Number)
		tMachineState.nPC = tCommand.tArg1.Number;
	else
		throw base::CException("Runtime error in Jump: Invalid argument.");
}

void CExecutor::Call(SMachineState& tMachineState, SCommandContext& tCommand)
{
	if (tCommand.tArg1.eType == CProcessor::SArgument::Number)
	{
		// Push PC
		tMachineState.nSP -= sizeof(t_index);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in Call: Stack overflow.");
		m_pMemory->At<t_index>(tMachineState.nSP) = tMachineState.nPC;

		// Push SF
		tMachineState.nSP -= sizeof(t_index);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in Call: Stack overflow.");
		m_pMemory->At<t_offset>(tMachineState.nSP) = tMachineState.nSF;

		// Change PC
		tMachineState.nPC = tCommand.tArg1.Number;
		// Change SF
		tMachineState.nSF = tMachineState.nSP;
	}
	else
		throw base::CException("Runtime error in Call: Invalid argument.");
}

void CExecutor::Ret(SMachineState& tMachineState, SCommandContext& tCommand)
{
	(void) tCommand;	// warning C4100
	
	// Pop SF
	if ((tMachineState.nSP + int(sizeof(t_index))) > tMachineState.nStackUBound)
		throw base::CException("Runtime error in Call: Stack underflow.");
	tMachineState.nSF = m_pMemory->cAt<t_offset>(tMachineState.nSP);
	tMachineState.nSP += sizeof(t_index);

	// Pop PC
	if ((tMachineState.nSP + int(sizeof(t_index))) > tMachineState.nStackUBound)
		throw base::CException("Runtime error in Call: Stack underflow.");
	tMachineState.nPC = m_pMemory->cAt<t_index>(tMachineState.nSP);
	tMachineState.nSP += sizeof(t_index);

}

void CExecutor::Exit(SMachineState& tMachineState, SCommandContext& tCommand)
{
	(void) tCommand;	// warning C4100
	
	// Stop running
	tMachineState.bRun = false;
}


// Assignment
void CExecutor::Assign(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get register
	t_parArgument parReg = GetArgument(tMachineState, tCommand, EArgIndex::First);

	// Check value
	if (tCommand.tArg2.eType != CProcessor::SArgument::Number)
		throw base::CException("Runtime error in Assign: 2nd argument is invalid.");

	switch (parReg.second)
	{
	case CProcessor::EOpSize::Byte:
		*reinterpret_cast<t_byte*>(parReg.first) = static_cast<t_byte>(tCommand.tArg2.nValue);
		break;
	case CProcessor::EOpSize::Word:
		*reinterpret_cast<t_word*>(parReg.first) = static_cast<t_word>(tCommand.tArg2.nValue);
		break;
	case CProcessor::EOpSize::DWord:
		*reinterpret_cast<t_dword*>(parReg.first) = static_cast<t_dword>(tCommand.tArg2.nValue);
		break;
	case CProcessor::EOpSize::QWord:
		*reinterpret_cast<t_qword*>(parReg.first) = static_cast<t_qword>(tCommand.tArg2.nValue);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::Move(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get 1st register
	t_parArgument parReg1 = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get 2nd register
	t_parArgument parReg2 = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parReg1.second != parReg2.second)
		throw base::CException("Runtime error in Move: Operand size mismatch.");

	switch (parReg1.second)
	{
	case CProcessor::EOpSize::Byte:
		*reinterpret_cast<t_byte*>(parReg1.first) = *reinterpret_cast<t_byte*>(parReg2.first);
		break;
	case CProcessor::EOpSize::Word:
		*reinterpret_cast<t_word*>(parReg1.first) = *reinterpret_cast<t_word*>(parReg2.first);
		break;
	case CProcessor::EOpSize::DWord:
		*reinterpret_cast<t_dword*>(parReg1.first) = *reinterpret_cast<t_dword*>(parReg2.first);
		break;
	case CProcessor::EOpSize::QWord:
		*reinterpret_cast<t_qword*>(parReg1.first) = *reinterpret_cast<t_qword*>(parReg2.first);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::Swap(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get 1st register
	t_parArgument parReg1 = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get 2nd register
	t_parArgument parReg2 = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parReg1.second != parReg2.second)
		throw base::CException("Runtime error in Move: Operand size mismatch.");

	switch (parReg1.second)
	{
	case CProcessor::EOpSize::Byte:
		std::swap(*reinterpret_cast<t_byte*>(parReg1.first), *reinterpret_cast<t_byte*>(parReg2.first));
		break;
	case CProcessor::EOpSize::Word:
		std::swap(*reinterpret_cast<t_word*>(parReg1.first), *reinterpret_cast<t_word*>(parReg2.first));
		break;
	case CProcessor::EOpSize::DWord:
		std::swap(*reinterpret_cast<t_dword*>(parReg1.first), *reinterpret_cast<t_dword*>(parReg2.first));
		break;
	case CProcessor::EOpSize::QWord:
		std::swap(*reinterpret_cast<t_qword*>(parReg1.first), *reinterpret_cast<t_qword*>(parReg2.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}


// Logical
void CExecutor::And(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnAND8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnAND16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnAND32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnAND64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Or(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnOR8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnOR16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnOR32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnOR64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Xor(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnXOR8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnXOR16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnXOR32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnXOR64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Not(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnNOT8(reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnNOT16(reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnNOT32(reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnNOT64(reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Nand(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnNAND8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnNAND16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnNAND32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnNAND64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Nor(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnNOR8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnNOR16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnNOR32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnNOR64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}


// Unsigned integral arithmetic
void CExecutor::Add(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnADD8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnADD16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnADD32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnADD64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Sub(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSUB8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSUB16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSUB32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSUB64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Mul(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnMUL8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnMUL16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnMUL32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnMUL64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Div(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnDIV8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnDIV16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnDIV32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnDIV64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Inc(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnINC8(reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnINC16(reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnINC32(reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnINC64(reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Dec(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnDEC8(reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnDEC16(reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnDEC32(reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnDEC64(reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}


// Signeg integral arithmetic
void CExecutor::AddC(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnADC8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnADC16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnADC32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnADC64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::SubB(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSBB8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSBB16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSBB32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSBB64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::IMul(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnIMUL8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnIMUL16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnIMUL32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnIMUL64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::IDiv(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::Third);

	if (parOpL.second != parOpR.second || parOpL.second != parTrgt.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnIDIV8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first), reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnIDIV16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first), reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnIDIV32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first), reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnIDIV64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first), reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Neg(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnNEG8(reinterpret_cast<t_byte*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnNEG16(reinterpret_cast<t_word*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnNEG32(reinterpret_cast<t_dword*>(parTrgt.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnNEG64(reinterpret_cast<t_qword*>(parTrgt.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}


// Shift
void CExecutor::ShiftR(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSHR8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSHR16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSHR32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSHR64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::ShiftL(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSHL8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSHL16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSHL32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSHL64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::RotateR(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnROR8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnROR16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnROR32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnROR64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::RotateL(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnROL8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnROL16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnROL32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnROL64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::ShiftAR(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSAR8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSAR16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSAR32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSAR64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);

}

void CExecutor::ShiftAL(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnSAL8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnSAL16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnSAL32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnSAL64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::RotateCR(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnRCR8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnRCR16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnRCR32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnRCR64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::RotateCL(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parArg = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parTrgt.second != parArg.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnRCL8(reinterpret_cast<t_byte*>(parTrgt.first), reinterpret_cast<t_byte*>(parArg.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnRCL16(reinterpret_cast<t_word*>(parTrgt.first), reinterpret_cast<t_word*>(parArg.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnRCL32(reinterpret_cast<t_dword*>(parTrgt.first), reinterpret_cast<t_dword*>(parArg.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnRCL64(reinterpret_cast<t_qword*>(parTrgt.first), reinterpret_cast<t_qword*>(parArg.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}


// Comparison
void CExecutor::Test(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parOpL.second != parOpR.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnTEST8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnTEST16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnTEST32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnTEST64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}

void CExecutor::Cmp(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get left operand
	t_parArgument parOpL = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get right operand
	t_parArgument parOpR = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parOpL.second != parOpR.second)
		throw base::CException("Runtime error: Operand size mismatch.");

	uint16 ui16Flags;
	switch (parOpL.second)
	{
	case CProcessor::EOpSize::Byte:
		ui16Flags = fnCMP8(reinterpret_cast<t_byte*>(parOpL.first), reinterpret_cast<t_byte*>(parOpR.first));
		break;
	case CProcessor::EOpSize::Word:
		ui16Flags = fnCMP16(reinterpret_cast<t_word*>(parOpL.first), reinterpret_cast<t_word*>(parOpR.first));
		break;
	case CProcessor::EOpSize::DWord:
		ui16Flags = fnCMP32(reinterpret_cast<t_dword*>(parOpL.first), reinterpret_cast<t_dword*>(parOpR.first));
		break;
	case CProcessor::EOpSize::QWord:
		ui16Flags = fnCMP64(reinterpret_cast<t_qword*>(parOpL.first), reinterpret_cast<t_qword*>(parOpR.first));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	tMachineState.oFlags.setFlags(ui16Flags);
}


// Stack
void CExecutor::Push(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		tMachineState.nSP -= sizeof(t_byte);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in PUSH: Stack overflow.");
		m_pMemory->At<t_byte>(tMachineState.nSP) = *reinterpret_cast<t_byte*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::Word:
		tMachineState.nSP -= sizeof(t_word);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in PUSH: Stack overflow.");
		m_pMemory->At<t_word>(tMachineState.nSP) = *reinterpret_cast<t_word*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::DWord:
		tMachineState.nSP -= sizeof(t_dword);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in PUSH: Stack overflow.");
		m_pMemory->At<t_dword>(tMachineState.nSP) = *reinterpret_cast<t_dword*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::QWord:
		tMachineState.nSP -= sizeof(t_qword);
		if (tMachineState.nSP < tMachineState.nStackLBound)
			throw base::CException("Runtime error in PUSH: Stack overflow.");
		m_pMemory->At<t_qword>(tMachineState.nSP) = *reinterpret_cast<t_qword*>(parTrgt.first);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::Pop(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);

	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		if ((tMachineState.nSP + int(sizeof(t_byte))) > tMachineState.nStackUBound)
			throw base::CException("Runtime error in POP: Stack underflow.");
		*reinterpret_cast<t_byte*>(parTrgt.first) = m_pMemory->cAt<t_byte>(tMachineState.nSP);
		tMachineState.nSP += sizeof(t_byte);
		break;
	case CProcessor::EOpSize::Word:
		if ((tMachineState.nSP + int(sizeof(t_word))) > tMachineState.nStackUBound)
			throw base::CException("Runtime error in POP: Stack underflow.");
		*reinterpret_cast<t_word*>(parTrgt.first) = m_pMemory->cAt<t_word>(tMachineState.nSP);
		tMachineState.nSP += sizeof(t_word);
		break;
	case CProcessor::EOpSize::DWord:
		if ((tMachineState.nSP + int(sizeof(t_dword))) > tMachineState.nStackUBound)
			throw base::CException("Runtime error in POP: Stack underflow.");
		*reinterpret_cast<t_dword*>(parTrgt.first) = m_pMemory->cAt<t_dword>(tMachineState.nSP);
		tMachineState.nSP += sizeof(t_dword);
		break;
	case CProcessor::EOpSize::QWord:
		if ((tMachineState.nSP + int(sizeof(t_qword))) > tMachineState.nStackUBound)
			throw base::CException("Runtime error in POP: Stack underflow.");
		*reinterpret_cast<t_qword*>(parTrgt.first) = m_pMemory->cAt<t_qword>(tMachineState.nSP);
		tMachineState.nSP += sizeof(t_qword);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::PushF(SMachineState& tMachineState, SCommandContext& tCommand)
{
	(void) tCommand;	// warning C4100

	tMachineState.nSP -= sizeof(uint16);
	if (tMachineState.nSP < tMachineState.nStackLBound)
		throw base::CException("Runtime error in PUSHF: Stack overflow.");
	m_pMemory->At<uint16>(tMachineState.nSP) = tMachineState.oFlags.getFlags();
}

void CExecutor::PopF(SMachineState& tMachineState, SCommandContext& tCommand)
{
	(void) tCommand;	// warning C4100

	if ((tMachineState.nSP + int(sizeof(uint16))) > tMachineState.nStackUBound)
		throw base::CException("Runtime error in POP: Stack underflow.");
	uint16 ui16Flags = m_pMemory->cAt<uint16>(tMachineState.nSP);
	tMachineState.nSP += sizeof(uint16);
	tMachineState.oFlags.setFlags(ui16Flags);
}


// Memory
void CExecutor::Load(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get left operand
	t_parArgument parAddress = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parAddress.second != CProcessor::EOpSize::DWord)
		throw base::CException("Runtime error in LOAD: Address operand size is invalid.");
	t_index nAddress = *reinterpret_cast<t_index*>(parAddress.first);

	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		if ((nAddress + sizeof(t_byte)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in LOAD: Out of memory bounds read.");
		*reinterpret_cast<t_byte*>(parTrgt.first) = m_pMemory->cAt<t_byte>(nAddress);
		break;
	case CProcessor::EOpSize::Word:
		if ((nAddress + sizeof(t_word)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in LOAD: Out of memory bounds read.");
		*reinterpret_cast<t_word*>(parTrgt.first) = m_pMemory->cAt<t_word>(nAddress);
		break;
	case CProcessor::EOpSize::DWord:
		if ((nAddress + sizeof(t_dword)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in LOAD: Out of memory bounds read.");
		*reinterpret_cast<t_dword*>(parTrgt.first) = m_pMemory->cAt<t_dword>(nAddress);
		break;
	case CProcessor::EOpSize::QWord:
		if ((nAddress + sizeof(t_qword)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in LOAD: Out of memory bounds read.");
		*reinterpret_cast<t_qword*>(parTrgt.first) = m_pMemory->cAt<t_qword>(nAddress);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::Store(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get left operand
	t_parArgument parAddress = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	if (parAddress.second != CProcessor::EOpSize::DWord)
		throw base::CException("Runtime error in STORE: Address operand size is invalid.");
	t_index nAddress = *reinterpret_cast<t_index*>(parAddress.first);

	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		if ((nAddress + sizeof(t_byte)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in STORE: Out of memory bounds write.");
		m_pMemory->At<t_byte>(nAddress) = *reinterpret_cast<t_byte*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::Word:
		if ((nAddress + sizeof(t_word)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in STORE: Out of memory bounds write.");
		m_pMemory->At<t_word>(nAddress) = *reinterpret_cast<t_word*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::DWord:
		if ((nAddress + sizeof(t_dword)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in STORE: Out of memory bounds write.");
		m_pMemory->At<t_dword>(nAddress) = *reinterpret_cast<t_dword*>(parTrgt.first);
		break;
	case CProcessor::EOpSize::QWord:
		if ((nAddress + sizeof(t_qword)) > m_pMemory->GetSize())
			throw base::CException("Runtime error in STORE: Out of memory bounds write.");
		m_pMemory->At<t_qword>(nAddress) = *reinterpret_cast<t_qword*>(parTrgt.first);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}


// Input & Output
void CExecutor::In(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parPort = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	t_index nPort = 0;
	if (parPort.second == CProcessor::EOpSize::Word)
		nPort = *reinterpret_cast<t_word*>(parPort.first);
	else if (parPort.second == CProcessor::EOpSize::DWord)
		nPort = *reinterpret_cast<t_dword*>(parPort.first);
	else
		throw base::CException("Runtime error in IO: Port operand size is invalid.");

	CValue tVal;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		tVal = CValue(EType::Byte);
		break;
	case CProcessor::EOpSize::Word:
		tVal = CValue(EType::Word);
		break;
	case CProcessor::EOpSize::DWord:
		tVal = CValue(EType::DWord);
		break;
	case CProcessor::EOpSize::QWord:
		tVal = CValue(EType::QWord);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	m_pPorts->In(nPort, tVal);

	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		*reinterpret_cast<t_byte*>(parTrgt.first) = static_cast<t_byte>(tVal);
		break;
	case CProcessor::EOpSize::Word:
		*reinterpret_cast<t_word*>(parTrgt.first) = static_cast<t_word>(tVal);
		break;
	case CProcessor::EOpSize::DWord:
		*reinterpret_cast<t_dword*>(parTrgt.first) = static_cast<t_dword>(tVal);
		break;
	case CProcessor::EOpSize::QWord:
		*reinterpret_cast<t_qword*>(parTrgt.first) = static_cast<t_qword>(tVal);
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}
}

void CExecutor::Out(SMachineState& tMachineState, SCommandContext& tCommand)
{
	// Get target operand
	t_parArgument parTrgt = GetArgument(tMachineState, tCommand, EArgIndex::First);
	// Get shift operand
	t_parArgument parPort = GetArgument(tMachineState, tCommand, EArgIndex::Second);

	t_index nPort = 0;
	if (parPort.second == CProcessor::EOpSize::Word)
		nPort = *reinterpret_cast<t_word*>(parPort.first);
	else if (parPort.second == CProcessor::EOpSize::DWord)
		nPort = *reinterpret_cast<t_dword*>(parPort.first);
	else
		throw base::CException("Runtime error in IO: Port operand size is invalid.");

	CValue tVal;
	switch (parTrgt.second)
	{
	case CProcessor::EOpSize::Byte:
		tVal = std::move(CValue(*reinterpret_cast<t_byte*>(parTrgt.first)));
		break;
	case CProcessor::EOpSize::Word:
		tVal = std::move(CValue(*reinterpret_cast<t_word*>(parTrgt.first)));
		break;
	case CProcessor::EOpSize::DWord:
		tVal = std::move(CValue(*reinterpret_cast<t_dword*>(parTrgt.first)));
		break;
	case CProcessor::EOpSize::QWord:
		tVal = std::move(CValue(*reinterpret_cast<t_qword*>(parTrgt.first)));
		break;
	default:
		throw base::CException("Runtime error: Unknown operand size.");
	}

	m_pPorts->Out(nPort, tVal);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

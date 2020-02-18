//
//	Includes
//
#include "main_commands.h"
#include <core_memory.h>
#include <base_exception.h>
#include <base_utility.h>

// STL
#include <sstream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CMainCommands Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMainCommands::CMainCommands() : CCommandBase()
{
	Register({t_csz("NOP"), EOpCode::NOP},
			 FuncCmdExec(&CMainCommands::Nop), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("BREAK"), EOpCode::BREAK},
			 FuncCmdExec(&CMainCommands::Exit), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("EXIT"), EOpCode::EXIT},
			 FuncCmdExec(&CMainCommands::Exit), FuncCmdDisasm(&CMainCommands::DisAsm));

	Register({t_csz("JUMP"), EOpCode::JUMPA, EOprType::Reg},
			 FuncCmdExec(&CMainCommands::JumpA), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("JUMP"), t_csz("J"), EOpCode::JUMPR, EOprType::RegImv, EImvType::SNum16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::HasCndtnCode | SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CMainCommands::JumpR), FuncCmdDisasm(&CMainCommands::dasmBranchRel));
	Register({t_csz("CALL"), EOpCode::CALLA, EOprType::Reg},
			 FuncCmdExec(&CMainCommands::CallA), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("CALL"), EOpCode::CALLR, EOprType::RegImv, EImvType::SNum16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CMainCommands::CallR), FuncCmdDisasm(&CMainCommands::dasmBranchRel));
	Register({t_csz("RET"), EOpCode::RET},
			 FuncCmdExec(&CMainCommands::Ret), FuncCmdDisasm(&CMainCommands::DisAsm));

	FuncCmdExec apfnLoad[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Load<uint8>), FuncCmdExec(&CMainCommands::Load<uint16>),
		FuncCmdExec(&CMainCommands::Load<uint32>), FuncCmdExec(&CMainCommands::Load<uint64>) };
	Register({t_csz("LOAD"), EOpCode::LOAD, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnLoad, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnStore[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Store<uint8>), FuncCmdExec(&CMainCommands::Store<uint16>),
		FuncCmdExec(&CMainCommands::Store<uint32>), FuncCmdExec(&CMainCommands::Store<uint64>) };
	Register({t_csz("STORE"), EOpCode::STORE, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnStore, FuncCmdDisasm(&CMainCommands::DisAsm));

	FuncCmdExec apfnLoadRel[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::LoadRel<uint8>), FuncCmdExec(&CMainCommands::LoadRel<uint16>),
		FuncCmdExec(&CMainCommands::LoadRel<uint32>), FuncCmdExec(&CMainCommands::LoadRel<uint64>)};
	Register({t_csz("LDREL"), EOpCode::LDREL, EOprType::Reg, EOprType::Reg, EImvType::SNum32,
			 SCommandMetaInfo::HasOprSize, true},
			 apfnLoadRel, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnStoreRel[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::StoreRel<uint8>), FuncCmdExec(&CMainCommands::StoreRel<uint16>),
		FuncCmdExec(&CMainCommands::StoreRel<uint32>), FuncCmdExec(&CMainCommands::StoreRel<uint64>)};
	Register({t_csz("STREL"), EOpCode::STREL, EOprType::Reg, EOprType::Reg, EImvType::SNum32,
			 SCommandMetaInfo::HasOprSize, true},
			 apfnStoreRel, FuncCmdDisasm(&CMainCommands::DisAsm));

	Register({t_csz("PUSHSF"), EOpCode::PUSHSF},
			 FuncCmdExec(&CMainCommands::PushSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("PUSHSF"), EOpCode::PUSHSF2, EOprType::RegImv, EImvType::Num16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CMainCommands::PushSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("POPSF"), EOpCode::POPSF},
			 FuncCmdExec(&CMainCommands::PopSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("POPSF"), EOpCode::POPSF2, EOprType::RegImv, EImvType::Num16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CMainCommands::PopSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnPushR[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Push<uint8>), FuncCmdExec(&CMainCommands::Push<uint16>), 
		FuncCmdExec(&CMainCommands::Push<uint32>), FuncCmdExec(&CMainCommands::Push<uint64>) };
	Register({t_csz("PUSH"), EOpCode::PUSH, EOprType::Reg, EOprType::Imv, EImvType::Count, SCommandMetaInfo::HasOprSize},
			 apfnPushR, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnPopR[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Pop<uint8>), FuncCmdExec(&CMainCommands::Pop<uint16>), 
		FuncCmdExec(&CMainCommands::Pop<uint32>), FuncCmdExec(&CMainCommands::Pop<uint64>) };
	Register({t_csz("POP"), EOpCode::POP, EOprType::Reg, EOprType::Imv, EImvType::Count, SCommandMetaInfo::HasOprSize},
			 apfnPopR, FuncCmdDisasm(&CMainCommands::DisAsm));
}

CMainCommands::~CMainCommands() = default;

//
//	Executor functions
//

void CMainCommands::Nop(SCommandContext&)
{
	// Do nothing
}

void CMainCommands::Exit(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.bRun = false;
}


//
// Execution control instructions
//
void CMainCommands::JumpA(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.nIP = *tCtxt.operand<t_address>(EOprIdx::First);
}

void CMainCommands::JumpR(SCommandContext& tCtxt)
{
	if (tCtxt.tInfo.eOprSize == EOprSize::Word)
	{
		tCtxt.tCPUState.nIP = static_cast<t_address>(static_cast<int32>(tCtxt.tCPUState.nIP) + static_cast<int32>(*tCtxt.operand<int16>(EOprIdx::First)));
	}
	else
	{
		VASM_CHECK_X(tCtxt.tInfo.eOprSize == EOprSize::DWord, t_csz("Invalid operand size detected for the Relative Jump instruction!"));
		tCtxt.tCPUState.nIP = static_cast<t_address>(static_cast<int32>(tCtxt.tCPUState.nIP) + *tCtxt.operand<int32>(EOprIdx::First));
	}
}

void CMainCommands::CallA(SCommandContext& tCtxt)
{
	// Save return address into RIP
	tCtxt.tCPUState.nRIP = tCtxt.tCPUState.nIP;
	// Change IP
	tCtxt.tCPUState.nIP = *tCtxt.operand<t_address>(EOprIdx::First);
}

void CMainCommands::CallR(SCommandContext& tCtxt)
{
	// Save return address into RIP
	tCtxt.tCPUState.nRIP = tCtxt.tCPUState.nIP;
	// Change IP
	if (tCtxt.tInfo.eOprSize == EOprSize::Word)
	{
		tCtxt.tCPUState.nIP = static_cast<t_address>(static_cast<int32>(tCtxt.tCPUState.nIP) + static_cast<int32>(*tCtxt.operand<int16>(EOprIdx::First)));
	}
	else
	{
		VASM_CHECK_X(tCtxt.tInfo.eOprSize == EOprSize::DWord, t_csz("Invalid operand size detected for the Relative Call instruction!"));
		tCtxt.tCPUState.nIP = static_cast<t_address>(static_cast<int32>(tCtxt.tCPUState.nIP) + *tCtxt.operand<int32>(EOprIdx::First));
	}
}

void CMainCommands::Ret(SCommandContext& tCtxt)
{
	// Restore IP from RIP
	tCtxt.tCPUState.nIP = tCtxt.tCPUState.nRIP;
}



//
// Memory access instructions
//
template <typename TOperandType>
void CMainCommands::Load(SCommandContext& tCtxt)
{
	bool bAlignmentCheck = !(tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSP || tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSF);
	tCtxt.oMemory.ReadAt<TOperandType>(*tCtxt.operand<t_address>(EOprIdx::Second), *tCtxt.operand<TOperandType>(EOprIdx::First), bAlignmentCheck);
}

template <typename TOperandType>
void CMainCommands::Store(SCommandContext& tCtxt)
{
	bool bAlignmentCheck = !(tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSP || tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSF);
	tCtxt.oMemory.WriteAt<TOperandType>(*tCtxt.operand<t_address>(EOprIdx::Second), *tCtxt.operand<TOperandType>(EOprIdx::First), bAlignmentCheck);
}

template <typename TOperandType>
void CMainCommands::LoadRel(SCommandContext& tCtxt)
{
	// Calculate effective address
	t_address const nEffectiveAddress = static_cast<t_address>(*tCtxt.operand<int32>(EOprIdx::Second) + *tCtxt.operand<int32>(EOprIdx::Third));
	bool bAlignmentCheck = !(tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSP || tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSF);
	tCtxt.oMemory.ReadAt<TOperandType>(nEffectiveAddress, *tCtxt.operand<TOperandType>(EOprIdx::First), bAlignmentCheck);
}

template <typename TOperandType>
void CMainCommands::StoreRel(SCommandContext& tCtxt)
{
	// Read effective address
	t_address const nEffectiveAddress = static_cast<t_address>(*tCtxt.operand<int32>(EOprIdx::Second) + *tCtxt.operand<int32>(EOprIdx::Third));
	bool bAlignmentCheck = !(tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSP || tCtxt.apOperands[EOprIdx::Second] == &tCtxt.tCPUState.nSF);
	tCtxt.oMemory.WriteAt<TOperandType>(nEffectiveAddress, *tCtxt.operand<TOperandType>(EOprIdx::First), bAlignmentCheck);
}

//
// Stack instructions
//
void CMainCommands::PushSF(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSP - 2 * sizeof(t_address)) < tCtxt.tCPUState.cnStackUBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));

	// Push RIP
	tCtxt.tCPUState.nSP -= sizeof(t_address);
	tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nRIP, false);
	// Push stack frame
	tCtxt.tCPUState.nSP -= sizeof(t_address);
	tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF, false);
	// Change SF
	tCtxt.tCPUState.nSF = tCtxt.tCPUState.nSP;
	// Allocate space on the the stack if specified
	if (tCtxt.tInfo.tMetaInfo.nOperandCount > 0)
	{
		t_uoffset nOffset;
		if (tCtxt.tInfo.eOprSize == EOprSize::Word)
		{
			nOffset = static_cast<t_uoffset>(*tCtxt.operand<uint16>(EOprIdx::First));
		}
		else
		{
			VASM_CHECK_X(tCtxt.tInfo.eOprSize == EOprSize::DWord, t_csz("Invalid operand size detected for the PushSF instruction!"));
			nOffset = static_cast<t_uoffset>(*tCtxt.operand<uint32>(EOprIdx::First));
		}

		if (tCtxt.tCPUState.nSP - nOffset < tCtxt.tCPUState.cnStackUBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack overflow in stack frame creation"));
		tCtxt.tCPUState.nSP -= nOffset;
	}
}

void CMainCommands::PopSF(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSF + 2 * sizeof(t_address)) > tCtxt.tCPUState.cnStackLBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));

	// Change SP to point to SF
	tCtxt.tCPUState.nSP = tCtxt.tCPUState.nSF;
	// Pop stack frame
	tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF, false);
	tCtxt.tCPUState.nSP += sizeof(t_address);
	// Pop RIP
	tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nRIP, false);
	tCtxt.tCPUState.nSP += sizeof(t_address);
	// Do stack cleanup
	if (tCtxt.tInfo.tMetaInfo.nOperandCount > 0)
	{
		t_uoffset nOffset;
		if (tCtxt.tInfo.eOprSize == EOprSize::Word)
		{
			nOffset = static_cast<t_uoffset>(*tCtxt.operand<uint16>(EOprIdx::First));
		}
		else
		{
			VASM_CHECK_X(tCtxt.tInfo.eOprSize == EOprSize::DWord, t_csz("Invalid operand size detected for the PopSF instruction!"));
			nOffset = static_cast<t_uoffset>(*tCtxt.operand<uint32>(EOprIdx::First));
		}

		if (tCtxt.tCPUState.nSP + nOffset > tCtxt.tCPUState.cnStackLBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack underflow on stack frame cleanup"));
		tCtxt.tCPUState.nSP += nOffset;
	}
}

template <typename TOperandType>
void CMainCommands::Push(SCommandContext& tCtxt)
{
	uint nCount = uint(*tCtxt.operand<uint8>(EOprIdx::Second));
	if (nCount == 0)
	{	// Push single register
		if ((tCtxt.tCPUState.nSP - sizeof(TOperandType)) < tCtxt.tCPUState.cnStackUBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));
		tCtxt.tCPUState.nSP -= sizeof(TOperandType);
		tCtxt.oMemory.WriteAt<TOperandType>(tCtxt.tCPUState.nSP, *tCtxt.operand<TOperandType>(EOprIdx::First), false);
	}
	else
	{	// Push nCount Registers
		// For GP registers multiply Reg idx to align with OpSize
		uint nRegIdx = AlignToOperandSize(tCtxt.tInfo.anRegIdx[EOprIdx::First], tCtxt.tInfo.eOprSize);
		uint nCountBuytes = nCount * OperandSize(tCtxt.tInfo.eOprSize);
		if (nRegIdx + nCountBuytes > SCPUStateBase::eRegisterPoolSize)
			VASM_THROW_ERROR(t_csz("CPU: GP Registers pool size excceded"));
		if ((tCtxt.tCPUState.nSP - nCountBuytes) < tCtxt.tCPUState.cnStackUBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));
		// Push GP registers
		tCtxt.tCPUState.nSP -= nCountBuytes;
		tCtxt.oMemory.Write<TOperandType>(tCtxt.tCPUState.nSP, tCtxt.operand<TOperandType>(EOprIdx::First), nCount, false);
	}
}

template <typename TOperandType>
void CMainCommands::Pop(SCommandContext& tCtxt)
{
	uint nCount = uint(*tCtxt.operand<uint8>(EOprIdx::Second));
	if (nCount == 0)
	{	// Pop single register
		if ((tCtxt.tCPUState.nSP + sizeof(TOperandType)) > tCtxt.tCPUState.cnStackLBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
		tCtxt.oMemory.ReadAt<TOperandType>(tCtxt.tCPUState.nSP, *tCtxt.operand<TOperandType>(EOprIdx::First), false);
		tCtxt.tCPUState.nSP += sizeof(TOperandType);
	}
	else
	{	// Pop nCount Registers
		// For GP registers multiply Reg idx to align with OpSize
		uint nRegIdx = AlignToOperandSize(tCtxt.tInfo.anRegIdx[EOprIdx::First], tCtxt.tInfo.eOprSize);
		uint nCountBuytes = nCount * OperandSize(tCtxt.tInfo.eOprSize);
		if (nRegIdx + nCountBuytes > SCPUStateBase::eRegisterPoolSize)
			VASM_THROW_ERROR(t_csz("CPU: GP Registers pool size excceded"));
		if ((tCtxt.tCPUState.nSP + nCountBuytes) > tCtxt.tCPUState.cnStackLBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
		// Pop GP registers
		tCtxt.oMemory.Read<TOperandType>(tCtxt.tCPUState.nSP, tCtxt.operand< TOperandType>(EOprIdx::First), nCount, false);
		tCtxt.tCPUState.nSP += nCountBuytes;
	}
}

//
//	Disassembly functions
//
t_string CMainCommands::dasmBranchRel(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd;
	core::IAddressRecoveryPtr pAddrRec = GetAddressRecovery();
	if (pAddrRec == nullptr)
		sCmd = Base::DisAsm(tCmd, bHexadecimal);
	else
	{
		// Command name
		sCmd = DisAsmCmd(tCmd);

		if (tCmd.eOprSwitch == EOprSwitch::Imv)
		{
			t_address nAddress = (t_address) (int64(tCmd.nAddress) + int64(tCmd.tMetaInfo.nLength) + int64(tCmd.i16Imv));
			// Get symbol from address
			t_string sSymbol = pAddrRec->ToSymbol(nAddress);
			if (sSymbol.empty() || (sSymbol.size() > 2 && sSymbol[0] == t_char('0') && sSymbol[1] == t_char('x')))
			{
				std::stringstream temp;
				if (bHexadecimal)
					temp << std::hex << std::uppercase << std::setw(4) << "0x";
				else if (tCmd.i16Imv > 0)
					temp << "+";
				temp << tCmd.i16Imv;
				sSymbol = temp.str();
			}

			sCmd += t_csz(" ");
			sCmd += sSymbol;
		}
		else
		{
			sCmd += DisAsmArgs(tCmd, bHexadecimal);
		}
	}
	return std::move(sCmd);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

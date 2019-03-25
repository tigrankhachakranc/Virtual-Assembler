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

	Register({t_csz("JUMP"), EOpCode::JUMPA, EOprType::AR},
			 FuncCmdExec(&CMainCommands::JumpA), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("JUMP"), t_csz("J"), EOpCode::JUMPR, EOprType::GRIMV, EImvType::SNum16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::HasCndtnCode | SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CMainCommands::JumpR), FuncCmdDisasm(&CMainCommands::dasmJumpR));
	Register({t_csz("CALL"), EOpCode::CALL, EOprType::AR},
			 FuncCmdExec(&CMainCommands::Call), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("RET"), EOpCode::RET},
			 FuncCmdExec(&CMainCommands::Ret), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("RET"), EOpCode::RET2, EOprType::GRIMV, EImvType::Num16,
			 SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CMainCommands::Ret2), FuncCmdDisasm(&CMainCommands::DisAsm));

	Register({t_csz("GFLR"), EOpCode::GFLR, EOprType::GR, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CMainCommands::GFLR), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("SFLR"), EOpCode::SFLR, EOprType::GR, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CMainCommands::SFLR), FuncCmdDisasm(&CMainCommands::DisAsm));

	FuncCmdExec apfnLoad[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Load<uint8>), FuncCmdExec(&CMainCommands::Load<uint16>),
		FuncCmdExec(&CMainCommands::Load<uint32>), FuncCmdExec(&CMainCommands::Load<uint64>) };
	Register({t_csz("LOAD"), EOpCode::LOAD, EOprType::AGR, EOprType::AR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
			 apfnLoad, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnStore[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::Store<uint8>), FuncCmdExec(&CMainCommands::Store<uint16>),
		FuncCmdExec(&CMainCommands::Store<uint32>), FuncCmdExec(&CMainCommands::Store<uint64>) };
	Register({t_csz("STORE"), EOpCode::STORE, EOprType::AGR, EOprType::AR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
			 apfnStore, FuncCmdDisasm(&CMainCommands::DisAsm));

	FuncCmdExec apfnLoadRel[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::LoadRel<uint8>), FuncCmdExec(&CMainCommands::LoadRel<uint16>),
		FuncCmdExec(&CMainCommands::LoadRel<uint32>), FuncCmdExec(&CMainCommands::LoadRel<uint64>)};
	Register({t_csz("LDREL"), EOpCode::LDREL, EOprType::AGR, EOprType::AR, EImvType::SNum32,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch, true},
			 apfnLoadRel, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnStoreRel[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::StoreRel<uint8>), FuncCmdExec(&CMainCommands::StoreRel<uint16>),
		FuncCmdExec(&CMainCommands::StoreRel<uint32>), FuncCmdExec(&CMainCommands::StoreRel<uint64>)};
	Register({t_csz("STREL"), EOpCode::STREL, EOprType::AGR, EOprType::AR, EImvType::SNum32,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch, true},
			 apfnStoreRel, FuncCmdDisasm(&CMainCommands::DisAsm));

	Register({t_csz("PUSH"), EOpCode::PUSHA, EOprType::AR},
			 FuncCmdExec(&CMainCommands::PushA), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("POP"), EOpCode::POPA, EOprType::AR},
			 FuncCmdExec(&CMainCommands::PopA), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("PUSHSF"), EOpCode::PUSHSF},
			 FuncCmdExec(&CMainCommands::PushSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	Register({t_csz("POPSF"), EOpCode::POPSF},
			 FuncCmdExec(&CMainCommands::PopSF), FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnPushR[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::PushR<uint8>), FuncCmdExec(&CMainCommands::PushR<uint16>), 
		FuncCmdExec(&CMainCommands::PushR<uint32>), FuncCmdExec(&CMainCommands::PushR<uint64>) };
	Register({t_csz("PUSH"), EOpCode::PUSHR, EOprType::GR, EOprType::IMV, EImvType::Count, SCommandMetaInfo::HasOprSize},
			 apfnPushR, FuncCmdDisasm(&CMainCommands::DisAsm));
	FuncCmdExec apfnPopR[int(EOprSize::Count)] = {
		FuncCmdExec(&CMainCommands::PopR<uint8>), FuncCmdExec(&CMainCommands::PopR<uint16>), 
		FuncCmdExec(&CMainCommands::PopR<uint32>), FuncCmdExec(&CMainCommands::PopR<uint64>) };
	Register({t_csz("POP"), EOpCode::POPR, EOprType::GR, EOprType::IMV, EImvType::Count, SCommandMetaInfo::HasOprSize},
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
	tCtxt.tCPUState.nIP = *tCtxt.tOpr[EOprIdx::First].ptr<t_address>();
}

void CMainCommands::JumpR(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.nIP += static_cast<t_address>(*tCtxt.tOpr[EOprIdx::First].pu16);
}

void CMainCommands::Call(SCommandContext& tCtxt)
{
	// Save return address into RIP
	tCtxt.tCPUState.nRIP = tCtxt.tCPUState.nIP;
	// Change IP
	tCtxt.tCPUState.nIP = *tCtxt.tOpr[EOprIdx::First].ptr<t_address>();

	//if ((tCtxt.tCPUState.nSP - 2 * sizeof(t_address)) < tCtxt.tCPUState.cnStackUBound)
	//	VASM_THROW_ERROR(t_csz("CPU: Stack overflow on Call"));
	//// Push IP
	//tCtxt.tCPUState.nSP -= sizeof(t_address);
	//tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nIP);
	//// Push stack frame
	//tCtxt.tCPUState.nSP -= sizeof(t_address);
	//tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF);
	//// Change IP & SF
	//tCtxt.tCPUState.nIP = *tCtxt.tOpr[EOprIdx::First].ptr<t_address>();
	//tCtxt.tCPUState.nSF = tCtxt.tCPUState.nSP;
}

void CMainCommands::Ret(SCommandContext& tCtxt)
{
	// Restore IP from RIP
	tCtxt.tCPUState.nIP = tCtxt.tCPUState.nRIP;

	//if ((tCtxt.tCPUState.nSP + 2 * sizeof(t_address)) > tCtxt.tCPUState.cnStackLBound)
	//	VASM_THROW_ERROR(t_csz("CPU: Stack underflow on Ret"));
	//// Pop stack frame
	//tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF);
	//tCtxt.tCPUState.nSP += sizeof(t_address);
	//// Pop IP
	//tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nIP);
	//tCtxt.tCPUState.nSP += sizeof(t_address);
}

void CMainCommands::Ret2(SCommandContext& tCtxt)
{
	// Call regular Ret
	Ret(tCtxt);
	// Do stack cleanup
	if (tCtxt.tCPUState.nSP + *tCtxt.tOpr[EOprIdx::First].pu16 > tCtxt.tCPUState.cnStackLBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack underflow on Ret (stack cleanup)"));
	tCtxt.tCPUState.nSP += sizeof(t_address);
}


//
//	Flags manipulation instructions
//
void CMainCommands::GFLR(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].pu16 = tCtxt.tCPUState.oFlags.getFlags();
}

void CMainCommands::SFLR(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.oFlags.setFlags(*tCtxt.tOpr[EOprIdx::First].pu16);
}


//
// Memory access instructions
//
template <typename TOperandType>
void CMainCommands::Load(SCommandContext& tCtxt)
{
	tCtxt.oMemory.ReadAt<TOperandType>(*tCtxt.tOpr[EOprIdx::Second].ptr<t_address>(), *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
}

template <typename TOperandType>
void CMainCommands::Store(SCommandContext& tCtxt)
{
	tCtxt.oMemory.WriteAt<TOperandType>(*tCtxt.tOpr[EOprIdx::Second].ptr<t_address>(), *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
}

template <typename TOperandType>
void CMainCommands::LoadRel(SCommandContext& tCtxt)
{
	// Read effective address
	t_address nEffectiveAddress = *tCtxt.tOpr[EOprIdx::Second].ptr<t_address>() + *tCtxt.tOpr[EOprIdx::Third].pi32;
	tCtxt.oMemory.ReadAt<TOperandType>(nEffectiveAddress, *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
}

template <typename TOperandType>
void CMainCommands::StoreRel(SCommandContext& tCtxt)
{
	// Read effective address
	t_address nEffectiveAddress = *tCtxt.tOpr[EOprIdx::Second].ptr<t_address>() + *tCtxt.tOpr[EOprIdx::Third].pi32;
	tCtxt.oMemory.WriteAt<TOperandType>(nEffectiveAddress, *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
}

//
// Stack instructions
//
void CMainCommands::PushA(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSP - sizeof(t_address)) < tCtxt.tCPUState.cnStackUBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));
	tCtxt.tCPUState.nSP -= sizeof(t_address);
	tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, *tCtxt.tOpr[EOprIdx::First].ptr<t_address>());
}

void CMainCommands::PopA(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSP + sizeof(t_address)) > tCtxt.tCPUState.cnStackLBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
	tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, *tCtxt.tOpr[EOprIdx::First].ptr<t_address>());
	tCtxt.tCPUState.nSP += sizeof(t_address);
}

void CMainCommands::PushSF(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSP - 2 * sizeof(t_address)) < tCtxt.tCPUState.cnStackUBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));

	// Push RIP
	tCtxt.tCPUState.nSP -= sizeof(t_address);
	tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nRIP);
	// Push stack frame
	tCtxt.tCPUState.nSP -= sizeof(t_address);
	tCtxt.oMemory.WriteAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF);
	// Change SF
	tCtxt.tCPUState.nSF = tCtxt.tCPUState.nSP;
}

void CMainCommands::PopSF(SCommandContext& tCtxt)
{
	if ((tCtxt.tCPUState.nSP + 2 * sizeof(t_address)) > tCtxt.tCPUState.cnStackLBound)
		VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
	
	// Change SP to point to SF
	tCtxt.tCPUState.nSP = tCtxt.tCPUState.nSF;
	// Pop stack frame
	tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nSF);
	tCtxt.tCPUState.nSP += sizeof(t_address);
	// Pop RIP
	tCtxt.oMemory.ReadAt<t_address>(tCtxt.tCPUState.nSP, tCtxt.tCPUState.nRIP);
	tCtxt.tCPUState.nSP += sizeof(t_address);
}

template <typename TOperandType>
void CMainCommands::PushR(SCommandContext& tCtxt)
{
	uint nCount = uint(*tCtxt.tOpr[EOprIdx::Second].pu8);
	if (nCount == 0)
	{	// Push single register
		if ((tCtxt.tCPUState.nSP - sizeof(TOperandType)) < tCtxt.tCPUState.cnStackUBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));
		tCtxt.tCPUState.nSP -= sizeof(TOperandType);
		tCtxt.oMemory.WriteAt<TOperandType>(tCtxt.tCPUState.nSP, *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
	}
	else
	{	// Push nCount Registers
		// For GP registers multiply Reg idx to align with OpSize
		uint nRegIdx = AlignToOperandSize(tCtxt.tInfo.nRegIdx[EOprIdx::First], tCtxt.tInfo.eOprSize);
		uint nCountBuytes = nCount * OperandSize(tCtxt.tInfo.eOprSize);
		if (nRegIdx + nCountBuytes > SCPUStateBase::eGeneralPurposeRegisterPoolSize)
			VASM_THROW_ERROR(t_csz("CPU: GP Registers pool size excceded"));
		if ((tCtxt.tCPUState.nSP - nCountBuytes) < tCtxt.tCPUState.cnStackUBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack overflow"));
		// Push GP registers
		tCtxt.tCPUState.nSP -= nCountBuytes;
		tCtxt.oMemory.Write<TOperandType>(tCtxt.tCPUState.nSP, tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>(), nCount);
	}
}

template <typename TOperandType>
void CMainCommands::PopR(SCommandContext& tCtxt)
{
	uint nCount = uint(*tCtxt.tOpr[EOprIdx::Second].pu8);
	if (nCount == 0)
	{	// Pop single register
		if ((tCtxt.tCPUState.nSP + sizeof(TOperandType)) > tCtxt.tCPUState.cnStackLBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
		tCtxt.oMemory.ReadAt<TOperandType>(tCtxt.tCPUState.nSP, *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
		tCtxt.tCPUState.nSP += sizeof(TOperandType);
	}
	else
	{	// Pop nCount Registers
		// For GP registers multiply Reg idx to align with OpSize
		uint nRegIdx = AlignToOperandSize(tCtxt.tInfo.nRegIdx[EOprIdx::First], tCtxt.tInfo.eOprSize);
		uint nCountBuytes = nCount * OperandSize(tCtxt.tInfo.eOprSize);
		if (nRegIdx + nCountBuytes > SCPUStateBase::eGeneralPurposeRegisterPoolSize)
			VASM_THROW_ERROR(t_csz("CPU: GP Registers pool size excceded"));
		if ((tCtxt.tCPUState.nSP + nCountBuytes) > tCtxt.tCPUState.cnStackLBound)
			VASM_THROW_ERROR(t_csz("CPU: Stack underflow"));
		// Pop GP registers
		tCtxt.oMemory.Read<TOperandType>(tCtxt.tCPUState.nSP, tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>(), nCount);
		tCtxt.tCPUState.nSP += nCountBuytes;
	}
}

//
//	Disassembly functions
//
t_string CMainCommands::dasmJumpR(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd;
	core::IAddressRecoveryPtr pAddrRec = GetAddressRecovery();
	if (pAddrRec == nullptr)
		sCmd = Base::DisAsm(tCmd, bHexadecimal);
	else
	{
		// Command name
		sCmd = DisAsmCmd(tCmd);

		if (tCmd.bOprSwitch1)
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

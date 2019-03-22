//
// Includes
//
#include "command.h"
#include <base_exception.h>
#include <base_utility.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SCommandMetaInfo implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SCommandMetaInfo::SCommandMetaInfo()
{
	pcszName = nullptr;
	pcszBaseName = nullptr;
	eOpCode = EOpCode::Invalid;
	eExtInfo = NoExtension;
	nOperandCount = 0;
	aeOprTypes[0] = EOprType::None;
	aeOprTypes[1] = EOprType::None;
	aeOprTypes[2] = EOprType::None;
	eImvType = EImvType::None;
	nLength = 0;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, EOpCode opc, uchar ext)
{
	pcszName = name;
	pcszBaseName = nullptr;
	eOpCode = opc;
	eExtInfo = ext;
	nOperandCount = 0;
	aeOprTypes[0] = EOprType::None;
	aeOprTypes[1] = EOprType::None;
	aeOprTypes[2] = EOprType::None;
	eImvType = EImvType::None;
	nLength = 2;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, EOpCode opc, EOprType op1, uchar ext)
{
	pcszName = name;
	pcszBaseName = nullptr;
	eOpCode = opc;
	eExtInfo = ext;
	nOperandCount = 1;
	aeOprTypes[0] = op1;
	aeOprTypes[1] = EOprType::None;
	aeOprTypes[2] = EOprType::None;
	eImvType = EImvType::None;
	nLength = bool(ext & MaskExtension) ? 4 : 2;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, EOpCode opc, EOprType op1, EImvType imv, uchar ext)
{
	pcszName = name;
	pcszBaseName = nullptr;
	eOpCode = opc;
	eExtInfo = ext;
	nOperandCount = 1;
	aeOprTypes[0] = op1;
	aeOprTypes[1] = EOprType::None;
	aeOprTypes[2] = EOprType::None;
	eImvType = imv;

	switch (imv)
	{
	case EImvType::None:
		nLength = bool(ext & MaskExtension) ? 4 : 2; // bad case
		break;
	case EImvType::Num8:
	case EImvType::SNum8:
		nLength = bool(ext & MaskExtension) ? 4 : 2;
		break;
	case EImvType::Num16:
	case EImvType::SNum16:
		nLength = 4;
		break;
	case EImvType::Num32:
	case EImvType::SNum32:
		nLength = 6;
		break;
	case EImvType::Num64:
	case EImvType::SNum64:
		nLength = 10;
		break;
	case EImvType::Count:
		VASM_THROW_INVALID_CONDITION(); // bad case
		break;
	case EImvType::Port:
		VASM_THROW_INVALID_CONDITION(); // bad case
		break;
	case EImvType::Index:
		VASM_THROW_INVALID_CONDITION(); // bad case
		break;
	default:
		VASM_THROW_INVALID_CONDITION();
	}
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, EOpCode opc, EOprType op1, EOprType op2, uchar ext)
{
	pcszName = name;
	pcszBaseName = nullptr;
	eOpCode = opc;
	eExtInfo = ext;
	nOperandCount = 2;
	aeOprTypes[0] = op1;
	aeOprTypes[1] = op2;
	aeOprTypes[2] = EOprType::None;
	eImvType = EImvType::None;
	nLength = 4;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, EOpCode opc, EOprType op1, EOprType op2, EImvType imv, uchar ext, bool opr3)
{
	pcszName = name;
	pcszBaseName = nullptr;
	eOpCode = opc;
	eExtInfo = ext;
	nOperandCount = opr3 ? 3 : 2;
	aeOprTypes[0] = op1;
	aeOprTypes[1] = op2;
	aeOprTypes[2] = opr3 ? EOprType::IMV : EOprType::None;
	eImvType = imv;

	switch (imv)
	{
	case EImvType::None:
		nLength = 4;
		break;
	case EImvType::Num8:
	case EImvType::SNum8:
		nLength = 4;
		break;
	case EImvType::Num16:
	case EImvType::SNum16:
		nLength = bool(ext & MaskExtension) ? 6 : 4;
		break;
	case EImvType::Num32:
	case EImvType::SNum32:
		nLength = bool(ext & MaskExtension) ? 8 : 6;
		break;
	case EImvType::Num64:
	case EImvType::SNum64:
		nLength = bool(ext & MaskExtension) ? 12 : 10;
		break;
	case EImvType::Count:
		nLength = 4;
		break;
	case EImvType::Port:
		nLength = 4;
		break;
	case EImvType::Index:
		VASM_ASSERT(opr3);
		nLength = 6;
		break;
	default:
		VASM_THROW_INVALID_CONDITION(); // bad case
		break;
	}
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, t_csz basename, EOpCode opc, EOprType op1, uchar ext) :
	SCommandMetaInfo(name, opc, op1, ext)
{
	pcszBaseName = basename;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, t_csz basename, EOpCode opc, EOprType op1, EImvType imv, uchar ext) :
	SCommandMetaInfo(name, opc, op1, imv, ext)
{
	pcszBaseName = basename;
}

SCommandMetaInfo::SCommandMetaInfo(
	t_csz name, t_csz basename, EOpCode opc, EOprType op1, EOprType op2, uchar ext) :
	SCommandMetaInfo(name, opc, op1, op2, ext)
{
	pcszBaseName = basename;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IInstruction implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IInstruction::~IInstruction() = default;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IDisassemble implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IDisassemble::~IDisassemble() = default;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IAddressRecovery implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IAddressRecovery::~IAddressRecovery() = default;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CCommandBase implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCommandBase::CCommandBase() = default;
CCommandBase::~CCommandBase() = default;

CCommandBase::SInstructionInfo CCommandBase::GetInstruction(t_index nIdx) const
{
	SInstructionInfo tInfo;
	VASM_CHECK_IDX(nIdx, m_aInstructions.size());
	tInfo.tMetaInfo = std::get<0>(m_aInstructions[nIdx]);
	tInfo.pExec = std::get<1>(m_aInstructions[nIdx]).get();
	tInfo.pDisasm = std::get<2>(m_aInstructions[nIdx]).get();
	return tInfo;
}

void CCommandBase::SetAddressRecovery(IAddressRecoveryPtr pAddrRecovery)
{
	m_pAddressRecovery = pAddrRecovery;
}

CCommandBase::CInstructionImpl::CInstructionImpl(CCommandBase& oMaster, FuncCmdExec pFunc) :
	m_oMaster(oMaster), m_pfnExec(pFunc)
{
	VASM_CHECK_PTR(m_pfnExec);
}

CCommandBase::CInstructionImpl::~CInstructionImpl() = default;

void CCommandBase::CInstructionImpl::Exec(SCommandContext& tCmdCtxt)
{
	(m_oMaster.*m_pfnExec)(tCmdCtxt);
}

CCommandBase::CInstructionImpl4::CInstructionImpl4(CCommandBase& oMaster, FuncCmdExec apFuncs[OprSizeCount]) :
	m_oMaster(oMaster)
{
	VASM_CHECK_PTR(apFuncs[0]);
	VASM_CHECK_PTR(apFuncs[1]);
	VASM_CHECK_PTR(apFuncs[2]);
	VASM_CHECK_PTR(apFuncs[3]);

	m_apfnExec[0] = apFuncs[0];
	m_apfnExec[1] = apFuncs[1];
	m_apfnExec[2] = apFuncs[2];
	m_apfnExec[3] = apFuncs[3];
}

CCommandBase::CInstructionImpl4::~CInstructionImpl4() = default;

void CCommandBase::CInstructionImpl4::Exec(SCommandContext& tCmdCtxt)
{
	VASM_CHECK_IDX((int) tCmdCtxt.tInfo.eOprSize, (int) EOprSize::Count);
	(m_oMaster.*m_apfnExec[(int) tCmdCtxt.tInfo.eOprSize])(tCmdCtxt);
}

CCommandBase::CDisassembleImpl::CDisassembleImpl(CCommandBase& oMaster, FuncCmdDisasm pfnDisasm) :
	m_oMaster(oMaster), m_pfnDisasm(pfnDisasm)
{
	VASM_CHECK_PTR(m_pfnDisasm);
}

CCommandBase::CDisassembleImpl::~CDisassembleImpl() = default;

t_string CCommandBase::CDisassembleImpl::Translate(SCommandInfo const& tCmd, bool bHexadecimal)
{
	return (m_oMaster.*m_pfnDisasm)(tCmd, bHexadecimal);
}

void CCommandBase::Register(SCommandMetaInfo const& tInfo, FuncCmdExec pfnExec, FuncCmdDisasm pfnDisasm)
{
	m_aInstructions.push_back(t_InstructionInfo(tInfo, new CInstructionImpl(*this, pfnExec), new CDisassembleImpl(*this, pfnDisasm)));
}

void CCommandBase::Register(SCommandMetaInfo const& tInfo, FuncCmdExec pfnExec[OprSizeCount], FuncCmdDisasm pfnDisasm)
{
	m_aInstructions.push_back(t_InstructionInfo(tInfo, new CInstructionImpl4(*this, pfnExec), new CDisassembleImpl(*this, pfnDisasm)));
}

t_string CCommandBase::DisAsm(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd = DisAsmCmd(tCmd);
	sCmd += DisAsmOprSize(tCmd);
	sCmd += DisAsmArgs(tCmd, bHexadecimal);
	return std::move(sCmd);
}

t_string CCommandBase::DisAsmCmd(SCommandInfo const& tCmd)
{
	bool const cbHasCndtn = bool(tCmd.tMetaInfo.eExtInfo | SCommandMetaInfo::HasCndtnCode) && (tCmd.eCdtnCode != ECndtnCode::None);
	t_string sCmd = cbHasCndtn ? tCmd.tMetaInfo.pcszBaseName : tCmd.tMetaInfo.pcszName;

	if (cbHasCndtn)
	{
		switch (tCmd.eCdtnCode)
		{
		case ECndtnCode::Equal:
			sCmd += t_csz("EQ");
			break;
		case ECndtnCode::NotEqual:
			sCmd += t_csz("NE");
			break;
		case ECndtnCode::Above:
			sCmd += t_csz("AB");
			break;
		case ECndtnCode::AboveEqual:
			sCmd += t_csz("AE");
			break;
		case ECndtnCode::Below:
			sCmd += t_csz("BL");
			break;
		case ECndtnCode::BelowEqual:
			sCmd += t_csz("BE");
			break;
		case ECndtnCode::Great:
			sCmd += t_csz("GR");
			break;
		case ECndtnCode::GreatEqual:
			sCmd += t_csz("GE");
			break;
		case ECndtnCode::Low:
			sCmd += t_csz("LO");
			break;
		case ECndtnCode::LowEqual:
			sCmd += t_csz("LE");
			break;
		case ECndtnCode::Overflow:
			sCmd += t_csz("OF");
			break;
		case ECndtnCode::NotOverflow:
			sCmd += t_csz("NO");
			break;
		case ECndtnCode::Signed:
			sCmd += t_csz("SN");
			break;
		case ECndtnCode::NotSigned:
			sCmd += t_csz("NS");
			break;
		}
	}

	return std::move(sCmd);
}

t_string CCommandBase::DisAsmOprSize(SCommandInfo const& tCmd, bool bForce)
{
	t_string sCmd;
	if (bForce || (tCmd.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSize))
	{
		switch (tCmd.eOprSize)
		{
		case EOprSize::Byte:
			sCmd = t_csz(" B ");
			break;
		case EOprSize::Word:
			sCmd = t_csz(" W ");
			break;
		case EOprSize::DWord:
			sCmd = t_csz(" DW");
			break;
		case EOprSize::QWord:
			sCmd = t_csz(" QW");
			break;
		default:
			VASM_ASSERT(false);
			break;
		}
	}
	return std::move(sCmd);
}

t_string CCommandBase::DisAsmArgs(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd;
	for (t_index nArg = 0; nArg < tCmd.tMetaInfo.nOperandCount; ++nArg)
	{
		if (nArg == 0)
			sCmd += t_csz(" ");
		else
			sCmd += t_csz(", ");

		sCmd += DisAsmArg(tCmd, bHexadecimal, nArg);
	}
	return std::move(sCmd);
}

t_string CCommandBase::DisAsmArg(SCommandInfo const& tCmd, bool bHexadecimal, t_index nArg)
{
	t_string sCmd;
	switch (tCmd.tMetaInfo.aeOprTypes[nArg])
	{
	case EOprType::AR:
		sCmd = base::toStr(t_csz("A%1"), (uint) tCmd.nRegIdx[nArg]);
		break;
	case EOprType::GR:
		sCmd = base::toStr(t_csz("R%1"), (uint) tCmd.nRegIdx[nArg] * OperandSize(tCmd.eOprSize));
		break;
	case EOprType::AGR:
	{
		VASM_CHECK(nArg != EOprIdx::Third);
		if ((nArg == EOprIdx::First && tCmd.bOprSwitch1) || (nArg == EOprIdx::Second && tCmd.bOprSwitch2))
			sCmd = base::toStr(t_csz("R%1"), (uint) tCmd.nRegIdx[nArg] * OperandSize(tCmd.eOprSize));
		else
			sCmd = base::toStr(t_csz("A%1"), (uint) tCmd.nRegIdx[nArg]);
		break;
	}
	case EOprType::IMV:
	case EOprType::GRIMV:
	{
		bool isGR = (tCmd.tMetaInfo.aeOprTypes[nArg] == EOprType::GRIMV &&
					 ((nArg == EOprIdx::First && !tCmd.bOprSwitch1) ||
					  (nArg == EOprIdx::Second && !tCmd.bOprSwitch2)));
		if (isGR)
			sCmd = base::toStr(t_csz("R%1"), (uint) tCmd.nRegIdx[nArg] * OperandSize(tCmd.eOprSize));
		else
			sCmd = DisAsmImv(tCmd, bHexadecimal);
		break;
	}
	default:
		VASM_ASSERT(false);
	}

	return std::move(sCmd);
}

t_string CCommandBase::DisAsmImv(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd;
	switch (tCmd.tMetaInfo.eImvType)
	{
	case EImvType::Num8:
		sCmd = std::move(base::toStr(tCmd.u8Imv, bHexadecimal));
		break;
	case EImvType::SNum8:
		sCmd = std::move(base::toStr(tCmd.i8Imv, bHexadecimal));
		break;
	case EImvType::Num16:
		sCmd = std::move(base::toStr(tCmd.u16Imv, bHexadecimal));
		break;
	case EImvType::SNum16:
		sCmd = std::move(base::toStr(tCmd.i16Imv, bHexadecimal));
		break;
	case EImvType::Num32:
		sCmd = std::move(base::toStr(tCmd.u32Imv, bHexadecimal));
		break;
	case EImvType::SNum32:
		sCmd = std::move(base::toStr(tCmd.i32Imv, bHexadecimal));
		break;
	case EImvType::Num64:
		sCmd = std::move(base::toStr(tCmd.u64Imv, bHexadecimal));
		break;
	case EImvType::SNum64:
		sCmd = std::move(base::toStr(tCmd.i64Imv, bHexadecimal));
		break;
	case EImvType::Count:
		sCmd = std::move(base::toStr(tCmd.u8Imv, false));
		break;
	case EImvType::Port:
		sCmd = std::move(base::toStr(tCmd.u16Imv, bHexadecimal));
		break;
	case EImvType::Index:
		sCmd = std::move(base::toStr(tCmd.i32Imv, bHexadecimal));
		break;
	}
	return std::move(sCmd);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace casm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Includes
//
#include "decoder.h"
#include <base_exception.h>

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CDecoder implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDecoder::CDecoder(CCommandLibraryPtr pCmdLib) :
	m_pCmdLibrary(pCmdLib)
{
	VASM_CHECK_PTR(pCmdLib);
}

CDecoder::~CDecoder() = default;

void CDecoder::Decode(uchar const* pCmd, SCommandInfo& tCmdInfo)
{
	tCmdInfo.tMetaInfo = (*m_pCmdLibrary)[(EOpCode) *pCmd].tMetaInfo;
	++pCmd; // Skip OpCode and move to next byte

	if (tCmdInfo.tMetaInfo.eExtInfo == SCommandMetaInfo::NoExtension)
	{	// No OpCode extension
		tCmdInfo.nExtension = 0;
		tCmdInfo.eOprSize = EOprSize::Default;
		tCmdInfo.bOprSwitch1 = false;
		tCmdInfo.bOprSwitch2 = false;
		tCmdInfo.eCdtnCode = ECndtnCode::None;
	}
	else
	{	// Decode extension
		uchar nExtension = *pCmd;

		tCmdInfo.eCdtnCode = bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasCndtnCode) ?
			(ECndtnCode) (nExtension & SCommandMetaInfo::MaskCndtnCode) : ECndtnCode::None;

		tCmdInfo.eOprSize = bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSize) ?
			(EOprSize) ((nExtension & SCommandMetaInfo::MaskOprSize) >> SCommandMetaInfo::OprSizeShift) : EOprSize::Default;

		if (bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSwitch))
		{
			tCmdInfo.bOprSwitch1 = bool(nExtension & SCommandMetaInfo::MaskOprSwitch1);
			tCmdInfo.bOprSwitch2 = bool(nExtension & SCommandMetaInfo::MaskOprSwitch2); 
		}
		else
		{
			tCmdInfo.bOprSwitch1 = false;
			tCmdInfo.bOprSwitch2 = false;
		}

		tCmdInfo.nExtension = bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::CustomExtension) ? nExtension : 0;

		++pCmd; // Skip Extenssion and move to next byte
	}

	tCmdInfo.u64Imv = 0; // Empty imv in any case
	tCmdInfo.nRegIdx[EOprIdx::First] = 0;
	tCmdInfo.nRegIdx[EOprIdx::Second] = 0;
	tCmdInfo.nRegIdx[EOprIdx::Third] = 0;

	if (tCmdInfo.tMetaInfo.nOperandCount > 0)
	{
		DecodeArg(pCmd, tCmdInfo, EOprIdx::First);

		if (tCmdInfo.tMetaInfo.nOperandCount > 1)
		{
			DecodeArg(pCmd, tCmdInfo, EOprIdx::Second);

			if (tCmdInfo.tMetaInfo.nOperandCount > 2)
				DecodeArg(pCmd, tCmdInfo, EOprIdx::Third);
		}
	}
}

void CDecoder::DecodeArg(uchar const*& pCmd, SCommandInfo& tCmdInfo, EOprIdx eOprIdx)
{
	bool bOprSwitch;
	if (eOprIdx == EOprIdx::First)
		bOprSwitch = tCmdInfo.bOprSwitch1;
	else if (eOprIdx == EOprIdx::Second)
		bOprSwitch = tCmdInfo.bOprSwitch2;
	else
		bOprSwitch = false;

	EOprType eOprType = tCmdInfo.tMetaInfo.aeOprTypes[(int) eOprIdx];
	switch (eOprType)
	{
	case EOprType::AR:
	case EOprType::GR:
	case EOprType::AGR:
	{
		tCmdInfo.nRegIdx[eOprIdx] = *pCmd;
		++pCmd; // Skip Operand and move to next byte
		break;
	}
	case EOprType::IMV:
	{
		ReadIMV(pCmd, tCmdInfo);
		break;
	}
	case EOprType::GRIMV:
	{
		if (bOprSwitch)
		{
			ReadIMV(pCmd, tCmdInfo);
		}
		else
		{
			tCmdInfo.nRegIdx[eOprIdx] = *pCmd;
			++pCmd; // Skip Operand and move to next byte
		}
		break;
	}
	default:
		VASM_THROW_INVALID_CONDITION();
	}

	if (bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSize))
	{
		// When GP register mixed with Address register specified OprSize must be DWord (size of address)
		if (eOprType == EOprType::AGR && !bOprSwitch && tCmdInfo.eOprSize != EOprSize::DWord)
			VASM_THROW_ERROR(t_csz("Address register operand size must be DWORD"));
	}
	else //(!bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSize))
	{
		// For GP registers read OpSize from MetaInfo if explicityly not specified
		if (eOprType == EOprType::GR || (eOprType == EOprType::AGR && bOprSwitch) || (eOprType == EOprType::GRIMV && !bOprSwitch))
			tCmdInfo.eOprSize = EOprSize(uchar(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::MaskFixedOprSize) >> 6);
	}
}

void CDecoder::ReadIMV(uchar const*& pCmd, SCommandInfo& tCmdInfo)
{
	switch (tCmdInfo.tMetaInfo.eImvType)
	{
	case EImvType::Num8:
	case EImvType::SNum8:
		tCmdInfo.u8Imv = *pCmd;
		pCmd += sizeof(uchar); // Skip IMV
		break;
	case EImvType::Num16:
	case EImvType::SNum16:
		tCmdInfo.u16Imv = *(uint16*)(pCmd);
		pCmd += sizeof(uint16); // Skip IMV
		break;
	case EImvType::Num32:
	case EImvType::SNum32:
		std::memcpy(&tCmdInfo.u32Imv, pCmd, sizeof(uint32));
		pCmd += sizeof(uint32); // Skip IMV
		break;
	case EImvType::Num64:
	case EImvType::SNum64:
		std::memcpy(&tCmdInfo.u64Imv, pCmd, sizeof(uint64));
		pCmd += sizeof(uint64); // Skip IMV
		break;
	case EImvType::Count:
		tCmdInfo.u8Imv = *pCmd;
		pCmd += sizeof(uchar); // Skip IMV
		break;
	case EImvType::Port:
		tCmdInfo.u16Imv = uint16(*pCmd) + (((uint16)(tCmdInfo.nExtension & 0xF0ui8)) << 4);
		pCmd += sizeof(uchar); // Skip IMV
		break;
	case EImvType::Index:
		std::memcpy((&tCmdInfo.i8Imv) + 1, pCmd, 3);
		tCmdInfo.i32Imv = tCmdInfo.i32Imv >> 8;
		pCmd += sizeof(3); // Skip IMV
		break;
	default:
		VASM_THROW_INVALID_CONDITION();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

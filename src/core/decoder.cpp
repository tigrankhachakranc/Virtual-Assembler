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

	if (0 == (tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::MaskExtension))
	{	// No OpCode extension
		tCmdInfo.nExtension = 0;
		tCmdInfo.eOprSize = (EOprSize) (uchar(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::MaskFixedOprSize) >> 6);
		tCmdInfo.eOprSwitch = EOprSwitch::Default;
		tCmdInfo.eCdtnCode = ECndtnCode::None;
	}
	else
	{	// Decode extension
		uchar nExtension = *pCmd;

		tCmdInfo.eCdtnCode = bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasCndtnCode) ?
			(ECndtnCode) (nExtension & SCommandMetaInfo::MaskCndtnCode) : ECndtnCode::None;

		tCmdInfo.eOprSize = bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSize) ?
			(EOprSize) ((nExtension & SCommandMetaInfo::MaskOprSize) >> SCommandMetaInfo::OprSizeShift) : 
			(EOprSize) (uchar(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::MaskFixedOprSize) >> 6);

		tCmdInfo.eOprSwitch =  bool(tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::HasOprSwitch) ?
			(EOprSwitch) ((nExtension & SCommandMetaInfo::MaskOprSwitch) >> SCommandMetaInfo::OprSwitchShift) : EOprSwitch::Default;

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
	EOprType eOprType = tCmdInfo.tMetaInfo.aeOprTypes[(int) eOprIdx];
	switch (eOprType)
	{
	case EOprType::Reg:
	{
		tCmdInfo.nRegIdx[eOprIdx] = *pCmd;
		++pCmd; // Skip Operand and move to next byte
		// Reg. idx should be OprSize aligned
		break;
	}
	case EOprType::Imv:
	{
		ReadIMV(pCmd, tCmdInfo);
		break;
	}
	case EOprType::RegImv:
	{
		if (tCmdInfo.eOprSwitch == EOprSwitch::Reg)
		{
			tCmdInfo.nRegIdx[eOprIdx] = *pCmd;
			++pCmd; // Skip Operand and move to next byte
		}
		else
		{
			ReadIMV(pCmd, tCmdInfo);
		}
		break;
	}
	default:
		VASM_THROW_INVALID_CONDITION();
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
		// Read lower 8 bits from argument and unpack elder 4 bits from lower bits of the extension
		tCmdInfo.u16Imv = uint16(*pCmd) + (static_cast<uint16>(tCmdInfo.nExtension & 0x0Fui8) << 8);
		pCmd += sizeof(uchar); // Skip IMV
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

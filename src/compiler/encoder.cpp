//
// Includes
//
#include "encoder.h"

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using EOpCode	= core::EOpCode;
using EOprType	= core::EOprType;
using EOprSize	= core::EOprSize;
using EOprIdx	= core::EOprIdx;
using EImvType	= core::EImvType;

using SCommandMetaInfo = core::SCommandMetaInfo;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CEncoder implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEncoder::CEncoder(t_aCode& aCodeBuffer,
				   t_aRelocTable& aSymblRelocTbl,
				   t_aRelocTable& aLblRelocTbl,
				   std::vector<t_index> const& aVarToSymblIdxMapping,
				   std::vector<t_index> const& aFuncToSymblIdxMapping,
				   CCommandLibraryPtr pCmdLib) :
	m_aCodeBuffer(aCodeBuffer),
	m_aSymblRelocTbl(aSymblRelocTbl),
	m_aLblRelocTbl(aLblRelocTbl),
	m_pCmdLibrary(pCmdLib),
	m_aVarToSymblIdxMapping(aVarToSymblIdxMapping),
	m_aFuncToSymblIdxMapping(aFuncToSymblIdxMapping)
{
	VASM_CHECK_PTR(pCmdLib);
}

CEncoder::~CEncoder() = default;


t_uoffset CEncoder::Encode(SCommand const& tCmd, t_uoffset nCodeOffset)
{
	SCommandMetaInfo const tInfo = (*m_pCmdLibrary)[tCmd.eOpCode].tMetaInfo;

	uint8* pCodeBase = &m_aCodeBuffer[nCodeOffset];
	uint8* pCode = pCodeBase;

	if (tInfo.eOpCode == EOpCode::Invalid)
		throw CError(base::toStr("Encoder: Unrecognized OpCode #%1", (uint) tCmd.eOpCode), tCmd.nLineNumber, tInfo.pcszName);
	else if (tInfo.nLength == 0)
		throw CError(base::toStr("Encoder: Invalid command with OpCode #%1", (uint) tCmd.eOpCode), tCmd.nLineNumber, tInfo.pcszName);
	else if (tInfo.nOperandCount != tCmd.nArgCount)
		throw CError(t_csz("Encoder: Wrong number of arguments"), tCmd.nLineNumber, tInfo.pcszName);
	else if ((uint64) pCode % core::cnCmdMinLength != 0)
		throw CError(t_csz("Encoder: Unaligned mempry location"), tCmd.nLineNumber, tInfo.pcszName);

	// Write OpCode
	*pCode = (uint8) tInfo.eOpCode;
	++pCode; // Move to next byte

	if (tInfo.eExtInfo & SCommandMetaInfo::MaskExtension)
	{
		uint8 nExt = 0;

		if (tInfo.eExtInfo & SCommandMetaInfo::HasCndtnCode)
			nExt |= uint8(tCmd.eCdtnCode) & uint8(SCommandMetaInfo::MaskCndtnCode);

		if (tInfo.eExtInfo & SCommandMetaInfo::HasOprSize)
			nExt |= (uint8(tCmd.eOprSize) << SCommandMetaInfo::OprSizeShift) & uint8(SCommandMetaInfo::MaskOprSize);

		// Special handling for the CAST command
		if (tInfo.eOpCode == EOpCode::CAST)
		{	// We remember that target operand size kept in the place of third operand/
			// Now pack it in the place of highest bits of the extension and rember that it is there
			nExt |= ((tCmd.aArguments[EOprIdx::Third].u8Val << 6) & 0xC0ui8);
		}
		else if (tInfo.eExtInfo & SCommandMetaInfo::HasOprSwitch)
		{
			if (tInfo.aeOprTypes[EOprIdx::First] == EOprType::AGR)
			{
				if (tCmd.aArguments[EOprIdx::First].eType != EArgType::AR)
					nExt |= uint8(SCommandMetaInfo::MaskOprSwitch1);
			}
			else if (tInfo.aeOprTypes[EOprIdx::First] == EOprType::GRIMV)
			{
				if (tCmd.aArguments[EOprIdx::First].eType != EArgType::GR)
					nExt |= uint8(SCommandMetaInfo::MaskOprSwitch1);
			}

			if (tInfo.aeOprTypes[EOprIdx::Second] == EOprType::AGR)
			{
				if (tCmd.aArguments[EOprIdx::Second].eType != EArgType::AR)
					nExt |= uint8(SCommandMetaInfo::MaskOprSwitch2);
			}
			else if (tInfo.aeOprTypes[EOprIdx::Second] == EOprType::GRIMV)
			{
				if (tCmd.aArguments[EOprIdx::Second].eType != EArgType::GR)
					nExt |= uint8(SCommandMetaInfo::MaskOprSwitch2);
			}
		}

		// Write extension
		*pCode = nExt;
		++pCode; // Move to next byte
	}

	for (uchar i = 0; i < tInfo.nOperandCount; ++i)
	{
		uchar eOprType = (uchar) tInfo.aeOprTypes[i];
		SArgument const& tArg = tCmd.aArguments[i];

		switch (tArg.eType)
		{
		case EArgType::AR:
		{
			if (!bool(eOprType & (uchar) EOprType::AR))
				throw CError(t_csz("Encoder: Invalid use of address register"), tCmd.nLineNumber, tInfo.pcszName);

			if (tArg.nIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
				throw CError(base::toStr("Encoder: Invalid address register index '%1'", tArg.nIdx), tCmd.nLineNumber, tInfo.pcszName);

			// Write extension
			*pCode = (uchar) tArg.nIdx;
			++pCode; // Move to next byte
			break;
		}
		case EArgType::GR:
		{
			if (!bool(eOprType & (uchar) EOprType::GR))
				throw CError(t_csz("Encoder: Invalid use of general pusrpose register"), tCmd.nLineNumber, tInfo.pcszName);

			if (tArg.nIdx >= core::SCPUStateBase::eGeneralPurposeRegisterPoolSize)
				throw CError(base::toStr("Encoder: Invalid general purpose register index '%1'", tArg.nIdx), tCmd.nLineNumber, tInfo.pcszName);

			uchar nOprSizeInBytes = 0;
			if (tInfo.eExtInfo & SCommandMetaInfo::HasOprSize)
				nOprSizeInBytes = core::OperandSize(tCmd.eOprSize);
			else
				nOprSizeInBytes = core::OperandSize((EOprSize) ((tInfo.eExtInfo >> 6) & 0x03));

			if (tArg.nIdx % nOprSizeInBytes != 0)
				throw CError(base::toStr("Encoder: Unaligned general purpose register index '%1'", tArg.nIdx), tCmd.nLineNumber, tInfo.pcszName);

			// Write operand
			*pCode = (uchar) tArg.nIdx / nOprSizeInBytes;
			++pCode; // Move to next byte
			break;
		}
		case EArgType::VAR:
		{
			if (!bool(eOprType & (uchar) EOprType::IMV))
				throw CError(t_csz("Encoder: Invalid use of variable"), tCmd.nLineNumber, tInfo.pcszName);
			if (tInfo.eOpCode != EOpCode::ASSIGNA4) // Variables could be used only with the instruction ASSIGN A?, VAR_NAME 
				throw CError(t_csz("Encoder: Invalid use of variable"), tCmd.nLineNumber, tInfo.pcszName);

			// Convert variable index into corresponding symbol index
			VASM_CHECK_IDX(tArg.nIdx, m_aVarToSymblIdxMapping.size());
			t_address nSymbolIdx = m_aVarToSymblIdxMapping[tArg.nIdx];
			// Write variable's symbol index for now, actual address will be resolved later by the linker
			std::memcpy(pCode, &nSymbolIdx, sizeof(t_address));
			// Keep lbl offset in the relocation table to complete with correct address later
			m_aSymblRelocTbl.push_back(nCodeOffset + (pCode - pCodeBase));
			pCode += sizeof(t_address); // Move to next byte
			break;
		}
		case EArgType::FUNC:
		{
			if (!bool(eOprType & (uchar) EOprType::IMV))
				throw CError(t_csz("Encoder: Invalid use of Function"), tCmd.nLineNumber, tInfo.pcszName);
			if (tInfo.eOpCode != EOpCode::ASSIGNA4) // Variables could be used only with the instruction ASSIGN A?, VAR_NAME 
				throw CError(t_csz("Encoder: Invalid use of Function"), tCmd.nLineNumber, tInfo.pcszName);

			// Convert function index into corresponding symbol index
			VASM_CHECK_IDX(tArg.nIdx, m_aFuncToSymblIdxMapping.size());
			t_address nSymbolIdx = m_aFuncToSymblIdxMapping[tArg.nIdx];
			// Write function's symbol index for now, actual address will be resolved later by the linker
			std::memcpy(pCode, &nSymbolIdx, sizeof(t_address));
			// Keep lbl offset in the relocation table to complete with correct address later
			m_aSymblRelocTbl.push_back(nCodeOffset + (pCode - pCodeBase));
			pCode += sizeof(t_address); // Move to next byte
			break;
		}
		case EArgType::LBL:
		{
			if (!bool(eOprType & (uchar) EOprType::IMV))
				throw CError(t_csz("Encoder: Invalid use of label"), tCmd.nLineNumber, tInfo.pcszName);
			if (tInfo.eOpCode != EOpCode::JUMPR && tInfo.eOpCode != EOpCode::ASSIGNR2)
				// Label could be used only with the JUMP & ASSIGN R?, LBL_NAME instructions
				throw CError(t_csz("Encoder: Invalid use of label"), tCmd.nLineNumber, tInfo.pcszName);

			// Write lbl index for now, actual address will be resolved later by the linker
			*reinterpret_cast<t_word*>(pCode) = (t_word) tArg.nIdx;
			// Keep lbl offset in the relocation table to complete with correct address later
			m_aLblRelocTbl.push_back(nCodeOffset + (pCode - pCodeBase));
			pCode += sizeof(t_word); // Move to next byte
			break;
		}
		case EArgType::NUM:
		{
			if (!bool(eOprType & (uchar) EOprType::IMV))
				throw CError(t_csz("Encoder: Invalid use of numeric value"), tCmd.nLineNumber, tInfo.pcszName);

			switch (tCmd.eImvType)
			{
			case EImvType::Num8:
				*reinterpret_cast<uint8*>(pCode) = tArg.u8Val;
				pCode += sizeof(uint8); // Move to next operand
				break;
			case EImvType::SNum8:
				*reinterpret_cast<int8*>(pCode) = tArg.i8Val;
				pCode += sizeof(int8); // Move to next operand
				break;
			case EImvType::Num16:
				*reinterpret_cast<uint16*>(pCode) = tArg.u16Val;
				pCode += sizeof(uint16); // Move to next operand
				break;
			case EImvType::SNum16:
				*reinterpret_cast<int16*>(pCode) = tArg.i16Val;
				pCode += sizeof(int16); // Move to next operand
				break;
			case EImvType::Num32:
				std::memcpy(pCode, &tArg.u32Val, sizeof(uint32));
				pCode += sizeof(uint32); // Move to next operand
				break;
			case EImvType::SNum32:
				std::memcpy(pCode, &tArg.i32Val, sizeof(int32));
				pCode += sizeof(int32); // Move to next operand
				break;
			case EImvType::Num64:
				std::memcpy(pCode, &tArg.u64Val, sizeof(uint64));
				pCode += sizeof(int32); // Move to next operand
				break;
			case EImvType::SNum64:
				std::memcpy(pCode, &tArg.i64Val, sizeof(int64));
				pCode += sizeof(int64); // Move to next operand
				break;
			case EImvType::Count:
				*reinterpret_cast<uint8*>(pCode) = tArg.u8Val;
				pCode += sizeof(uint8); // Move to next operand
				break;
			case EImvType::Port:
				// write lower 8 bits of 12 bit value in place of the argument
				*reinterpret_cast<uint8*>(pCode) = static_cast<uint8>(tArg.u16Val);
				// Pack elder 4 bits of 12 bit value into lower 4 bits of the extension
				*(pCodeBase + 1) = (static_cast<uint8>(tArg.u16Val >> 8) & 0x0Fui8) | (*(pCodeBase + 1) & 0xF0ui8);
				pCode += sizeof(uint8); // Move to next operand
				break;
			case EImvType::Index:
				//TODO! Failed to preserve endianess, this code is Little Endian speciifc
				std::memcpy(pCode, &tArg.i64Val, 3);
				pCode += sizeof(int64); // Move to next operand
				break;
			default:
				break;
			}
			break;
		}
		case EArgType::SNUM:
		{
			if (!bool(eOprType & (uchar) EOprType::IMV))
				throw CError(t_csz("Encoder: Invalid use of signed numeric value"), tCmd.nLineNumber, tInfo.pcszName);
			if (tInfo.eOpCode != EOpCode::JUMPR && tInfo.eOpCode != EOpCode::ASSIGNR2)
				// Label could be used only with the JUMP & ASSIGN R?, LBL_NAME instructions
				throw CError(t_csz("Encoder: Invalid use of signed numeric value"), tCmd.nLineNumber, tInfo.pcszName);

			switch (tCmd.eImvType)
			{
			case EImvType::Num8:
				VASM_ASSERT(false);
				break;
			case EImvType::SNum8:
				*reinterpret_cast<int8*>(pCode) = tArg.i8Val;
				pCode += sizeof(int8); // Move to next operand
				break;
			case EImvType::Num16:
				VASM_ASSERT(false);
				break;
			case EImvType::SNum16:
				*reinterpret_cast<int16*>(pCode) = tArg.i16Val;
				pCode += sizeof(int16); // Move to next operand
				break;
			case EImvType::Num32:
				VASM_ASSERT(false);
				break;
			case EImvType::SNum32:
				std::memcpy(pCode, &tArg.i32Val, sizeof(int32));
				pCode += sizeof(int32); // Move to next operand
				break;
			case EImvType::Num64:
				VASM_ASSERT(false);
				break;
			case EImvType::SNum64:
				std::memcpy(pCode, &tArg.i64Val, sizeof(int64));
				pCode += sizeof(int64); // Move to next operand
				break;
			case EImvType::Count:
				VASM_ASSERT(false);
				break;
			case EImvType::Port:
				VASM_ASSERT(false);
				break;
			case EImvType::Index:
				//TODO! Failed to preserve endianess, this code is Little Endian speciifc
				std::memcpy(pCode, &tArg.i64Val, 3);
				pCode += sizeof(int64); // Move to next operand
				break;
			default:
				break;
			}
			break;
		}
		default:
			VASM_ASSERT(false);
			break;
		}
	}

	// Return command total length
	return tInfo.nLength;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Includes
//
#include "output_generator.h"
#include <core_package_info.h>

// STL
#include <vector>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using SSectionInfo = core::SSectionInfo;
using SPackageHeader = core::SPackageHeader;
using SProgramInfoSection = core::SProgramInfoSection;
using SSymbolTableSection = core::SSymbolTableSection;
using SRelocationTableSection = core::SRelocationTableSection;
using SDebugInfoSection = core::SDebugInfoSection;
using SCheckSumSection = core::SCheckSumSection;

//
//	C-Tors
//
COutputGenerator::COutputGenerator() = default;
COutputGenerator::~COutputGenerator() = default;


//
//	Methods
//

void COutputGenerator::ProduceExecutable(SPackage const& tPackage, std::ostream& os)
{
	//
	// Program info section preparation
	//
	SProgramInfoSection tInfoSctn;
	tInfoSctn.nCodeBase = 0; // Code goes first
	tInfoSctn.nDataBase = 0; // Data goes immediately after code
	tInfoSctn.nProgramStart = 0; // Start always is zero because Linker put special code there that calls Main
	tInfoSctn.nStackSize = tPackage.tInfo.nStackSize;

	//
	//	Symbols section preparation
	//
	t_size const cnSmblTblSctnSize = sizeof(SSymbolTableSection);
	t_size const cnSmblTblSctnEntrySize = sizeof(SSymbolTableSection::SEntry);

	// Determine buffer size for symbols
	t_size nBufferSize = cnSmblTblSctnSize;
	nBufferSize += cnSmblTblSctnEntrySize * tPackage.oSymbolTbl.aEntries.size();
	t_size nBufferMarker = nBufferSize;
	for (t_index i = 0; i < tPackage.oSymbolTbl.aEntries.size(); ++i)
		nBufferSize += sizeof(t_string::value_type) * (tPackage.oSymbolTbl.aEntries[i].sName.size() + 1); // One more for '\0'
	if (nBufferSize % 16 != 0)
		nBufferSize += 16 - nBufferSize % 16;

	// Fill Symbols buffer
	std::vector<uint8> aSymbolsBuff((size_t) nBufferSize, 0ui8);

	uint8* const cpSymbolsBuff = &aSymbolsBuff[0];
	SSymbolTableSection& tSymbolsSctn = reinterpret_cast<SSymbolTableSection&>(*cpSymbolsBuff);
	tSymbolsSctn.nMainIndex = tPackage.tInfo.nEntryPoint;
	tSymbolsSctn.nEntryCount = (t_count) tPackage.oSymbolTbl.aEntries.size();

	for (t_index i = 0; i < tSymbolsSctn.nEntryCount; ++i)
	{
		SSymbolTable::SEntry const& tEntry = tPackage.oSymbolTbl.aEntries[i];
		tSymbolsSctn.aEntries[i].eType = tEntry.eType;
		tSymbolsSctn.aEntries[i].isFunc = tEntry.isFunc;
		tSymbolsSctn.aEntries[i].nBase = tEntry.nBase;
		tSymbolsSctn.aEntries[i].nSize = tEntry.nSize;
		tSymbolsSctn.aEntries[i].nNamePos = nBufferMarker;
		tSymbolsSctn.aEntries[i].nNameSize = (t_uoffset) tEntry.sName.size();

		t_size nStrSize = sizeof(t_string::value_type) * (tEntry.sName.size() + 1); // One more for '\0'
		std::memcpy(&aSymbolsBuff[nBufferMarker], tEntry.sName.data(), nStrSize);
		nBufferMarker += nStrSize;
	}

	//
	//	Relocation table section preparation
	//
	t_size const cnRelocTblSctnSize = sizeof(SRelocationTableSection);

	// Determine buffer size
	nBufferSize = cnRelocTblSctnSize;
	nBufferSize += sizeof(t_uoffset) * tPackage.oRelocTbl.aAddressLocations.size();
	if (nBufferSize % 16 != 0)
		nBufferSize += 16 - nBufferSize % 16;

	// Fill relocation table buffer
	std::vector<uint8> aRelocTblBuff((size_t) nBufferSize, 0ui8);

	uint8* const cpRelocTblBuff = &aRelocTblBuff[0];
	SRelocationTableSection& tRelocTblSctn = reinterpret_cast<SRelocationTableSection&>(*cpRelocTblBuff);
	tRelocTblSctn.nCount = (t_count) tPackage.oRelocTbl.aAddressLocations.size();
	for (t_index i = 0; i < tRelocTblSctn.nCount; ++i)
		tRelocTblSctn.aAddressLocations[i] = tPackage.oRelocTbl.aAddressLocations[i];

	//
	//	Debug info section preparation
	//
	t_size const cnDbgTblSctnSize = sizeof(SDebugInfoSection);
	t_size const cnDbgTblSctnEntrySize = sizeof(SDebugInfoSection::SEntry);
	t_size const cnDbgTblSctnLblEntrySize = sizeof(SDebugInfoSection::SLabelEntry);

	// Determine buffer size
	nBufferSize = cnDbgTblSctnSize;
	nBufferSize += cnDbgTblSctnEntrySize * tPackage.oDebugInfo.aEntries.size();
	nBufferMarker = nBufferSize;
	for (SDebugInfo::SEntry const& tEntry : tPackage.oDebugInfo.aEntries)
	{
		nBufferSize += sizeof(t_string::value_type) * (tEntry.sSrcUnitName.size() + 1); // One more for '\0'
		VASM_CHECK(tEntry.aCodeTbl.empty() || tEntry.aCodeTbl.size() == tEntry.nSizeLine);
		nBufferSize += sizeof(t_uoffset) * tEntry.aCodeTbl.size();
		nBufferSize += cnDbgTblSctnLblEntrySize * tEntry.aLabelEntries.size();
		for (SDebugInfo::SLabelEntry const& tLblEntry : tEntry.aLabelEntries)
			nBufferSize += sizeof(t_string::value_type) * (tLblEntry.sName.size() + 1); // One more for '\0'
	}
	if (nBufferSize % 16 != 0)
		nBufferSize += 16 - nBufferSize % 16;

	// Fill Debug info buffer
	std::vector<uint8> aDbgInfoBuff((size_t) nBufferSize, 0ui8);
	uint8* const cpDbgInfoBuff = &aDbgInfoBuff[0];

	SDebugInfoSection& tDbgInfoSctn = reinterpret_cast<SDebugInfoSection&>(*cpDbgInfoBuff);
	tDbgInfoSctn.nEntryCount = (t_count) tPackage.oDebugInfo.aEntries.size();

	for (t_index i = 0; i < tDbgInfoSctn.nEntryCount; ++i)
	{
		SDebugInfo::SEntry const& tEntry = tPackage.oDebugInfo.aEntries[i];
		tDbgInfoSctn.aEntries[i].nSymbolIndex = tEntry.nSymbolIndex;
		tDbgInfoSctn.aEntries[i].nBaseLine = tEntry.nBaseLine;
		tDbgInfoSctn.aEntries[i].nSizeLine = tEntry.nSizeLine;

		tDbgInfoSctn.aEntries[i].nSrcUnitPos = nBufferMarker;
		tDbgInfoSctn.aEntries[i].nSrcUnitSize = (t_uoffset) tEntry.sSrcUnitName.size();
		t_size nStrSize = sizeof(t_string::value_type) * (tEntry.sSrcUnitName.size() + 1); // One more for '\0'
		std::memcpy(&aDbgInfoBuff[nBufferMarker], tEntry.sSrcUnitName.data(), nStrSize);
		nBufferMarker += nStrSize;

		if (tEntry.aCodeTbl.empty())
			tDbgInfoSctn.aEntries[i].nCodeTblPos = core::cnInvalidAddress;
		else
		{
			t_size nCodeSize = sizeof(t_uoffset) * (tEntry.aCodeTbl.size());
			tDbgInfoSctn.aEntries[i].nCodeTblPos = nBufferMarker;
			std::memcpy(&aDbgInfoBuff[nBufferMarker], tEntry.aCodeTbl.data(), nCodeSize);
			nBufferMarker += nCodeSize;
		}

		t_size nBufferSubMarker = nBufferMarker + cnDbgTblSctnLblEntrySize * tEntry.aLabelEntries.size();
		tDbgInfoSctn.aEntries[i].nLabelEntryCount = (t_count) tEntry.aLabelEntries.size();
		tDbgInfoSctn.aEntries[i].nLabelEntriesPos = tEntry.aLabelEntries.empty() ? core::cnInvalidAddress : nBufferMarker;
		for (t_index j = 0; j < tEntry.aLabelEntries.size(); ++j)
		{
			SDebugInfo::SLabelEntry const& tLblEntry = tEntry.aLabelEntries[j];
			SDebugInfoSection::SLabelEntry& tLblSctnEntry = 
				reinterpret_cast<SDebugInfoSection::SLabelEntry&>(aDbgInfoBuff[nBufferMarker]);
			nBufferMarker += cnDbgTblSctnLblEntrySize;

			tLblSctnEntry.nOffset = tLblEntry.nOffset;
			tLblSctnEntry.nNamePos = nBufferSubMarker;
			tLblSctnEntry.nNameSize = (t_uoffset) tLblEntry.sName.size();
			nStrSize = sizeof(t_string::value_type) * (tLblEntry.sName.size() + 1); // One more for '\0'
			std::memcpy(&aDbgInfoBuff[nBufferSubMarker], tLblEntry.sName.data(), nStrSize);
			nBufferSubMarker += nStrSize;
		}
		nBufferMarker = nBufferSubMarker;
	}

	//
	//	CheckSum section preparation
	//
	auto fnChkSum = [](std::vector<uint8> const& aBuffer) -> uint32
	{
		uint32 sum = 0;
		uint16 r = 55665, c1 = 52845, c2 = 22719;
		for (uint8 value : aBuffer)
		{
			uint8 cipher = (value ^ (r >> 8));
			r = (cipher + r) * c1 + c2;
			sum += cipher;
		}
		return sum;
	};

	SCheckSumSection tChkSumSctn;
	tChkSumSctn.nVersion = 1;
	tChkSumSctn.nCheckSumCode = fnChkSum(tPackage.aCode);
	tChkSumSctn.nCheckSumData = fnChkSum(tPackage.aData);
	tChkSumSctn.nCheckSumSymbolTbl = fnChkSum(aSymbolsBuff);

	//
	//	Package preparation
	//

	// Fill in package header
	SPackageHeader tHedr;
	t_size const cnPkgHedrSize = sizeof(SPackageHeader);
	t_size nOutputMarker = cnPkgHedrSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Program section header info
	t_size const cnProgInfoSctnSize = sizeof(SProgramInfoSection);
	tHedr.aSections[0].eType = SSectionInfo::EType::Info;
	tHedr.aSections[0].nBase = nOutputMarker;
	tHedr.aSections[0].nSize = cnProgInfoSctnSize;
	nOutputMarker += tHedr.aSections[0].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Code section header info
	tHedr.aSections[1].eType = SSectionInfo::EType::Code;
	tHedr.aSections[1].nBase = nOutputMarker;
	tHedr.aSections[1].nSize = (t_size) tPackage.aCode.size();
	nOutputMarker += tHedr.aSections[1].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Data section header info
	tHedr.aSections[2].eType = SSectionInfo::EType::Data;
	tHedr.aSections[2].nBase = nOutputMarker;
	tHedr.aSections[2].nSize = (t_size) tPackage.aData.size();
	nOutputMarker += tHedr.aSections[2].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Symbols section header info
	tHedr.aSections[3].eType = SSectionInfo::EType::SymbolTbl;
	tHedr.aSections[3].nBase = nOutputMarker;
	tHedr.aSections[3].nSize = (t_size) aSymbolsBuff.size();
	nOutputMarker += tHedr.aSections[3].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Relocation table section header info
	tHedr.aSections[4].eType = SSectionInfo::EType::RelocTbl;
	tHedr.aSections[4].nBase = nOutputMarker;
	tHedr.aSections[4].nSize = (t_size) aRelocTblBuff.size();
	nOutputMarker += tHedr.aSections[4].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Debug info section header info
	tHedr.aSections[5].eType = SSectionInfo::EType::DbgInfo;
	tHedr.aSections[5].nBase = nOutputMarker;
	tHedr.aSections[5].nSize = (t_size) aDbgInfoBuff.size();
	nOutputMarker += tHedr.aSections[5].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	// Write Checksum section header info
	t_size const cnChkSumSctnSize = sizeof(SCheckSumSection);
	tHedr.aSections[6].eType = SSectionInfo::EType::CeckSum;
	tHedr.aSections[6].nBase = nOutputMarker;
	tHedr.aSections[6].nSize = cnChkSumSctnSize;
	nOutputMarker += tHedr.aSections[6].nSize;
	VASM_ASSERT(nOutputMarker % 16 == 0);

	//
	// Write outout to the stream
	//
	os.write((char const*) &tHedr, cnPkgHedrSize);
	os.write((char const*) &tInfoSctn, cnProgInfoSctnSize);
	os.write((char const*) tPackage.aCode.data(), tPackage.aCode.size());
	os.write((char const*) tPackage.aData.data(), tPackage.aData.size());
	os.write((char const*) aSymbolsBuff.data(), aSymbolsBuff.size());
	os.write((char const*) aRelocTblBuff.data(), aRelocTblBuff.size());
	os.write((char const*) aDbgInfoBuff.data(), aDbgInfoBuff.size());
	os.write((char const*) &tChkSumSctn, cnChkSumSctnSize);
	os.flush();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

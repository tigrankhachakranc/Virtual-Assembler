//
//	Includes
//
#include "loader.h"

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CMemory					= core::CMemory;

using SPackageHeader			= core::SPackageHeader;
using SProgramInfoSection		= core::SProgramInfoSection;
using SSymbolTableSection		= core::SSymbolTableSection;
using SRelocationTableSection	= core::SRelocationTableSection;
using SDebugInfoSection			= core::SDebugInfoSection;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CLoader Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-tors
//
CLoader::CLoader() = default;
CLoader::~CLoader() = default;

//
//	Methods
//
void CLoader::Load(std::istream& is, CMemoryPtr pMemory, SPackageInfo& tPkgInfo)
{
	VASM_CHECK_PTR(pMemory);

	// Read package Header
	SPackageHeader tPkgHdr;
	is.read((char*) &tPkgHdr, sizeof(SPackageHeader));
	VASM_CHECK_X(is.good(), t_csz("Invalid package"));

	// Check signature and version
	if (std::strcmp(tPkgHdr.cszSignatue, t_csz("TK.VASM.PKG")) != 0)
		VASM_THROW_ERROR(t_csz("Invalid package signature"));
	if (tPkgHdr.cnVersion != SPackageHeader::CurrentVersion)
		VASM_THROW_ERROR(base::toStr("Invalid package version: %1", tPkgHdr.cnVersion));

	SSectionInfo tCodeSection(SSectionInfo::EType::Code);
	SSectionInfo tDataSection(SSectionInfo::EType::Data);
	SSectionInfo tSymbolsSection(SSectionInfo::EType::SymbolTbl);
	SSectionInfo tDbgInfoSection(SSectionInfo::EType::DbgInfo);

	// Go read sections
	for (t_index i = 0; i < tPkgHdr.nSectionCount; ++i)
	{
		SSectionInfo const& tSection = tPkgHdr.aSections[i];
		if (tSection.nBase == 0 || tSection.nSize == 0)
			continue; // Skip section

		switch (tSection.eType)
		{
		case SSectionInfo::EType::Info:
		{
			SProgramInfoSection tSctn;
			is.seekg((std::istream::pos_type) tSection.nBase);
			VASM_CHECK_X(is.good(), t_csz("Loader: Program info section could not be loaded"));
			is.read((char*) &tSctn, sizeof(SProgramInfoSection));
			VASM_CHECK_X(is.good(), t_csz("Loader: Program info section could not be loaded"));

			tPkgInfo.nCodeBase = tSctn.nCodeBase;
			tPkgInfo.nDataBase = tSctn.nDataBase;

			tPkgInfo.nStackSize = tSctn.nStackSize;

			// Program start is relative to code section, it should be adjusted to be absolute
			tPkgInfo.nProgramStart = tSctn.nProgramStart + tPkgInfo.nCodeBase;
			//
			break;
		}
		case SSectionInfo::EType::Code:
		{	// Keep code section to load later
			tCodeSection = tSection;
			break;
		}
		case SSectionInfo::EType::Data:
		{	// Keep data section to load later
			tDataSection = tSection;
			break;
		}
		case SSectionInfo::EType::SymbolTbl:
		{
			tSymbolsSection = tSection;
			break;
		}
		case SSectionInfo::EType::RelocTbl:
		{
			//TODO! If code or data section is not loaded from the preferred position then relocation table is required to adjust addresses!
			break;
		}
		case SSectionInfo::EType::DbgInfo:
		{
			tDbgInfoSection = tSection;
			break;
		}
		default:
			break;
		}
	}

	// Calculate required memory size
	t_uoffset nMemorySize = 0;
	
	if (tCodeSection.nBase == 0 || tCodeSection.nSize == 0)
		VASM_THROW_ERROR(t_csz("Invalid package: Code is missing"));
	VASM_CHECK(tPkgInfo.nCodeBase % 64 == 0); // Alignment check
	tPkgInfo.nCodeSize = tCodeSection.nSize;
	//
	nMemorySize = tPkgInfo.nCodeBase + tPkgInfo.nCodeSize;
	
	// Align to 64 byte boundary
	uint nReminder = nMemorySize % 16;
	if (nReminder > 0)
		nMemorySize += 16 - nReminder;

	if (tDataSection.nBase > 0 && tDataSection.nSize > 0)
	{
		if (tPkgInfo.nDataBase == 0)
			tPkgInfo.nDataBase = nMemorySize;
		else if (tPkgInfo.nDataBase < nMemorySize)
			VASM_THROW_ERROR(t_csz("Invalid package: Data section base conflict"));
		else
		{
			VASM_CHECK(tPkgInfo.nDataBase % 16 == 0); // Alignment check
			nMemorySize = tPkgInfo.nDataBase;
		}

		nMemorySize += tDataSection.nSize;
		tPkgInfo.nDataSize = tDataSection.nSize;

		// Align to 64 byte boundary
		nReminder = nMemorySize % 16;
		if (nReminder > 0)
			nMemorySize += 16 - nReminder;
	}
	else
	{
		tPkgInfo.nDataBase = 0;
		tPkgInfo.nDataSize = 0;
	}

	if (tPkgInfo.nStackSize == 0)
		tPkgInfo.nStackSize = CMemory::DefaultStackSize;
	else if (tPkgInfo.nStackSize < CMemory::MinStackSize)
		tPkgInfo.nStackSize = CMemory::MinStackSize;
	else if (tPkgInfo.nStackSize > CMemory::MaxStackSize)
		VASM_THROW_ERROR(t_csz("Invalid package: Requested stack size exceeds maximum allowed size"));
	// Align to 64 byte boundary
	nReminder = tPkgInfo.nStackSize % 64;
	if (nReminder > 0)
		tPkgInfo.nStackSize += 64 - nReminder;
	//
	nMemorySize += tPkgInfo.nStackSize;

	if (nMemorySize > CMemory::MaxSize)
		VASM_THROW_ERROR(t_csz("Invalid package: Total memory size exceeds maximum allowed size"));

	// Load Symbols
	if (tSymbolsSection.nBase > 0 && tSymbolsSection.nSize > 0)
	{
		LoadSymbols(is, tPkgInfo, tSymbolsSection, tPkgInfo.nCodeBase, tPkgInfo.nDataBase);

		// Load debug info (it has dependency from symbols and could be loaded only after symbols)
		if (tDbgInfoSection.nBase > 0 && tDbgInfoSection.nSize > 0)
			LoadDebuginfo(is, tPkgInfo, tDbgInfoSection);
	}

	// Allocate requested memory
	pMemory->Resize(nMemorySize);

	// Load code section
	LoadIntoMemory(is, pMemory, tCodeSection, tPkgInfo.nCodeBase);

	// Load data section
	if (tPkgInfo.nDataBase > 0 && tPkgInfo.nDataSize > 0)
		LoadIntoMemory(is, pMemory, tDataSection, tPkgInfo.nDataBase);
}

void CLoader::LoadSymbols(
	std::istream& is, SPackageInfo& tPkgInfo,
	SSectionInfo const& tSection, t_address nCodeBase, t_address nDataBase)
{
	m_aSymbolToPackageFuncIdx.clear();

	SSymbolTableSection tSymbols;
	is.seekg((std::istream::pos_type) tSection.nBase);
	VASM_CHECK_X(is.good(), t_csz("Loader: Symbols section could not be loaded"));
	is.read((char*) &tSymbols, sizeof(SSymbolTableSection));
	VASM_CHECK_X(is.good(), t_csz("Loader: Symbols section could not be loaded"));

	if (tSymbols.nEntryCount == 0)
		return;

	// Read Symbol entries
	std::vector<SSymbolTableSection::SEntry> aEntries(tSymbols.nEntryCount);
	is.read((char*) aEntries.data(), tSymbols.nEntryCount * sizeof(SSymbolTableSection::SEntry));
	VASM_CHECK_X(is.good(), t_csz("Loader: Symbols section entries could not be loaded"));

	m_aSymbolToPackageFuncIdx.resize(tSymbols.nEntryCount, g_ciInvalid);

	for (t_index i = 0; i < tSymbols.nEntryCount; ++i)
	{
		SSymbolTableSection::SEntry const& tEntry = aEntries.at(i);
		// Read symbol name
		VASM_CHECK_X(tEntry.nNamePos > 0 && tEntry.nNameSize > 0, base::toStr("Loader: Invalid symbol entry at index %1", i));
		t_string sSymbolName((t_string::size_type) tEntry.nNameSize, char(0));
		is.seekg((std::istream::pos_type) tEntry.nNamePos);
		VASM_CHECK_X(is.good(), t_csz("Loader: Symbol name could not be loaded"));
		is.read(&sSymbolName.front(), tEntry.nNameSize);
		VASM_CHECK_X(is.good(), t_csz("Loader: Symbol name could not be loaded"));

		if (tEntry.isFunc)
		{
			m_aSymbolToPackageFuncIdx[i] = (t_index) tPkgInfo.aFunctionTable.size();
			tPkgInfo.aFunctionTable.push_back(SFunctionInfo(
				std::move(sSymbolName), tEntry.eType, tEntry.nBase + nCodeBase, tEntry.nSize));
		}
		else
		{
			tPkgInfo.aVariableTable.push_back(SVariableInfo(
				std::move(sSymbolName), tEntry.eType, tEntry.nBase + nDataBase, tEntry.nSize));
		}
	}
}

void CLoader::LoadDebuginfo(
	std::istream& is, SPackageInfo& tPkgInfo, SSectionInfo const& tSection)
{
	SDebugInfoSection tDbgInfo;
	is.seekg((std::istream::pos_type) tSection.nBase);
	VASM_CHECK_X(is.good(), t_csz("Loader: Debug info section could not be loaded"));
	is.read((char*) &tDbgInfo, sizeof(SDebugInfoSection));
	VASM_CHECK_X(is.good(), t_csz("Loader: Debug info section could not be loaded"));

	if (tDbgInfo.nEntryCount == 0)
		return;

	// Read debug entries
	std::vector<SDebugInfoSection::SEntry> aEntries(tDbgInfo.nEntryCount);
	is.read((char*) aEntries.data(), tDbgInfo.nEntryCount * sizeof(SDebugInfoSection::SEntry));
	VASM_CHECK_X(is.good(), t_csz("Loader: Debug info section entries could not be loaded"));

	for (t_index i = 0; i < tDbgInfo.nEntryCount; ++i)
	{
		SDebugInfoSection::SEntry const& tEntry = aEntries.at(i);
		VASM_CHECK_IDX_X(tEntry.nSymbolIndex, (t_size) m_aSymbolToPackageFuncIdx.size(),
					     t_csz("Loader: Debug info symbol index doesn't match with symbol table"));

		t_index nFuncIdx = m_aSymbolToPackageFuncIdx[tEntry.nSymbolIndex];
		VASM_CHECK_X(nFuncIdx != g_ciInvalid, 
					 t_csz("Loader: Debug info symbol index doesn't match with symbol table"));

		SFunctionInfo& tFuncInfo = tPkgInfo.aFunctionTable[nFuncIdx];

		// Read source unit name
		if (tEntry.nSrcUnitPos > 0 && tEntry.nSrcUnitSize > 0)
		{
			t_string sUnitName((t_string::size_type) tEntry.nSrcUnitSize, char(0));
			is.seekg((std::istream::pos_type) tEntry.nSrcUnitPos);
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(source unit name) could not be loaded"));
			is.read(&sUnitName.front(), tEntry.nSrcUnitSize);
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(source unit name) could not be loaded"));
			tFuncInfo.sSrcUnit = std::move(sUnitName);
		}

		// Save Function line number info
		tFuncInfo.nBaseLine = tEntry.nBaseLine;
		tFuncInfo.nSizeLine = tEntry.nSizeLine;

		// Read function local labels entries
		if (tEntry.nLabelEntriesPos > 0 && tEntry.nLabelEntryCount > 0)
		{
			std::vector<SDebugInfoSection::SLabelEntry> aLblEntries(tEntry.nLabelEntryCount);
			is.seekg((std::istream::pos_type) tEntry.nLabelEntriesPos);
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(label entries) could not be loaded"));
			is.read((char*) aLblEntries.data(), tEntry.nLabelEntryCount * sizeof(SDebugInfoSection::SLabelEntry));
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(label entries) could not be loaded"));

			for (t_index j = 0; j < tEntry.nLabelEntryCount; ++j)
			{
				SDebugInfoSection::SLabelEntry const& tLblEntry = aLblEntries.at(j);

				// Read label name
				VASM_CHECK_X(tLblEntry.nNamePos > 0 && tLblEntry.nNameSize > 0,
							 base::toStr("Loader: Invalid function label entry at index (%1, %2)", i, j));
				t_string sLblName((t_string::size_type) tLblEntry.nNameSize, char(0));
				is.seekg((std::istream::pos_type) tLblEntry.nNamePos);
				VASM_CHECK_X(is.good(), t_csz("Loader: Local label name could not be loaded"));
				is.read(&sLblName.front(), tLblEntry.nNameSize);
				VASM_CHECK_X(is.good(), t_csz("Loader: Loal label name could not be loaded"));

				tFuncInfo.aLabels.push_back(SSymbolInfo(std::move(sLblName), tFuncInfo.nAddress + tLblEntry.nOffset));
			}
		}

		// And finally load the PDB (line number to address mapping)
		if (tEntry.nCodeTblPos > 0)
		{
			std::vector<t_address> aCodeTbl(tEntry.nSizeLine, 0);

			is.seekg((std::istream::pos_type) tEntry.nCodeTblPos);
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(code table) could not be loaded"));
			is.read((char*) aCodeTbl.data(), tEntry.nSizeLine * sizeof(t_address));
			VASM_CHECK_X(is.good(), t_csz("Loader: DbgInfo(code table) could not be loaded"));

			// Adjust addresses
			for (auto& nVal : aCodeTbl)
				nVal += tFuncInfo.nAddress;

			tFuncInfo.aCodeTbl = std::move(aCodeTbl);
		}
	}
}

void CLoader::LoadIntoMemory(
	std::istream& is, CMemoryPtr pMemory, SSectionInfo const& tSection, t_address nBase)
{
	is.seekg((std::istream::pos_type) tSection.nBase);
	VASM_CHECK_X(is.good(), base::toStr("Loader: Section %1 could not be loaded", int(tSection.eType)));
	is.read( &(pMemory->operator[]<char>(nBase)), tSection.nSize);
	VASM_CHECK_X(is.good(), base::toStr("Loader: Section %1 could not be loaded", int(tSection.eType)));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef VASM_CORE_PACKAGE_INFO_H
#define VASM_CORE_PACKAGE_INFO_H

//
// Includes
//
#include "definitions.h"
#include "value.h"
#include <base_utility.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Section info
//
struct SSectionInfo
{
	enum class EType
	{
		Info,		// package information section
		Code,		// Compiled code section
		Data,		// Compiled data section
		SymbolTbl,	// Symbol table
		RelocTbl,	// Code relocation table
		DbgInfo,	// Debug information
		CeckSum,	// Package check summary
	};

	EType	eType; // section type
	t_size	nBase; // offset of the section within tte package file
	t_size	nSize; // section size in bytes

	// Contructors
	inline SSectionInfo(EType type);
	inline SSectionInfo(EType type, t_uoffset base, t_uoffset size);
	inline SSectionInfo(SSectionInfo const&) = default;
	inline SSectionInfo& operator=(SSectionInfo const&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Package header
//
struct SPackageHeader
{
	enum
	{
		SignatureSize	= 12,
		CurrentVersion	= 1,
		SectionCount_V1	= 7,
	};

	char const		cszSignatue[SignatureSize]; // "TK.VASM.PKG" + '\0'
	uint const		cnVersion; // Package version
	uint64 const	cnPadding; // reserved
	
	t_count	const	nSectionCount = SectionCount_V1;
	SSectionInfo	aSections[SectionCount_V1];

	inline SPackageHeader();
	inline SPackageHeader(SPackageHeader const&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Program Info section
//
struct SProgramInfoSection
{
	enum { Type = int(SSectionInfo::EType::Info) };

	t_uoffset	nCodeBase		= 0; // Preferred base address from where Code section should be loaded
									 // 0 means load from the first available address
	t_uoffset	nDataBase		= 0; // Preferred base address from where Data section should be loaded						
									 // 0 means load from the first available address
	t_uoffset	nProgramStart	= 0; // Program entry point offset within the code section
	t_uoffset	nStackSize		= 0; // Stack size used by the program, 0 means use default stack size

	inline SProgramInfoSection() = default;
	inline SProgramInfoSection(SProgramInfoSection const&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Symbol table section
//
struct SSymbolTableSection
{
	enum { Type = int(SSectionInfo::EType::SymbolTbl) };

	struct SEntry
	{
		bool		isFunc = false;	// Functions vs Variable
		EValueType	eType = EValueType::Unknown; // Viriable type or Function return type
		t_uoffset	nBase = 0;		// Offset within the corresponding section
		t_uoffset	nSize = 0;		// Occupied memory size in bytes
		t_uoffset	nNamePos = 0;	// Symbol name position in the local buffer following entries
		t_uoffset	nNameSize = 0;	// Symbol name size
	};

	t_count	nEntryCount = 0;
	SEntry	aEntries[];

	//char	aBuffer; // After entries local character buffer follows which contains symbol names

	inline SSymbolTableSection() = default;
	inline SSymbolTableSection(SSymbolTableSection const&) = default;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Relocation table section
//
struct SRelocationTableSection
{
	enum { Type = int(SSectionInfo::EType::RelocTbl) };

	t_count		nFuncCount	= 0;
	t_count		nVarCount	= 0;

	// Offsets within the code section where immediate address value is written 
	// At first function address locations are stored then Variable address locations
	// Usually it is ASSIGN A?, ADDR_VAL instruction, table elements point to the address location relative to the code
	t_uoffset	aAddressLocations[]; 

	inline SRelocationTableSection() = default;
	inline SRelocationTableSection(SRelocationTableSection const&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Debug information table section
//
struct SDebugInfoSection
{
	enum { Type = int(SSectionInfo::EType::DbgInfo) };

	struct SLabelEntry
	{
		t_uoffset	nOffset		= 0; // Label offset within the function code relative to function base address
		t_uoffset	nNamePos	= 0; // Label name start pos later within the section
		t_uoffset	nNameSize	= 0; // Label name size
	};

	struct SEntry
	{
		t_index		nSymbolIndex = 0; // Index in the symbol table

		t_index		nBaseLine = 0;	// Funtion base line number within the src unit
		t_count		nSizeLine = 0;	// Function size in line numbers within the src unit

		t_uoffset	nSrcUnitPos  = 0; // Source unit name start pos later within the section
		t_count		nSrcUnitSize = 0; // Source unit name size

		t_uoffset	nCodeTblPos = 0; // Function relative line number to address mapping start pos later within the section (PDB)

		t_uoffset	nLabelEntriesPos = 0; // nLabelEntries start pos later within the section
		t_count		nLabelEntryCount = 0; // Function local label entries count
	};

	t_size	nEntryCount = 0;
	SEntry	aEntries[];

	inline SDebugInfoSection() = default;
	inline SDebugInfoSection(SDebugInfoSection const&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Check summary section
//
struct SCheckSumSection
{
	enum { Type = int(SSectionInfo::EType::CeckSum) };

	uint16	nVersion = 0;
	uint16	nPadding = 0;
	uint32	nCheckSumCode = 0;
	uint32	nCheckSumData = 0;
	uint32	nCheckSumSymbolTbl = 0;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	SSectionInfo
//
inline SSectionInfo::SSectionInfo(EType tp) :
	eType(tp), nBase(0), nSize(0)
{
}

inline SSectionInfo::SSectionInfo(EType tp, t_uoffset base, t_uoffset size) :
	eType(tp), nBase(base), nSize(size)
{
}

//
//	SPackageHeader
//
inline SPackageHeader::SPackageHeader() :
	cszSignatue{'T', 'K', '.', 'V', 'A', 'S', 'M', '.', 'P', 'K', 'G', '\0'},
	cnVersion(CurrentVersion),
	cnPadding(0),
	aSections{SSectionInfo::EType::Info,
			  SSectionInfo::EType::Code,
			  SSectionInfo::EType::Data,
			  SSectionInfo::EType::SymbolTbl,
			  SSectionInfo::EType::RelocTbl,
			  SSectionInfo::EType::DbgInfo,
			  SSectionInfo::EType::CeckSum}
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_PACKAGE_INFO_H

#ifndef VASM_CL_PACKAGE_H
#define VASM_CL_PACKAGE_H

//
// Includes
//
#include <core_definitions.h>
#include <core_value.h>
#include <core_package_info.h>


// STL
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_count		= core::t_count;
using t_uoffset		= core::t_uoffset;
using t_address		= core::t_address;
using EValueType	= core::EValueType;

////////////////////////////////////////////////////////////////////////////////
//
//	Program Info
//
struct SProgramInfo
{
	// Program entry point (index of the Main function in the symbol table)
	t_index		nEntryPoint	= g_ciInvalid;
	// Stack size used by the program, 0 means use default stack size
	t_uoffset	nStackSize		= 0;

	// Package name (compiled unit)
	t_string	sName;

	inline SProgramInfo() = default;
	inline SProgramInfo(SProgramInfo const&) = default;
	inline SProgramInfo(SProgramInfo&&) = default;
	inline SProgramInfo& operator=(SProgramInfo const&) = default;
	inline SProgramInfo& operator=(SProgramInfo&&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Symbol table section
//
struct SSymbolTable
{
	struct SEntry
	{
		bool		isFunc = false;	// Functions vs Variable
		EValueType	eType = EValueType::Unknown; // Viriable type or Function return type
		t_uoffset	nBase = 0;		// Offset within the corresponding section
		t_uoffset	nSize = 0;		// For variables size is elements count
									// For functions size is the occupied memory size in bytes
		t_string	sName;
	};

	std::vector<SEntry>	aEntries;


	inline SSymbolTable() = default;
	inline SSymbolTable(SSymbolTable const&) = default;
	inline SSymbolTable(SSymbolTable&&) = default;
	inline SSymbolTable& operator=(SSymbolTable const&) = default;
	inline SSymbolTable& operator=(SSymbolTable&&) = default;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Relocation table
//
struct SRelocationTable
{
	// Offsets within the code section at where Immediate address values are written 
	// Functions and Variables address locations are stored together
	// Usually it is ASSIGN A?, ADDR_VAL instruction, table elements point to the address location relative to the code
	std::vector<t_uoffset>	aAddressLocations; 

	inline SRelocationTable() = default;
	inline SRelocationTable(SRelocationTable const&) = default;
	inline SRelocationTable(SRelocationTable&&) = default;
	inline SRelocationTable& operator=(SRelocationTable const&) = default;
	inline SRelocationTable& operator=(SRelocationTable&&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Debug information table
//
struct SDebugInfo
{
	struct SLabelEntry
	{
		t_uoffset	nOffset	= 0; // Label offset within the function code relative to function base address
		t_string	sName;
	};

	struct SEntry
	{
		t_index		nSymbolIndex = 0; // Index in the symbol table

		t_index		nBaseLine = 0;	// Funtion base line number within the src unit
		t_count		nSizeLine = 0;	// Function size in line numbers within the src unit

		t_string	sSrcUnitName;

		// Function relative line number to code section relative address mapping (PDB)
		std::vector<t_uoffset>	aCodeTbl; 

		// Function local label entries
		std::vector<SLabelEntry> aLabelEntries;
	};

	std::vector<SEntry>	aEntries;

	inline SDebugInfo() = default;
	inline SDebugInfo(SDebugInfo const&) = default;
	inline SDebugInfo(SDebugInfo&&) = default;
	inline SDebugInfo& operator=(SDebugInfo const&) = default;
	inline SDebugInfo& operator=(SDebugInfo&&) = default;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Package
//
struct SPackage
{
	SProgramInfo		tInfo;
	SSymbolTable		oSymbolTbl;
	SRelocationTable	oRelocTbl;
	SDebugInfo			oDebugInfo; // PDB

	// Program code 
	std::vector<uint8>	aCode;

	// Program data 
	std::vector<uint8>	aData;

	// Contructors
	inline SPackage() = default;
	inline SPackage(SPackage const&) = default;
	inline SPackage(SPackage&&) = default;
	inline SPackage& operator=(SPackage const&) = default;
	inline SPackage& operator=(SPackage&&) = default;
};

using t_aPackages = std::vector<SPackage>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_PACKAGE_H

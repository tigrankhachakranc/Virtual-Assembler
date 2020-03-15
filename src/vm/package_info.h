#ifndef VASM_VM_PACKAGE_INFO_H
#define VASM_VM_PACKAGE_INFO_H

//
// Includes
//
#include <core_package_info.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_count		= core::t_count;
using t_address		= core::t_address;
using t_uoffset		= core::t_uoffset;

using CValue		= core::CValue;
using EValueType	= core::EValueType;

////////////////////////////////////////////////////////////////////////////////
//
//	Symbol information
//
struct SSymbolInfo
{
	t_string	sName;
	t_address	nAddress; // Base address in the memory

	// Contructors
	inline SSymbolInfo();
	inline SSymbolInfo(t_string name, t_address addr);
	inline SSymbolInfo(SSymbolInfo const&) = default;
	inline SSymbolInfo(SSymbolInfo&&) = default;
	inline SSymbolInfo& operator=(SSymbolInfo const&) = default;
	inline SSymbolInfo& operator=(SSymbolInfo&&) = default;
};

using t_SymbolTable = std::vector<SSymbolInfo>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Variable information
//
struct SVariableInfo : SSymbolInfo
{
	EValueType		eType;	// Variable type
	t_count			nSize;	// Variable array size (number of elements)

	// Contructors
	inline SVariableInfo();
	inline SVariableInfo(t_string name, EValueType type, t_address addr, t_size size);
	inline SVariableInfo(SVariableInfo const&) = default;
	inline SVariableInfo(SVariableInfo&&) = default;
	inline SVariableInfo& operator=(SVariableInfo const&) = default;
	inline SVariableInfo& operator=(SVariableInfo&&) = default;
};

using t_VariableTable = std::vector<SVariableInfo>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Function information
//
struct SFunctionInfo : SSymbolInfo
{
	EValueType		eType;		// Function return value type
	t_uoffset		nSize;		// Function size in bytes
	
	t_SymbolTable	aLabels;	// Local labels table declared within the function
	t_string		sSrcUnit;	// Source unit (original file name from where function comes)

	t_index					nBaseLine;	// Base line number in src
	t_size					nSizeLine;	// Function size by line number in src
	std::vector<t_address>	aCodeTbl;	// Function relative line number to address mapping (per line number code offset array)

	// Contructors
	inline SFunctionInfo();
	inline SFunctionInfo(t_string name, EValueType type, t_address baseAddr, t_uoffset size);
	inline SFunctionInfo(SFunctionInfo const&) = default;
	inline SFunctionInfo(SFunctionInfo&&) = default;
	inline SFunctionInfo& operator=(SFunctionInfo const&) = default;
	inline SFunctionInfo& operator=(SFunctionInfo&&) = default;
};

using t_FunctionTable = std::vector<SFunctionInfo>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Package Info
//
struct SPackageInfo
{
	// Program code base address
	t_address		nCodeBase = 0;
	// Program code size in bytes counted from its base
	t_uoffset		nCodeSize = 0;

	// Program data base address
	t_address		nDataBase = 0;
	// Program data size in bytes counted from its base
	t_uoffset		nDataSize = 0;

	// Program stack size in bytes, 0 means use default stack size
	t_uoffset		nStackSize = 0;

	// Address of the entry point of the program
	t_address		nProgramStart = 0;

	// Index of the Main function
	t_index			nMainFuncIndex = g_ciInvalid;

	// Reserved
	uint32			nPadding = 0; 

	// Variables information defined in the package
	t_VariableTable	aVariableTable;

	// Functions information defined in the package
	t_FunctionTable	aFunctionTable;

	// Contructors
	inline SPackageInfo() = default;
	inline SPackageInfo(SPackageInfo const&) = default;
	inline SPackageInfo(SPackageInfo&&) = default;
	inline SPackageInfo& operator=(SPackageInfo const&) = default;
	inline SPackageInfo& operator=(SPackageInfo&&) = default;
};

using t_PackageTable = std::vector<SPackageInfo>;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	SSymbolInfo
//
inline SSymbolInfo::SSymbolInfo() :
	nAddress(0)
{
}

inline SSymbolInfo::SSymbolInfo(t_string name, t_address addr) :
	sName(std::move(name)), nAddress(addr)
{
}


//
//	SVariableInfo
//
inline SVariableInfo::SVariableInfo() :
	SSymbolInfo(), eType(EValueType::Unknown), nSize(0)
{
}

inline SVariableInfo::SVariableInfo(
	t_string name, EValueType type, t_address addr, t_size size) :
	SSymbolInfo(std::move(name), addr), eType(type), nSize(size)
{
}


//
//	SFunctionInfo
//
inline SFunctionInfo::SFunctionInfo() :
	nSize(0), nBaseLine(0), nSizeLine(0)
{
}

inline SFunctionInfo::SFunctionInfo(
	t_string name, EValueType type, t_address baseAddr, t_uoffset size) :
	SSymbolInfo(std::move(name), baseAddr), eType(type), nSize(size), nBaseLine(0), nSizeLine(0)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_VM_PACKAGE_INFO_H

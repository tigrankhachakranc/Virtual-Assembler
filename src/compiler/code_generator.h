#ifndef VASM_CL_CODE_GENERATOR_H
#define VASM_CL_CODE_GENERATOR_H

//
// Includes
//
#include "intermediate.h"
#include "package.h"
#include <core_command_library.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CCommandLibraryPtr = core::CCommandLibraryPtr;
using t_mapSymbolTable = std::unordered_map<CStringRef, t_index, std::hash<t_string>, std::equal_to<t_string> >;

////////////////////////////////////////////////////////////////////////////////
//
//	Code Generator
//	Generates VASM binary code from parsed intermediate input
//
class CCodeGenerator
{
public:
	CCodeGenerator(CCommandLibraryPtr pCmdLib);
	~CCodeGenerator();

	CCodeGenerator(CCodeGenerator const&) = delete;
	CCodeGenerator(CCodeGenerator&&) = delete;
		
public:
	//
	//	Methods
	//
	void Make(SUnit const& tUnit, SPackage& tPackage);
	
private:
	//
	//	Implementation
	//
	void MakeData	(SUnit const& tUnit, SPackage& tPackage);
	void MakeCode	(SUnit const& tUnit, SPackage& tPackage, bool bIncludeDebugInfo);
	void MakeFunc	(SFunction const& tFunc, SPackage& tPackage,
					 t_uoffset& nCodeMarker, SDebugInfo::SEntry* pDbgInfo);

private:
	//
	//	Contents
	//
	CCommandLibraryPtr	 m_pCmdLibrary;
	t_mapSymbolTable	 m_mapSymbolTable;
	std::vector<t_index> m_aVarToSymblIdxMapping;
	std::vector<t_index> m_aFuncToSymblIdxMapping;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_CODE_GENERATOR_H

#ifndef VASM_CL_COMPILER_H
#define VASM_CL_COMPILER_H

//
// Includes
//
#include "package.h"
#include <core_command_library.h>
#include <base_exception.h>

// STL
#include <list>
#include <vector>
#include <string>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CCommandLibraryPtr = core::CCommandLibraryPtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Compiler
//	Compiles VASM Program
//
class CCompiler
{
public:
	CCompiler();
	~CCompiler();

public:
	//
	// Methods
	//

	// Compiles input and keeps compiled package
	// Compile could be called multiple times before Link
	void Compile(t_string sInputUnitName, std::istream& oInput);

	// Links compiled packages and writes final binary excutable to the output
	void Link(t_string sOutpuPackageName, std::ostream& oOutput);

	// Static helper to compile source files and to output binary file
	static void Build(std::vector<t_string> const& aSrcFiles, t_string const& sOutputFile);
	static void Build(std::vector<t_string> const& aSrcFiles, t_string sOutputName, std::ostream& oOutput);

private:
	//
	//	Contents
	//
	std::list<SPackage>	m_lstPackages;
	CCommandLibraryPtr	m_pCmdLib;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_COMPILER_H
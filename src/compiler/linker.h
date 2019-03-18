#ifndef VASM_CL_LINKER_H
#define VASM_CL_LINKER_H

//
// Includes
//
#include "package.h"
#include <base_utility.h>
#include <core_command_library.h>

// STL
#include <list>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_lstPackages = std::list<SPackage>;
using t_mapSymbolTable = std::unordered_map<CStringRef, t_index, std::hash<t_string>, std::equal_to<t_string> >;
using CCommandLibraryPtr = core::CCommandLibraryPtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Linker
//	Links multiple packages together into single Package
//
class CLinker
{
public:
	CLinker(CCommandLibraryPtr pCmdLib);
	~CLinker();

	CLinker(CLinker const&) = delete;
	CLinker(CLinker&&) = delete;

public:
	//
	//	Linker Error
	//
	class CError : public base::CException
	{
		typedef base::CException Base;
	public:
		CError() = default;
		CError(t_csz psErrMsg, t_string const& sPackage);
		CError(t_string const& sErrMsg, t_string const& sPackage);
		~CError() = default;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

	public:
		inline t_string const& ErrorMsg() const;
		inline t_string const& Package() const;

	private:
		t_string	m_sPkgName;
	};

public:
	//
	// Method
	//

	// Merges together packages and produces binary executable
	SPackage Build(t_lstPackages const& lstPackages, t_string sProgramName);

private:
	//
	//	Implementation
	//
	// Writes program header code wich is the actual program entry point
	void WriteHeader(SPackage& tFinalPackage);
	// Merges packages together
	void Merge(SPackage& tFinalPackage, t_lstPackages const& lstPackages);
	// Resolves label indexes and fills in final offsets
	void ResolveOffsets(SPackage& tFinalPackage);

private:
	//
	//	Contents
	//
	t_mapSymbolTable	m_mapSymbols;
	CCommandLibraryPtr	m_pCmdLib;

	static const t_uoffset s_cnGranul = 16;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	CLinker::CError
//
inline CLinker::CError::CError(
	t_csz psErrMsg, t_string const& sPkgName) :
	Base(t_string(psErrMsg)), m_sPkgName(sPkgName)
{
}

inline CLinker::CError::CError(
	t_string const& sErrMsg, t_string const& sPkgName) :
	Base(sErrMsg), m_sPkgName(sPkgName)
{
}

inline t_string const& CLinker::CError::ErrorMsg() const
{
	return m_sErrorMsg;
}

inline t_string const& CLinker::CError::Package() const
{
	return m_sPkgName;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_LINKER_H

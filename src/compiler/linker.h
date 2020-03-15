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
	class CError : public base::CError
	{
		using Base = base::CError;
	public:
		CError(t_csz const pcszErrMsg, t_string const& sPackage);
		CError(t_string const& sErrMsg, t_string const& sPackage);
		CError(t_string&& sErrMsg, t_string const& sPackage);
		~CError() override;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

		void operator=(CError const&) = delete;
		void operator=(CError&&) = delete;

	public:
		inline t_string const& ErrorMsg() const;
		inline t_string const& Package() const;

	private:
		t_string const	m_csPkgName;

		static t_csz const s_FixedError;
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
	void WriteCodeHeader(SPackage& tFinalPackage);
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
	t_csz const pcszErrMsg, t_string const& sPkgName) :
	Base(pcszErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_csPkgName(sPkgName)
{
}

inline CLinker::CError::CError(
	t_string const& sErrMsg, t_string const& sPkgName) :
	Base(sErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_csPkgName(sPkgName)
{
}

inline CLinker::CError::CError(
	t_string&& sErrMsg, t_string const& sPkgName) :
	Base(sErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_csPkgName(sPkgName)
{
}

inline t_string const& CLinker::CError::ErrorMsg() const
{
	return m_sErrorMsg;
}

inline t_string const& CLinker::CError::Package() const
{
	return m_csPkgName;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_LINKER_H

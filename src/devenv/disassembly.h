#ifndef VASM_DEV_DISASSEMBLY_H
#define VASM_DEV_DISASSEMBLY_H

//
// Includes
//
#include <core_package_info.h>
#include <core_command_library.h>
#include <core_address_recovery.h>
#include <core_memory.h>
#include <vm_loader.h>

// STL
#include <string>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace dev {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_byte		= core::t_byte;
using t_word		= core::t_word;
using t_dword		= core::t_dword;
using t_qword		= core::t_qword;
using CValue		= core::CValue;
using EValueType	= core::EValueType;
using t_address		= core::t_address;

using CMemoryPtr			= core::CMemoryPtr;
using IAddressRecoveryPtr	= core::IAddressRecoveryPtr;
using CAddressRecoveryPtr	= core::CAddressRecoveryPtr;
using CCommandLibraryPtr	= core::CCommandLibraryPtr;
using SPackageInfo			= vm::SPackageInfo;


////////////////////////////////////////////////////////////////////////////////
//
//	Disassembler
//
class CDisassembler
{
public:
	CDisassembler();
	~CDisassembler();

	// Neither copiable, nor movable
	CDisassembler(CDisassembler const&) = delete;
	CDisassembler(CDisassembler&&) = delete;
	void operator=(CDisassembler const&) = delete;
	void operator=(CDisassembler&&) = delete;

public:
	//
	//	Methods
	//

	// Loads binary input, disassembles and produces text output
	void Disassemble(std::istream& oInput, std::ostream& oOutput);

	// Static helper to disassemble binary executable
	static void Disassemble(t_string const& sBinaryFile, t_string const& sOutputFile);

protected:
	//
	//	Implementation
	//
	void InitAddressRecovery();
	void ProcessVariables(std::ostream& oOutput);
	void ProcessFunctions(std::ostream& oOutput);
	void ProcessFunction (std::ostream& oOutput, vm::SFunctionInfo const& tFunc);

	// Address to symbol map (used by address recovery)
	using t_AddrssToSymbolMap = core::CAddressRecovery::t_AddrssToSymbolMap;

private:
	//
	//	Contents
	//
	CCommandLibraryPtr		m_pCmdLib;
	CMemoryPtr				m_pMemory;

	// Package with all necessary debug information
	SPackageInfo			m_tPackage;

	CAddressRecoveryPtr		m_pAddressRecovery;
};

using CDisassemblerPtr = std::shared_ptr<CDisassembler>;
using CDisassemblerWPtr = std::weak_ptr<CDisassembler>;
using CDisassemblerUPtr = std::unique_ptr<CDisassembler>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace dev
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_DEV_DISASSEMBLY_H
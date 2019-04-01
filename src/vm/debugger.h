#ifndef VASM_VM_DEBUGGER_H
#define VASM_VM_DEBUGGER_H

//
// Includes
//
#include "processor.h"
#include "package_info.h"
#include <base_utility.h>
#include <core_address_recovery.h>
#include <core_io_controller.h>

// STL
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_byte = core::t_byte;
using CMemory = core::CMemory;
using CAddressRecoveryPtr = core::CAddressRecoveryPtr;
using CCommandLibraryPtr  = core::CCommandLibraryPtr;
using CCommandLibrary	  = core::CCommandLibrary;
using CIOControllerPtr	  = core::CIOControllerPtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Debugger
//
class CDebugger
{
public:
	CDebugger();
	~CDebugger();

	// Neither copiable, nor movable
	CDebugger(CDebugger const&) = delete;
	CDebugger(CDebugger&&) = delete;
	void operator=(CDebugger const&) = delete;
	void operator=(CDebugger&&) = delete;

public:
	struct SCodeLineInfo
	{
		t_address	nAddress	= core::cnInvalidAddress;
		t_uoffset	nRelOffset	= core::cnInvalidAddress; // Function relative offset
		t_index		nLineNum	= g_ciInvalid;
		CStringRef	sFuncName;
		CStringRef	sUnitName;
	};

	using t_aCodeLineInfos = std::vector<SCodeLineInfo>;

public:
	//
	//	Methods
	//
	void Reset();

	// Initializers
	void Init(CCommandLibraryPtr pCmdLib, CProcessorPtr pCPU,
			  CIOControllerPtr pIOController, SPackageInfo tPackageInfo);

	// Runs till Breakpoint hit or program finishes
	void Run(bool bNoDebug = false);
	// Runs till specifed adress or program finishes
	void RunTo(t_address nTargetAddress, bool bNoDebug = false);
	// Runs single instruction
	void StepIn(t_size nCount = 1);
	// Runs single instruction by skipping function calls
	void StepOver(t_size nCount = 1);
	// Runs till Breakpoint or program will return from the current function
	void StepOut();
	// Stops program execution if it currently running
	void Stop();

	// Other debug functions
	void ResetProgram();

	// Lookups corrsponding address for the specified src unit and line number and sets IP
	// Returns true if address found, false otherwise
	bool ChangeIP(t_address nNewIP);
	void ChangeFlags(core::CFlags const&);

	enum class ERegType {ADR, GP};
	void ChangeRegister(ERegType eRegType, uint nRegIdx, CValue const& oValue);

	// Lookups variable by name and changes its value
	void ChangeVariable(t_string const& sName, CValue const& oValue);
	// Lookups variable by name and returns pointer to its info structure if found
	SVariableInfo GetVariableInfo(t_string const& sName) const;

	// Breakpoints support
	t_aCodeLineInfos GetBreakPoints();
	// Sets debug break-point for the specified src unit and line number 
	// throws error if src unit & line number not found
	void SetBreakPoint(t_address nAddress);
	// Sets debug break-point for the specified src unit and line number 
	// throws error if src unit & line number not found
	void RemoveBreakPoint(t_address nAddress);
	void RemoveBreakPointByIndex(t_index nIndex);
	void RemoveAllBreakPoints();

	// Debug helpers to resolve address
	t_address ResolveAddressFromLabel(t_string const& sLabel) const;
	t_address ResolveAddressFromFunction(t_string const& sFunction, t_string const& sLabel = {}) const;
	t_address ResolveAddressFromSource(t_index nLineNumber, t_string const& sSrcUnit) const;
	bool CheckCodeAddress(t_address nAddress) const;

	// Returns src code info fir the specifed code address
	SCodeLineInfo GetCodeLineInfo(t_address nAddress) const;
	// Returns disassembled text for the specified code address
	// also changes address value to point to the next command
	t_string GetDisassembledCommand(t_address& nAddress, t_string* pBinaryRepresentation) const;

	// Extracts and returns current function call stack
	t_aCodeLineInfos GetFunctionCallStack() const;

	// Returns registered port ranges and associated device names
	std::vector<std::pair<core::t_PortRange, t_string>> GetPortsInfo() const;

	// Dumpers
	void Dump(std::ostream&, bool bText) const;
	void DumpMemory(std::ostream&, t_address nAddress, t_size nSizeBytes, bool bText) const;
	void DumpCode(std::ostream&, t_address nAddress, t_size nSizeBytes, bool bText) const;
	void DumpData(std::ostream&, t_address nAddress, t_size nSizeBytes, bool bText) const;
	void DumpStack(std::ostream&, t_size nDepth, bool bText) const;

	// Helper Accessors
	inline t_VariableTable const& Variables() const;
	inline CProcessor::SStatus const& CPUStatus() const;
	inline CProcessor::SState const& CPUState() const;
	inline CMemory const& Memory() const;

protected:
	//
	//	Debug Info
	//

	// Variable name to index map
	using t_mapVariables = std::unordered_map<CStringRef, t_index, std::hash<t_string>, std::equal_to<t_string> >;

	// Functions
	using t_AddressRange = base::TRange<core::t_address>;
	using t_AddressRangeComparator = base::TRangeLessThanComparator<core::t_address>;
	// Function address range to index map
	using t_FunctionMap = std::map<t_AddressRange, t_index, t_AddressRangeComparator>;

	// Breakpoint store: memory address to instruction OpCode map
	using t_BreakPointMap = std::unordered_map<core::t_address, EOpCode>;

	// Address to symbol map (used by address recovery)
	using t_AddrssToSymbolMap = core::CAddressRecovery::t_AddrssToSymbolMap;

	// Function call stack (array of function index, relative line number pairs
	using t_CallStack = std::vector<std::pair<t_index, t_index> >;

protected:
	//
	//	Internal routines
	//
	inline CMemory& RWMemory();

	// Turns all breakpoints On/Off
	void TurnBreakPoints(bool bOn);
	// If specified address is under BP then turns it On/Off and returns true, false otherwise
	bool TurnBreakPointAt(t_address nAddress, bool bOn);
	// Sets Break point at the specified address
	void SetBreakPointAt(t_address nAddress, bool bOn);
	// Remove Break point from the specified address, returns true if BP actually removed
	bool RemoveBreakPointAt(t_address nAddress);

	// Finds corresponding code address for the specified source unit & line number
	// Returns invalid address if not found, complexity of this this function is O(n)
	t_address LookupCode(t_string const& sSrcUnit, t_index nLineNumber) const;
	// Finds corresponding function index for the specified source unit & function name
	// Returns invalid index if not found, complexity of this this function is O(n)
	t_index LookupFunction(t_string const& sFuncName) const;
	// Reverse finds corresponding function index & function relative line number for the specified code address
	// Returns true if found, false otherwise, complexity of this function is O(Log(n))+O(m)
	bool LookupSource(t_address nCodeAddr, t_index& nFuncIdx, t_index& nLineNumber) const;
	// Reverse finds corresponding source unit & line number for the specified code address
	// Returns true if found, false otherwise
	bool LookupSource(t_address nCodeAddr, CStringRef& sFuncName, CStringRef& sSrcUnit,
					  t_index& nLineNumber, t_uoffset& nFuncOffset) const;

	// Extracts and returns current function call stack
	t_CallStack ExtractFunctionCallStack() const;

	static void DumpHelper(std::ostream&, t_address nAddress, t_size nSizeBytes, t_byte const* pMemory);

private:
	//
	//	Contents
	//
	CCommandLibraryPtr		m_pCmdLib;
	CProcessorPtr			m_pCPU;
	CIOControllerPtr		m_pIOController;

	// Package with all necessary debug information
	SPackageInfo			m_tPackage;

	t_mapVariables			m_mapVariables;
	t_FunctionMap			m_mapFunctions;
	CAddressRecoveryPtr		m_pAddressRecovery;
	
	t_BreakPointMap			m_mapBreakPoints;

	bool					m_bHexadecimalOutput = true;
};

using CDebuggerPtr = std::shared_ptr<CDebugger>;
using CDebuggerWPtr = std::weak_ptr<CDebugger>;
using CDebuggerUPtr = std::unique_ptr<CDebugger>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline t_VariableTable const& CDebugger::Variables() const
{
	return m_tPackage.aVariableTable;
}

inline CProcessor::SStatus const& CDebugger::CPUStatus() const
{
	VASM_CHECK_PTR(m_pCPU);
	return m_pCPU->Status();
}

inline CProcessor::SState const& CDebugger::CPUState() const
{
	VASM_CHECK_PTR(m_pCPU);
	return m_pCPU->State();
}

inline CMemory const& CDebugger::Memory() const
{
	VASM_ASSERT_PTR(m_pCPU);
	return m_pCPU->Memory();
}

inline CMemory& CDebugger::RWMemory()
{
	VASM_ASSERT_PTR(m_pCPU);
	return m_pCPU->Memory();
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_VM_DEBUGGER_H
//
//	Component
//
#define __COMPONENT__ "Debugger"

//
//	Includes
//
#include "debugger.h"
#include <core_decoder.h>
#include <base_utility.h>

// STL
#include <iomanip>
#include <sstream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CDebugger Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-tors
//
CDebugger::CDebugger() = default;
CDebugger::~CDebugger() = default;

//
//	Methods
//
void CDebugger::Reset()
{
	m_pCmdLib = nullptr;
	m_pCPU = nullptr;
	m_pIOController = nullptr;
	m_pAddressRecovery = nullptr;

	m_mapVariables.clear();
	m_mapFunctions.clear();
	m_mapBreakPoints.clear();

	m_tPackage = SPackageInfo();
}

void CDebugger::Init(
	CCommandLibraryPtr pCmdLib, CProcessorPtr pCPU,
	CIOControllerPtr pIOController, SPackageInfo tPackageInfo)
{
	if (pCmdLib == nullptr)
		VASM_THROW_ERROR("Failed to initialize debugger: Invalid Command Library pointer.");
	if (pCPU == nullptr)
		VASM_THROW_ERROR("Failed to initialize debugger: Invalid CPU pointer.");

	Reset();

	m_pCmdLib = pCmdLib;
	m_pCPU = pCPU;
	m_pIOController = pIOController;
	m_tPackage = std::move(tPackageInfo);

	// Also collect symbols to intialize adress recovery
	t_AddrssToSymbolMap mapSymbols;

	// Init variables
	t_size nCount = t_size(m_tPackage.aVariableTable.size());
	for (t_index idx = 0; idx < nCount; ++idx)
	{
		SVariableInfo const& tVarInfo = m_tPackage.aVariableTable.at(idx);

		mapSymbols.insert(t_AddrssToSymbolMap::value_type(tVarInfo.nAddress, &tVarInfo.sName));
		m_mapVariables.insert({&tVarInfo.sName, idx});
	}

	// Init functions
	nCount = t_size(m_tPackage.aFunctionTable.size());
	for (t_index idx = 0; idx < nCount; ++idx)
	{
		SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable.at(idx);

		mapSymbols.insert({tFuncInfo.nAddress, &tFuncInfo.sName});
		
		t_AddressRange tFuncAddrRange(tFuncInfo.nAddress, tFuncInfo.nAddress + tFuncInfo.nSize - 1);
		m_mapFunctions.insert({tFuncAddrRange, idx});

		for (auto const& tItem : tFuncInfo.aLabels)
			mapSymbols.insert({tItem.nAddress, &tItem.sName});
	}

	// Niti address recover
	m_pAddressRecovery = CAddressRecoveryPtr(new core::CAddressRecovery(std::move(mapSymbols)));
	m_pCmdLib->SetAddressRecovery(m_pAddressRecovery);
}

void CDebugger::Run(bool bNoDebug)
{
	VASM_CHECK_PTR(m_pCPU);

	if (bNoDebug)
	{
		TurnBreakPoints(false);
		m_pCPU->Run();
		return;
	}

	// Enable break points
	TurnBreakPoints(true);

	// If Current instruction is BP then temporary turn it off
	bool bCurrBP = TurnBreakPointAt(m_pCPU->State().nIP, false);
	if (bCurrBP)
	{
		// Run single instruction then restore the BP and continue again
		m_pCPU->Run(true); 

		// Restore BP to hit next time CPU will pass over the same instruction
		// IP already shows next instruction, but CIP yet keeps previous value
		TurnBreakPointAt(m_pCPU->State().nCIP, true);
	}

	// Run CPU until finish or BP
	CProcessor::EStatus eStatus = m_pCPU->Run();

	if (eStatus == CProcessor::EStatus::Break)
	{	// Execution stopped on BP but IP shows to the next command
		// Move IP back to not skip original command at next run
		m_pCPU->ChangeableState().nIP = m_pCPU->State().nCIP;
	}

	// Disable break points
	TurnBreakPoints(false);
}

void CDebugger::RunTo(t_address nTargetAddrerss, bool bNoDebug)
{
	VASM_CHECK_PTR(m_pCPU);

	if (bNoDebug)
		TurnBreakPoints(false);
	else
		TurnBreakPoints(true);

	// If Current instruction is BP then temporary turn it off
	bool bCurrBP = TurnBreakPointAt(m_pCPU->State().nIP, false);
	// Run single instruction then restore the BP and continue again
	m_pCPU->Run(true);

	// Restore BP to hit next time CPU will pass over the same instruction
	// IP already shows next instruction, but CIP yet keeps previous value
	if (!bNoDebug && bCurrBP)
		TurnBreakPointAt(m_pCPU->State().nCIP, true);

	while (nTargetAddrerss != m_pCPU->State().nIP &&
		   m_pCPU->Status().eStatus == CProcessor::EStatus::Ready)
	{
		m_pCPU->Run(true);
	}
}

void CDebugger::StepIn(t_size nCount)
{
	VASM_CHECK_PTR(m_pCPU);

	while (nCount > 0 && m_pCPU->CanRun())
	{
		--nCount;

		// If Current instruction is BP then temporary turn it off
		TurnBreakPointAt(m_pCPU->State().nIP, false);
		// Run single instruction (trace)
		m_pCPU->Run(true);
	}
}

void CDebugger::StepOver(t_size nCount)
{
	VASM_CHECK_PTR(m_pCPU);

	// Enable break points
	TurnBreakPoints(true);

	while (nCount > 0 && m_pCPU->CanRun())
	{
		--nCount;

		// If Current instruction is BP then temporary turn it off
		bool bCurrBP = TurnBreakPointAt(m_pCPU->State().nIP, false);

		// Need to analyze target instruction to skip over CALL instruction
		EOpCode eOpCode = Memory().operator[]<EOpCode>(m_pCPU->State().nIP);

		if (eOpCode != EOpCode::CALLA && eOpCode != EOpCode::CALLR)
		{	// Behave like regular Step
			// Run single instruction (trace)
			m_pCPU->Run(true);

			// Restore BP to hit next time CPU will pass over the same instruction
			if (bCurrBP)
			{	// IP already shows next instruction, but CIP yet keeps previous value
				TurnBreakPointAt(m_pCPU->State().nCIP, true);
				bCurrBP = false;
			}
		}
		else
		{
			// Trace within loop until BP or execution will reach to the next instruction
			int nCallLevel = 0;
			CProcessor::EStatus eStatus;
			do
			{
				eStatus = m_pCPU->Run(true);

				// Analyze just executed instruction to find out corresponding CALLs & RETs
				// IP already shows next instruction, but CIP yet keeps reference on the previous one
				eOpCode = Memory().operator[]<EOpCode>(m_pCPU->State().nCIP);
				if (eOpCode == EOpCode::CALLA || eOpCode == EOpCode::CALLR)
					++nCallLevel;
				else if (eOpCode == EOpCode::RET)
					--nCallLevel;

				// Restore BP to hit next time CPU will pass over the same instruction
				if (bCurrBP)
				{	// IP already shows next instruction, but CIP yet keeps previous value
					TurnBreakPointAt(m_pCPU->State().nCIP, true);
					bCurrBP = false;
				}
			} while (eStatus == CProcessor::EStatus::Ready && nCallLevel != 0);

			if (eStatus == CProcessor::EStatus::Break)
			{	// Execution stopped on BP but IP shows to the next command
				// Move IP back to not skip original command at next run
				m_pCPU->ChangeableState().nIP = m_pCPU->State().nCIP;
			}
		}
	}
}

void CDebugger::StepOut()
{
	VASM_CHECK_PTR(m_pCPU);

	// Enable break points
	TurnBreakPoints(true);

	// If Current instruction is BP then temporary turn it off
	bool bCurrBP = TurnBreakPointAt(m_pCPU->State().nIP, false);

	// Trace within loop until BP or execution will return from the current function
	int nCallLevel = 1; // We assume that CPU currently is within the function
	CProcessor::EStatus eStatus;
	do
	{
		eStatus = m_pCPU->Run(true);

		// Analyze just executed instruction to find out corresponding CALLs & RETs
		// IP already shows next instruction, but CIP yet keeps reference on the previous one
		EOpCode eOpCode = Memory().operator[]<EOpCode>(m_pCPU->State().nCIP);
		if (eOpCode == EOpCode::CALLA || eOpCode == EOpCode::CALLR)
			++nCallLevel;
		else if (eOpCode == EOpCode::RET)
			--nCallLevel;

		// Restore BP to hit next time CPU will pass over the same instruction
		if (bCurrBP)
		{	// IP already shows next instruction, but CIP yet keeps previous value
			TurnBreakPointAt(m_pCPU->State().nCIP, true);
			bCurrBP = false;
		}

	} while (eStatus == CProcessor::EStatus::Ready && nCallLevel != 0);

	if (eStatus == CProcessor::EStatus::Break)
	{	// Execution stopped on BP but IP shows to the next command
		// Move IP back to not skip original command at next run
		m_pCPU->ChangeableState().nIP = m_pCPU->State().nCIP;
	}
}

void CDebugger::Stop()
{
	VASM_CHECK_PTR(m_pCPU);
	m_pCPU->Stop();
}

void CDebugger::ResetProgram()
{
	VASM_CHECK_PTR(m_pCPU);
	if (m_pCPU->Status().eStatus == CProcessor::EStatus::Running)
		throw CError(t_csz("Debugger: Can't reset runnning CPU."));
	else if (m_pCPU->Status().eStatus == CProcessor::EStatus::NotInitialized)
		throw CError(t_csz("Debugger: Can't reset uninitialized CPU."));

	m_pCPU->Reinit(m_tPackage.nProgramStart); 
}

bool CDebugger::ChangeIP(t_address nNewIP)
{
	VASM_CHECK_PTR(m_pCPU);
	bool bChanged = false;

	// Lookup IP address
	t_index nFunc, nLine;
	if (nNewIP != core::cnInvalidAddress && nNewIP % 2 == 0 &&
		LookupSource(nNewIP, nFunc, nLine))
	{
		m_pCPU->ChangeableState().nIP = nNewIP;
		bChanged = true;
	}

	return bChanged;
}

void CDebugger::ChangeFlags(core::CFlags const& oFlags)
{
	VASM_CHECK_PTR(m_pCPU);
	m_pCPU->ChangeableState().oFlags = oFlags;
}

void CDebugger::ChangeRegister(ERegType eRegType, uint nRegIdx, CValue const& oValue)
{
	CProcessor::SState& tState = m_pCPU->ChangeableState();
	if (eRegType == ERegType::ADR)
	{
		nRegIdx += core::SCPUStateBase::eARBaseIndex;
		if (nRegIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
			throw CError(t_csz("Debugger: Invalid address register index"));
		if (oValue.GetType() != core::ValueType<t_address>() || oValue.GetCount() != 1)
			throw CError(t_csz("Debugger: Invalid address register value"));
		tState.rega<t_address>(nRegIdx) = static_cast<t_address>(oValue);
	}
	else //(eRegType == ERegType::GP)
	{
		if (nRegIdx >= core::SCPUStateBase::eRegisterPoolSize ||
			nRegIdx % oValue.GetElementSize() != 0)
			throw CError(t_csz("Debugger: Invalid GP register index"));
		if (!oValue.IsValid() || oValue.GetCount() != 1 ||
			(oValue.GetSize() + nRegIdx) >= core::SCPUStateBase::eRegisterPoolSize)
			throw CError(t_csz("Debugger: Invalid GP register value"));

		switch (oValue.GetType())
		{
		case EValueType::Byte:
			tState.reg<uint8>(nRegIdx) = static_cast<uint8>(oValue);
			break;
		case EValueType::Word:
			tState.reg<uint16>(nRegIdx) = static_cast<uint16>(oValue);
			break;
		case EValueType::DWord:
			tState.reg<uint32>(nRegIdx) = static_cast<uint32>(oValue);
			break;
		case EValueType::QWord:
			tState.reg<uint64>(nRegIdx) = static_cast<uint64>(oValue);
			break;
		case EValueType::Char:
			tState.reg<char>(nRegIdx) = static_cast<uint8>(oValue);
		default:
			break;
		}
	}
}

void CDebugger::ChangeVariable(t_string const& sName, CValue const& oValue)
{
	// Lookup variable address
	auto it = m_mapVariables.find(&sName);
	if (it == m_mapVariables.end())
		throw CError(base::toStr("Debugger: Variable '%1' not found", sName));

	t_index nVarIdx = it->second;
	VASM_CHECK_IDX(nVarIdx, m_tPackage.aVariableTable.size());

	SVariableInfo const& tInfo = m_tPackage.aVariableTable.at(nVarIdx);
	if (oValue.GetType() != tInfo.eType || oValue.GetCount() > tInfo.nSize)
		throw CError(base::toStr("Debugger: Invalid value for the variable '%1'", sName));

	RWMemory().WriteValue(tInfo.nAddress, oValue);
}

SVariableInfo CDebugger::GetVariableInfo(t_string const& sName) const
{
	VASM_CHECK_PTR(m_pCPU);

	// Lookup variable address
	auto it = m_mapVariables.find(&sName);
	if (it == m_mapVariables.end())
		throw CError(base::toStr("Debugger: Variable '%1' not found", sName));
	
	t_index nVarIdx = it->second;
	VASM_CHECK_IDX(nVarIdx, m_tPackage.aVariableTable.size());

	return m_tPackage.aVariableTable.at(nVarIdx);
}


CDebugger::t_aCodeLineInfos CDebugger::GetBreakPoints()
{
	t_aCodeLineInfos aBPs;
	aBPs.reserve(m_mapBreakPoints.size());
	for (auto const& tItem : m_mapBreakPoints)
	{
		SCodeLineInfo tBP;
		t_index nFuncIdx = g_ciInvalid;
		if (LookupSource(tItem.first, nFuncIdx, tBP.nLineNum))
		{
			auto const& tInfo = m_tPackage.aFunctionTable.at(nFuncIdx);
			tBP.sUnitName = &tInfo.sSrcUnit;
			tBP.sFuncName = &tInfo.sName;
			tBP.nRelOffset = tItem.first - tInfo.nAddress;
			tBP.nAddress = tItem.first;
			aBPs.push_back(tBP);
		}
	}
	return std::move(aBPs);
}

void CDebugger::SetBreakPoint(t_address nAddress)
{
	VASM_CHECK_PTR(m_pCPU);
	if (nAddress != core::cnInvalidAddress && nAddress % 2 == 0 &&
		nAddress < m_pCPU->State().cnCodeSize)
		SetBreakPointAt(nAddress, true);
}

void CDebugger::RemoveBreakPoint(t_address nAddress)
{
	VASM_CHECK_PTR(m_pCPU);
	if (nAddress != core::cnInvalidAddress && nAddress % 2 == 0 &&
		nAddress < m_pCPU->State().cnCodeSize)
		RemoveBreakPointAt(nAddress);
}

void CDebugger::RemoveBreakPointByIndex(t_index nIndex)
{
	t_address nAddress = core::cnInvalidAddress;
	for (auto const& tItem : m_mapBreakPoints)
	{
		if (nIndex == 0)
		{
			nAddress = tItem.first;
			break;
		}
		--nIndex;
	}

	if (nAddress != core::cnInvalidAddress)
		RemoveBreakPointAt(nAddress);
}

void CDebugger::RemoveAllBreakPoints()
{
	// Restore all saved OpCodes
	TurnBreakPoints(false);
	// Clear breakpoints map
	m_mapBreakPoints.clear();
}

//
//	Debug Helpers
//
t_address CDebugger::ResolveAddressFromVariable(t_string const& sVarName) const
{
	VASM_CHECK_PTR(m_pCPU);
	t_address nResolved = core::cnInvalidAddress;
	auto it = m_mapVariables.find(CStringRef{&sVarName});
	if (it != m_mapVariables.end())
	{
		t_index nVarIdx = it->second;
		VASM_CHECK_IDX(nVarIdx, m_tPackage.aVariableTable.size());
		nResolved = m_tPackage.aVariableTable.at(nVarIdx).nAddress;
	}
	return nResolved;
}

t_address CDebugger::ResolveAddressFromLabel(t_string const& sLabel) const
{
	VASM_CHECK_PTR(m_pCPU);
	t_address nResolved = core::cnInvalidAddress;

	// Get current function index to lookup Label
	t_index nFunc, nLineNumber;
	t_address nCurrIP = m_pCPU->State().nIP;
	if (LookupSource(nCurrIP, nFunc, nLineNumber))
	{
		VASM_ASSERT(nFunc != g_ciInvalid);
		SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable[nFunc];
		for (auto const& tLblInfo : tFuncInfo.aLabels)
		{
			if (tLblInfo.sName == sLabel)
			{
				nResolved = tLblInfo.nAddress;
				break;
			}
		}
	}

	return nResolved;
}

t_address CDebugger::ResolveAddressFromFunction(
	t_string const& sFunction, t_string const& sLabel) const
{
	VASM_CHECK_PTR(m_pCPU);
	t_address nResolved = core::cnInvalidAddress;

	// Get current function index to lookup Label
	t_index nFunc = LookupFunction(sFunction);
	if (nFunc != g_ciInvalid)
	{
		SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable[nFunc];
		nResolved = tFuncInfo.nAddress;

		if (!sLabel.empty())
		{
			for (auto const& tLblInfo : tFuncInfo.aLabels)
			{
				if (tLblInfo.sName == sLabel)
				{
					nResolved = tLblInfo.nAddress;
					break;
				}
			}
		}
	}

	return nResolved;
}

t_address CDebugger::ResolveAddressFromSource(t_index nLineNumber, t_string const& sSrcUnit) const
{
	VASM_CHECK_PTR(m_pCPU);

	CStringRef sSrcUnitRef = &sSrcUnit;
	if (sSrcUnit.empty())
	{	// Get current source
		// Get current function index to obtain src unit
		t_index nFunc, nLine;
		t_address nCurrIP = m_pCPU->State().nIP;
		if (LookupSource(nCurrIP, nFunc, nLine))
		{
			VASM_ASSERT(nFunc != g_ciInvalid);
			SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable[nFunc];
			sSrcUnitRef = &tFuncInfo.sSrcUnit;
		}
		else if (m_tPackage.nMainFuncIndex != g_ciInvalid)
		{
			// If there is only source then take that
			SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable[m_tPackage.nMainFuncIndex];
			sSrcUnitRef = &tFuncInfo.sSrcUnit;
		}
	}

	t_address nResolved = core::cnInvalidAddress;
	if (sSrcUnitRef != nullptr)
		nResolved = LookupCode(sSrcUnitRef.str(), nLineNumber);
	return nResolved;
}

bool CDebugger::CheckVariableAddress(t_address const nAddress) const
{
	bool bAnswer = false;
	for (auto const& tInfo : m_tPackage.aVariableTable)
	{
		if (tInfo.nAddress == nAddress)
		{
			bAnswer = true;
			break;
		}
	}
	return bAnswer;
}

bool CDebugger::CheckCodeAddress(t_address const nAddress) const
{
	t_index nFunc, nLine;
	return (nAddress != core::cnInvalidAddress && nAddress % 2 == 0 && LookupSource(nAddress, nFunc, nLine));
}

t_address CDebugger::NextCodeAddress(t_address const nAddress) const
{
	t_address nNextAddress = core::cnInvalidAddress;
	if (nAddress < (m_tPackage.nCodeBase + m_tPackage.nCodeSize))
	{
		EOpCode const& eOpCode = Memory().operator[]<EOpCode>(nAddress);
		CCommandLibrary::SInstructionInfo const& tInfo = (*m_pCmdLib)[eOpCode];
		if (tInfo.pDisasm != nullptr)
		{
			// Decode the command to disasemble
			core::SCommandInfo tCmdInfo;
			core::CDecoder oDecoder(m_pCmdLib);
			oDecoder.Decode((uchar*) &eOpCode, tCmdInfo);
			nNextAddress = nAddress + tCmdInfo.tMetaInfo.nLength;
			if (nNextAddress >= m_pCPU->State().cnCodeSize - 1)
				nNextAddress = core::cnInvalidAddress;
		}
	}
	return nNextAddress;
}

CDebugger::SCodeLineInfo CDebugger::GetCodeLineInfo(t_address nAddress) const
{
	t_index nFuncIdx = g_ciInvalid;
	SCodeLineInfo tInfo;
	if (nAddress >= m_tPackage.nCodeBase && nAddress < (m_tPackage.nCodeBase + core::cnGranul))
	{	// Exception case, address 0 is the actual program starting point
		tInfo.nAddress = nAddress;
		tInfo.nRelOffset = nAddress - m_tPackage.nCodeBase;
		tInfo.nFuncSize = core::cnGranul;
	}
	else if (LookupSource(nAddress, nFuncIdx, tInfo.nLineNum))
	{
		auto const& tFuncInfo = m_tPackage.aFunctionTable.at(nFuncIdx);
		tInfo.nAddress = nAddress;
		tInfo.nRelOffset = nAddress - tFuncInfo.nAddress;
		tInfo.nFuncSize = tFuncInfo.nSize;
		tInfo.nRelLineNum = tInfo.nLineNum - tFuncInfo.nBaseLine;
		tInfo.nFuncLineCount = tFuncInfo.nSizeLine;
		tInfo.sFuncName = &tFuncInfo.sName;
		tInfo.sUnitName = &tFuncInfo.sSrcUnit;
	}
	else
		tInfo.nRelOffset = nAddress;
	return tInfo;
}

t_string CDebugger::GetDisassembledCommand(t_address& nAddress, t_string* pBinaryRepresentation) const
{
	VASM_CHECK_PTR(m_pCmdLib);
	VASM_CHECK_PTR(m_pCPU);

	t_string sCommand;

	if (nAddress < (m_tPackage.nCodeBase + m_tPackage.nCodeSize))
	{
		EOpCode const& eOpCode = Memory().operator[]<EOpCode>(nAddress);
		CCommandLibrary::SInstructionInfo const& tInfo = (*m_pCmdLib)[eOpCode];
		uint8 const* pCmd = reinterpret_cast<uint8 const*>(&eOpCode);

		if (pBinaryRepresentation)
		{
			if (eOpCode == EOpCode::Invalid)
			{
				*pBinaryRepresentation = t_csz("0000");
			}
			else
			{
				std::stringstream os;
				for (uchar i = tInfo.tMetaInfo.nLength; i > 0; --i)
				{
					if (i % 2 == 0 && i < tInfo.tMetaInfo.nLength)
						os << " ";
					os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +(*(pCmd + (i - 1)));
				}
				*pBinaryRepresentation = os.str();
			}
		}

		if (tInfo.pDisasm == nullptr)
		{
			if (eOpCode == EOpCode::Invalid)
			{
				sCommand = t_csz("Invalid command");
				nAddress += core::cnCmdMinLength;
			}
			else
			{
				sCommand = t_csz("Invalid or unknown command");
				nAddress = core::cnInvalidAddress;
			}
		}
		else
		{
			// Decode the command to disasemble
			core::SCommandInfo tCmdInfo;
			core::CDecoder oDecoder(m_pCmdLib);
			oDecoder.Decode((uchar*) &eOpCode, tCmdInfo);
			tCmdInfo.nAddress = nAddress;
			nAddress += tCmdInfo.tMetaInfo.nLength;
			if (nAddress >= m_pCPU->State().cnCodeSize - 1)
				nAddress = core::cnInvalidAddress;
			sCommand = std::move(tInfo.pDisasm->Translate(tCmdInfo, m_bHexadecimalOutput));
		}
	}

	return std::move(sCommand);
}

CDebugger::t_aCodeLineInfos CDebugger::GetFunctionCallStack() const
{
	t_CallStack aCallStack = ExtractFunctionCallStack();
	t_aCodeLineInfos aCodeLines(aCallStack.size());

	t_index i = 0;
	for (auto const& tIdx : aCallStack)
	{
		auto const& tInfo = m_tPackage.aFunctionTable.at(tIdx.nFuncIdx);

		aCodeLines[i].sUnitName  = &tInfo.sSrcUnit;
		aCodeLines[i].sFuncName  = &tInfo.sName;
		aCodeLines[i].nLineNum   = tIdx.nLineNum;
		aCodeLines[i].nAddress   = tInfo.nAddress;
		aCodeLines[i].nRelOffset = tIdx.nAddress - tInfo.nAddress;
		
		++i;
	}

	return std::move(aCodeLines);
}

std::pair<t_address, t_address> CDebugger::GetReturnLinkFromStackFrame() const
{
	std::pair<t_address, t_address> tReturnLinkImage(core::cnInvalidAddress, core::cnInvalidAddress);
	t_address nCurrSF = m_pCPU->State().nSF;
	t_uoffset const cnStackBottom = m_pCPU->State().cnStackLBound - 2 * sizeof(t_address);

	if (nCurrSF <= cnStackBottom)
	{
		Memory().ReadAt<t_address>(nCurrSF + sizeof(t_address), tReturnLinkImage.first); // RIP Image
		Memory().ReadAt<t_address>(nCurrSF, tReturnLinkImage.second); // SF Image
	}
	return tReturnLinkImage;
}

std::vector<std::pair<core::t_PortRange, t_string>> CDebugger::GetPortsInfo() const
{
	std::vector<std::pair<core::t_PortRange, t_string>> aPortsInfo;
	if (m_pIOController != nullptr)
	{
		auto aPorts = std::move(m_pIOController->GetPorts());
		aPortsInfo.reserve(aPorts.size());
		for (auto const& tItem : aPorts)
		{
			if (tItem.second != nullptr)
				aPortsInfo.push_back({tItem.first, tItem.second->GetName()});
		}
	}
	return std::move(aPortsInfo);
}

void CDebugger::Dump(std::ostream& os, bool bText) const
{
	if (bText)
	{
		DumpState(os, true);
		os << std::endl;
		DumpStackBacktrace(os);
		os << std::endl;
		DumpCode(os, m_tPackage.nCodeBase, m_tPackage.nCodeSize, bText);
		os << std::endl;
		DumpData(os, m_tPackage.nDataBase, m_tPackage.nDataSize, bText);
		os << std::endl;
		DumpStack(os, g_ciInvalid, bText);
		os << std::endl;
	}
	else
	{
		DumpMemory(os, 0, 0, bText);
	}
}

void CDebugger::DumpState(std::ostream& os, bool const bHexadecimal) const
{
	CProcessor::SState const& tState = CPUState();

	os << std::resetiosflags(0);
	os << "Processor status flags: ----------------------------------------------------------" << std::endl;
	os <<  "CF(" << std::dec << tState.oFlags.getCarry() << ")";
	os << " ZF(" << std::dec << tState.oFlags.getZero() << ")";
	os << " SF(" << std::dec << tState.oFlags.getSign() << ")";
	os << " OF(" << std::dec << tState.oFlags.getOverflow() << ")";

	os << std::endl;
	if (bHexadecimal)
	{
		os << "Address registers (0x): ----------------------------------------------------------" << std::endl;
		os << "IP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nIP << "  ";
		//os << "CIP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nCIP << "  ";
		os << "RIP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nRIP << "  ";
		os << std::endl;
		os << "SP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nSP << "  ";
		os << " SF = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nSF << "  ";
		os << std::endl;
		os << "A0 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 0) << "  ";
		os << " A1 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 1) << "  ";
		os << std::endl;
		os << "A2 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 2) << "  ";
		os << " A3 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 3) << "  ";
	}
	else
	{
		os << "Address registers: ---------------------------------------------------------------" << std::endl;
		os << "IP = " << std::dec << std::setfill(' ') << std::setw(10) << tState.nIP << "  ";
		//os << "CIP = " << std::dec << std::setfill(' ') << std::setw(4) << tState.nCIP << "  ";
		os << "RIP = " << std::dec << std::setfill(' ') << std::setw(10) << tState.nRIP << "  ";
		os << std::endl;
		os << "SP = " << std::dec << std::setfill(' ') << std::setw(10) << tState.nSP << "  ";
		os << " SF = " << std::dec << std::setfill(' ') << std::setw(10) << tState.nSF << "  ";
		os << std::endl;
		os << "A0 = " << std::dec << std::setfill(' ') << std::setw(10) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 0) << "  ";
		os << " A1 = " << std::dec << std::setfill(' ') << std::setw(10) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 1) << "  ";
		os << std::endl;
		os << "A2 = " << std::dec << std::setfill(' ') << std::setw(10) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 2) << "  ";
		os << " A3 = " << std::dec << std::setfill(' ') << std::setw(10) << tState.rega<t_address>(core::SCPUStateBase::eARBaseIndex + 3) << "  ";
	}

	os << std::endl;
	if (bHexadecimal)
	{
		os << "General purpose registers (0x): --------------------------------------------------" << std::endl;
		for (uint nRIdx = 0 /*core::SCPUStateBase::eGPRBaseIndex*/; nRIdx < core::SCPUStateBase::eRegisterPoolSize; nRIdx += 16)
		{
			os << "R" << std::dec << nRIdx + 15 << "-" << std::setfill('0') << std::setw(2) << nRIdx << ": ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 15) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 14) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 13) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 12) << " ";
			os << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 11) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 10) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 9) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 8) << " ";
			os << "  ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 7) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 6) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 5) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 4) << " ";
			os << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 3) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 2) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 1) << " ";
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.reg<uint8>(nRIdx + 0) << " ";
			os << std::endl;
		}
	}
	else
	{
		os << "General purpose registers: -------------------------------------------------------" << std::endl;
		for (uint nRIdx = 0 /*core::SCPUStateBase::eGPRBaseIndex*/; nRIdx < core::SCPUStateBase::eRegisterPoolSize; nRIdx += 16)
		{
			os << "R" << std::dec << nRIdx + 15 << "-" << std::setfill('0') << std::setw(2) << nRIdx << ": ";
			os << std::dec << std::setfill(' ') << std::setw(10) << tState.reg<uint32>(nRIdx + 12) << "  ";
			os << std::dec << std::setfill(' ') << std::setw(10) << tState.reg<uint32>(nRIdx + 8) << "  ";
			os << std::dec << std::setfill(' ') << std::setw(10) << tState.reg<uint32>(nRIdx + 4) << "  ";
			os << std::dec << std::setfill(' ') << std::setw(10) << tState.reg<uint32>(nRIdx + 0) << "  ";
			os << std::endl;
		}
	}
}

void CDebugger::DumpMemory(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pCPU);
	if (nBaseAddress >= Memory().GetSize())
		throw CError(t_csz("Debugger: memory dump failed due to invalid address"));

	// Adjust number of bytes 
	if (nSizeBytes == 0)
		nSizeBytes = Memory().GetSize() - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, Memory().GetSize() - nBaseAddress);

	if (bText)
	{
		os << "# Offset: 15 14 13 12  11 10 09 08   07 06 05 04  03 02 01 00\t................" << std::endl;
		DumpHelper(os, nBaseAddress, nSizeBytes, &(Memory()[0]), true);
	}
	else
	{
		os.write(&Memory().operator[]<char>(nBaseAddress), nSizeBytes);
	}
}

void CDebugger::DumpCode(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pCPU);

	// Adjust base address
	t_size nCodeEnd = m_tPackage.nCodeBase + m_tPackage.nCodeSize;
	if (nBaseAddress == 0)
		nBaseAddress = m_tPackage.nCodeBase;

	if (nBaseAddress >= Memory().GetSize() || nBaseAddress < m_tPackage.nCodeBase || nBaseAddress >= nCodeEnd)
		throw CError(t_csz("Debugger: memory dump of code section failed due to invalid address"));

	// Adjust size
	nCodeEnd = std::min(nCodeEnd, Memory().GetSize());
	if (nSizeBytes == 0)
		nSizeBytes = nCodeEnd - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, nCodeEnd - nBaseAddress);

	if (bText)
	{
		os << ".code" << std::endl;
		os << "# Offset: 15 14 13 12  11 10 09 08   07 06 05 04  03 02 01 00" << std::endl;
		auto nAddress = nBaseAddress;
		auto const nEndAddress = nBaseAddress + nSizeBytes;
		while (nAddress < nEndAddress)
		{
			auto itFunc = m_mapFunctions.find({nAddress, nAddress});
			if (itFunc != m_mapFunctions.end())
			{
				auto const& tFunc = m_tPackage.aFunctionTable.at(itFunc->second);
				os  << "# " << tFunc.sSrcUnit << '.' << tFunc.sName << std::endl;

				auto const nSize = std::min(nEndAddress, tFunc.nAddress + tFunc.nSize) - nAddress;
				DumpHelper(os, nAddress, nSize, &(Memory()[0]));
				nAddress += nSize + (nSize % core::cnGranul == 0 ? 0 : core::cnGranul - nSize % core::cnGranul);
			}
			else
			{
				DumpHelper(os, nAddress, core::cnGranul, &(Memory()[0]));
				nAddress += core::cnGranul;
			}
		}
	}
	else
	{
		os.write(&Memory().operator[]<char>(nBaseAddress), nSizeBytes);
	}
}

void CDebugger::DumpData(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pCPU);

	// Adjust base address
	t_size nDataEnd = m_tPackage.nDataBase + m_tPackage.nDataSize;
	if (nBaseAddress == 0)
		nBaseAddress = m_tPackage.nDataBase;

	if (nBaseAddress >= Memory().GetSize() || nBaseAddress < m_tPackage.nDataBase || nBaseAddress >= nDataEnd)
		throw CError(t_csz("Debugger: memory dump of data section failed due to invalid address"));

	// Adjust size
	nDataEnd = std::min(nDataEnd, Memory().GetSize());
	if (nSizeBytes == 0)
		nSizeBytes = nDataEnd - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, nDataEnd - nBaseAddress);

	if (bText)
	{
		os << ".data" << std::endl;
		for (auto const& tVar : m_tPackage.aVariableTable)
		{
			t_size const nVarSize = tVar.nSize * core::SizeOfValueType(tVar.eType);
			if ((tVar.nAddress + nVarSize) <= nBaseAddress ||
				tVar.nAddress >= (nBaseAddress + nSizeBytes))
				continue;

			os  << "# " << tVar.sName << " : " << CValue::TypeToCStr(tVar.eType) << ", 0x"
				<< std::setw(8) << std::hex << std::uppercase << std::setfill('0') << tVar.nAddress;
			
			if (tVar.nSize > 1)
				os << " + " << std::setw(0) << std::dec << +SizeOfValueType(tVar.eType) << "*" << tVar.nSize << std::endl;
			else
				os << " + " << std::setw(0) << std::dec << +SizeOfValueType(tVar.eType) << std::endl;
		}

		os << "# Offset: 15 14 13 12  11 10 09 08   07 06 05 04  03 02 01 00\t................" << std::endl;
		DumpHelper(os, nBaseAddress, nSizeBytes, &(Memory()[0]), true);
	}
	else
	{
		os.write(&Memory().operator[]<char>(nBaseAddress), nSizeBytes);
	}
}

void CDebugger::DumpStack(std::ostream& os, t_size const cnFixedDepth, bool bText) const
{
	VASM_CHECK_PTR(m_pCPU);

	t_address nStackTop = m_pCPU->State().nSP;
	t_uoffset nStackBottom = m_pCPU->State().cnStackLBound;

	// Find out stack size
	if (nStackTop < nStackBottom)
	{
		constexpr t_size const cnLinkSize = 2 * sizeof(t_address);
		t_size nDepth = cnFixedDepth;
		t_size nSize = nStackBottom - nStackTop;
		if (nDepth > 0)
		{	// Cals stack size by unwinding the stack
			t_address nStackBase = m_pCPU->State().nSF;
			nSize = nStackBase - nStackTop;
			while (nStackBase < nStackBottom && --nDepth > 0)
			{
				// Next frame 
				t_address nPrevStackBase = nStackBase;
				if (nStackBase <= nStackBottom - cnLinkSize)
					Memory().ReadAt<t_address>(nStackBase, nStackBase);
				else
					nStackBase = nStackBottom;
				if (int64(nStackBase) - int64(nPrevStackBase) < int64(cnLinkSize))
					throw CError(base::toStr("Stack corruption detected at stack offset %1", nStackBottom - nPrevStackBase));
				// Update stack size
				nSize = nStackBase - nStackTop;
			}
		}

		if (bText)
		{
			// Unwind the stack and dump frame info
			t_address nStackBase = m_pCPU->State().nSF;
			t_address nCurrentTop = nStackTop;
			nDepth = 0;
			os << ".stack" << std::endl;
			do 
			{
				os	<< "# SF(" << nDepth << "): 0x" << std::hex << std::setw(8) << std::setfill('0') << nCurrentTop
					<< " - 0x" << std::setw(8) << std::setfill('0') << nStackBase << std::endl;
				t_address nPrevStackBase = nStackBase;
				if (nStackBase <= nStackBottom - cnLinkSize)
					Memory().ReadAt<t_address>(nStackBase, nStackBase);
				else
					nStackBase = nStackBottom;
				if (int64(nStackBase) - int64(nPrevStackBase) < int64(cnLinkSize))
					throw CError(base::toStr("Stack corruption detected at stack offset %1", nStackBottom - nPrevStackBase));
				nCurrentTop = nPrevStackBase + cnLinkSize;
				++nDepth;
			} while (nCurrentTop < nStackBottom);

			os << "# Offset: 15 14 13 12  11 10 09 08   07 06 05 04  03 02 01 00" << std::endl;
			DumpHelper(os, nStackTop, nSize, &(Memory()[0]), false);
		}
		else
		{
			os.write(&Memory().operator[]<char>(nStackTop), nSize);
		}
	}
}

void CDebugger::DumpStackBacktrace(std::ostream& os) const
{
	auto aCodeLines = std::move(GetFunctionCallStack());

	os << std::resetiosflags(0);
	os << "Function call stack: --------------------------------------------------------------" << std::endl;
	for (auto const& tInfo : aCodeLines)
	{
		os << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tInfo.nAddress << ": ";
		if (tInfo.sFuncName != nullptr)
			os << tInfo.sFuncName.str() << "  ";
		os << "+0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tInfo.nRelOffset;
		if (tInfo.sUnitName != nullptr)
			os << "  Unit: " << tInfo.sUnitName.str() << ",  Line: " << std::setw(0) << std::dec << tInfo.nLineNum;
		os << std::endl;
	}
}

void CDebugger::DumpHelper(
	std::ostream& os, t_address nBaseAddress, t_size nSizeBytes,
	t_byte const* pMemory, bool const bSupplementalText)
{
	// Round to 16 byte boundaries
	t_address const nEndAddress = nBaseAddress + nSizeBytes;
	t_address nStartRounded = nBaseAddress - nBaseAddress % 16;
	t_address nEndRounded = nEndAddress + (nEndAddress % 16 == 0 ? 0 : (16 - nEndAddress % 16));

	// Loop by 16 bytes
	for (t_address nAddress = nStartRounded; nAddress < nEndRounded; nAddress += 16)
	{
		//if (nAddress > nStartRounded && nAddress % 64 == 0)
		//	os << std::endl;
		// Print address
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nAddress << ": ";
		// Print memory content
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 15] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 14] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 13] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 12] << " ";
		os << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 11] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 10] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 9]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 8]  << " ";
		os << "  ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 7]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 6]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 5]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 4]  << " ";
		os << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 3]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 2]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 1]  << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +pMemory[nAddress + 0]  << " ";
		
		if (bSupplementalText)
		{
			os << "\t";
			for (auto i = 0; i < 16; ++i)
			{
				t_byte n = pMemory[nAddress + i];
				if (n >= 32 && n < 127)
					os << n;
				else
					os << ' ';
			}
		}

		os << std::endl;
	}
}

//
//	Internal Routines
//
void CDebugger::TurnBreakPoints(bool bOn)
{
	VASM_CHECK_PTR(m_pCPU);
	for (auto const& tItem : m_mapBreakPoints)
	{
		RWMemory().WriteAt<EOpCode>(tItem.first, bOn ? EOpCode::BREAK : tItem.second);
	}
}

bool CDebugger::TurnBreakPointAt(t_address nAddress, bool bOn)
{
	VASM_CHECK_PTR(m_pCPU);
	auto it = m_mapBreakPoints.find(nAddress);
	if (it != m_mapBreakPoints.end())
	{
		RWMemory().WriteAt<EOpCode>(it->first, bOn ? EOpCode::BREAK : it->second);
		return true;
	}
	return false;
}

void CDebugger::SetBreakPointAt(t_address nAddress, bool bOn)
{
	VASM_CHECK_PTR(m_pCPU);

	// Check for BP already set, otherwise BREAK instruction could be saved over the original one
	auto it = m_mapBreakPoints.find(nAddress);
	if (it == m_mapBreakPoints.end())
	{
		EOpCode eOpCodeBackup;
		Memory().ReadAt<EOpCode>(nAddress, eOpCodeBackup);
		m_mapBreakPoints.insert({nAddress, eOpCodeBackup});
	}

	TurnBreakPointAt(nAddress, bOn);
}

bool CDebugger::RemoveBreakPointAt(t_address nAddress)
{
	// Restore saved instruction
	bool bExist = TurnBreakPointAt(nAddress, false);
	if (bExist)
		m_mapBreakPoints.erase(nAddress);
	return bExist;
}

t_address CDebugger::LookupCode(t_string const& sSrcUnit, t_index nLineNumber) const
{
	t_address nCodeAddr = core::cnInvalidAddress;
	for (SFunctionInfo const& tInfo : m_tPackage.aFunctionTable)
	{
		if (sSrcUnit == tInfo.sSrcUnit && nLineNumber >= tInfo.nBaseLine &&
			nLineNumber < tInfo.nBaseLine + tInfo.nSizeLine)
		{	// Found
			t_size nLineNumberOffset = nLineNumber - tInfo.nBaseLine;
			if (nLineNumberOffset < tInfo.aCodeTbl.size())
			{
				nCodeAddr = tInfo.aCodeTbl.at(nLineNumberOffset);
				break;
			}
		}
	}
	return nCodeAddr;
}

t_index CDebugger::LookupFunction(t_string const& sFuncName) const
{
	t_index iFunc = g_ciInvalid;
	for (auto it = m_tPackage.aFunctionTable.begin(); it != m_tPackage.aFunctionTable.end(); ++it)
	{
		SFunctionInfo const& tInfo = *it;
		if (sFuncName == tInfo.sName)
		{	// Found
			iFunc = t_index(it - m_tPackage.aFunctionTable.begin());
			break;
		}
	}
	return iFunc;
}

bool CDebugger::LookupSource(t_address nCodeAddr, t_index& nFuncIdx, t_index& nLineNumber) const
{
	nFuncIdx = g_ciInvalid;
	nLineNumber = g_ciInvalid;

	if (nCodeAddr == 0 || nCodeAddr >= m_pCPU->State().cnCodeSize)
		return false;

	auto itFunc = m_mapFunctions.find({nCodeAddr, nCodeAddr});
	if (itFunc != m_mapFunctions.end())
	{
		// Function Found, now lookup the line number
		nFuncIdx = itFunc->second;
		auto const& tInfo = m_tPackage.aFunctionTable.at(nFuncIdx);
		for (t_index iLine = 0; iLine < tInfo.aCodeTbl.size(); ++iLine)
		{
			if (tInfo.aCodeTbl.at(iLine) == nCodeAddr)
			{
				do
				{
					nLineNumber = tInfo.nBaseLine + iLine; // Convert function relative line number to unit level
				} while (++iLine < tInfo.aCodeTbl.size() && tInfo.aCodeTbl.at(iLine) == nCodeAddr);
				break;
			}
		}
	}

	return (itFunc != m_mapFunctions.end());
}

CDebugger::t_CallStack CDebugger::ExtractFunctionCallStack() const
{
	VASM_CHECK_PTR(m_pCPU);

	t_CallStack aCallStack;
	CProcessor::EStatus eStatus = m_pCPU->Status().eStatus;
	if (bool(uint(eStatus) & uint(CProcessor::EStatus::Ready)))
	{
		t_address nCurrIP = m_pCPU->State().nIP;
		t_address nCurrSF = m_pCPU->State().nSF;
		t_address nRetIP = m_pCPU->State().nRIP;
		t_uoffset const cnStackBottom = m_pCPU->State().cnStackLBound - 2 * sizeof(t_address);

		t_index	nFuncIdx;
		uint	nLineNumber;

		if (LookupSource(nCurrIP, nFuncIdx, nLineNumber))
		{
			aCallStack.push_back({nFuncIdx, nLineNumber, nCurrIP});
			nCurrIP = nRetIP;

			if (nCurrSF <= cnStackBottom)
			{
				// Check if stack frame created (RIP == RIP Image means there is stack frame)
				Memory().ReadAt<t_address>(nCurrSF + sizeof(t_address), nRetIP);
				if (nRetIP == nCurrIP)
					Memory().ReadAt<t_address>(nCurrSF, nCurrSF);
			}
			
			while (LookupSource(nCurrIP, nFuncIdx, nLineNumber))
			{
				aCallStack.push_back({nFuncIdx, nLineNumber, nCurrIP});

				if (nCurrSF > cnStackBottom)
					break;

				// Unwind to next IP & SF
				Memory().ReadAt<t_address>(nCurrSF + sizeof(t_address), nCurrIP);
				Memory().ReadAt<t_address>(nCurrSF, nCurrSF);
			}
		}
	}
	return aCallStack;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


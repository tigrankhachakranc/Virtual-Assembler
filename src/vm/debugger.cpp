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

void CDebugger::Init(
	CCommandLibraryPtr pCmdLib, CMemoryPtr pMemory,
	CProcessorPtr pCPU, CIOControllerPtr pIOController,
	SPackageInfo tPackageInfo)
{
	if (pCmdLib == nullptr)
		VASM_THROW_ERROR("Failed to initialize debugger: Invalid Command Library pointer.");
	if (pCPU == nullptr)
		VASM_THROW_ERROR("Failed to initialize debugger: Invalid CPU pointer.");
	if (pMemory == nullptr)
		VASM_THROW_ERROR("Failed to initialize debugger: Invalid Memory pointer.");

	m_pCmdLib = pCmdLib;
	m_pMemory = pMemory;
	m_pCPU = pCPU;
	m_pIOController = pIOController;
	m_tPackage = std::move(tPackageInfo);

	m_mapVariables.clear();
	m_mapFunctions.clear();
	m_mapBreakPoints.clear();

	// Also collect symbols to intialize adress recovery
	t_AddrssToSymbolMap mapSymbols;

	// Init variables
	t_size nCount = m_tPackage.aVariableTable.size();
	for (t_index idx = 0; idx < nCount; ++idx)
	{
		SVariableInfo const& tVarInfo = m_tPackage.aVariableTable.at(idx);

		mapSymbols.insert(t_AddrssToSymbolMap::value_type(tVarInfo.nAddress, &tVarInfo.sName));
		m_mapVariables.insert({&tVarInfo.sName, idx});
	}

	// Init functions
	nCount = m_tPackage.aFunctionTable.size();
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
	VASM_CHECK_PTR(m_pMemory);

	// Enable break points
	TurnBreakPoints(true);

	while (nCount > 0 && m_pCPU->CanRun())
	{
		--nCount;

		// If Current instruction is BP then temporary turn it off
		bool bCurrBP = TurnBreakPointAt(m_pCPU->State().nIP, false);

		// Need to analyze target instruction to skip over CALL instruction
		EOpCode eOpCode = m_pMemory->operator[]<EOpCode>(m_pCPU->State().nIP);

		if (eOpCode != EOpCode::CALL)
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

				// Restore BP to hit next time CPU will pass over the same instruction
				if (bCurrBP)
				{	// IP already shows next instruction, but CIP yet keeps previous value
					TurnBreakPointAt(m_pCPU->State().nCIP, true);
					bCurrBP = false;
				}

				// Analyze just executed instruction to find out corresponding CALLs & RETs
				// IP already shows next instruction, but CIP yet keeps reference on the previous one
				eOpCode = m_pMemory->operator[]<EOpCode>(m_pCPU->State().nCIP);
				if (eOpCode == EOpCode::CALL)
					++nCallLevel;
				else if (eOpCode == EOpCode::RET || eOpCode == EOpCode::RET2)
					--nCallLevel;

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
	VASM_CHECK_PTR(m_pMemory);

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

		// Restore BP to hit next time CPU will pass over the same instruction
		if (bCurrBP)
		{	// IP already shows next instruction, but CIP yet keeps previous value
			TurnBreakPointAt(m_pCPU->State().nCIP, true);
			bCurrBP = false;
		}

		// Analyze just executed instruction to find out corresponding CALLs & RETs
		// IP already shows next instruction, but CIP yet keeps reference on the previous one
		EOpCode eOpCode = m_pMemory->operator[]<EOpCode>(m_pCPU->State().nCIP);
		if (eOpCode == EOpCode::CALL)
			++nCallLevel;
		else if (eOpCode == EOpCode::RET || eOpCode == EOpCode::RET2)
			--nCallLevel;

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
		VASM_THROW_ERROR(t_csz("Debugger: Can't reset runnning CPU."));
	else if (m_pCPU->Status().eStatus == CProcessor::EStatus::NotInitialized)
		VASM_THROW_ERROR(t_csz("Debugger: Can't reset uninitialized CPU."));

	m_pCPU->Init(m_pCmdLib, m_pMemory, m_pCPU->State().cnCodeSize,
				 m_pCPU->State().cnStackLBound - m_pCPU->State().cnStackUBound,
				 m_tPackage.nProgramStart); 
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
		if (nRegIdx < core::SCPUStateBase::eARBaseIndex ||
			nRegIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
			VASM_THROW_ERROR(t_csz("Debugger: Invalid address register index"));
		if (oValue.GetType() != core::ValueType<t_address>() || oValue.GetCount() != 1)
			VASM_THROW_ERROR(t_csz("Debugger: Invalid address register value"));
		tState.anARPool[nRegIdx] = static_cast<t_address>(oValue);
	}
	else //(eRegType == ERegType::GP)
	{
		if (nRegIdx >= core::SCPUStateBase::eGeneralPurposeRegisterPoolSize ||
			nRegIdx % oValue.GetElementSize() != 0)
			VASM_THROW_ERROR(t_csz("Debugger: Invalid GP register index"));
		if (!oValue.IsValid() || oValue.GetCount() != 1 ||
			(oValue.GetSize() + nRegIdx) >= core::SCPUStateBase::eGeneralPurposeRegisterPoolSize)
			VASM_THROW_ERROR(t_csz("Debugger: Invalid GP register value"));

		switch (oValue.GetType())
		{
		case EValueType::Byte:
			tState.aui8GPRPool[nRegIdx] = static_cast<uint8>(oValue);
			break;
		case EValueType::Word:
			reinterpret_cast<uint16&>(tState.aui8GPRPool[nRegIdx]) = static_cast<uint16>(oValue);
			break;
		case EValueType::DWord:
			reinterpret_cast<uint32&>(tState.aui8GPRPool[nRegIdx]) = static_cast<uint32>(oValue);
			break;
		case EValueType::QWord:
			reinterpret_cast<uint64&>(tState.aui8GPRPool[nRegIdx]) = static_cast<uint64>(oValue);
			break;
		case EValueType::Char:
			tState.aui8GPRPool[nRegIdx] = static_cast<uint8>(oValue);
		default:
			break;
		}
	}
}

void CDebugger::ChangeVariable(t_string const& sName, CValue const& oValue)
{
	VASM_CHECK_PTR(m_pMemory);

	// Lookup variable address
	auto it = m_mapVariables.find(&sName);
	if (it == m_mapVariables.end())
		VASM_THROW_ERROR(base::toStr("Debugger: Variable '%1' not found", sName));

	t_index nVarIdx = it->second;
	VASM_CHECK_IDX(nVarIdx, m_tPackage.aVariableTable.size());

	SVariableInfo const& tInfo = m_tPackage.aVariableTable.at(nVarIdx);
	if (oValue.GetType() != tInfo.eType || oValue.GetCount() > tInfo.nSize)
		VASM_THROW_ERROR(base::toStr("Debugger: Invalid value for the variable '%1'", sName));

	m_pMemory->WriteValue(tInfo.nAddress, oValue);
}

SVariableInfo CDebugger::GetVariableInfo(t_string const& sName) const
{
	VASM_CHECK_PTR(m_pMemory);

	// Lookup variable address
	auto it = m_mapVariables.find(&sName);
	if (it == m_mapVariables.end())
		VASM_THROW_ERROR(base::toStr("Debugger: Variable '%1' not found", sName));
	
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
		if (LookupSource(tItem.first, tBP.sFuncName, tBP.sUnitName, tBP.nLineNum, tBP.nRelOffset))
		{
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
	}

	t_address nResolved = LookupCode(sSrcUnitRef.str(), nLineNumber);
	return nResolved;
}

bool CDebugger::CheckCodeAddress(t_address nAddress) const
{
	t_index nFunc, nLine;
	return (nAddress != core::cnInvalidAddress && nAddress % 2 == 0 && LookupSource(nAddress, nFunc, nLine));
}

CDebugger::SCodeLineInfo CDebugger::GetCodeLineInfo(t_address nAddress) const
{
	SCodeLineInfo tInfo;
	if (LookupSource(nAddress, tInfo.sFuncName, tInfo.sUnitName, tInfo.nLineNum, tInfo.nRelOffset))
		tInfo.nAddress = nAddress;
	return tInfo;
}

t_string CDebugger::GetDisassembledCommand(t_address& nAddress, t_string* pBinaryRepresentation) const
{
	VASM_CHECK_PTR(m_pCmdLib);
	VASM_CHECK_PTR(m_pMemory);

	t_string sCommand;

	if (nAddress < (m_tPackage.nCodeBase + m_tPackage.nCodeSize))
	{
		EOpCode const& eOpCode = m_pMemory->operator[]<EOpCode>(nAddress);
		CCommandLibrary::SInstructionInfo const& tInfo = (*m_pCmdLib)[eOpCode];
		uint8 const* pCmd = reinterpret_cast<uint8 const*>(&eOpCode);

		if (pBinaryRepresentation)
		{
			std::stringstream os;
			for (uchar i = 0; i < tInfo.tMetaInfo.nLength; ++i)
				os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << *(pCmd + i) << ' ';
			*pBinaryRepresentation = os.str();
		}

		if (tInfo.pDisasm == nullptr)
			sCommand = t_csz("Invalid or unknown command");
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
		auto const& tInfo = m_tPackage.aFunctionTable.at(tIdx.first);

		aCodeLines[i].sUnitName = &tInfo.sSrcUnit;
		aCodeLines[i].sFuncName = &tInfo.sName;
		aCodeLines[i].nLineNum = tIdx.second + tInfo.nBaseLine;
		aCodeLines[i].nAddress = tInfo.nAddress;
		
		++i;
	}

	return std::move(aCodeLines);
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
		os << ".Code" << std::endl << std::endl;
		DumpCode(os, m_tPackage.nCodeBase, m_tPackage.nCodeSize, bText);

		os << ".Data" << std::endl << std::endl;
		DumpData(os, m_tPackage.nDataBase, m_tPackage.nDataSize, bText);

		os << ".Stack" << std::endl << std::endl;
		DumpStack(os, g_ciInvalid, bText);
	}
	else
	{
		DumpMemory(os, 0, 0, bText);
	}
}

void CDebugger::DumpMemory(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pMemory);
	if (nBaseAddress >= m_pMemory->GetSize())
		VASM_THROW_ERROR(t_csz("Debugger: memory dump failed due to invalid address"));

	// Adjust number of bytes 
	if (nSizeBytes == 0)
		nSizeBytes = m_pMemory->GetSize() - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, m_pMemory->GetSize() - nBaseAddress);

	if (bText)
		DumpHelper(os, nBaseAddress, nSizeBytes, &(*m_pMemory)[0]);
	else
		os.write(&m_pMemory->operator[]<char>(nBaseAddress), nSizeBytes);
}

void CDebugger::DumpCode(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pMemory);

	// Adjust base address
	t_size nCodeEnd = m_tPackage.nCodeBase + m_tPackage.nCodeSize;
	if (nBaseAddress == 0)
		nBaseAddress = m_tPackage.nCodeBase;

	if (nBaseAddress >= m_pMemory->GetSize() || nBaseAddress < m_tPackage.nCodeBase || nBaseAddress >= nCodeEnd)
		VASM_THROW_ERROR(t_csz("Debugger: memory dump for the code failed due to invalid address"));

	// Adjust size
	nCodeEnd = std::min(nCodeEnd, m_pMemory->GetSize());
	if (nSizeBytes == 0)
		nSizeBytes = nCodeEnd - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, nCodeEnd - nBaseAddress);

	if (bText)
	{
		for (auto const& tFunc : m_tPackage.aFunctionTable)
		{
			if ((tFunc.nAddress + tFunc.nSize) <= nBaseAddress ||
				tFunc.nAddress >= (nBaseAddress + nSizeBytes))
				continue;
			
			os << "# " << tFunc.sName << " : " << tFunc.sSrcUnit << std::endl;
			DumpHelper(os, tFunc.nAddress, tFunc.nSize, &(*m_pMemory)[0]);
			os << std::endl;
		}
	}
	else
	{
		os.write(&m_pMemory->operator[]<char>(nBaseAddress), nSizeBytes);
	}
}

void CDebugger::DumpData(std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, bool bText) const
{
	VASM_CHECK_PTR(m_pMemory);

	// Adjust base address
	t_size nDataEnd = m_tPackage.nDataBase + m_tPackage.nDataSize;
	if (nBaseAddress == 0)
		nBaseAddress = m_tPackage.nDataBase;

	if (nBaseAddress >= m_pMemory->GetSize() || nBaseAddress < m_tPackage.nDataBase || nBaseAddress >= nDataEnd)
		VASM_THROW_ERROR(t_csz("Debugger: memory dump for the code failed due to invalid address"));

	// Adjust size
	nDataEnd = std::min(nDataEnd, m_pMemory->GetSize());
	if (nSizeBytes == 0)
		nSizeBytes = nDataEnd - nBaseAddress;
	else
		nSizeBytes = std::min(nSizeBytes, nDataEnd - nBaseAddress);

	if (bText)
	{
		for (auto const& tVar : m_tPackage.aVariableTable)
		{
			t_size const nVarSize = tVar.nSize * core::SizeOfValueType(tVar.eType);
			if ((tVar.nAddress + nVarSize) <= nBaseAddress ||
				tVar.nAddress >= (nBaseAddress + nSizeBytes))
				continue;

			os << "# " << tVar.sName << " : " << CValue::TypeToCStr(tVar.eType) << std::endl;
			DumpHelper(os, tVar.nAddress, nVarSize, &(*m_pMemory)[0]);
			os << std::endl;
		}
	}
	else
	{
		os.write(&m_pMemory->operator[]<char>(nBaseAddress), nSizeBytes);
	}
}

void CDebugger::DumpStack(std::ostream& os, t_size nDepth, bool bText) const
{
	VASM_CHECK_PTR(m_pCPU);
	VASM_CHECK_PTR(m_pMemory);

	t_address nStackTop = m_pCPU->State().nSP;
	t_address nStackBase = m_pCPU->State().nSF;
	t_uoffset const cnStackBottom = m_pCPU->State().cnStackLBound - 2 * sizeof(t_address);

	// Adjust depth
	if (nDepth == 0)
		nDepth = g_ciInvalid;

	if (nStackTop < nStackBase)
	{
		if (!bText)
		{
			t_index nCounter = 0;
			t_CallStack aCallStack = ExtractFunctionCallStack();

			// Unwind stack
			do
			{
				auto const& tFunc = m_tPackage.aFunctionTable.at(nCounter);
				os << "# " << tFunc.sName << " : " << tFunc.sSrcUnit << std::endl;
				DumpHelper(os, nStackTop, nStackTop - nStackBase, &(*m_pMemory)[0]);
				os << std::endl;

				// Next frame 
				nStackTop = nStackBase;
				if (nStackBase <= cnStackBottom)
					m_pMemory->ReadAt<t_address>(nStackBase, nStackBase);
				--nDepth;
				++nCounter;

			} while (nStackTop <= cnStackBottom && nDepth > 0);
		}
		else
		{
			// Unwind stack
			while (nStackBase <= cnStackBottom && --nDepth > 0)
			{
				// Next frame 
				m_pMemory->ReadAt<t_address>(nStackBase, nStackBase);
			}

			os.write(&m_pMemory->operator[]<char>(nStackTop), nStackTop - nStackBase);
		}
	}
}

void CDebugger::DumpHelper(
	std::ostream& os, t_address nBaseAddress, t_size nSizeBytes, t_byte const* pMemory)
{
	t_address nStart = nBaseAddress % 16;
	t_address nEnd = nBaseAddress + nSizeBytes;
	t_size nIterationCount = (nEnd - nStart) / 16 + bool((nEnd - nStart) % 16);

	os << std::hex << std::uppercase << std::setfill('0') << std::setw(8);

	t_address nAddress = nStart;
	for (t_size i = 0; i < nIterationCount; ++i)
	{
		os << nAddress << ":  ";
		for (t_size j = 0; j < 4; ++j)
		{
			for (t_size k = 0; k < 4; ++k)
			{
				if (nAddress >= nBaseAddress && nAddress < nEnd)
				{
					core::t_byte n = pMemory[nAddress];
					os << n << " ";
				}
				else
				{
					os << " ";
				}
			}

			os << "  ";
		}

		os << std::endl;
	}
}

//
//	Internal Routines
//
void CDebugger::TurnBreakPoints(bool bOn)
{
	VASM_CHECK_PTR(m_pMemory);
	for (auto const& tItem : m_mapBreakPoints)
	{
		m_pMemory->WriteAt<EOpCode>(tItem.first, bOn ? EOpCode::BREAK : tItem.second);
	}
}

bool CDebugger::TurnBreakPointAt(t_address nAddress, bool bOn)
{
	VASM_CHECK_PTR(m_pMemory);
	auto it = m_mapBreakPoints.find(nAddress);
	if (it != m_mapBreakPoints.end())
	{
		m_pMemory->WriteAt<EOpCode>(it->first, bOn ? EOpCode::BREAK : it->second);
		return true;
	}
	return false;
}

void CDebugger::SetBreakPointAt(t_address nAddress, bool bOn)
{
	VASM_CHECK_PTR(m_pMemory);

	// Check for BP already set, otherwise BREAK instruction could be saved over the original one
	auto it = m_mapBreakPoints.find(nAddress);
	if (it == m_mapBreakPoints.end())
	{
		EOpCode eOpCodeBackup;
		m_pMemory->ReadAt<EOpCode>(nAddress, eOpCodeBackup);
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
			iFunc = it - m_tPackage.aFunctionTable.begin();
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
			if ((tInfo.aCodeTbl.at(iLine) == nCodeAddr))
			{
				nLineNumber = tInfo.nBaseLine + iLine;
				break;
			}
		}
	}

	return (itFunc != m_mapFunctions.end());
}

bool CDebugger::LookupSource(
	t_address nCodeAddr, CStringRef& sFuncName,
	CStringRef& sSrcUnit, t_index& nLineNumber, t_uoffset& nFuncOffset) const
{
	t_index nFuncIdx;
	sFuncName = nullptr;
	sSrcUnit = nullptr;
	nFuncOffset = 0;
	bool bFound = LookupSource(nCodeAddr, nFuncIdx, nLineNumber);
	if (bFound)
	{
		auto const& tInfo = m_tPackage.aFunctionTable.at(nFuncIdx);
		sSrcUnit = &tInfo.sSrcUnit;
		sFuncName = &tInfo.sName;
		nFuncOffset = nCodeAddr - tInfo.nAddress;
		nLineNumber += tInfo.nBaseLine; // Convert function relative line number to unti
	}
	return bFound;
}

CDebugger::t_CallStack CDebugger::ExtractFunctionCallStack() const
{
	VASM_CHECK_PTR(m_pCPU);
	VASM_CHECK_PTR(m_pMemory);

	t_CallStack aCallStack;
	CProcessor::EStatus eStatus = m_pCPU->Status().eStatus;
	if (bool(uint(eStatus) & uint(CProcessor::EStatus::Ready)))
	{
		t_address nCurrIP = m_pCPU->State().nIP;
		t_address nCurrSF = m_pCPU->State().nSF;
		t_uoffset const cnStackBottom = m_pCPU->State().cnStackLBound - 2 * sizeof(t_address);

		t_index	nFuncIdx;
		uint	nLineNumber;

		while (nCurrSF <= cnStackBottom && LookupSource(nCurrIP, nFuncIdx, nLineNumber))
		{
			aCallStack.push_back({nFuncIdx, nLineNumber});

			// Extract next IP & SF
			m_pMemory->ReadAt<t_address>(nCurrSF + sizeof(t_address), nCurrIP);
			m_pMemory->ReadAt<t_address>(nCurrSF, nCurrSF);
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


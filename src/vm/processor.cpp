//
//	Component
//
#define __COMPONENT__ "VM"

//
//	Includes
//
#include "processor.h"
#include <base_utility.h>

// STL
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::SState Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::SState::SState(t_address cs, t_uoffset slb, t_uoffset sub) :
	SCPUStateBase(cs, slb, sub)
{
}

CProcessor::SState& CProcessor::SState::operator=(SState const& o)
{
	SCPUStateBase::operator=(o);
	return *this;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::CProcessor()
{
}

CProcessor::~CProcessor()
{
}

void CProcessor::Reset()
{
	if (m_tStatus.eStatus == EStatus::Running)
	{	// CPU currently is running, do delayed reset
		m_tState.bRun = false;
		m_tStatus.eStatus = EStatus::NotInitialized;
	}
	else
	{
		m_tState = SState();
		m_tStatus = SStatus();
		m_pCmdLib = nullptr;
		m_pMemory = nullptr;
		m_pDecoder = nullptr;
		m_pCurrentCommand = nullptr;
	}
}

void CProcessor::Init(
	CCommandLibraryPtr pCmdLib, CMemoryPtr pMemory, 
	t_uoffset nCodeSize, t_uoffset nStackSize, t_address nProgramStart)
{
	if (pCmdLib == nullptr)
		VASM_THROW_ERROR("Failed to initialize processor: Invalid Command Library pointer.");
	if (pMemory == nullptr)
		VASM_THROW_ERROR("Failed to initialize processor: Invalid Memory pointer.");
	if (nCodeSize < 2)
		VASM_THROW_ERROR("Failed to initialize processor: Invalid Code size.");
	if (nStackSize < CMemory::MinStackSize)
		VASM_THROW_ERROR("Failed to initialize processor: Invalid Stack size.");
	
	t_uoffset const cnMemSize = pMemory->GetSize();
	if (cnMemSize < nCodeSize + nStackSize)
		VASM_THROW_ERROR("Failed to initialize processor: Memory size does not fit with code and stack.");
	if (nCodeSize - nProgramStart < 2)
		VASM_THROW_ERROR("Failed to initialize processor: Program entry point is out of code bounds.");

	if (m_tStatus.eStatus == EStatus::Running)
		VASM_THROW_ERROR("Failed to initialize processor: Processor currenlty is running.");
	
	m_pCmdLib = pCmdLib;
	m_pMemory = pMemory;
	m_pDecoder = std::make_unique<CDecoder>(m_pCmdLib);

	// initialize the state
	m_tState = SState(nCodeSize, cnMemSize, cnMemSize - nStackSize);
	m_tState.nIP = nProgramStart;
	m_tState.nSP = m_tState.nSF = m_tState.cnStackLBound;

	// Create commands execution context
	m_pCurrentCommand = std::move(SCmdCtxtPtr(new SCommandContextEx(m_tState, *m_pMemory)));

	// Now processor is ready to Run!
	m_tStatus.eStatus = EStatus::Ready;
	m_tStatus.oErrorInfo = {};
}

void CProcessor::Reinit(t_address nProgramStart)
{
	Init(m_pCmdLib, m_pMemory, State().cnCodeSize,
		 State().cnStackLBound - State().cnStackUBound,
		 nProgramStart);
}


CProcessor::EStatus CProcessor::Run(bool bOnce)
{
	if (m_tStatus.eStatus == EStatus::NotInitialized)
		VASM_THROW_ERROR(t_csz("Processor is not initialized"));
	if (m_tStatus.eStatus == EStatus::Running)
		VASM_THROW_ERROR(t_csz("Processor already running"));
	if (m_tStatus.eStatus == EStatus::Finished)
		VASM_THROW_ERROR(t_csz("Processor already finished"));
	//if (m_tStatus.eStatus == EStatus::Failed)
	//	VASM_THROW_ERROR(t_csz("Processor status is not valid"));
	if (m_tState.bRun)
		VASM_THROW_ERROR(t_csz("CPU: Attempting to run processor repetitive while it already is running."));

	// Start running
	m_tState.bRun = !bOnce;
	m_tStatus.eStatus = EStatus::Running;
	m_tStatus.oErrorInfo = {};

	SCommandContextEx& tCurrentCommand = CurrentCommandContext();

	// Main loop
	do 
	{
		try
		{
			// Always init R0 because for the program R0 is a readonly ZERO register
			m_tState.reg<uint32>(0) = 0;
			// Save current IP
			m_tState.nCIP = m_tState.nIP;
			// Fetch current command
			uchar const* pCmd = Fetch();
			// Decode current command
			Decode(pCmd, tCurrentCommand);
			// Increment program counter
			m_tState.nIP += tCurrentCommand.tCmdInfo.tMetaInfo.nLength;
			// Execute current command
			Execute(tCurrentCommand);
		}
		catch (base::CError const& err)
		{
			m_tStatus.eStatus = EStatus::Failed;
			m_tStatus.oErrorInfo = err.GetInfo(true);
			m_tState.bRun = false;
		}
		catch (base::CException const& ex)
		{
			m_tStatus.eStatus = EStatus::Failed;
			m_tStatus.oErrorInfo = {ex.Info()};
			m_tState.bRun = false;
		}
		catch (std::exception const& se)
		{
			m_tStatus.eStatus = EStatus::Failed;
			m_tStatus.oErrorInfo = {};
			m_tStatus.oErrorInfo.cszFixedErrorMsg = se.what();
			m_tState.bRun = false;
		}
		catch (...)
		{
			m_tStatus.eStatus = EStatus::Failed;
			m_tStatus.oErrorInfo = {VASM_ERR_INFO("CPU: Unknown exception caught in Run")};
			m_tState.bRun = false;
		}
	} while (m_tState.bRun);

	if (m_tStatus.eStatus == EStatus::Running)
	{
		if (tCurrentCommand.tCmdInfo.tMetaInfo.eOpCode == EOpCode::EXIT)
			m_tStatus.eStatus = EStatus::Finished;
		else if (tCurrentCommand.tCmdInfo.tMetaInfo.eOpCode == EOpCode::BREAK)
			m_tStatus.eStatus = EStatus::Break;
		else
			m_tStatus.eStatus = EStatus::Ready;
	}
	else if (m_tStatus.eStatus == EStatus::NotInitialized)
	{	// CPU Reseted while running, complete Reset
		Reset();
	}

	return m_tStatus.eStatus;
}

void CProcessor::Stop()
{
	if (m_tStatus.eStatus == EStatus::Running)
	{
		m_tState.bRun = false;
		m_tStatus.eStatus = EStatus::Stopped;
	}
}

uchar const* CProcessor::Fetch() const
{
	if (m_tState.nIP >= m_tState.cnCodeSize)
		VASM_THROW_ERROR(t_csz("CPU: Out of code bounds execute."));
	return &Memory().operator[]<uchar>(m_tState.nIP);
}

void CProcessor::Decode(uchar const* pCmd, SCommandContextEx& tCmdCtxt)
{
	// Do basic decode
	m_pDecoder->Decode(pCmd, tCmdCtxt.tCmdInfo);

	SCommandInfo const& tCmdInfo = tCmdCtxt.tCmdInfo;

	if (tCmdInfo.tMetaInfo.eOpCode == EOpCode::Invalid || tCmdInfo.tMetaInfo.nLength == 0)
		VASM_THROW_ERROR(t_csz("CPU: Invalid instruction."));

	// Finalize decoding
	tCmdCtxt.pExec = (*m_pCmdLib)[tCmdInfo.tMetaInfo.eOpCode].pExec;
	tCmdCtxt.apOperands[EOprIdx::First] = nullptr;
	tCmdCtxt.apOperands[EOprIdx::Second] = nullptr;
	tCmdCtxt.apOperands[EOprIdx::Third] = nullptr;

	for (int eOprIdx = EOprIdx::First; eOprIdx < tCmdInfo.tMetaInfo.nOperandCount; ++eOprIdx)
	{
		EOprType eOprType = tCmdInfo.tMetaInfo.aeOprTypes[eOprIdx];
		if (eOprType == EOprType::Reg || (eOprType == EOprType::RegImv && tCmdInfo.eOprSwitch == EOprSwitch::Reg))
		{
			// Multiply Reg idx with OprSize to align with OpSize
			uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCmdInfo.anRegIdx[eOprIdx]), tCmdInfo.eOprSize);
			if (nRegIdx + OperandSize(tCmdInfo.eOprSize) > SState::eRegisterPoolSize)
				throw base::CError(base::toStr("CPU: Invalid GP register index #%1", int(nRegIdx)));
			tCmdCtxt.apOperands[eOprIdx] = &m_tState.aui8RPool[nRegIdx];
		}
		else if (eOprType == EOprType::Imv || (eOprType == EOprType::RegImv && tCmdInfo.eOprSwitch == EOprSwitch::Imv))
		{
			if (tCmdInfo.tMetaInfo.eImvType == EImvType::None)
				VASM_THROW_ERROR(t_csz("CPU: Invalid IMV type"));
			tCmdCtxt.apOperands[eOprIdx] = &tCmdCtxt.tCmdInfo.u64Imv;
		}
		else
		{
			VASM_THROW_ERROR(t_csz("CPU: Invalid Operand type"));
		}
	}
}

void CProcessor::Execute(SCommandContextEx& tCmdCtxt)
{
	if (tCmdCtxt.pExec == nullptr)
		VASM_THROW_ERROR(t_csz("CPU: Invalid instruction executer."));

	if (tCmdCtxt.tCmdInfo.eCdtnCode == ECndtnCode::None ||
		bool(tCmdCtxt.tCmdInfo.tMetaInfo.eExtInfo & SCommandMetaInfo::SkipCdtnCheck) ||
		m_tState.oFlags.isCC(tCmdCtxt.tCmdInfo.eCdtnCode))
	{
		tCmdCtxt.pExec->Exec(tCmdCtxt);
	}
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace casm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

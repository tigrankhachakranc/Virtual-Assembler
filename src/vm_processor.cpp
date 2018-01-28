//
//	Includes
//
#include "vm_processor.h"
#include "vm_executor.h"
#include "vm_command_parser.h"

// STL
#include <cstring>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IPorts Implementation
//
////////////////////////////////////////////////////////////////////////////////
IPorts::~IPorts() = default;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IDebugger Implementation
//
////////////////////////////////////////////////////////////////////////////////
IDebugger::~IDebugger() = default;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::SFlags Implementation
//
////////////////////////////////////////////////////////////////////////////////
bool CProcessor::CFlags::IsEqual() const
{
	return (getZero() == true);
}

bool CProcessor::CFlags::IsNotEqual() const
{
	return (getZero() == false);
}

bool CProcessor::CFlags::IsAbove() const
{
	return (getCarry() == false && getZero() == false);
}

bool CProcessor::CFlags::IsAboveOrEqual() const
{
	return (getCarry() == false);
}

bool CProcessor::CFlags::IsBelow() const
{
	return (getCarry() == true);
}

bool CProcessor::CFlags::IsBelowOrEqual() const
{
	return (getCarry() == true || getZero() == true);
}

bool CProcessor::CFlags::IsGreat() const
{
	return (getZero() == false && getSign() == getOverflow());
}

bool CProcessor::CFlags::IsGreatOrEqual() const
{
	return (getSign() == getOverflow());
}

bool CProcessor::CFlags::IsLow() const
{
	return (getSign() != getOverflow());
}

bool CProcessor::CFlags::IsLowOrEqual() const
{
	return (getZero() == true && getSign() != getOverflow());
}

bool CProcessor::CFlags::IsOverflow() const
{
	return (getOverflow() == true);
}

bool CProcessor::CFlags::IsNotOverflow() const
{
	return (getOverflow() == false);
}

bool CProcessor::CFlags::IsSigned() const
{
	return (getSign() == true);
}

bool CProcessor::CFlags::IsNotSigned() const
{
	return (getSign() == false);
}

bool CProcessor::CFlags::IsParity() const
{
	return (getParity() == true);
}

bool CProcessor::CFlags::IsNotParity() const
{
	return (getParity() == false);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::SState Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::SState::SState() :
	nPC(0),
	bRun(false),
	nSP(reinterpret_cast<t_offset&>(aui32ARPool[0])),
	nSF(reinterpret_cast<t_offset&>(aui32ARPool[1])),
	nStackLBound(0),
	nStackUBound(0)
{
	std::memset(aui32ARPool, 0, size_t(eAddressRegistersPoolSize * sizeof(uint32)));
	std::memset(aui8GPRPool, 0, size_t(eGeneralPurposeRegisterPoolSize));
}

CProcessor::SState::SState(SState const& o) :
	nSP(reinterpret_cast<t_offset&>(aui32ARPool[0])),
	nSF(reinterpret_cast<t_offset&>(aui32ARPool[1])),
	nStackLBound(o.nStackLBound),
	nStackUBound(o.nStackUBound)
{
	operator=(o);
}

CProcessor::SState& CProcessor::SState::operator=(SState const& o)
{
	if (&o != this)
	{
		nPC = o.nPC;
		oFlags = o.oFlags;
		bRun = o.bRun;

		std::memcpy(aui32ARPool, o.aui32ARPool, size_t(eAddressRegistersPoolSize * sizeof(uint32)));
		std::memcpy(aui8GPRPool, o.aui8GPRPool, size_t(eGeneralPurposeRegisterPoolSize));
	}
	return *this;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::IDecodeFilter Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::IDecodeFilter::~IDecodeFilter() = default;

bool CProcessor::IDecodeFilter::Preprocess(std::string&)
{
	return true;
}

CProcessor::SArgument CProcessor::IDecodeFilter::ParseArgument(std::string const&)
{
	return SArgument();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::CProcessor() = default;
CProcessor::~CProcessor() = default;
CProcessor::IExecutor::~IExecutor() = default;

bool CProcessor::IsValid() const
{
	return (m_pCode != nullptr && m_pMemory != nullptr &&
			m_pPorts != nullptr && m_pExecutor != nullptr);
}

void CProcessor::Init(
	CCodePtr pCode, CMemoryPtr pMemory, IPortsPtr pPorts, t_size nStackSize)
{
	if (pCode == nullptr)
		throw base::CException("Failed to initialize processor: Invalid Code pointer.");
	if (pMemory == nullptr)
		throw base::CException("Failed to initialize processor: Invalid Memory pointer.");
	if (pPorts == nullptr)
		throw base::CException("Failed to initialize processor: Invalid I/O Ports pointer.");
	if (pCode->GetSize() < 1)
		throw base::CException("Failed to initialize processor: Code is empty.");
	if (pMemory->GetSize() < nStackSize)
		throw base::CException("Failed to initialize processor: memory size is too small.");

	m_pCode = pCode;
	m_pMemory = pMemory;
	m_pPorts = pPorts;

	// Create executor
	m_pExecutor = CreateExecutor();
	if (m_pExecutor == nullptr)
		throw base::CException("Failed to initialize execution unit.");

	m_tState = SState();
	m_tState.nStackLBound = (t_offset) (pMemory->GetSize() - nStackSize);
	m_tState.nStackUBound = (t_offset) pMemory->GetSize();
	m_tState.nSP = m_tState.nStackUBound;
	m_tState.nSF = m_tState.nStackUBound;

	// Now processor is ready to Run!
}

void CProcessor::AttachDebugger(IDebuggerPtr pDebugger)
{
	if (pDebugger == nullptr)
		throw base::CException("Failed to attach debugger: Invalid pointer.");

	m_pDebugger = pDebugger;
	m_tState.oFlags.setTrap(true);
}

void CProcessor::DetachDebugger()
{
	m_pDebugger = nullptr;
}

void CProcessor::SetDecodeFilter(IDecodeFilterPtr pDecodeFilter)
{
	m_pDecodeFilter = pDecodeFilter;
}

void CProcessor::Run(CException* pError)
{
	try
	{
		if (!IsValid())
			throw base::CException("Error: Attempting to run unitizlized processor.");
		if (m_tState.bRun)
			throw base::CException("Error: Attempting to run processor repetitive while it already is running.");

		// Start running
		m_tState.bRun = true;

		if (m_pDebugger != nullptr)
			m_tState.bRun &= m_pDebugger->Start();

		// Main loop
		while (m_tState.bRun)
		{
			// Fetch current command
			m_sCurrentCommand = std::move(Fetch());
			// Decode current command
			m_tCurrentCommandContext = Decode(m_sCurrentCommand);
			// Increment program counter
			++m_tState.nPC;
			// Execute current command
			Execute(m_tCurrentCommandContext);
			// Check trap for debugger
			if (m_tState.oFlags.getTrap())
			{
				// Break program executon
				if (m_pDebugger != nullptr)
					m_tState.bRun &= m_pDebugger->Break();
				else
					m_tState.bRun = false;
			}
		}

		if (m_pDebugger != nullptr)
			m_pDebugger->End();
	}
	catch (CException& e)
	{
		if (m_pDebugger != nullptr)
			m_pDebugger->Error();

		if (pError == nullptr)
			throw; //rethrow
		else
		{	// caller wants to get error manually rather than catching it
			// This happens when Run() called within the context of worker thread
			*pError = std::move(e);
		}
	}
	catch (base::CException& be)
	{
		if (m_pDebugger != nullptr)
			m_pDebugger->Error();

		if (pError == nullptr)
			throw; //rethrow
		else
		{	// caller wants to get error manually rather than catching it
			// This happens when Run() called within the context of worker thread
			CException e(std::move(be), m_tState.nPC, m_sCurrentCommand);
			*pError = std::move(e);
		}
	}
	catch (std::exception const& se)
	{
		if (m_pDebugger != nullptr)
			m_pDebugger->Error();

		if (pError == nullptr)
			throw; //rethrow
		else
		{	// caller wants to get error manually rather than catching it
			// This happens when Run() called within the context of worker thread
			CException e(se, m_tState.nPC, m_sCurrentCommand);
			*pError = std::move(e);
		}
	}
	catch (...)
	{
		if (m_pDebugger != nullptr)
			m_pDebugger->Error();

		if (pError == nullptr)
			throw; //rethrow
		else
		{	// caller wants to get error manually rather than catching it
			// This happens when Run() called within the context of worker thread
			CException e("Unknown exception caught in Run().", m_tState.nPC, m_sCurrentCommand);
			*pError = std::move(e);
		}
	}
}

void CProcessor::Stop()
{
	m_tState.bRun = false;
}

void CProcessor::SetState(SState const& oState)
{
	m_tState = oState;
}

std::string CProcessor::Fetch()
{
	if (m_tState.nPC >= m_pCode->GetSize())
		throw CException("Error: Invalid PC value.", m_tState.nPC, std::string {});
	return m_pCode->GetAt(m_tState.nPC);
}

CProcessor::SCommandContext CProcessor::Decode(std::string& sCommand)
{
	if (sCommand.size() == 0)
		throw CException("Error: Empty command happened.", m_tState.nPC, sCommand);

	SCommandContext tCmdCtxt;
	tCmdCtxt.nLine = m_tState.nPC;

	bool bContinue = true;
	if (m_pDecodeFilter != nullptr)
		bContinue = m_pDecodeFilter->Preprocess(sCommand);

	if (bContinue)
	{
		try
		{
			CCommandParser oParser(sCommand);
			oParser.Parse(tCmdCtxt, m_pDecodeFilter.get());
		}
		catch (base::CException& err)
		{
			throw CException(std::move(err), tCmdCtxt.nLine, m_sCurrentCommand);
		}
	}

	return tCmdCtxt;
}

void CProcessor::Execute(SCommandContext& tCommand)
{
	if (tCommand.pfnCommand == nullptr)
		throw CException("Invalid native command.", tCommand.nLine, m_sCurrentCommand);

	bool bContinue = true;
	if (tCommand.pfnCondition != nullptr)
		bContinue = (m_tState.oFlags.*tCommand.pfnCondition)();

	if (bContinue)
	{
		try
		{
			(m_pExecutor.get()->*tCommand.pfnCommand)(m_tState, tCommand);
		}
		catch (base::CException& err)
		{
			throw CException(std::move(err), tCommand.nLine, m_sCurrentCommand);
		}
	}
}

CProcessor::IExecutorPtr CProcessor::CreateExecutor()
{
	return IExecutorPtr(new CExecutor(m_pMemory, m_pPorts));
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

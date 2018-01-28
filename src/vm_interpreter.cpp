//
//	Includes
//
#include "vm_interpreter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CDecodeFilter Implementation
//
CDecodeFilter::CDecodeFilter(
	t_mapSymbolTable&& mapLabels, CMemoryManagerPtr pMemMan)
	: m_mapLabels(std::move(mapLabels)), m_pMemMan(pMemMan)
{
}

CDecodeFilter::~CDecodeFilter() = default;

CProcessor::SArgument CDecodeFilter::ParseArgument(
	std::string const& sArgument)
{
	CProcessor::SArgument tArg;
	CMemoryManagerPtr pMemMan = m_pMemMan.lock();

	// Try label
	auto itLbl = m_mapLabels.find(sArgument);
	if (itLbl != m_mapLabels.end())
	{	// Found
		tArg.eType = CProcessor::SArgument::Number;
		tArg.nValue = itLbl->second;
	}
	else if (pMemMan != nullptr)
	{	// Try variable anme
		tArg.nValue = pMemMan->GetVariableOffset(sArgument);
		if (tArg.nValue > 0)
			tArg.eType = CProcessor::SArgument::Number;
	}
	return tArg;
}

//
//	CInterpreter Implementation
//
CInterpreter::CInterpreter() = default;

CInterpreter::~CInterpreter()
{
	Reset();
}

void CInterpreter::Init(std::istream& oInput)
{
	// Reset first
	Reset();

	// Pre-process input and get code
	t_size nStackSize = 0;
	t_aVariables aVariables;
	t_mapSymbolTable mapLabels;
	m_pCode = Preprocessor::Run(oInput, mapLabels, aVariables, nStackSize);
	if (m_pCode == nullptr)
		throw base::CException("Failed to Pre-process input.");

	// Create the Memory Manager and initialize
	m_pMemMan = CMemoryManagerPtr(new CMemoryManager);
	m_pMemMan->Init(aVariables, nStackSize);

	// Create the IO Manager
	m_pIOMan = CIOManagerPtr(new CIOManager);

	// Create decode filter and pass labels to it
	m_pDecoeFilter = CDecodeFilterPtr(new CDecodeFilter(std::move(mapLabels), m_pMemMan));

	// Create the CPU and intialize
	m_pCPU = CProcessorPtr(new CProcessor);
	m_pCPU->Init(m_pCode, m_pMemMan->GetMemory(), m_pIOMan->GetPorts(), m_pMemMan->GetStackSize());
	m_pCPU->SetDecodeFilter(m_pDecoeFilter);

	if (m_pDebugger != nullptr)
		m_pCPU->AttachDebugger(m_pDebugger);
}

bool CInterpreter::IsValid() const
{
	bool bOk = (m_pCode != nullptr && m_pMemMan != nullptr &&
				m_pIOMan != nullptr && m_pCPU != nullptr &&
				m_pCPU->IsValid() && m_pMemMan->IsValid());
	return bOk;
}


void CInterpreter::SetDebugger(IDebuggerPtr pDebugger)
{
	m_pDebugger = pDebugger;
	if (m_pCPU != nullptr)
	{
		if (m_pDebugger != nullptr)
			m_pCPU->AttachDebugger(m_pDebugger);
		else
			m_pCPU->DetachDebugger();
	}
}

void CInterpreter::Run(bool bAssynch)
{
	if (!IsValid())
		throw base::CException("Failed to run: Interpreter is not initialized.");

	if (m_pCPU->IsRunning() || m_oWorkerThread.joinable())
		throw base::CException("Failed to run: Interpreter already is running.");

	m_oLastError.Clear();

	if (bAssynch)
	{
		auto lfuncRunCaller = [this]
		{
			this->m_pCPU->Run(&this->m_oLastError);
			this->Finished();
		};

		m_oWorkerThread = std::move(std::thread(lfuncRunCaller));
	}
	else
	{
		m_pCPU->Run();
	}
}

void CInterpreter::Stop()
{
	if (m_pCPU != nullptr)
		m_pCPU->Stop();
}

bool CInterpreter::IsRunning() const
{
	bool bRunning = false;
	if (m_pCPU != nullptr)
		bRunning = (m_pCPU->IsRunning() || m_oWorkerThread.joinable());
	return bRunning;
}

bool CInterpreter::IsAssynch() const
{
	return m_oWorkerThread.joinable();
}

void CInterpreter::Reset()
{
	bool isRunning = false;
	if (m_pCPU != nullptr)
	{
		isRunning = m_pCPU->IsRunning();
		if (isRunning)
			m_pCPU->Stop();
	}

	m_pCPU = nullptr;
	m_pCode = nullptr;
	m_pMemMan = nullptr;
	m_pIOMan = nullptr;
	m_pDecoeFilter = nullptr;
	m_pDebugger = nullptr;

	if (m_oWorkerThread.joinable())
	{	// We have two options here
		// Either wait to the thread, but if it is hunged then we also will hung.
		//
		// Or detach from the thread and live it in his fortune by lossing 
		// all references to allocated resources in air,
		// but program will remain until the thread is finished anyway.
		//
		// We will jon for now
		m_oWorkerThread.join();
	}
	else if (isRunning)
	{	// CPU is working in the same thread
		// Very very bad situation, almost Fatal, never ever should happen!!!
		throw base::CException("Fatal error: Interpreter is reset or destroyed during run.");
	}

	m_oLastError.Clear();
}

void CInterpreter::Finished()
{
	;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

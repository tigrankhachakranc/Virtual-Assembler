//
//	Component
//
#define __COMPONENT__ "VM"

//
//	Includes
//
#include "machine.h"
#include "loader.h"
#include "processor.h"
#include "debugger.h"
#include "basic_commands.h"
#include "main_commands.h"
#include "io_commands.h"
#include "io_devices.h"
#include <core_command_library.h>

// STL
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CCommandPtr = core::CCommandPtr;

//
//	C-Tors
//
CMachine::CMachine(std::istream& cin, std::ostream& cout)
{
	m_pProcessor = std::make_shared<CProcessor>();
	m_pIOController = std::make_shared<core::CIOController>();

	// Prepare commnad library
	m_pCmdLib = CreateAndInitCommandLibrary(m_pIOController);

	// Register IO devices
	IDevicePtr pDevice = std::make_shared<CConsoleDevice>(cin, cout);
	m_pIOController->Register(pDevice);
}

CMachine::~CMachine()
{
	try
	{
		Reset();
	}
	catch (std::exception const& e)
	{
		VASM_UNUSED(e);
	}
}


//
//	Initialization
//
CCommandLibraryPtr CMachine::CreateAndInitCommandLibrary(
	CIOControllerPtr pIOController)
{
	CCommandLibraryPtr pCmdLib(new core::CCommandLibrary);
	pCmdLib->Register(CCommandPtr(new CMainCommands));
	pCmdLib->Register(CCommandPtr(new CBasicCommands));
	pCmdLib->Register(CCommandPtr(new CIOCommands(pIOController)));
	return pCmdLib;
}

void CMachine::Init(std::istream& oInput)
{
	// Reset first
	Reset();

	VASM_CHECK_PTR(m_pProcessor);
	VASM_CHECK_PTR(m_pIOController);
	VASM_CHECK_PTR(m_pCmdLib);

	// Load package
	CLoader oLoader;
	SPackageInfo tPackage;
	CMemoryPtr pMemory(new core::CMemory);
	oLoader.Load(oInput, pMemory, tPackage);

	// Init the CPU
	m_pProcessor->Init(m_pCmdLib, pMemory, tPackage.nCodeBase + tPackage.nCodeSize,
					   tPackage.nStackSize, tPackage.nProgramStart);

	// No exceptions, save the state
	m_tPackageInfo = std::move(tPackage);
	m_pMemory = pMemory;

	// Also initialize debugger if available
	CDebuggerPtr pDebugger = m_pDebugger.lock();
	if (pDebugger != nullptr)
		pDebugger->Init(m_pCmdLib, m_pProcessor, m_pIOController, m_tPackageInfo);
}

bool CMachine::IsValid() const
{
	bool bOk = (m_pCmdLib != nullptr && m_pMemory != nullptr &&
				m_pIOController != nullptr && m_pProcessor != nullptr &&
				m_pProcessor->Status().eStatus != CProcessor::EStatus::NotInitialized);
	return bOk;
}


void CMachine::SetDebugger(CDebuggerPtr pDebugger)
{
	if (IsValid())
	{
		if (m_pProcessor->Status().eStatus == CProcessor::EStatus::Running)
			VASM_THROW_ERROR(t_csz("Machine: CPU currently is running"));

		if (pDebugger != nullptr)
			pDebugger->Init(m_pCmdLib, m_pProcessor, m_pIOController, m_tPackageInfo);
	}
	else if (pDebugger != nullptr)
		pDebugger->Reset();

	m_pDebugger = pDebugger;
}

void CMachine::Run(bool bAssynch)
{
	if (!IsValid())
		throw base::CException("Machine: VM is not initialized");

	if (m_pProcessor->Status().eStatus == CProcessor::EStatus::Running || m_oWorkerThread.joinable())
		throw base::CException("Machine: CPU currently is running");
	else if (m_pProcessor->Status().eStatus == CProcessor::EStatus::Finished)
		throw base::CException("Machine: CPU already is finished, reinit the machine first");
	else if (m_pProcessor->Status().eStatus == CProcessor::EStatus::Failed)
		throw base::CException("Machine: CPU is in failed state, reset first");

	m_oLastError.Clear();

	if (bAssynch)
	{
		auto lfuncRunCaller = [this]
		{
			this->RunInternal();
		};

		m_oWorkerThread = std::move(std::thread(lfuncRunCaller));
	}
	else
	{
		RunInternal();
	}
}

void CMachine::RunInternal()
{
	try
	{
		m_pProcessor->Run();
		m_oLastError = m_pProcessor->Status().oErrorInfo;
		Finished();
	}
	catch (base::CException const& e)
	{
		m_oLastError = e;
	}
	catch (std::exception const& se)
	{
		m_oLastError = se;
	}
	catch (...)
	{
		m_oLastError = base::CException(t_csz("Unknown error"));
	}
}

void CMachine::Stop()
{
	VASM_CHECK_PTR(m_pProcessor);
	if (m_pProcessor != nullptr)
		m_pProcessor->Stop();
}

bool CMachine::IsRunning() const
{
	VASM_CHECK_PTR(m_pProcessor);
	bool bRunning = false;
	if (m_pProcessor != nullptr)
		bRunning = (m_pProcessor->Status().eStatus == CProcessor::EStatus::Running || m_oWorkerThread.joinable());
	return bRunning;
}

bool CMachine::IsAssynch() const
{
	return m_oWorkerThread.joinable();
}

void CMachine::Reset()
{
	bool isRunning = IsRunning();

	// Reset CPU
	VASM_CHECK_PTR(m_pProcessor);
	m_pProcessor->Reset();

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
		VASM_THROW_ERROR("Fatal error: Machine reset or destroyed during run.");
	}

	m_pMemory = nullptr;
	m_tPackageInfo = SPackageInfo();
	m_oWorkerThread = std::thread();
	m_oLastError.Clear();

	CDebuggerPtr pDebugger = m_pDebugger.lock();
	if (pDebugger != nullptr)
		pDebugger->Reset();
}

void CMachine::Finished()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

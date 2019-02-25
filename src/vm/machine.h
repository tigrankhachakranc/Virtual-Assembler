#ifndef VASM_VM_MACHINE_H
#define VASM_VM_MACHINE_H

//
// Includes
//
#include "package_info.h"

// STL
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Forward declarations
class CMemory;
class CIOController;
class CCommandLibrary;

using CMemoryPtr = std::shared_ptr<CMemory>;
using CIOControllerPtr = std::shared_ptr<CIOController>;
using CCommandLibraryPtr = std::shared_ptr<CCommandLibrary>;
using CCommandLibraryCPtr = std::shared_ptr<CCommandLibrary const>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CMemoryPtr = core::CMemoryPtr;
using CIOControllerPtr = core::CIOControllerPtr;
using CCommandLibraryPtr = core::CCommandLibraryPtr;
using CCommandLibraryCPtr = core::CCommandLibraryCPtr;

// Forward declarations
class CProcessor;
class CDebugger;

using CProcessorPtr = std::shared_ptr<CProcessor>;
using CDebuggerPtr = std::shared_ptr<CDebugger>;
using CDebuggerWPtr = std::weak_ptr<CDebugger>;

////////////////////////////////////////////////////////////////////////////////
//
//	The Virtual Machine
//	Main class that handles machine logic
//	It creates and manages Memory, Commnands, Processor, IOController
//	Could attach/dettach debugger to it
//
class CMachine
{
public:
	CMachine(std::istream& cin, std::ostream& cout);
	virtual ~CMachine();

public:
	//
	//	Static helpers
	//
	static CCommandLibraryPtr CreateAndInitCommandLibrary(CIOControllerPtr = nullptr);

public:
	//
	//	Methods
	//
	// Initializes machine with input stream that contains binary program
	virtual void Init(std::istream& oInput);
	
	// Returns true if machine is initialized and ready to run
	bool IsValid() const;
	
	// Attaches/detaches the debugger
	void SetDebugger(CDebuggerPtr pDebugger);

	// Runs machine
	// If Assynch is true then machine will run within a different thread
	// If debugger is attached then it will call Debugger's Run methos
	void Run(bool bAssynch);
	
	// Stops current execution of program
	void Stop();
	
	// 
	bool IsRunning() const;
	bool IsAssynch() const;
	
	// Resets machine to uninitialized state
	void Reset();

	// Returns machines command library
	CCommandLibraryCPtr GetCommandLibrary() const;
	// Returnes attached debugger
	inline CDebuggerPtr	GetDebugger() const;

	// Returns most recent error info
	// This technique is necessary for the asynhronous run to get errors of the working thread
	inline base::CException GetLastError() const;

protected:
	//
	//	Implementation
	//
	inline SPackageInfo const&	PackageInfo() const;
	inline CMemoryPtr			GetMemory() const;
	inline CIOControllerPtr		GetIOController() const;
	inline CProcessorPtr		GetProcessor() const;

	inline base::CException&	LastError();

private:
	//
	// Internal rotines
	//
	void RunInternal();
	virtual void Finished();

private:
	//
	//	Content
	//

	// Current program info
	SPackageInfo		m_tPackageInfo;

	CCommandLibraryPtr	m_pCmdLib;
	CMemoryPtr			m_pMemory;
	CProcessorPtr		m_pProcessor;
	CIOControllerPtr	m_pIOController;
	CDebuggerWPtr		m_pDebugger;

	// Worker thread
	std::thread			m_oWorkerThread;
	base::CException	m_oLastError;
};

using CMachinePtr = std::shared_ptr<CMachine>;
using CMachineWPtr = std::weak_ptr<CMachine>;
using CMachineUPtr = std::unique_ptr<CMachine>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline base::CException& CMachine::LastError()
{
	return m_oLastError;
}

inline base::CException CMachine::GetLastError() const
{
	return m_oLastError;
}

inline SPackageInfo const& CMachine::PackageInfo() const
{
	return m_tPackageInfo;
}
inline CCommandLibraryCPtr CMachine::GetCommandLibrary() const
{
	return m_pCmdLib;
}

inline CMemoryPtr CMachine::GetMemory() const
{
	return m_pMemory;
}

inline CIOControllerPtr CMachine::GetIOController() const
{
	return m_pIOController;
}

inline CProcessorPtr CMachine::GetProcessor() const
{
	return m_pProcessor;
}

inline CDebuggerPtr CMachine::GetDebugger() const
{
	return m_pDebugger.lock();
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_VM_MACHINE_H

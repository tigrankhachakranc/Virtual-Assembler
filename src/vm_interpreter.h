#ifndef VM_INTERPRETER_H
#define VM_INTERPRETER_H

//
// Includes
//
#include "vm_preprocessor.h"
#include "vm_processor.h"
#include "vm_memory_manager.h"
#include "vm_io_manager.h"

// STL
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Decode Filter Definition
//
class CDecodeFilter : public CProcessor::IDecodeFilter
{
public:
	CDecodeFilter(t_mapSymbolTable&& mapLabels, CMemoryManagerPtr pMemMan);
	~CDecodeFilter() override;

	// Handles labels and variable names
	CProcessor::SArgument ParseArgument(std::string const& sArgument) override;

private:
	using CMemoryManagerWPtr = std::weak_ptr<CMemoryManager>;

	CMemoryManagerWPtr	m_pMemMan;
	t_mapSymbolTable	m_mapLabels;
};

using CDecodeFilterPtr = std::shared_ptr<CDecodeFilter>;

//
//	Interpretator definition
//	Main class that represents the virtual machine
//
class CInterpreter
{
public:
	CInterpreter();
	~CInterpreter();

public:
	//
	//	Methods
	//
	// Initializes interpreter with input stream that contains program
	void Init(std::istream& oInput);
	// Returns true if interpreter is initialized
	bool IsValid() const;
	// Attaches/detached the debugger
	void SetDebugger(IDebuggerPtr pDebugger);
	// Runs interpreter (from initial or previous state)
	// If Assynch set interpreter will run with a different thread
	void Run(bool bAssynch);
	// Stops current execution of program
	void Stop();
	// Returns true 
	bool IsRunning() const;
	bool IsAssynch() const;
	// Resets interpreter to initial state
	void Reset();

	inline CCodePtr GetCode() const;
	inline CMemoryManagerPtr GetMemMan() const;
	inline CIOManagerPtr GetIOMan() const;
	inline CProcessorPtr GetProcessor() const;

private:
	//
	// Internal rotines
	//
	void Finished();

private:
	//
	//	Content
	//
	CCodePtr			m_pCode;
	CMemoryManagerPtr	m_pMemMan;
	CIOManagerPtr		m_pIOMan;
	CProcessorPtr		m_pCPU;
	CDecodeFilterPtr	m_pDecoeFilter;
	IDebuggerPtr		m_pDebugger;

	// Worker thread
	std::thread			m_oWorkerThread;
	CException			m_oLastError;
};

using CInterpreterPtr = std::shared_ptr<CInterpreter>;


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CCodePtr CInterpreter::GetCode() const
{
	return m_pCode;
}

inline CMemoryManagerPtr CInterpreter::GetMemMan() const
{
	return m_pMemMan;
}

inline CIOManagerPtr CInterpreter::GetIOMan() const
{
	return m_pIOMan;
}

inline CProcessorPtr CInterpreter::GetProcessor() const
{
	return m_pCPU;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_INTERPRETER_H
#ifndef VM_DEBUGGER_H
#define VM_DEBUGGER_H

//
// Includes
//
#include "vm_interpreter.h"

// STL
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Debugger definition
//
class CDebugger : public IDebugger
{
public:
	CDebugger(CInterpreterPtr pMachine);
	~CDebugger() override;

	// Neither copiable, nor movable
	CDebugger(CDebugger const&) = delete;
	CDebugger(CDebugger&&) = delete;
	void operator=(CDebugger const&) = delete;
	void operator=(CDebugger&&) = delete;

public:
	//
	//	Methods
	//
	bool Start() override;
	void End() override;
	void Error() override;

	// Processor calls will call Break() upon breakpoint or trap from its execution loop
	// If Break() returns true CPU execution should continue
	// If Break() returns FALSE CPU execution should be stopped
	bool Break() override;

private:
	// Internal routines
	void PrintState(CProcessor::SState const& tState);

private:
	//
	//	Content
	//
	CInterpreterPtr		m_pMachine;
	std::string const	m_sBreakCmd;
	std::unordered_map<t_size, std::string>	m_mapCodeCache;
};

using CDebuggerPtr = std::shared_ptr<CDebugger>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_DEBUGGER_H
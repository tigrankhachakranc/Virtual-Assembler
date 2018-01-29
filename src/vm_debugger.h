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
	void End(bool bSuccessfull) override;
	bool Error(CException const&) override;
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
	// Additional helpers
	char				m_chLastCommand;
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
#ifndef VASM_CORE_COMMAND_LIBRARY_H
#define VASM_CORE_COMMAND_LIBRARY_H

//
// Includes
//
#include "command.h"
#include <base_exception.h>

// STL
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Command library
//	Keeps command objects and OpCode to Command meta info collection
//
class CCommandLibrary final
{
public:
	CCommandLibrary();
	~CCommandLibrary();

	// Non copiable nor movable
	CCommandLibrary(CCommandLibrary const&) = delete;
	CCommandLibrary(CCommandLibrary&&) = delete;
	void operator=(CCommandLibrary const&) = delete;
	void operator=(CCommandLibrary&&) = delete;

public:
	//
	//	Instruction access operator
	//
	using SInstructionInfo = CCommandBase::SInstructionInfo;
	inline SInstructionInfo const& operator[](EOpCode) const;

public:
	//
	//	Methods
	//

	void SetAddressRecovery(IAddressRecoveryPtr p);

	// Commands registration
	void Register(CCommandPtr pCommand);
	void Unregister(CCommandPtr pCommand);

private:
	using t_InstructionStore = std::vector<SInstructionInfo>;
	using t_CommandStore = std::unordered_set<CCommandPtr>;

	t_CommandStore		m_setCommands;
	t_InstructionStore	m_aInstructions;
	IAddressRecoveryPtr	m_pAddressRecovery;
};

// CCommandBase smart pointers
using CCommandLibraryPtr  = std::shared_ptr<CCommandLibrary>;
using CCommandLibraryCPtr = std::shared_ptr<CCommandLibrary const>;
using CCommandLibraryWPtr = std::weak_ptr<CCommandLibrary>;
using CCommandLibraryUPtr = std::unique_ptr<CCommandLibrary>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CCommandLibrary::SInstructionInfo const& CCommandLibrary::operator[](EOpCode eOpCode) const
{
	if ((t_index) eOpCode > 0 && (t_index) eOpCode < m_aInstructions.size())
		return m_aInstructions[(t_index) eOpCode];
	return m_aInstructions[0]; // OpCode Zero is reserved for the Invalid instruction
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_COMMAND_LIBRARY_H

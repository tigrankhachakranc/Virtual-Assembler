#ifndef VASM_VM_MAIN_COMMAND_H
#define VASM_VM_MAIN_COMMAND_H

//
// Includes
//
#include <core_command.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_address		= core::t_address;
using t_uoffset		= core::t_uoffset;

using EOpCode		= core::EOpCode;
using EOprType		= core::EOprType;
using EOprIdx		= core::EOprIdx;
using EOprSize		= core::EOprSize;
using EOprSwitch	= core::EOprSwitch;
using EImvType		= core::EImvType;
using ECndtnCode	= core::ECndtnCode;
using SCPUStateBase = core::SCPUStateBase;

using SCommandContext	= core::SCommandContext;
using SCommandInfo		= core::SCommandInfo;
using SCommandMetaInfo	= core::SCommandMetaInfo;

////////////////////////////////////////////////////////////////////////////////
//
//	Main commands
//	Implementation of main CPU commands such as constrol, flags manipluation & memory access instructions
//
class CMainCommands : public core::CCommandBase
{
	typedef core::CCommandBase Base;

public:
	CMainCommands();
	~CMainCommands();

private:
	//
	// Instruction execution functions
	//
	void Nop(SCommandContext&);
	void Exit(SCommandContext&);

	// Execution control instructions
	void JumpA(SCommandContext&);	// Absolute unconditionnal jump
	void JumpR(SCommandContext&);	// Relative [conditional] jump
	void CallA(SCommandContext&);	// Absolute call
	void CallR(SCommandContext&);	// Relative call
	void Ret(SCommandContext&);		// Return

	// Memory access instructions
	template <typename TOperandType>
	void Load(SCommandContext&);	// Load operand from memory
	template <typename TOperandType>
	void Store(SCommandContext&);	// Store operand into memory
	template <typename TOperandType>
	void LoadRel(SCommandContext&);	// Load operand from memory with relative 32 bit offset
	template <typename TOperandType>
	void StoreRel(SCommandContext&);// Store operand into memory with relative 32 bit offset

	// Stack instructions
	void PushSF(SCommandContext&);	// Push stack frame
	void PopSF(SCommandContext&);	// Pop stack frame
	template <typename TOperandType>
	void Push(SCommandContext&);	// Push register(s) into the stack
	template <typename TOperandType>
	void Pop(SCommandContext&);		// Pop register(s) from the stack

private:
	//
	// Custom Disassembly functions
	//
	t_string dasmJumpR(SCommandInfo const&, bool bHexadecimal);
};
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_VM_MAIN_COMMAND_H

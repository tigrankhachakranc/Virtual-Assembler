#ifndef VASM_VM_IO_COMMANDS_H
#define VASM_VM_IO_COMMANDS_H

//
// Includes
//
#include <core_io_controller.h>
#include <core_command.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_address		= core::t_address;
using t_uoffset		= core::t_uoffset;
using t_port		= core::t_port;

using CValue		= core::CValue;
using EValueType	= core::EValueType;

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

using CIOController		= core::CIOController;
using CIOControllerPtr	= core::CIOControllerPtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Input & Output commands
//	Implementation of basic instruction set which doesn't touch Flags
//
class CIOCommands : public core::CCommandBase
{
public:
	CIOCommands(CIOControllerPtr);
	~CIOCommands();

private:
	//
	// Instruction execution functions
	//
	template <typename TOperandType>
	void In(SCommandContext&);
	template <typename TOperandType>
	void Out(SCommandContext&);

private:
	//
	//	Contents
	//
	CIOControllerPtr	m_pIOController;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_IO_CONTROLLER_H

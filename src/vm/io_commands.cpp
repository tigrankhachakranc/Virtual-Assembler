//
//	Component
//
#define __COMPONENT__ "VM"

//
//	Includes
//
#include "io_commands.h"
#include <base_exception.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CIOCommands Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CIOCommands::CIOCommands(CIOControllerPtr pIOController) :
	CCommandBase(), m_pIOController(pIOController)
{
	FuncCmdExec apfnIn[int(EOprSize::Count)] = {
		FuncCmdExec(&CIOCommands::In<uint8>), FuncCmdExec(&CIOCommands::In<uint16>),
		FuncCmdExec(&CIOCommands::In<uint32>), FuncCmdExec(&CIOCommands::In<uint64>) };
	Register({t_csz("IN"), EOpCode::IN, EOprType::Reg, EOprType::RegImv, EImvType::Port,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch |
			 SCommandMetaInfo::SingularOperandSize | SCommandMetaInfo::CustomExtension},
			 apfnIn, FuncCmdDisasm(&CIOCommands::DisAsm));

	FuncCmdExec apfnOut[int(EOprSize::Count)] = {
		FuncCmdExec(&CIOCommands::Out<uint8>), FuncCmdExec(&CIOCommands::Out<uint16>),
		FuncCmdExec(&CIOCommands::Out<uint32>), FuncCmdExec(&CIOCommands::Out<uint64>) };
	Register({t_csz("OUT"), EOpCode::OUT, EOprType::Reg, EOprType::RegImv, EImvType::Port,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch |
			 SCommandMetaInfo::SingularOperandSize | SCommandMetaInfo::CustomExtension},
			 apfnOut, FuncCmdDisasm(&CIOCommands::DisAsm));

	//if (m_pIOController == nullptr)
	//	VASM_THROW_ERROR("Failed to initialize I/O commands: Invalid Controller pointer.");
}

CIOCommands::~CIOCommands() = default;

//
//	Executor functions
//

template <typename TOperandType>
void CIOCommands::In(SCommandContext& tCtxt)
{
	VASM_CHECK_PTR(m_pIOController);
	CValue oValue(core::ValueType<TOperandType>());
	t_port nPort = *tCtxt.operand<t_port>(EOprIdx::Second);
	m_pIOController->In(nPort, oValue);
	*tCtxt.operand<TOperandType>(EOprIdx::First) = static_cast<TOperandType>(oValue);
}

template <typename TOperandType>
void CIOCommands::Out(SCommandContext& tCtxt)
{
	VASM_CHECK_PTR(m_pIOController);
	CValue oValue(*tCtxt.operand<TOperandType>(EOprIdx::First));
	t_port nPort = *tCtxt.operand<t_port>(EOprIdx::Second);
	m_pIOController->Out(nPort, oValue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


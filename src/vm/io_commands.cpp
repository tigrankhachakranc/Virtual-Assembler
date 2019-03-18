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
	Register({t_csz("IN"), EOpCode::IN, EOprType::GR, EOprType::GRIMV, EImvType::Port,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
			 apfnIn, FuncCmdDisasm(&CIOCommands::DisAsm));

	FuncCmdExec apfnOut[int(EOprSize::Count)] = {
		FuncCmdExec(&CIOCommands::Out<uint8>), FuncCmdExec(&CIOCommands::Out<uint16>),
		FuncCmdExec(&CIOCommands::Out<uint32>), FuncCmdExec(&CIOCommands::Out<uint64>) };
	Register({t_csz("OUT"), EOpCode::OUT, EOprType::GR, EOprType::GRIMV, EImvType::Port,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
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
	t_port nPort = *tCtxt.tOpr[EOprIdx::Second].ptr<t_port>();
	m_pIOController->In(nPort, oValue);
	*tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>() = static_cast<TOperandType>(oValue);
}

template <typename TOperandType>
void CIOCommands::Out(SCommandContext& tCtxt)
{
	VASM_CHECK_PTR(m_pIOController);
	CValue oValue(*tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>());
	t_port nPort = *tCtxt.tOpr[EOprIdx::Second].ptr<t_port>();
	m_pIOController->Out(nPort, oValue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


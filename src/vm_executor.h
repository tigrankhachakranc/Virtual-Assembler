#ifndef VM_EXECUTOR_H
#define VM_EXECUTOR_H

//
// Includes
//
#include "vm_processor.h"

// STL
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Executor definition for Win32 & x64 environment
//
class CExecutor : public CProcessor::IExecutor
{
public:
	using SMachineState = CProcessor::SState;
	using SCommandContext = CProcessor::SCommandContext;

	CExecutor(CMemoryPtr pMemory, IPortsPtr pPorts);
	~CExecutor() override;

	// Commands
	void Nop(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Break(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Execution control
	void Jump(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Call(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Ret(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Exit(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Assignment
	void Assign(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Move(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Swap(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Logical
	void And(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Or(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Xor(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Not(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Nand(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Nor(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Unsigned integral arithmetic
	void Add(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Sub(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Mul(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Div(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Inc(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Dec(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Signeg integral arithmetic
	void AddC(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void SubB(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void IMul(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void IDiv(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Neg(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Shift
	void ShiftR(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void ShiftL(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void RotateR(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void RotateL(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void ShiftAR(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void ShiftAL(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void RotateCR(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void RotateCL(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Comparison
	void Test(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Cmp(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Stack
	void Push(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Pop(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void PushF(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void PopF(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Memory
	void Load(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Store(SMachineState& tMachineState, SCommandContext& tCommand) override;

	// Input & Output
	void In(SMachineState& tMachineState, SCommandContext& tCommand) override;
	void Out(SMachineState& tMachineState, SCommandContext& tCommand) override;

protected:
	//
	//	Implementation
	//
	enum class EArgIndex
	{
		First,
		Second,
		Third
	};

	// Internal routines
	typedef std::pair<void*, CProcessor::EOpSize> t_parArgument;
	t_parArgument GetArgument(	SMachineState& tMachineState,
								SCommandContext& tCommand,
								EArgIndex eArgIdx);

private:
	//
	//	Content
	//
	CMemoryPtr			m_pMemory;
	IPortsPtr			m_pPorts;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_EXECUTOR_H
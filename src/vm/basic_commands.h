#ifndef VASM_VM_BASIC_COMMAND_H
#define VASM_VM_BASIC_COMMAND_H

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
using EImvType		= core::EImvType;
using ECndtnCode	= core::ECndtnCode;
using SCPUStateBase = core::SCPUStateBase;

using SCommandContext	= core::SCommandContext;
using SCommandInfo		= core::SCommandInfo;
using SCommandMetaInfo	= core::SCommandMetaInfo;

////////////////////////////////////////////////////////////////////////////////
//
//	Basic commands
//	Implementation of basic arithmetic & logic instruction set which are modifiing Flags
//
class CBasicCommands : public core::CCommandBase
{
	typedef core::CCommandBase Base;

public:
	CBasicCommands();
	~CBasicCommands();

private:
	//
	// Instruction execution functions
	//

	// Address increment/decrement
	void Inc(SCommandContext&);
	void Dec(SCommandContext&);
	void Inc2(SCommandContext&);
	void Dec2(SCommandContext&);

	// Assignemnt instuctions
	void AssignA0(SCommandContext&);	// Assign NULL to address register
	void AssignA4(SCommandContext&);	// Assign dword to address register
	void AssignR1(SCommandContext&);	// Assign byte to GP register
	void AssignR2(SCommandContext&);	// Assign word to GP register
	void AssignR4(SCommandContext&);	// Assign dword to GP register
	void AssignR8(SCommandContext&);	// Assign qword to GP register

	// Register manipulation instructions
	template <typename TOperandType>
	void Move(SCommandContext&);
	template <typename TOperandType>
	void Swap(SCommandContext&);

	// Comparison instructions
	void TestB(SCommandContext&);
	void TestW(SCommandContext&);
	void TestDW(SCommandContext&);
	void TestQW(SCommandContext&);

	void CmpB(SCommandContext&);
	void CmpW(SCommandContext&);
	void CmpDW(SCommandContext&);
	void CmpQW(SCommandContext&);

	template <typename TOperandType>
	void Set(SCommandContext&);		// Set register to 1 based on CC

	// Logical Instructions
	void AndB(SCommandContext&);
	void AndW(SCommandContext&);
	void AndDW(SCommandContext&);
	void AndQW(SCommandContext&);

	void OrB(SCommandContext&);
	void OrW(SCommandContext&);
	void OrDW(SCommandContext&);
	void OrQW(SCommandContext&);

	void XorB(SCommandContext&);
	void XorW(SCommandContext&);
	void XorDW(SCommandContext&);
	void XorQW(SCommandContext&);

	void NandB(SCommandContext&);
	void NandW(SCommandContext&);
	void NandDW(SCommandContext&);
	void NandQW(SCommandContext&);

	void NorB(SCommandContext&);
	void NorW(SCommandContext&);
	void NorDW(SCommandContext&);
	void NorQW(SCommandContext&);

	void NotB(SCommandContext&);
	void NotW(SCommandContext&);
	void NotDW(SCommandContext&);
	void NotQW(SCommandContext&);


	// Shift instructions
	void ShlB(SCommandContext&);	// Shift left
	void ShlW(SCommandContext&);
	void ShlDW(SCommandContext&);
	void ShlQW(SCommandContext&);

	void ShrB(SCommandContext&);	// Shift right
	void ShrW(SCommandContext&);
	void ShrDW(SCommandContext&);
	void ShrQW(SCommandContext&);

	void RolB(SCommandContext&);	// Rotate left
	void RolW(SCommandContext&);
	void RolDW(SCommandContext&);
	void RolQW(SCommandContext&);

	void RorB(SCommandContext&);	// Rotate right
	void RorW(SCommandContext&);
	void RorDW(SCommandContext&);
	void RorQW(SCommandContext&);

	void SalB(SCommandContext&);	// Shift arithmetic left
	void SalW(SCommandContext&);
	void SalDW(SCommandContext&);
	void SalQW(SCommandContext&);

	void SarB(SCommandContext&);	// Shift arithmetic right
	void SarW(SCommandContext&);
	void SarDW(SCommandContext&);
	void SarQW(SCommandContext&);

	void RclB(SCommandContext&);	// Rotate with carry left
	void RclW(SCommandContext&);
	void RclDW(SCommandContext&);
	void RclQW(SCommandContext&);

	void RcrB(SCommandContext&);	// Rotate with cary right
	void RcrW(SCommandContext&);
	void RcrDW(SCommandContext&);
	void RcrQW(SCommandContext&);

	// Unsigned integral arithmetic instructions
	void AddB(SCommandContext&);	// Addition
	void AddW(SCommandContext&);
	void AddDW(SCommandContext&);
	void AddQW(SCommandContext&);

	void SubB(SCommandContext&);	// Substruction
	void SubW(SCommandContext&);
	void SubDW(SCommandContext&);
	void SubQW(SCommandContext&);

	void MulB(SCommandContext&);	// Multiplication
	void MulW(SCommandContext&);
	void MulDW(SCommandContext&);
	void MulQW(SCommandContext&);

	void DivB(SCommandContext&);	// Division
	void DivW(SCommandContext&);
	void DivDW(SCommandContext&);
	void DivQW(SCommandContext&);

	// Signed integral arithmetic instructions
	void AdcB(SCommandContext&);	// Addition with carry
	void AdcW(SCommandContext&);
	void AdcDW(SCommandContext&);
	void AdcQW(SCommandContext&);

	void SbbB(SCommandContext&);	// Substruction with borrow
	void SbbW(SCommandContext&);
	void SbbDW(SCommandContext&);
	void SbbQW(SCommandContext&);

	void IMulB(SCommandContext&);	// Multiplication
	void IMulW(SCommandContext&);
	void IMulDW(SCommandContext&);
	void IMulQW(SCommandContext&);

	void IDivB(SCommandContext&);	// Division
	void IDivW(SCommandContext&);
	void IDivDW(SCommandContext&);
	void IDivQW(SCommandContext&);

	void NegB(SCommandContext&);	// Sign negation
	void NegW(SCommandContext&);
	void NegDW(SCommandContext&);
	void NegQW(SCommandContext&);

	template <typename TOperandType>
	void Cast(SCommandContext&);	// Sign extension (type conversion)

private:
	//
	// Custom Disassembly functions
	//
	t_string dasmAssignA0(SCommandInfo const&, bool bHexadecimal);
	t_string dasmAssignA4(SCommandInfo const&, bool bHexadecimal);
	t_string dasmAssignRn(SCommandInfo const&, bool bHexadecimal);
	t_string dasmCast(SCommandInfo const&, bool bHexadecimal);
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // VASM_VM_BASIC_COMMAND_H

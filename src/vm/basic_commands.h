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

	// Assignemnt operations
	void MovIA(SCommandContext&);	// Assign dword address to register
	void MovI1(SCommandContext&);	// Assign byte to register
	void MovI2(SCommandContext&);	// Assign word to register
	void MovI4(SCommandContext&);	// Assign dword to register
	//void MovI8(SCommandContext&);	// Assign qword to register

	// Register manipulation instructions
	template <typename TOperandType>
	void Move(SCommandContext&);
	template <typename TOperandType>
	void Swap(SCommandContext&);

	// Flags manipulation instructions
	void GetF(SCommandContext&);	// Get low word Flags into register
	void SetF(SCommandContext&);	// Set low word Flags from register
	void CLC(SCommandContext&);		// Clear Carry Flag
	void STC(SCommandContext&);		// Set Carry Flag
	void CMC(SCommandContext&);		// Complement (toggle) Carry Flag
	template <typename TOperandType>
	void Set(SCommandContext&);		// Set register to 1 based on CC

	// Comparison instructions
	void TestB(SCommandContext&);
	void TestW(SCommandContext&);
	void TestDW(SCommandContext&);
	void TestQW(SCommandContext&);

	void CmpB(SCommandContext&);
	void CmpW(SCommandContext&);
	void CmpDW(SCommandContext&);
	void CmpQW(SCommandContext&);

	// Logical bitwise operations
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

	// Shift & Rotate
	void ShlB(SCommandContext&);	// Shift left
	void ShlW(SCommandContext&);
	void ShlDW(SCommandContext&);
	void ShlQW(SCommandContext&);

	void ShrB(SCommandContext&);	// Shift right
	void ShrW(SCommandContext&);
	void ShrDW(SCommandContext&);
	void ShrQW(SCommandContext&);

	void SalB(SCommandContext&);	// Shift arithmetic left
	void SalW(SCommandContext&);
	void SalDW(SCommandContext&);
	void SalQW(SCommandContext&);

	void SarB(SCommandContext&);	// Shift arithmetic right
	void SarW(SCommandContext&);
	void SarDW(SCommandContext&);
	void SarQW(SCommandContext&);

	void RolB(SCommandContext&);	// Rotate left
	void RolW(SCommandContext&);
	void RolDW(SCommandContext&);
	void RolQW(SCommandContext&);

	void RorB(SCommandContext&);	// Rotate right
	void RorW(SCommandContext&);
	void RorDW(SCommandContext&);
	void RorQW(SCommandContext&);

	void RclB(SCommandContext&);	// Rotate with carry left
	void RclW(SCommandContext&);
	void RclDW(SCommandContext&);
	void RclQW(SCommandContext&);

	void RcrB(SCommandContext&);	// Rotate with cary right
	void RcrW(SCommandContext&);
	void RcrDW(SCommandContext&);
	void RcrQW(SCommandContext&);

	// Integral arithmetic
	void AddB(SCommandContext&);	// Addition
	void AddW(SCommandContext&);
	void AddDW(SCommandContext&);
	void AddQW(SCommandContext&);

	void AdcB(SCommandContext&);	// Addition with carry
	void AdcW(SCommandContext&);
	void AdcDW(SCommandContext&);
	void AdcQW(SCommandContext&);

	void SubB(SCommandContext&);	// Substruction
	void SubW(SCommandContext&);
	void SubDW(SCommandContext&);
	void SubQW(SCommandContext&);

	void SbbB(SCommandContext&);	// Substruction with borrow
	void SbbW(SCommandContext&);
	void SbbDW(SCommandContext&);
	void SbbQW(SCommandContext&);

	void NegB(SCommandContext&);	// Sign negation
	void NegW(SCommandContext&);
	void NegDW(SCommandContext&);
	void NegQW(SCommandContext&);

	// Address increment/decrement
	void Inc(SCommandContext&);
	void Dec(SCommandContext&);

	// Multiplication & Division
	void MulB(SCommandContext&);	// Unsigned multiplication
	void MulW(SCommandContext&);
	void MulDW(SCommandContext&);
	void MulQW(SCommandContext&);

	void DivB(SCommandContext&);	// Unsigned division
	void DivW(SCommandContext&);
	void DivDW(SCommandContext&);
	void DivQW(SCommandContext&);

	void IMulB(SCommandContext&);	// Signed multiplication
	void IMulW(SCommandContext&);
	void IMulDW(SCommandContext&);
	void IMulQW(SCommandContext&);

	void IDivB(SCommandContext&);	// Signed division
	void IDivW(SCommandContext&);
	void IDivDW(SCommandContext&);
	void IDivQW(SCommandContext&);

	// Type conversion
	template <typename TOperandType>
	void Cast(SCommandContext&);	// Sign extension 

private:
	//
	// Custom Disassembly functions
	//
	t_string dasmMovIA(SCommandInfo const&, bool bHexadecimal);
	t_string dasmMovIR(SCommandInfo const&, bool bHexadecimal);
	t_string dasmCast(SCommandInfo const&, bool bHexadecimal);
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // VASM_VM_BASIC_COMMAND_H

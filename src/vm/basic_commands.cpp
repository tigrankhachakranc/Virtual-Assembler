//
//	Includes
//
#include "basic_commands.h"
#include <base_exception.h>
#include <base_utility.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Assembly operations forward declarations
//	All functions implemented return Flags
//

// Logical operations
extern "C" uint16_t __stdcall fnAND8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnXOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnXOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnXOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnXOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnNAND8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnNAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnNAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnNAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnNOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnNOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnNOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnNOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnNOT8(uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnNOT16(uint16_t* pTarget);
extern "C" uint16_t __stdcall fnNOT32(uint32_t* pTarget);
extern "C" uint16_t __stdcall fnNOT64(uint64_t* pTarget);

// Unsigned integral arithmetic operations
extern "C" uint16_t __stdcall fnADD8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnADD16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnADD32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnADD64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnSUB8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnSUB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnSUB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnSUB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnMUL8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnMUL16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnMUL32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnDIV8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnDIV16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnDIV32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

//extern "C" uint16_t __stdcall fnINC8 (uint8_t*  pTarget);
//extern "C" uint16_t __stdcall fnINC16(uint16_t* pTarget);
//extern "C" uint16_t __stdcall fnINC32(uint32_t* pTarget);
//extern "C" uint16_t __stdcall fnINC64(uint64_t* pTarget);

//extern "C" uint16_t __stdcall fnDEC8 (uint8_t*  pTarget);
//extern "C" uint16_t __stdcall fnDEC16(uint16_t* pTarget);
//extern "C" uint16_t __stdcall fnDEC32(uint32_t* pTarget);
//extern "C" uint16_t __stdcall fnDEC64(uint64_t* pTarget);

// Signed integral arithmetic operations
extern "C" uint16_t __stdcall fnADC8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget, bool bCarryFlag);

extern "C" uint16_t __stdcall fnSBB8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget, bool bCarryFlag);

extern "C" uint16_t __stdcall fnIMUL8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnIMUL16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnIMUL32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnIMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnIDIV8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnIDIV16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pTarget);
extern "C" uint16_t __stdcall fnIDIV32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pTarget);
extern "C" uint16_t __stdcall fnIDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pTarget);

extern "C" uint16_t __stdcall fnNEG8 (uint8_t*  pTarget);
extern "C" uint16_t __stdcall fnNEG16(uint16_t* pTarget);
extern "C" uint16_t __stdcall fnNEG32(uint32_t* pTarget);
extern "C" uint16_t __stdcall fnNEG64(uint64_t* pTarget);

// Shift & Rotate operations
extern "C" uint16_t __stdcall fnSHR8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHR16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHR32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHR64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnSHL8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHL16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHL32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSHL64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnROR8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROR16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROR32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROR64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnROL8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROL16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROL32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnROL64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnSAR8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAR16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAR32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAR64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnSAL8 (uint8_t*  pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAL16(uint16_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAL32(uint32_t* pTarget, uint8_t* pOperand);
extern "C" uint16_t __stdcall fnSAL64(uint64_t* pTarget, uint8_t* pOperand);

extern "C" uint16_t __stdcall fnRCR8 (uint8_t*  pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR16(uint16_t* pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR32(uint32_t* pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR64(uint64_t* pTarget, uint8_t* pOperand, bool bCarryFlag);

extern "C" uint16_t __stdcall fnRCL8 (uint8_t*  pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL16(uint16_t* pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL32(uint32_t* pTarget, uint8_t* pOperand, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL64(uint64_t* pTarget, uint8_t* pOperand, bool bCarryFlag);

// Comparison operations
extern "C" uint16_t __stdcall fnTEST8 (uint8_t*  pLeft, uint8_t*  pRight);
extern "C" uint16_t __stdcall fnTEST16(uint16_t* pLeft, uint16_t* pRight);
extern "C" uint16_t __stdcall fnTEST32(uint32_t* pLeft, uint32_t* pRight);
extern "C" uint16_t __stdcall fnTEST64(uint64_t* pLeft, uint64_t* pRight);

extern "C" uint16_t __stdcall fnCMP8 (uint8_t*  pLeft, uint8_t*  pRight);
extern "C" uint16_t __stdcall fnCMP16(uint16_t* pLeft, uint16_t* pRight);
extern "C" uint16_t __stdcall fnCMP32(uint32_t* pLeft, uint32_t* pRight);
extern "C" uint16_t __stdcall fnCMP64(uint64_t* pLeft, uint64_t* pRight);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CBasicCommands Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CBasicCommands::CBasicCommands() : CCommandBase()
{
	Register({t_csz("INC"), EOpCode::INC, EOprType::AR, EOprType::GR, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::Inc), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("DEC"), EOpCode::DEC, EOprType::AR, EOprType::GR, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::Dec), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("INC"), EOpCode::INC2, EOprType::AR, EOprType::IMV, EImvType::Num16},
			 FuncCmdExec(&CBasicCommands::Inc2), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("DEC"), EOpCode::DEC2, EOprType::AR, EOprType::IMV, EImvType::Num16},
			 FuncCmdExec(&CBasicCommands::Dec2), FuncCmdDisasm(&CBasicCommands::DisAsm));

	Register({t_csz("ASSIGN"), EOpCode::ASSIGNA0, EOprType::AR},
			 FuncCmdExec(&CBasicCommands::AssignA0), FuncCmdDisasm(&CBasicCommands::dasmAssignA0));
	Register({t_csz("ASSIGN"), EOpCode::ASSIGNA4, EOprType::AR, EOprType::IMV, EImvType::Num32},
			 FuncCmdExec(&CBasicCommands::AssignA4), FuncCmdDisasm(&CBasicCommands::dasmAssignA4));
	Register({t_csz("ASSIGN"), EOpCode::ASSIGNR1, EOprType::GR, EOprType::IMV, EImvType::SNum8, SCommandMetaInfo::FixedOprSizeByte},
			 FuncCmdExec(&CBasicCommands::AssignR1), FuncCmdDisasm(&CBasicCommands::dasmAssignRn));
	Register({t_csz("ASSIGN"), EOpCode::ASSIGNR2, EOprType::GR, EOprType::IMV, EImvType::SNum16, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CBasicCommands::AssignR2), FuncCmdDisasm(&CBasicCommands::dasmAssignRn));
	Register({t_csz("ASSIGN"), EOpCode::ASSIGNR4, EOprType::GR, EOprType::IMV, EImvType::SNum32, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::AssignR4), FuncCmdDisasm(&CBasicCommands::dasmAssignRn));
	Register({t_csz("ASSIGN"), EOpCode::ASSIGNR8, EOprType::GR, EOprType::IMV, EImvType::SNum64, SCommandMetaInfo::FixedOprSizeQWord},
			 FuncCmdExec(&CBasicCommands::AssignR8), FuncCmdDisasm(&CBasicCommands::dasmAssignRn));

	FuncCmdExec apfnMove[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Move<uint8>),  FuncCmdExec(&CBasicCommands::Move<uint16>), 
		FuncCmdExec(&CBasicCommands::Move<uint32>), FuncCmdExec(&CBasicCommands::Move<uint64>) };
	Register({t_csz("MOVE"), t_csz("MOV"), EOpCode::MOVE, EOprType::AGR, EOprType::AGR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::HasCndtnCode},
			 apfnMove, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSwap[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Swap<uint8>),  FuncCmdExec(&CBasicCommands::Swap<uint16>),
		FuncCmdExec(&CBasicCommands::Swap<uint32>), FuncCmdExec(&CBasicCommands::Swap<uint64>) };
	Register({t_csz("SWAP"), t_csz("SWP"), EOpCode::SWAP, EOprType::AGR, EOprType::AGR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::HasCndtnCode},
			 apfnSwap, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnTest[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::TestB),  FuncCmdExec(&CBasicCommands::TestW),
		FuncCmdExec(&CBasicCommands::TestDW), FuncCmdExec(&CBasicCommands::TestQW) };
	Register({t_csz("TEST"), EOpCode::TEST, EOprType::AGR, EOprType::AGR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
			 apfnTest, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnCmp[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::CmpB),  FuncCmdExec(&CBasicCommands::CmpW),
		FuncCmdExec(&CBasicCommands::CmpDW), FuncCmdExec(&CBasicCommands::CmpQW) };
	Register({t_csz("CMP"), EOpCode::CMP, EOprType::AGR, EOprType::AGR, SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch},
			apfnTest, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnSet[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Set<uint8>),  FuncCmdExec(&CBasicCommands::Set<uint16>),
		FuncCmdExec(&CBasicCommands::Set<uint32>), FuncCmdExec(&CBasicCommands::Set<uint64>) };
	Register({t_csz("SET"), t_csz("ST"), EOpCode::SET, EOprType::GR,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode | SCommandMetaInfo::SkipCdtnCheck},
			 apfnSet, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnAnd[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AndB),  FuncCmdExec(&CBasicCommands::AndW),
		FuncCmdExec(&CBasicCommands::AndDW), FuncCmdExec(&CBasicCommands::AndQW) };
	Register({t_csz("AND"), EOpCode::AND, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnAnd, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnOr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::OrB),  FuncCmdExec(&CBasicCommands::OrW),
		FuncCmdExec(&CBasicCommands::OrDW), FuncCmdExec(&CBasicCommands::OrQW) };
	Register({t_csz("OR"), EOpCode::OR, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnOr, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnXor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::XorB),  FuncCmdExec(&CBasicCommands::XorW),
		FuncCmdExec(&CBasicCommands::XorDW), FuncCmdExec(&CBasicCommands::XorQW) };
	Register({t_csz("XOR"), EOpCode::XOR, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnXor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNand[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NandB),  FuncCmdExec(&CBasicCommands::NandW),
		FuncCmdExec(&CBasicCommands::NandDW), FuncCmdExec(&CBasicCommands::NandQW) };
	Register({t_csz("NAND"), EOpCode::NAND, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnNand, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NorB),  FuncCmdExec(&CBasicCommands::NorW),
		FuncCmdExec(&CBasicCommands::NorDW), FuncCmdExec(&CBasicCommands::NorQW) };
	Register({t_csz("NOR"), EOpCode::NOR, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnNor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNot[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NotB),  FuncCmdExec(&CBasicCommands::NotW),
		FuncCmdExec(&CBasicCommands::NotDW), FuncCmdExec(&CBasicCommands::NotQW) };
	Register({t_csz("NOT"), EOpCode::NOT, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnNot, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnShl[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::ShlB),  FuncCmdExec(&CBasicCommands::ShlW),
		FuncCmdExec(&CBasicCommands::ShlDW), FuncCmdExec(&CBasicCommands::ShlQW) };
	Register({t_csz("SHL"), EOpCode::SHL, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnShl, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnShr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::ShrB),  FuncCmdExec(&CBasicCommands::ShrW),
		FuncCmdExec(&CBasicCommands::ShrDW), FuncCmdExec(&CBasicCommands::ShrQW) };
	Register({t_csz("SHR"), EOpCode::SHR, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnShr, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRol[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RolB),  FuncCmdExec(&CBasicCommands::RolW),
		FuncCmdExec(&CBasicCommands::RolDW), FuncCmdExec(&CBasicCommands::RolQW) };
	Register({t_csz("ROL"), EOpCode::ROL, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRol, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RorB),  FuncCmdExec(&CBasicCommands::RorW),
		FuncCmdExec(&CBasicCommands::RorDW), FuncCmdExec(&CBasicCommands::RorQW) };
	Register({t_csz("ROR"), EOpCode::ROR, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSal[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SalB),  FuncCmdExec(&CBasicCommands::SalW),
		FuncCmdExec(&CBasicCommands::SalDW), FuncCmdExec(&CBasicCommands::SalQW) };
	Register({t_csz("SAL"), EOpCode::SAL, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnSal, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSar[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SarB),  FuncCmdExec(&CBasicCommands::SarW),
		FuncCmdExec(&CBasicCommands::SarDW), FuncCmdExec(&CBasicCommands::SarQW) };
	Register({t_csz("SAR"), EOpCode::SAR, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnSar, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRcl[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RclB),  FuncCmdExec(&CBasicCommands::RclW),
		FuncCmdExec(&CBasicCommands::RclDW), FuncCmdExec(&CBasicCommands::RclQW) };
	Register({t_csz("RCL"), EOpCode::RCL, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRcl, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRcr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RcrB),  FuncCmdExec(&CBasicCommands::RcrW),
		FuncCmdExec(&CBasicCommands::RcrDW), FuncCmdExec(&CBasicCommands::RcrQW) };
	Register({t_csz("RCR"), EOpCode::RCR, EOprType::GR, EOprType::GRIMV, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRcr, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnAdd[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AddB),  FuncCmdExec(&CBasicCommands::AddW),
		FuncCmdExec(&CBasicCommands::AddDW), FuncCmdExec(&CBasicCommands::AddQW) };
	Register({t_csz("ADD"), EOpCode::ADD, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnAdd, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSub[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SubB),  FuncCmdExec(&CBasicCommands::SubW),
		FuncCmdExec(&CBasicCommands::SubDW), FuncCmdExec(&CBasicCommands::SubQW) };
	Register({t_csz("SUB"), EOpCode::SUB, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnSub, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnMul[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::MulB),  FuncCmdExec(&CBasicCommands::MulW),
		FuncCmdExec(&CBasicCommands::MulDW), FuncCmdExec(&CBasicCommands::MulQW) };
	Register({t_csz("MUL"), EOpCode::MUL, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnMul, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnDiv[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::DivB),  FuncCmdExec(&CBasicCommands::DivW),
		FuncCmdExec(&CBasicCommands::DivDW), FuncCmdExec(&CBasicCommands::DivQW) };
	Register({t_csz("DIV"), EOpCode::DIV, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnDiv, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnAdc[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AdcB),  FuncCmdExec(&CBasicCommands::AdcW),
		FuncCmdExec(&CBasicCommands::AdcDW), FuncCmdExec(&CBasicCommands::AdcQW) };
	Register({t_csz("ADC"), EOpCode::ADC, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnAdc, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSbb[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SbbB),  FuncCmdExec(&CBasicCommands::SbbW),
		FuncCmdExec(&CBasicCommands::SbbDW), FuncCmdExec(&CBasicCommands::SbbQW) };
	Register({t_csz("SBB"), EOpCode::SBB, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnSbb, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnIMul[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::IMulB),  FuncCmdExec(&CBasicCommands::IMulW),
		FuncCmdExec(&CBasicCommands::IMulDW), FuncCmdExec(&CBasicCommands::IMulQW) };
	Register({t_csz("IMUL"), EOpCode::IMUL, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnIMul, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnIDiv[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::IDivB),  FuncCmdExec(&CBasicCommands::IDivW),
		FuncCmdExec(&CBasicCommands::IDivDW), FuncCmdExec(&CBasicCommands::IDivQW) };
	Register({t_csz("IDIV"), EOpCode::IDIV, EOprType::GR, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnIDiv, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNeg[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NegB),  FuncCmdExec(&CBasicCommands::NegW),
		FuncCmdExec(&CBasicCommands::NegDW), FuncCmdExec(&CBasicCommands::NegQW) };
	Register({t_csz("NEG"), EOpCode::NEG, EOprType::GR, SCommandMetaInfo::HasOprSize},
			 apfnNeg, FuncCmdDisasm(&CBasicCommands::DisAsm));
	
	FuncCmdExec apfnSexd[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Cast<int8>),  FuncCmdExec(&CBasicCommands::Cast<int16>),
		FuncCmdExec(&CBasicCommands::Cast<int32>), FuncCmdExec(&CBasicCommands::Cast<int64>) };
	Register({t_csz("CAST"), EOpCode::CAST, EOprType::GR, SCommandMetaInfo::HasOprSize | SCommandMetaInfo::CustomExtension},
			 apfnSexd, FuncCmdDisasm(&CBasicCommands::dasmCast));
}

CBasicCommands::~CBasicCommands() = default;

//
// Address increment/decrement
//
void CBasicCommands::Inc(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pOffset = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16 uFlags = fnADD32(pTarget, pOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}

void CBasicCommands::Dec(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pOffset = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16 uFlags = fnSUB32(pTarget, pOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}

void CBasicCommands::Inc2(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.tOpr[EOprIdx::First].ptr<t_address>();
	uint32 nOffset = static_cast<uint32>(*tCtxt.tOpr[EOprIdx::Second].pu16);
	uint16 uFlags = fnADD32(pTarget, &nOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}

void CBasicCommands::Dec2(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.tOpr[EOprIdx::First].ptr<t_address>();
	uint32 nOffset = static_cast<uint32>(*tCtxt.tOpr[EOprIdx::Second].pu16);
	uint16 uFlags = fnSUB32(pTarget, &nOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}


//
// Assignemnt instuctions
//
void CBasicCommands::AssignA0(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].ptr<t_address>() = 0;
}

void CBasicCommands::AssignA4(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].ptr<t_address>() = static_cast<t_address>(*tCtxt.tOpr[EOprIdx::Second].pu32);
}

void CBasicCommands::AssignR1(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].pu8 = *tCtxt.tOpr[EOprIdx::Second].pu8;
}

void CBasicCommands::AssignR2(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].pu16 = *tCtxt.tOpr[EOprIdx::Second].pu16;
}

void CBasicCommands::AssignR4(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].pu32 = *tCtxt.tOpr[EOprIdx::Second].pu32;
}

void CBasicCommands::AssignR8(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].pu64 = *tCtxt.tOpr[EOprIdx::Second].pu64;
}

//
// Register manipulation instructions
//
template <typename TOperandType>
void CBasicCommands::Move(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>() = *tCtxt.tOpr[EOprIdx::Second].ptr<TOperandType>();
}

template <typename TOperandType>
void CBasicCommands::Swap(SCommandContext& tCtxt)
{
	TOperandType nTmp = *tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>();
	*tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>() = *tCtxt.tOpr[EOprIdx::Second].ptr<TOperandType>();
	*tCtxt.tOpr[EOprIdx::Second].ptr<TOperandType>() = nTmp;
}


//
// Comparison instructions
//

// TEST
void CBasicCommands::TestB(SCommandContext& tCtxt)
{
	uint8* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnTEST8(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestW(SCommandContext& tCtxt)
{
	uint16* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnTEST16(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestDW(SCommandContext& tCtxt)
{
	uint32* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnTEST32(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestQW(SCommandContext& tCtxt)
{
	uint64* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnTEST64(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// CMP
void CBasicCommands::CmpB(SCommandContext& tCtxt)
{
	uint8* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnCMP8(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpW(SCommandContext& tCtxt)
{
	uint16* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnCMP16(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpDW(SCommandContext& tCtxt)
{
	uint32* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnCMP32(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpQW(SCommandContext& tCtxt)
{
	uint64* pSrc1 = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc2 = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnCMP64(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

template <typename TOperandType>
void CBasicCommands::Set(SCommandContext& tCtxt)
{
	*tCtxt.tOpr[EOprIdx::First].ptr<TOperandType>() =
		static_cast<TOperandType>(tCtxt.tCPUState.oFlags.isCC(tCtxt.tInfo.eCdtnCode));
}


//
// Logical Instructions
//

// AND
void CBasicCommands::AndB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnAND8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnAND16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnAND32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnAND64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// OR
void CBasicCommands::OrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// XOR
void CBasicCommands::XorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnXOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnXOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnXOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnXOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NAND
void CBasicCommands::NandB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnNAND8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnNAND16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnNAND32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnNAND64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NOR
void CBasicCommands::NorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnNOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnNOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnNOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnNOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NOT
void CBasicCommands::NotB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint16 nFlags = fnNOT8(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16  nFlags = fnNOT16(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint16  nFlags = fnNOT32(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint16  nFlags = fnNOT64(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Shift instructions
//

// SHL
void CBasicCommands::ShlB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSHL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SHR
void CBasicCommands::ShrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSHR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSHR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// ROL
void CBasicCommands::RolB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnROL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// ROR
void CBasicCommands::RorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnROR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnROR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SAL
void CBasicCommands::SalB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSAL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SAR
void CBasicCommands::SarB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSAR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnSAR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// RCL
void CBasicCommands::RclB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnRCL8(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCL16(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCL32(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCL64(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// RCR
void CBasicCommands::RcrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnRCR8(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCR16(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCR32(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint8*  pCnt = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16  nFlags = fnRCR64(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Unsigned integral arithmetic instructions
//

// ADD
void CBasicCommands::AddB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnADD8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnADD16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnADD32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnADD64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SUB
void CBasicCommands::SubB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSUB8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnSUB16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnSUB32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnSUB64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// MUL
void CBasicCommands::MulB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnMUL8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnMUL16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnMUL32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnMUL64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// DIV
void CBasicCommands::DivB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnDIV8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnDIV16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnDIV32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnDIV64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Signed integral arithmetic instructions
//

// ADC
void CBasicCommands::AdcB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnADC8(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnADC16(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnADC32(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnADC64(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SBB
void CBasicCommands::SbbB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnSBB8(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnSBB16(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnSBB32(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnSBB64(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// IMUL
void CBasicCommands::IMulB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnIMUL8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnIMUL16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnIMUL32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnIMUL64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// IDIV
void CBasicCommands::IDivB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint8* pSrc = tCtxt.tOpr[EOprIdx::Second].pu8;
	uint16 nFlags = fnIDIV8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16* pSrc = tCtxt.tOpr[EOprIdx::Second].pu16;
	uint16  nFlags = fnIDIV16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint32* pSrc = tCtxt.tOpr[EOprIdx::Second].pu32;
	uint16  nFlags = fnIDIV32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint64* pSrc = tCtxt.tOpr[EOprIdx::Second].pu64;
	uint16  nFlags = fnIDIV64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NEG
void CBasicCommands::NegB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.tOpr[EOprIdx::First].pu8;
	uint16 nFlags = fnNEG8(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.tOpr[EOprIdx::First].pu16;
	uint16  nFlags = fnNEG16(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.tOpr[EOprIdx::First].pu32;
	uint16  nFlags = fnNEG32(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.tOpr[EOprIdx::First].pu64;
	uint16  nFlags = fnNEG64(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// CAST
template <typename TTargetType>
void CBasicCommands::Cast(SCommandContext& tCtxt)
{
	EOprSize eOprSzSrc = tCtxt.tInfo.eOprSize;
	EOprSize eOprSzTrgt = EOprSize(uchar(tCtxt.tInfo.nExtension & 0xC0) >> 6);

	if (eOprSzTrgt > eOprSzSrc)
	{
		// Cast from lower to higher type, no loss of data
		switch (eOprSzTrgt)
		{
		case EOprSize::Word:
			*tCtxt.tOpr[EOprIdx::First].pi16 = static_cast<int16>(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		case EOprSize::DWord:
			*tCtxt.tOpr[EOprIdx::First].pi32 = static_cast<int32>(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		case EOprSize::QWord:
			*tCtxt.tOpr[EOprIdx::First].pi64 = static_cast<int64>(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		default:
			VASM_THROW_ERROR(t_csz("CPU: Invalid operand size for the CAST instruction"));
		}

		tCtxt.tCPUState.oFlags.clearCarry();
		tCtxt.tCPUState.oFlags.clearOverflow();
		tCtxt.tCPUState.oFlags.setZero(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() == 0);
		tCtxt.tCPUState.oFlags.setSign(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() < 0);
	}
	else if (eOprSzTrgt < eOprSzSrc)
	{
		// Cast from higher to lower type, possible loss of data
		bool bLoss = true;
		switch (eOprSzTrgt)
		{
		case EOprSize::Byte:
			bLoss = (static_cast<TTargetType>(*tCtxt.tOpr[EOprIdx::First].pi8) != *tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		case EOprSize::Word:
			bLoss = (static_cast<TTargetType>(*tCtxt.tOpr[EOprIdx::First].pi16) != *tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		case EOprSize::DWord:
			bLoss = (static_cast<TTargetType>(*tCtxt.tOpr[EOprIdx::First].pi32) != *tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>());
			break;
		default:
			VASM_THROW_ERROR(t_csz("CPU: Invalid operand size for the CAST instruction"));
		}

		tCtxt.tCPUState.oFlags.setCarry(bLoss);
		tCtxt.tCPUState.oFlags.setOverflow(bLoss);
		tCtxt.tCPUState.oFlags.setZero(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() == 0);
		tCtxt.tCPUState.oFlags.setSign(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() < 0);
	}
	else
	{
		tCtxt.tCPUState.oFlags.clearCarry();
		tCtxt.tCPUState.oFlags.clearOverflow();
		tCtxt.tCPUState.oFlags.setZero(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() == 0);
		tCtxt.tCPUState.oFlags.setSign(*tCtxt.tOpr[EOprIdx::First].ptr<TTargetType>() < 0);
	}
}


//
//	Disassembly functions
//
t_string CBasicCommands::dasmAssignA0(SCommandInfo const& tCmd, bool)
{
	// Command name
	t_string sCmd = DisAsmCmd(tCmd);
	// Arguments
	sCmd += base::toStr(t_csz(" A%1, null"), (uint) tCmd.nRegIdx[EOprIdx::First]);
	//
	return std::move(sCmd);
}

t_string CBasicCommands::dasmAssignA4(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd;
	core::IAddressRecoveryPtr pAddrRec = GetAddressRecovery();
	if (pAddrRec == nullptr)
		sCmd = Base::DisAsm(tCmd, bHexadecimal);
	else
	{
		// Get symbol from address
		t_string sSymbol = pAddrRec->ToSymbol(tCmd.u32Imv);
		// Command name
		sCmd = DisAsmCmd(tCmd);
		// Arguments
		sCmd += base::toStr(t_csz(" A%1, %2"), (uint) tCmd.nRegIdx[EOprIdx::First], sSymbol);
	}
	return std::move(sCmd);
}

t_string CBasicCommands::dasmAssignRn(SCommandInfo const& tCmd, bool bHexadecimal)
{
	t_string sCmd = DisAsmCmd(tCmd);
	sCmd += DisAsmOprSize(tCmd, true);
	sCmd += DisAsmArgs(tCmd, bHexadecimal);
	return std::move(sCmd);
}

t_string CBasicCommands::dasmCast(SCommandInfo const& tCmd, bool bHexadecimal)
{
	// Command name
	t_string sCmd = DisAsmCmd(tCmd);

	EOprSize eOprSzSrc = tCmd.eOprSize;
	EOprSize eOprSzTrgt = EOprSize(uchar(tCmd.nExtension & 0xC0) >> 6);

	// Source operand size
	switch (eOprSzSrc)
	{
	case EOprSize::Byte:
		sCmd += t_csz(" B");
		break;
	case EOprSize::Word:
		sCmd += t_csz(" W");
		break;
	case EOprSize::DWord:
		sCmd += t_csz(" DW");
		break;
	case EOprSize::QWord:
		sCmd += t_csz(" QW");
		break;
	default:
		VASM_ASSERT(false);
		break;
	}

	// Target operand size
	switch (eOprSzTrgt)
	{
	case EOprSize::Byte:
		sCmd += t_csz(" B");
		break;
	case EOprSize::Word:
		sCmd += t_csz(" W");
		break;
	case EOprSize::DWord:
		sCmd += t_csz(" DW");
		break;
	case EOprSize::QWord:
		sCmd += t_csz(" QW");
		break;
	default:
		VASM_ASSERT(false);
		break;
	}

	// Argument
	sCmd += DisAsmArgs(tCmd, bHexadecimal);

	return std::move(sCmd);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

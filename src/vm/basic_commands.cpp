//
//	Component
//
#define __COMPONENT__ "VM"

//
//	Includes
//
#include "basic_commands.h"
#include <base_exception.h>
#include <base_utility.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Assembly operations forward declarations
//	All below functions return VASM 16Bit Flags
//

// Comparison operations
extern "C" uint16_t __stdcall fnTEST8 (uint8_t*  pLeft, uint8_t*  pRight);
extern "C" uint16_t __stdcall fnTEST16(uint16_t* pLeft, uint16_t* pRight);
extern "C" uint16_t __stdcall fnTEST32(uint32_t* pLeft, uint32_t* pRight);
extern "C" uint16_t __stdcall fnTEST64(uint64_t* pLeft, uint64_t* pRight);

extern "C" uint16_t __stdcall fnCMP8 (uint8_t*  pLeft, uint8_t*  pRight);
extern "C" uint16_t __stdcall fnCMP16(uint16_t* pLeft, uint16_t* pRight);
extern "C" uint16_t __stdcall fnCMP32(uint32_t* pLeft, uint32_t* pRight);
extern "C" uint16_t __stdcall fnCMP64(uint64_t* pLeft, uint64_t* pRight);

// Logical operations
extern "C" uint16_t __stdcall fnAND8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnXOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnXOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnXOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnXOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnNAND8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnNAND16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnNAND32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnNAND64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnNOR8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnNOR16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnNOR32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnNOR64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnNOT8 (uint8_t*  pOperand);
extern "C" uint16_t __stdcall fnNOT16(uint16_t* pOperand);
extern "C" uint16_t __stdcall fnNOT32(uint32_t* pOperand);
extern "C" uint16_t __stdcall fnNOT64(uint64_t* pOperand);

// Shift & Rotate operations
extern "C" uint16_t __stdcall fnSHL8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHL16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHL32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHL64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnSHR8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHR16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHR32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSHR64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnSAL8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAL16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAL32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAL64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnSAR8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAR16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAR32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnSAR64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnROL8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROL16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROL32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROL64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnROR8 (uint8_t*  pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROR16(uint16_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROR32(uint32_t* pOperand, uint8_t* pCount);
extern "C" uint16_t __stdcall fnROR64(uint64_t* pOperand, uint8_t* pCount);

extern "C" uint16_t __stdcall fnRCL8 (uint8_t*  pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL16(uint16_t* pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL32(uint32_t* pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCL64(uint64_t* pOperand, uint8_t* pCount, bool bCarryFlag);

extern "C" uint16_t __stdcall fnRCR8 (uint8_t*  pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR16(uint16_t* pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR32(uint32_t* pOperand, uint8_t* pCount, bool bCarryFlag);
extern "C" uint16_t __stdcall fnRCR64(uint64_t* pOperand, uint8_t* pCount, bool bCarryFlag);

// Integral arithmetic operations
extern "C" uint16_t __stdcall fnADD8 (uint8_t*  pLeft, uint8_t*  pRight,  uint8_t* pResult);
extern "C" uint16_t __stdcall fnADD16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnADD32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnADD64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnADC8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnADC64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult, bool bCarryFlag);

extern "C" uint16_t __stdcall fnSUB8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult);
extern "C" uint16_t __stdcall fnSUB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult);
extern "C" uint16_t __stdcall fnSUB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult);
extern "C" uint16_t __stdcall fnSUB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult);

extern "C" uint16_t __stdcall fnSBB8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pResult, bool bCarryFlag);
extern "C" uint16_t __stdcall fnSBB64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pResult, bool bCarryFlag);

extern "C" uint16_t __stdcall fnNEG8 (uint8_t*  pOperand);
extern "C" uint16_t __stdcall fnNEG16(uint16_t* pOperand);
extern "C" uint16_t __stdcall fnNEG32(uint32_t* pOperand);
extern "C" uint16_t __stdcall fnNEG64(uint64_t* pOperand);

// Multiplication and Division
extern "C" uint16_t __stdcall fnMUL8 (uint8_t*  pLeft, uint8_t*  pRight, uint16_t* pProduct);
extern "C" uint16_t __stdcall fnMUL16(uint16_t* pLeft, uint16_t* pRight, uint32_t* pProduct);
extern "C" uint16_t __stdcall fnMUL32(uint32_t* pLeft, uint32_t* pRight, uint64_t* pProduct);
extern "C" uint16_t __stdcall fnMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pProduct);

extern "C" uint16_t __stdcall fnDIV8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pQuotient, uint8_t*  pRemainder);
extern "C" uint16_t __stdcall fnDIV16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pQuotient, uint16_t* pReminder);
extern "C" uint16_t __stdcall fnDIV32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pQuotient, uint32_t* pReminder);
extern "C" uint16_t __stdcall fnDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pQuotient, uint64_t* pReminder);

extern "C" uint16_t __stdcall fnIMUL8 (uint8_t*  pLeft, uint8_t*  pRight, uint16_t* pProduct);
extern "C" uint16_t __stdcall fnIMUL16(uint16_t* pLeft, uint16_t* pRight, uint32_t* pProduct);
extern "C" uint16_t __stdcall fnIMUL32(uint32_t* pLeft, uint32_t* pRight, uint64_t* pProduct);
extern "C" uint16_t __stdcall fnIMUL64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pProduct);

extern "C" uint16_t __stdcall fnIDIV8 (uint8_t*  pLeft, uint8_t*  pRight, uint8_t*  pQuotient, uint8_t*  pRemainder);
extern "C" uint16_t __stdcall fnIDIV16(uint16_t* pLeft, uint16_t* pRight, uint16_t* pQuotient, uint16_t* pRemainder);
extern "C" uint16_t __stdcall fnIDIV32(uint32_t* pLeft, uint32_t* pRight, uint32_t* pQuotient, uint32_t* pRemainder);
extern "C" uint16_t __stdcall fnIDIV64(uint64_t* pLeft, uint64_t* pRight, uint64_t* pQuotient, uint64_t* pRemainder);
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
	Register({t_csz("MOVI"), EOpCode::MOVIA, EOprType::Reg, EOprType::Imv, EImvType::Num32, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::MovIA), FuncCmdDisasm(&CBasicCommands::dasmMovIA));
	Register({t_csz("MOVI"), EOpCode::MOVI1, EOprType::Reg, EOprType::Imv, EImvType::SNum8, SCommandMetaInfo::FixedOprSizeByte},
			 FuncCmdExec(&CBasicCommands::MovI1), FuncCmdDisasm(&CBasicCommands::dasmMovIR));
	Register({t_csz("MOVI"), EOpCode::MOVI2, EOprType::Reg, EOprType::Imv, EImvType::SNum16, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CBasicCommands::MovI2), FuncCmdDisasm(&CBasicCommands::dasmMovIR));
	Register({t_csz("MOVI"), EOpCode::MOVI4, EOprType::Reg, EOprType::Imv, EImvType::SNum32, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::MovI4), FuncCmdDisasm(&CBasicCommands::dasmMovIR));
	//Register({t_csz("MOVI"), EOpCode::MOVI8, EOprType::Reg, EOprType::Imv, EImvType::SNum64, SCommandMetaInfo::FixedOprSizeQWord},
	//		 FuncCmdExec(&CBasicCommands::MovI8), FuncCmdDisasm(&CBasicCommands::dasmMovIR));

	FuncCmdExec apfnMove[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Move<uint8>),  FuncCmdExec(&CBasicCommands::Move<uint16>), 
		FuncCmdExec(&CBasicCommands::Move<uint32>), FuncCmdExec(&CBasicCommands::Move<uint64>) };
	Register({t_csz("MOVE"), t_csz("MOV"), EOpCode::MOVE, EOprType::Reg, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnMove, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSwap[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Swap<uint8>),  FuncCmdExec(&CBasicCommands::Swap<uint16>),
		FuncCmdExec(&CBasicCommands::Swap<uint32>), FuncCmdExec(&CBasicCommands::Swap<uint64>) };
	Register({t_csz("SWAP"), t_csz("SWP"), EOpCode::SWAP, EOprType::Reg, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnSwap, FuncCmdDisasm(&CBasicCommands::DisAsm));

	Register({t_csz("GETF"), EOpCode::GETF, EOprType::Reg, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CBasicCommands::GetF), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("SETF"), EOpCode::SETF, EOprType::Reg, SCommandMetaInfo::FixedOprSizeWord},
			 FuncCmdExec(&CBasicCommands::SetF), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("CLC"), EOpCode::CLC},
			 FuncCmdExec(&CBasicCommands::CLC), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("STC"), EOpCode::STC},
			 FuncCmdExec(&CBasicCommands::STC), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("CMC"), EOpCode::CMC},
			 FuncCmdExec(&CBasicCommands::CMC), FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSet[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Set<uint8>),  FuncCmdExec(&CBasicCommands::Set<uint16>),
		FuncCmdExec(&CBasicCommands::Set<uint32>), FuncCmdExec(&CBasicCommands::Set<uint64>) };
	Register({t_csz("SET"), t_csz("ST"), EOpCode::SET, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode | SCommandMetaInfo::SkipCdtnCheck},
			 apfnSet, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnTest[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::TestB),  FuncCmdExec(&CBasicCommands::TestW),
		FuncCmdExec(&CBasicCommands::TestDW), FuncCmdExec(&CBasicCommands::TestQW) };
	Register({t_csz("TEST"), EOpCode::TEST, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnTest, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnCmp[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::CmpB),  FuncCmdExec(&CBasicCommands::CmpW),
		FuncCmdExec(&CBasicCommands::CmpDW), FuncCmdExec(&CBasicCommands::CmpQW) };
	Register({t_csz("CMP"), EOpCode::CMP, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnCmp, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnAnd[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AndB),  FuncCmdExec(&CBasicCommands::AndW),
		FuncCmdExec(&CBasicCommands::AndDW), FuncCmdExec(&CBasicCommands::AndQW) };
	Register({t_csz("AND"), EOpCode::AND, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnAnd, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnOr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::OrB),  FuncCmdExec(&CBasicCommands::OrW),
		FuncCmdExec(&CBasicCommands::OrDW), FuncCmdExec(&CBasicCommands::OrQW) };
	Register({t_csz("OR"), EOpCode::OR, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnOr, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnXor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::XorB),  FuncCmdExec(&CBasicCommands::XorW),
		FuncCmdExec(&CBasicCommands::XorDW), FuncCmdExec(&CBasicCommands::XorQW) };
	Register({t_csz("XOR"), EOpCode::XOR, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnXor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNand[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NandB),  FuncCmdExec(&CBasicCommands::NandW),
		FuncCmdExec(&CBasicCommands::NandDW), FuncCmdExec(&CBasicCommands::NandQW) };
	Register({t_csz("NAND"), EOpCode::NAND, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnNand, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NorB),  FuncCmdExec(&CBasicCommands::NorW),
		FuncCmdExec(&CBasicCommands::NorDW), FuncCmdExec(&CBasicCommands::NorQW) };
	Register({t_csz("NOR"), EOpCode::NOR, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnNor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNot[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NotB),  FuncCmdExec(&CBasicCommands::NotW),
		FuncCmdExec(&CBasicCommands::NotDW), FuncCmdExec(&CBasicCommands::NotQW) };
	Register({t_csz("NOT"), EOpCode::NOT, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnNot, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnShl[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::ShlB),  FuncCmdExec(&CBasicCommands::ShlW),
		FuncCmdExec(&CBasicCommands::ShlDW), FuncCmdExec(&CBasicCommands::ShlQW) };
	Register({t_csz("SHL"), EOpCode::SHL, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnShl, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnShr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::ShrB),  FuncCmdExec(&CBasicCommands::ShrW),
		FuncCmdExec(&CBasicCommands::ShrDW), FuncCmdExec(&CBasicCommands::ShrQW) };
	Register({t_csz("SHR"), EOpCode::SHR, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnShr, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSal[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SalB),  FuncCmdExec(&CBasicCommands::SalW),
		FuncCmdExec(&CBasicCommands::SalDW), FuncCmdExec(&CBasicCommands::SalQW) };
	Register({t_csz("SAL"), EOpCode::SAL, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnSal, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSar[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SarB),  FuncCmdExec(&CBasicCommands::SarW),
		FuncCmdExec(&CBasicCommands::SarDW), FuncCmdExec(&CBasicCommands::SarQW) };
	Register({t_csz("SAR"), EOpCode::SAR, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnSar, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRol[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RolB),  FuncCmdExec(&CBasicCommands::RolW),
		FuncCmdExec(&CBasicCommands::RolDW), FuncCmdExec(&CBasicCommands::RolQW) };
	Register({t_csz("ROL"), EOpCode::ROL, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRol, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRor[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RorB),  FuncCmdExec(&CBasicCommands::RorW),
		FuncCmdExec(&CBasicCommands::RorDW), FuncCmdExec(&CBasicCommands::RorQW) };
	Register({t_csz("ROR"), EOpCode::ROR, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRor, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRcl[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RclB),  FuncCmdExec(&CBasicCommands::RclW),
		FuncCmdExec(&CBasicCommands::RclDW), FuncCmdExec(&CBasicCommands::RclQW) };
	Register({t_csz("RCL"), EOpCode::RCL, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRcl, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnRcr[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::RcrB),  FuncCmdExec(&CBasicCommands::RcrW),
		FuncCmdExec(&CBasicCommands::RcrDW), FuncCmdExec(&CBasicCommands::RcrQW) };
	Register({t_csz("RCR"), EOpCode::RCR, EOprType::Reg, EOprType::RegImv, EImvType::Count,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasOprSwitch | SCommandMetaInfo::SingularOperandSize},
			 apfnRcr, FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnAdd[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AddB),  FuncCmdExec(&CBasicCommands::AddW),
		FuncCmdExec(&CBasicCommands::AddDW), FuncCmdExec(&CBasicCommands::AddQW) };
	Register({t_csz("ADD"), EOpCode::ADD, EOprType::Reg, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnAdd, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnAdc[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::AdcB),  FuncCmdExec(&CBasicCommands::AdcW),
		FuncCmdExec(&CBasicCommands::AdcDW), FuncCmdExec(&CBasicCommands::AdcQW) };
	Register({t_csz("ADC"), EOpCode::ADC, EOprType::Reg, EOprType::Reg, 
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnAdc, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSub[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SubB),  FuncCmdExec(&CBasicCommands::SubW),
		FuncCmdExec(&CBasicCommands::SubDW), FuncCmdExec(&CBasicCommands::SubQW) };
	Register({t_csz("SUB"), EOpCode::SUB, EOprType::Reg, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnSub, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnSbb[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::SbbB),  FuncCmdExec(&CBasicCommands::SbbW),
		FuncCmdExec(&CBasicCommands::SbbDW), FuncCmdExec(&CBasicCommands::SbbQW) };
	Register({t_csz("SBB"), EOpCode::SBB, EOprType::Reg, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnSbb, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnNeg[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::NegB),  FuncCmdExec(&CBasicCommands::NegW),
		FuncCmdExec(&CBasicCommands::NegDW), FuncCmdExec(&CBasicCommands::NegQW) };
	Register({t_csz("NEG"), EOpCode::NEG, EOprType::Reg,
			 SCommandMetaInfo::HasOprSize | SCommandMetaInfo::HasCndtnCode},
			 apfnNeg, FuncCmdDisasm(&CBasicCommands::DisAsm));

	Register({t_csz("INC"), EOpCode::INC, EOprType::Reg, EOprType::Imv, EImvType::Num16, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::Inc), FuncCmdDisasm(&CBasicCommands::DisAsm));
	Register({t_csz("DEC"), EOpCode::DEC, EOprType::Reg, EOprType::Imv, EImvType::Num16, SCommandMetaInfo::FixedOprSizeDWord},
			 FuncCmdExec(&CBasicCommands::Dec), FuncCmdDisasm(&CBasicCommands::DisAsm));

	FuncCmdExec apfnMul[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::MulB),  FuncCmdExec(&CBasicCommands::MulW),
		FuncCmdExec(&CBasicCommands::MulDW), FuncCmdExec(&CBasicCommands::MulQW) };
	Register({t_csz("MUL"), EOpCode::MUL, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnMul, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnDiv[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::DivB),  FuncCmdExec(&CBasicCommands::DivW),
		FuncCmdExec(&CBasicCommands::DivDW), FuncCmdExec(&CBasicCommands::DivQW) };
	Register({t_csz("DIV"), EOpCode::DIV, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnDiv, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnIMul[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::IMulB),  FuncCmdExec(&CBasicCommands::IMulW),
		FuncCmdExec(&CBasicCommands::IMulDW), FuncCmdExec(&CBasicCommands::IMulQW) };
	Register({t_csz("IMUL"), EOpCode::IMUL, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnIMul, FuncCmdDisasm(&CBasicCommands::DisAsm));
	FuncCmdExec apfnIDiv[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::IDivB),  FuncCmdExec(&CBasicCommands::IDivW),
		FuncCmdExec(&CBasicCommands::IDivDW), FuncCmdExec(&CBasicCommands::IDivQW) };
	Register({t_csz("IDIV"), EOpCode::IDIV, EOprType::Reg, EOprType::Reg, SCommandMetaInfo::HasOprSize},
			 apfnIDiv, FuncCmdDisasm(&CBasicCommands::DisAsm));
	
	FuncCmdExec apfnCast[int(EOprSize::Count)] = {
		FuncCmdExec(&CBasicCommands::Cast<int8>),  FuncCmdExec(&CBasicCommands::Cast<int16>),
		FuncCmdExec(&CBasicCommands::Cast<int32>), FuncCmdExec(&CBasicCommands::Cast<int64>) };
	Register({t_csz("CAST"), EOpCode::CAST, EOprType::Reg, SCommandMetaInfo::HasOprSize | SCommandMetaInfo::CustomExtension},
			 apfnCast, FuncCmdDisasm(&CBasicCommands::dasmCast));
}

CBasicCommands::~CBasicCommands() = default;

//
// Assignemnt instuctions
//
void CBasicCommands::MovIA(SCommandContext& tCtxt)
{
	*tCtxt.operand<t_address>(EOprIdx::First) = static_cast<t_address>(*tCtxt.operand<uint32>(EOprIdx::Second));
}

void CBasicCommands::MovI1(SCommandContext& tCtxt)
{
	*tCtxt.operand<uint8>(EOprIdx::First) = *tCtxt.operand<uint8>(EOprIdx::Second);
}

void CBasicCommands::MovI2(SCommandContext& tCtxt)
{
	*tCtxt.operand<uint16>(EOprIdx::First) = *tCtxt.operand<uint16>(EOprIdx::Second);
}

void CBasicCommands::MovI4(SCommandContext& tCtxt)
{
	*tCtxt.operand<uint32>(EOprIdx::First) = *tCtxt.operand<uint32>(EOprIdx::Second);
}

//void CBasicCommands::MovI8(SCommandContext& tCtxt)
//{
//	*tCtxt.operand<uint64>(EOprIdx::First) = *tCtxt.operand<uint64>(EOprIdx::Second);
//}

//
// Register manipulation instructions
//
template <typename TOperandType>
void CBasicCommands::Move(SCommandContext& tCtxt)
{
	*tCtxt.operand<TOperandType>(EOprIdx::First) = *tCtxt.operand<TOperandType>(EOprIdx::Second);
}

template <typename TOperandType>
void CBasicCommands::Swap(SCommandContext& tCtxt)
{
	TOperandType nTmp = *tCtxt.operand<TOperandType>(EOprIdx::First);
	*tCtxt.operand<TOperandType>(EOprIdx::First) = *tCtxt.operand<TOperandType>(EOprIdx::Second);
	*tCtxt.operand<TOperandType>(EOprIdx::Second) = nTmp;
}

//
//	Flags manipulation instructions
//
void CBasicCommands::GetF(SCommandContext& tCtxt)
{
	*tCtxt.operand<uint16>(EOprIdx::First) = tCtxt.tCPUState.oFlags.getFlags();
}

void CBasicCommands::SetF(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.oFlags.setFlags(*tCtxt.operand<uint16>(EOprIdx::First));
}

void CBasicCommands::CLC(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.oFlags.clearCarry();
}

void CBasicCommands::STC(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.oFlags.setCarry(true);
}

void CBasicCommands::CMC(SCommandContext& tCtxt)
{
	tCtxt.tCPUState.oFlags.setCarry(!tCtxt.tCPUState.oFlags.getCarry());
}

template <typename TOperandType>
void CBasicCommands::Set(SCommandContext& tCtxt)
{
	*tCtxt.operand<TOperandType>(EOprIdx::First) =
		static_cast<TOperandType>(tCtxt.tCPUState.oFlags.isCC(tCtxt.tInfo.eCdtnCode));
}


//
// Comparison instructions
//

// TEST
void CBasicCommands::TestB(SCommandContext& tCtxt)
{
	uint8* pSrc1 = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc2 = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnTEST8(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestW(SCommandContext& tCtxt)
{
	uint16* pSrc1 = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc2 = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnTEST16(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestDW(SCommandContext& tCtxt)
{
	uint32* pSrc1 = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc2 = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnTEST32(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::TestQW(SCommandContext& tCtxt)
{
	uint64* pSrc1 = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc2 = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnTEST64(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// CMP
void CBasicCommands::CmpB(SCommandContext& tCtxt)
{
	uint8* pSrc1 = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc2 = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnCMP8(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpW(SCommandContext& tCtxt)
{
	uint16* pSrc1 = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc2 = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnCMP16(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpDW(SCommandContext& tCtxt)
{
	uint32* pSrc1 = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc2 = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnCMP32(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::CmpQW(SCommandContext& tCtxt)
{
	uint64* pSrc1 = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc2 = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnCMP64(pSrc1, pSrc2);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Logical Instructions
//

// AND
void CBasicCommands::AndB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnAND8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnAND16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnAND32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AndQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnAND64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// OR
void CBasicCommands::OrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::OrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// XOR
void CBasicCommands::XorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnXOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnXOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnXOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::XorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnXOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NAND
void CBasicCommands::NandB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnNAND8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnNAND16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnNAND32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NandQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnNAND64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NOR
void CBasicCommands::NorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnNOR8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnNOR16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnNOR32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnNOR64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NOT
void CBasicCommands::NotB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint16 nFlags = fnNOT8(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16  nFlags = fnNOT16(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint16  nFlags = fnNOT32(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NotQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint16  nFlags = fnNOT64(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Shift instructions
//

// SHL
void CBasicCommands::ShlB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSHL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShlQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SHR
void CBasicCommands::ShrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSHR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::ShrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSHR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SAL
void CBasicCommands::SalB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSAL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SalQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SAR
void CBasicCommands::SarB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSAR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SarQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnSAR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// ROL
void CBasicCommands::RolB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnROL8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROL16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROL32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RolQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROL64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// ROR
void CBasicCommands::RorB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnROR8(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROR16(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROR32(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RorQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnROR64(pDst, pCnt);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// RCL
void CBasicCommands::RclB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnRCL8(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCL16(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCL32(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RclQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCL64(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// RCR
void CBasicCommands::RcrB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnRCR8(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCR16(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCR32(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::RcrQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint8*  pCnt = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16  nFlags = fnRCR64(pDst, pCnt, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Integral arithmetic instructions
//

// ADD
void CBasicCommands::AddB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnADD8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnADD16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnADD32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AddQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnADD64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SUB
void CBasicCommands::SubB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSUB8(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnSUB16(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnSUB32(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SubQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnSUB64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// ADC
void CBasicCommands::AdcB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnADC8(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnADC16(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnADC32(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::AdcQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnADC64(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// SBB
void CBasicCommands::SbbB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnSBB8(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnSBB16(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnSBB32(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::SbbQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnSBB64(pDst, pSrc, pDst, tCtxt.tCPUState.oFlags.getCarry());
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// NEG
void CBasicCommands::NegB(SCommandContext& tCtxt)
{
	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint16 nFlags = fnNEG8(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegW(SCommandContext& tCtxt)
{
	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16  nFlags = fnNEG16(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegDW(SCommandContext& tCtxt)
{
	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint16  nFlags = fnNEG32(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::NegQW(SCommandContext& tCtxt)
{
	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint16  nFlags = fnNEG64(pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


//
// Address increment/decrement
//
void CBasicCommands::Inc(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.operand<t_address>(EOprIdx::First);
	uint32 nOffset = static_cast<uint32>(*tCtxt.operand<uint16>(EOprIdx::Second));
	uint16 uFlags = fnADD32(pTarget, &nOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}

void CBasicCommands::Dec(SCommandContext& tCtxt)
{
	uint32* pTarget = tCtxt.operand<t_address>(EOprIdx::First);
	uint32 nOffset = static_cast<uint32>(*tCtxt.operand<uint16>(EOprIdx::Second));
	uint16 uFlags = fnSUB32(pTarget, &nOffset, pTarget);
	tCtxt.tCPUState.oFlags.setFlags(uFlags);
}


//
// Multiplication and Division Instructions
//

// MUL
void CBasicCommands::MulB(SCommandContext& tCtxt)
{
	// For MUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of MUL command is double sized"));

	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnMUL8(pDst, pSrc, reinterpret_cast<uint16*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulW(SCommandContext& tCtxt)
{
	// For MUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of MUL command is double sized"));

	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnMUL16(pDst, pSrc, reinterpret_cast<uint32*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulDW(SCommandContext& tCtxt)
{
	// For MUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of MUL command is double sized"));

	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnMUL32(pDst, pSrc, reinterpret_cast<uint64*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::MulQW(SCommandContext& tCtxt)
{
	// For MUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of MUL command is double sized"));

	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnMUL64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// DIV
void CBasicCommands::DivB(SCommandContext& tCtxt)
{
	// For DIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of DIV command is double sized"));

	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnDIV8(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivW(SCommandContext& tCtxt)
{
	// For DIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of DIV command is double sized"));

	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnDIV16(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivDW(SCommandContext& tCtxt)
{
	// For DIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of DIV command is double sized"));

	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnDIV32(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::DivQW(SCommandContext& tCtxt)
{
	// For DIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of DIV command is double sized"));

	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnDIV64(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// IMUL
void CBasicCommands::IMulB(SCommandContext& tCtxt)
{
	// For IMUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IMUL command is double sized"));

	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnIMUL8(pDst, pSrc, reinterpret_cast<uint16*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulW(SCommandContext& tCtxt)
{
	// For IMUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IMUL command is double sized"));

	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnIMUL16(pDst, pSrc, reinterpret_cast<uint32*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulDW(SCommandContext& tCtxt)
{
	// For IMUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IMUL command is double sized"));

	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnIMUL32(pDst, pSrc, reinterpret_cast<uint64*>(pDst));
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IMulQW(SCommandContext& tCtxt)
{
	// For IMUL instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IMUL command is double sized"));

	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnIMUL64(pDst, pSrc, pDst);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// IDIV
void CBasicCommands::IDivB(SCommandContext& tCtxt)
{
	// For IDIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IDIV command is double sized"));

	uint8* pDst = tCtxt.operand<uint8>(EOprIdx::First);
	uint8* pSrc = tCtxt.operand<uint8>(EOprIdx::Second);
	uint16 nFlags = fnIDIV8(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivW(SCommandContext& tCtxt)
{
	// For IDIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IDIV command is double sized"));

	uint16* pDst = tCtxt.operand<uint16>(EOprIdx::First);
	uint16* pSrc = tCtxt.operand<uint16>(EOprIdx::Second);
	uint16  nFlags = fnIDIV16(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivDW(SCommandContext& tCtxt)
{
	// For IDIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IDIV command is double sized"));

	uint32* pDst = tCtxt.operand<uint32>(EOprIdx::First);
	uint32* pSrc = tCtxt.operand<uint32>(EOprIdx::Second);
	uint16  nFlags = fnIDIV32(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}

void CBasicCommands::IDivQW(SCommandContext& tCtxt)
{
	// For IDIV instruction first operand should be double size of the operand size
	// Multiply Reg idx with OprSize to align with OpSize
	uint nRegIdx = AlignToOperandSize(static_cast<uint>(tCtxt.tInfo.anRegIdx[0]), tCtxt.tInfo.eOprSize);
	if (nRegIdx + 2 * OperandSize(tCtxt.tInfo.eOprSize) > SCPUStateBase::eRegisterPoolSize)
		throw base::CError(t_csz("CPU: Invalid GP register index 0, first operand of IDIV command is double sized"));

	uint64* pDst = tCtxt.operand<uint64>(EOprIdx::First);
	uint64* pSrc = tCtxt.operand<uint64>(EOprIdx::Second);
	uint16  nFlags = fnIDIV64(pDst, pSrc, pDst, pDst + 1);
	tCtxt.tCPUState.oFlags.setFlags(nFlags);
}


// CAST
template <typename TTargetType>
void CBasicCommands::Cast(SCommandContext& tCtxt)
{
	EOprSize eOprSzTrgt = tCtxt.tInfo.eOprSize;
	EOprSize eOprSzSrc = EOprSize(uchar(tCtxt.tInfo.nExtension & 0xC0) >> 6);

	if (eOprSzTrgt > eOprSzSrc)
	{	// Cast Up
		switch (eOprSzSrc)
		{
		case EOprSize::Byte:
			CastUp<int8, TTargetType>(tCtxt);
			break;
		case EOprSize::Word:
			CastUp<int16, TTargetType>(tCtxt);
			break;
		case EOprSize::DWord:
			CastUp<int32, TTargetType>(tCtxt);
			break;
		case EOprSize::QWord:
			VASM_THROW_ERROR(t_csz("CPU: Invalid operand sizes for the up CAST instruction"));
			break;
		default:
			VASM_THROW_ERROR(t_csz("CPU: Invalid source operand size for the up CAST instruction"));
		}
	}
	else if (eOprSzTrgt < eOprSzSrc)
	{	// Cast Down
		switch (eOprSzSrc)
		{
		case EOprSize::Byte:
			VASM_THROW_ERROR(t_csz("CPU: Invalid operand sizes for the down CAST instruction"));
			break;
		case EOprSize::Word:
			CastDown<int16, TTargetType>(tCtxt);
			break;
		case EOprSize::DWord:
			CastDown<int32, TTargetType>(tCtxt);
			break;
		case EOprSize::QWord:
			CastDown<int64, TTargetType>(tCtxt);
			break;
		default:
			VASM_THROW_ERROR(t_csz("CPU: Invalid source operand size for the down CAST instruction"));
		}
	}
	else
	{	// Sourse and Target sizes are the same
		// Just Update CPU Flags
		tCtxt.tCPUState.oFlags.clearCarry();
		tCtxt.tCPUState.oFlags.clearOverflow();
		tCtxt.tCPUState.oFlags.setZero(*tCtxt.operand<TTargetType>(EOprIdx::First) == 0);
		tCtxt.tCPUState.oFlags.setSign(*tCtxt.operand<TTargetType>(EOprIdx::First) < 0);
	}
}

template <typename TSourceType, typename TTargetType>
void CBasicCommands::CastUp(SCommandContext& tCtxt)
{
	// Cast from lower to higher type, no loss of data
	*tCtxt.operand<TTargetType>(EOprIdx::First) = static_cast<TTargetType>(*tCtxt.operand<TSourceType>(EOprIdx::First));
	// Update CPU Flags
	tCtxt.tCPUState.oFlags.clearCarry();
	tCtxt.tCPUState.oFlags.clearOverflow();
	tCtxt.tCPUState.oFlags.setZero(*tCtxt.operand<TTargetType>(EOprIdx::First) == 0);
	tCtxt.tCPUState.oFlags.setSign(*tCtxt.operand<TTargetType>(EOprIdx::First) < 0);
}

template <typename TSourceType, typename TTargetType>
void CBasicCommands::CastDown(SCommandContext& tCtxt)
{
	// Cast from higher to lower type, possible loss of data
	TSourceType const nTempCopy = *tCtxt.operand<TSourceType>(EOprIdx::First);
	*tCtxt.operand<TSourceType>(EOprIdx::First) = TSourceType(0);
	*tCtxt.operand<TTargetType>(EOprIdx::First) = static_cast<TTargetType>(nTempCopy);
	// Update CPU Flags
	bool const bLoss = (nTempCopy != *tCtxt.operand<TSourceType>(EOprIdx::First));
	tCtxt.tCPUState.oFlags.setCarry(bLoss);
	tCtxt.tCPUState.oFlags.setOverflow(bLoss);
	tCtxt.tCPUState.oFlags.setZero(*tCtxt.operand<TTargetType>(EOprIdx::First) == 0);
	tCtxt.tCPUState.oFlags.setSign(*tCtxt.operand<TTargetType>(EOprIdx::First) < 0);
}

//
//	Disassembly functions
//
t_string CBasicCommands::dasmMovIA(SCommandInfo const& tCmd, bool bHexadecimal)
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
		sCmd += base::toStr(t_csz(" A%1, %2"), (uint) tCmd.anRegIdx[EOprIdx::First] - SCPUStateBase::eARBaseIndex, sSymbol);
	}
	return std::move(sCmd);
}

t_string CBasicCommands::dasmMovIR(SCommandInfo const& tCmd, bool bHexadecimal)
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

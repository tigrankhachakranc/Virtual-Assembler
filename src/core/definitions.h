#ifndef VASM_CORE_DEFINITIONS_H
#define VASM_CORE_DEFINITIONS_H

//
// Includes
//
#include <base_definitions.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Definitioon for the machine address
//
using t_address = uint32;	// Virtual Machine can address up 4GB
using t_uoffset = uint32;	// Unsigned offset, same as address
using t_soffset = int32;	// Signed offset
using t_count   = uint32;

static const t_address cnInvalidAddress = UINT32_MAX;
static const uchar cnCmdMinLength = 2;  // Minimum length of the command
static const uchar cnCmdMaxLength = 12; // Maximum possible length of the command
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Operatrion codes 
//
//	[] - means optional parameter
//	Number - Signed|unsigned numeric intermediate value (8|16|32|64|)
//	Number - Unsigned numeric intermediate value which higer 4 bits are in CC field (12 bit)
//	Offset - Signed|unsigned numeric intermediate value (16|32 bit)
//	Count - Unsigned numeric intermediate value (8 bit)
//	AR(n) - Stands for the Nth Address Register
//	GR(n) - Stands for the Nth OpSize aligned General Purpose Register
//			It means GR(3) with OpSize=uint16 actually is the 3*sizeof(uint16)=GP(6-7) registers
//	opsz - Operand size (Byte, Word, DWord, QWord)
//	cc - condition code
//	IL - stands for the instruction length in bytes
//
enum class EOpCode : uchar
{
	Invalid	 = 0,
	
	// 
	NOP,	// IL(2)								No operation	
	BREAK,	// IL(2)								Break point
	EXIT,	// IL(2)								End of program (stops CPU execution)

	// Execution control instructions
	JUMPA,	// IL(2) Jump AR(n)						Absolute unconditionnal jump
	JUMPR,	// IL(4) Jump[cc] GR(n)|Offset16		Relative jump (conditional or unconditional), offset may be negative
	CALL,	// IL(2) Call AR(n)						Absolute call
	RET,	// IL(2) RET 							Returns to the caller
	RET2,	// IL(4) RET GR(n)|Offset16				Returns to the caller and cleans the stack
	
	Reserved09,	// Reserved for INT
	Reserved10,	// Reserved for IRET
	Reserved11,	// Reserved for HALT
	Reserved12,
	Reserved13,
	Reserved14,
	Reserved15,

	// Flags manipulation instructions
	GFLR,	// IL(2) GFLR GR(n)						Gets 2 byte packed status flags into specified general purpose register
	SFLR,	// IL(2) SFLR GR(n)						Sets 2 byte packed status flags from specified general purpose register

	// Memory access instructions
	LOAD,	// IL(4) LOAD  [opsz] AR(n)|GR(n) <- *AR(n)		Reads from memory into the target register
	STORE,	// IL(4) STORE [opsz] AR(n)|GR(n) -> *AR(n)		Writes into memory from the target register
	LEA,	// IL(6) LEA AR(n) <- *AR(n)+Index24			Loads effective address, Index is signed 24 bit numeric value
			//												Reads with from in memory address table by index into address register

	// Stack instructions
	PUSHSF,	// IL(2) PUSHSF							Pushes stack frame
	POPSF,	// IL(2) POPSF							Pops stack frame
	PUSHA,	// IL(2) PUSH AR(n)						Pushes specifed address register into the Stack
	POPA,	// IL(2) POP AR(n)						Pops from the Stack into specifed address register 
	PUSHR,	// IL(4) PUSH [opsz] GR(n), [Count]		Pushes specifed general purpose register(s) into the Stack
	POPR,	// IL(4) POP [opsz] GR(n), [Count]		Pops from the Stack into specifed genersal pupose register(s)
			//										If Count is specified then OPSZ is multipled with it and resulted number of bytes copied
	//PUSHF,// IL(2) PUSHF							Pushes 2 byte packed status flags into the stack
	//POPF,	// IL(2) POPF							Pops 2 byte packed status flags from the stack

	Reserved27,
	Reserved28,
	Reserved29,

	// Input from/Output to port instructions
	IN,		// IL(4) IN  [opsz] GR(n) <- GR(n)|Port (Number12)
	OUT,	// IL(4) OUT [opsz] GR(n) <- GR(n)|Port (Number12)

	// Address calculation instructions
	INC,	// IL(4) INC AR(n) <- GR(n, DWord)		Increments specifed address register by DWord GR value
	DEC,	// IL(4) DEC AR(n) <- GR(n, DWord)		Decrements specified address register by DWord GR value
	INC2,	// IL(4) INC AR(n) <- Offset16			Increments specifed address register by the specified Offset
	DEC2,	// IL(4) DEC AR(n) <- Offset16			Decrements specified address register by the specified Offset

	// Assignemnt instuctions
	ASSIGNA0,// IL(2)  ASSIGN AR(n) <- 0 (nullptr)
	ASSIGNA4,// IL(6)  ASSIGN AR(n) <- Number32
	ASSIGNR1,// IL(4)  ASSIGN GR(n) <- Number8
	ASSIGNR2,// IL(4)  ASSIGN GR(n) <- Number16
	ASSIGNR4,// IL(6)  ASSIGN GR(n) <- Number32
	ASSIGNR8,// IL(10) ASSIGN GR(n) <- Number64

	// Register manipulation instructions
	MOVE,	// IL(4) Move  [cc] [opsz] AR|GR(n) <-  AR|GR(n)			Copies second (source) operand to the first (destination) operand
			//															when CC code is specified then skips operation if CC is not met 
	SWAP,	// IL(4) Swap  [cc] [opsz] AR|GR(n) <-> AR|GR(n)			Swaps first and second operands 
			//															when CC code is specified then skips operation if CC is not met 
	//MOVSX,// IL(4) MovSX [cc] [opsz_src] [opsz_trgt] GR(n) <- GR(n)	Copies second (source) operand to the first (destination) operand
			//															and sign extends (casts) the value from opsz_src to opsz_trgt (checks CC and skips if so)
	//MOVZX,// IL(4) MOvZX [cc] [opsz_src] [opsz_trgt] GR(n) <- GR(n)	Copies second (source) operand to the first (destination) operand
			//															and zero extends the value from opsz_src to opsz_trgt (checks CC and skips if so)
	Reserved44,
	Reserved45,
	Reserved46,

	// Comparison instructions
	TEST,	// IL(4) TEST [opsz] AR(n)|GR(n), AR(n)|GR(n)	ANDs operands and sets flags
	CMP,	// IL(4) CMP  [opsz] AR(n)|GR(n), AR(n)|GR(n)	SUBs operands and sets flags
	SET,	// IL(4) Set(cc) [opsz] GR(n)			Sets specifed register to 1 if CC takes place, 0 otherwise

	// Logical Instructions
	AND,	// IL(4) AND [opsz] GR(n) <- GR(n)
	OR,		// IL(4) 
	XOR,	// IL(4) 
	NAND,	// IL(4) 										Not AND
	NOR,	// IL(4) 										Not OR
	NOT,	// IL(4) NOT [opsz] GR(n)						Bitwise negation

	// Shift instructions
	SHL,	// IL(4) SHL [opsz] GR(n) <- GR(n)|Count		Shift left
	SHR,	// IL(4) SHR [opsz] GR(n) <- GR(n)|Count		Shift right
	ROL,	// IL(4) ROL [opsz] GR(n) <- GR(n)|Count		Rotate left
	ROR,	// IL(4) ROR [opsz] GR(n) <- GR(n)|Count		Rotate right
	SAL,	// IL(4) SAL [opsz] GR(n) <- GR(n)|Count		Shift arithmetic left
	SAR,	// IL(4) SHR [opsz] GR(n) <- GR(n)|Count		Shift arithmetic right
	RCL,	// IL(4) RCL [opsz] GR(n) <- GR(n)|Count		Rotate with carry left
	RCR,	// IL(4) RCR [opsz] GR(n) <- GR(n)|Count		Rotate with carry right

	// Unsigned integral arithmetic instructions
	ADD,	// IL(4) ADD [opsz] GR(n) <- GR(n)
	SUB,	// IL(4) 
	MUL,	// IL(4)
	DIV,	// IL(4)

	// Signed integral arithmetic instructions
	ADC,	// IL(4) ADC [opsz] GR(n) <- GR(n)				Add with carry
	SBB,	// IL(4)										Sub with borrow
	IMUL,	// IL(4)
	IDIV,	// IL(4)
	NEG,	// IL(4) NEG [opsz] RG(n)						Sign negation 
	CAST,	// IL(4) CAST [opsz_src] [opsz_trgt] GR(n)		Sing extends specifed general purpose register from SRC_OPRSZ to TRGT_OPRSZ
			//												in case of shrinking if number is truncated then OF & CF flags will set respectively

	// TODO! FPU Instructions

	// End of instruction set
	Reserved
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Condition Codes (CC)
//
enum class ECndtnCode : uchar
{
	None,
	
	Equal,			// zf=1				xE|xZ
	NotEqual,		// zf=0				xNE|xNZ
	Above,			// cf=0 & zf=0		xA|xNBE
	AboveEqual,		// cf=0				xAE|xNB
	Below,			// cf=1				xB|xNAE
	BelowEqual,		// cf=1 | zf=1		xBE|xNA
	Great,			// zf=0 & sf=of		xG|xNLE
	GreatEqual,		// sf=of			xGE|xNL
	Low,			// sf<>of			xL|xNGE
	LowEqual,		// zf=1 | sf<>of	xLE|xNG
	Overflow,		// of=1				xO
	NotOverflow,	// of=0				xNO
	Signed,			// sf=1				xS
	NotSigned		// sf=0				xNS
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Operand size
//
enum class EOprSize : uchar
{
	Byte = 0,
	Word = 1,
	DWord = 2,
	QWord = 3,

	Count,
	Default = DWord
};

inline uchar OperandSize(EOprSize eOprSz)
{
	return (uchar(1) << uchar(eOprSz));
}

inline uint AlignToOperandSize(uint uIdxByte, EOprSize eOprSz)
{
	return uint(uIdxByte) << uchar(eOprSz);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Operand type
//	Describes what kind of operand could be specific operand of the specific command
//
enum class EOprType : uchar
{
	None	= 0,	// No operand
	AR		= 0x01,	// Address register index
	GR		= 0x02,	// General purpose register index
	AGR		= 0x03,	// Address or general purpose register index
	IMV		= 0x04,	// Intermediate value
	GRIMV	= 0x06,	// General purpose register index or intermediate value
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Operand index
//	Maximum operand count is 3
//
enum EOprIdx : uchar
{
	First	= 0,
	Second	= 1,
	Third	= 2,

	Count = 3
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Intermediate value type
//	There is implicit rule that all unsigned types are odd and signed ones are even numberd
//
enum class EImvType : uchar
{
	None,
	
	Num8,	// 8 bit numeric value
	SNum8,	// 8 bit signed numeric value

	Num16,	// 16 bit numeric value
	SNum16,	// 16 bit signed numeric value
	
	Num32,	// 32 bit numeric value
	SNum32,	// 32 bit signed numeric value
	
	Num64,	// 64 bit numeric value
	SNum64,	// 64 bit signed numeric value
	
	Num12,	// 12 bit numeric value, in this case 4 highest bits are lowest bits of the extension
	SNum24	// 24 bit signed numeric value, becomes lower 24 bits of SNum32 value with 1 filled highest byte 
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_DEFINITIONS_H

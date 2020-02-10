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

constexpr static const t_address cnInvalidAddress = UINT32_MAX;
constexpr static const uchar cnCmdMinLength = 2;	// Minimum length of the command (in bytes)
constexpr static const uchar cnCmdMaxLength = 8;	// Maximum possible length of the command (in bytes)
constexpr static const uchar cnCmdAvrgLength = 4;	// Average length of the command (in bytes)
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
//	An - Stands for the Nth Address Register
//	Rn - Stands for the Nth OpSize aligned General Purpose Register
//		 It means R3 with OpSize=uint16 actually is the 3*sizeof(uint16)=GP(6-7) registers
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
	JUMPA,	// IL(2) Jump An						Absolute unconditionnal jump
	JUMPR,	// IL(4) Jump[cc] Rn|Offset16			Relative jump (conditional or unconditional), offset may be negative
	CALLA,	// IL(2) Call An						Absolute call
	CALLR,	// IL(4) Call Rn|Offset16				Relative call, offset may be negative
	RET,	// IL(2) RET 							Returns to the caller

	Reserved09,	// Reserved for INT
	Reserved10,	// Reserved for IRET
	Reserved11,	// Reserved for HALT

	// Memory access instructions
	LOAD,	// IL(4) LOAD  [opsz] Rn <- *An				Reads specified number of bytes from memory at source address into the target register
	STORE,	// IL(4) STORE [opsz] *An <- Rn				Writes into memory at target address from the source register
	LDREL,	// IL(8) LDREL [opsz] Rn <- *An±Offset32	Load relative from source address, Offset is signed 32 bit numeric value
	STREL,	// IL(8) STREL [opsz] *An±Offset32 <- Rn	Store relative to target address, Offset is signed 32 bit numeric value

	// Stack instructions
	PUSHSF,	// IL(2) PUSHSF	[Rn]					Pushes stack frame
	PUSHSF2,// IL(4) PUSHSF Offset24				Pushes stack frame and allocates space on the stack
	POPSF,	// IL(2) POPSF [Rn]						Pops stack frame
	POPSF2,	// IL(4) POPSF	Offset24				Pops stack frame and clean the stack
	PUSH,	// IL(4) PUSH [opsz] Rn, [Count]		Pushes specifed register(s) into the Stack
	POP,	// IL(4) POP  [opsz] Rn, [Count]		Pops from the Stack into specifed register(s)
			//										If Count is specified then OPSZ is multipled with it and resulted number of bytes copied

	// Assignemnt instuctions (move immediate)
	MOVIA,	// IL(4)  MOVI An <- Address32
	Reserved23,	// Reserved for 64 bit affresses
	MOVI1,	// IL(4)  MOVI Rn <- Number8
	MOVI2,	// IL(4)  MOVI Rn <- Number16
	MOVI4,	// IL(6)  MOVI Rn <- Number32
	Reserved27,	// Reserved for MOVI8


	// Register manipulation instructions
	MOVE,	// IL(4) Move  [cc] [opsz] Rn <- Rn						Copies second (source) operand to the first (destination) operand
			//														when CC code is specified then skips operation if CC is not met 
	SWAP,	// IL(4) Swap  [cc] [opsz] Rn <-> Rn					Swaps first and second operands 
			//														when CC code is specified then skips operation if CC is not met 
	//MOVSX,// IL(4) MovSX [cc] [opsz_src] [opsz_trgt] Rn <- Rn		Copies second (source) operand to the first (destination) operand
			//														and sign extends (casts) the value from opsz_src to opsz_trgt (checks CC and skips if so)
	//MOVZX,// IL(4) MovZX [cc] [opsz_src] [opsz_trgt] Rn <- Rn		Copies second (source) operand to the first (destination) operand
			//														and zero extends the value from opsz_src to opsz_trgt (checks CC and skips if so)
	Reserved30,	// Reserved for MOVSX
	Reserved31,	// Reserved for MOVZX
	

	// Flags manipulation instructions
	GETF,	// IL(2) GETF Rn				Gets lower half 2 byte packed status flags into specified register
	SETF,	// IL(2) SETF Rn				Sets 2 byte packed status flags from specified register into lower half of status register
	CLC,	// IL(2) CLC					Clears Carry flag
	STC,	// IL(2) STC 					Sets Carry flag
	CMC,	// IL(2) CMC					Complements (toggles) Carry flag
	SET,	// IL(4) St(cc) [opsz] Rn		Sets specifed register to 1 if CC takes place, 0 otherwise

	Reserved38,	// Reserved for 4 byte flags get
	Reserved39, // Reserved for 4 byte flags set

	// Comparison instructions
	TEST,	// IL(4) TEST [opsz] Rn, Rn		ANDs operands and discards result to set only flags
	CMP,	// IL(4) CMP  [opsz] Rn, Rn		SUBs operands and discards result to set only flags

	// Logical Instructions
	AND,	// IL(4) AND  [opsz] Rn <- Rn	Does logical AND
	OR,		// IL(4) OR	  [opsz] Rn <- Rn	Does logical OR
	XOR,	// IL(4) XOR  [opsz] Rn <= Rn	Does logical XOR
	NAND,	// IL(4) NAND [opsz] Rn <- Rn	Not AND
	NOR,	// IL(4) NOR  [opsz] Rn <- Rn	Not OR
	NOT,	// IL(4) NOT  [opsz] Rn			Bitwise negation

	// Shift & rotate instructions
	SHL,	// IL(4) SHL [opsz] Rn <- Rn|Count		Shift left
	SHR,	// IL(4) SHR [opsz] Rn <- Rn|Count		Shift right
	SAL,	// IL(4) SAL [opsz] Rn <- Rn|Count		Shift arithmetic left
	SAR,	// IL(4) SHR [opsz] Rn <- Rn|Count		Shift arithmetic right
	ROL,	// IL(4) ROL [opsz] Rn <- Rn|Count		Rotate left
	ROR,	// IL(4) ROR [opsz] Rn <- Rn|Count		Rotate right
	RCL,	// IL(4) RCL [opsz] Rn <- Rn|Count		Rotate with carry left
	RCR,	// IL(4) RCR [opsz] Rn <- Rn|Count		Rotate with carry right

	// Signed & Unsigned integral arithmetic instructions
	ADD,	// IL(4) ADD [cc] [opsz] Rn <- Rn	Add, Op1 = Op1 + Op2
	ADC,	// IL(4) ADC [cc] [opsz] Rn <- Rn	Add with carry, Op1 = Op1 + Op2 OpCF
	SUB,	// IL(4) SUB [cc] [opsz] Rn <- Rn	Sub, Op1 = Op1 - Op2
	SBB,	// IL(4) SBB [cc] [opsz] Rn <- Rn	Sub with borrow, Op1 = Op1 - Op2 - CF
	NEG,	// IL(4) NEG [cc] [opsz] Rn			Sign negation , Op = -Op
	
	// Address calculation instructions
	INC,	// IL(4) INC An <- Offset16		Increments specifed address register by DWord value
	DEC,	// IL(4) DEC An <- Offset16		Decrements specified address register by DWord value

	// Signed & unsigned multiplication & division instructions
	MUL,	// IL(4) MUL  [opsz] Rn <- Rn	2xOp1 = Op1 * Op2
	DIV,	// IL(4) DIV  [opsz] Rn <- Rn	Op1 = 2xOp1 / Op2, 2xOp1 mod Op2
	IMUL,	// IL(4) IMUL [opsz] Rn <- Rn
	IDIV,	// IL(4) IDIV [opsz] Rn <- Rn

	CAST,	// IL(4) CAST opsz_src opsz_trgt GR(n)			Sing extends specifed general purpose register from SRC_OPRSZ to TRGT_OPRSZ
			//												in case of shrinking if number is truncated then OF & CF flags will set respectively

	// Reserved for atomic operations
	Reserved68,
	Reserved69,
	Reserved70,
	Reserved71,

	// Input from/Output to port instructions
	IN,		// IL(4) IN  [opsz] Rn <- Rn|Port (Number12)	Reads corresponding number of bytes from the source port int the target register
	OUT,	// IL(4) OUT [opsz] Rn|Port <- Rn (Number12)	Writes corresponding number of bytes from source register into target port

	// TODO! FPU & Future Instructions
	Reserved

	// End of instruction set
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Condition Codes (CC)
//
enum class ECndtnCode : uchar
{
	None = 0,
	
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
	NotSigned,		// sf=0				xNS

	Invalid
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

	// Auxiliary meta-values
	Count,	// Number of supported operand sizes
	Default = DWord
};

inline uchar OperandSize(EOprSize const eOprSz)
{
	return (uchar(1) << uchar(eOprSz));
}

inline uint AlignToOperandSize(uint const uIdxByte, EOprSize const eOprSz)
{
	return uint(uIdxByte) << uchar(eOprSz);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Operand switch
//
enum class EOprSwitch : uchar
{
	Reg = 0,
	Imv = 1,

	Default = Reg,
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Operand type
//	Describes what kind of operand could be specific operand of the specific command
//
enum class EOprType : uchar
{
	None	= 0,	// No operand
	Reg		= 0x01,	// Register index (address or general purpose) 
	Imv		= 0x02,	// Immediate value
	RegImv	= 0x03,	// Register index or immediate value
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
	
	Num8,	// 8 bit unsigned numeric value
	SNum8,	// 8 bit signed numeric value

	Num16,	// 16 bit unsigned numeric value
	SNum16,	// 16 bit signed numeric value
	
	Num32,	// 32 bit unsigned numeric value
	SNum32,	// 32 bit signed numeric value
	
	Num64,	// 64 bit unsigned numeric value
	SNum64,	// 64 bit signed numeric value
	
	Count,	// 8 bit unsigned numeric value that serves as Count of something (shift bits, registers & etc...)
	Port	// 12 bit unsigned numeric value that serves as I/O Port number, (4 highest bits packed in the lowest bits of the extension)
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_DEFINITIONS_H

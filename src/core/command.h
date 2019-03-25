#ifndef VASM_CORE_COMMAND_H
#define VASM_CORE_COMMAND_H

//
// Includes
//
#include "definitions.h"
#include "flags.h"

// STL
#include <tuple>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Command meta information
//	Describes specifc command structure
//
#pragma pack(push, 1)
struct SCommandMetaInfo
{
	enum {OC = int(EOprIdx::Count)};

	// Extension info (decode helpers)
	enum EExtInfo : uchar	
	{
		// Non zero value means that second byte is the extension
		// which could contain OprSize, ArgSpec, CC bits & etc.
		NoExtension		= 0,
		MaskExtension	= 0x0F,

		HasCndtnCode	= 0x01,
		HasOprSize		= 0x02,
		HasOprSwitch	= 0x04,
		CustomExtension	= 0x08,

		SkipCdtnCheck	= 0x10,

		FixedOprSizeByte  = 0x00,
		FixedOprSizeWord  = 0x40,
		FixedOprSizeDWord = 0x80,
		FixedOprSizeQWord = 0xC0,
		MaskFixedOprSize  = 0xC0,

		// | OpSwitch1 | OpSwitch2 | OpSize | Cndtn Code | 
		//       7           6        5  4    3  2  1  0
		MaskCndtnCode	= 0x0F,
		MaskOprSize		= 0x30,
		MaskOprSwitch1	= 0x40,
		MaskOprSwitch2	= 0x80,
		
		OprSizeShift	= 4
	};

	t_csz		pcszName;
	t_csz		pcszBaseName;

	EOpCode		eOpCode;		// Operation code
	uchar		eExtInfo;		// Externsion info
	uchar		nOperandCount;	// Number of operands
	EOprType	aeOprTypes[OC];	// Operands types (max op. count is 3)
	EImvType	eImvType;		// Intermediate value type
	uchar		nLength;		// Command length in bytes

	SCommandMetaInfo();
	SCommandMetaInfo(t_csz, EOpCode, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, EOpCode, EOprType, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, EOpCode, EOprType, EImvType, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, EOpCode, EOprType, EOprType, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, EOpCode, EOprType, EOprType, EImvType, uchar = EExtInfo::NoExtension, bool opr3 = false);

	SCommandMetaInfo(t_csz, t_csz, EOpCode, EOprType, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, t_csz, EOpCode, EOprType, EImvType, uchar = EExtInfo::NoExtension);
	SCommandMetaInfo(t_csz, t_csz, EOpCode, EOprType, EOprType, uchar = EExtInfo::NoExtension);
};

using SCommandMetaInfoArray = std::vector<SCommandMetaInfo>;
#pragma pack(pop)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Decoded (expanded) command information
//
#pragma pack(push, 1)
struct SCommandInfo
{
	SCommandMetaInfo	tMetaInfo;

	uchar		nExtension;		// OpCode extension

	// Decoded (expanded) extension
	ECndtnCode	eCdtnCode;		// Condition code (when applicable)
	EOprSize	eOprSize;		// Operands size
	bool		bOprSwitch1;	// Switch bit for operand 1
	bool		bOprSwitch2;	// Switch bit for operand 2

	// Register idx for Operands (when applicable)
	uchar		nRegIdx[EOprIdx::Count];

	// Immediate value for Op1, Op2 or Op3 (when applicable)
	union
	{
		int8	i8Imv;
		uint8	u8Imv;

		int16	i16Imv;
		uint16	u16Imv;

		int32	i32Imv;
		uint32	u32Imv;
		
		int64	i64Imv;
		uint64	u64Imv;
	};

	t_address	nAddress = 0;	// Commands address/location in the memory

	inline SCommandInfo(SCommandMetaInfo const& = SCommandMetaInfo());
};
#pragma pack(pop)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CPU State Base
//	Basic CPU state information required by command context
//
struct SCPUStateBase
{
	// Constants
	enum
	{	
		eA0Index = 0,
		eRIPIndex = 1,
		eSPIndex = 2,
		eSFIndex = 3,

		eARBaseIndex = 4,
		
		eAddressRegistersPoolSize = 8,
		eGeneralPurposeRegisterPoolSize = 64
	};

	// CPU Flags
	CFlags	oFlags;
	
	// Instruction pointer of the next command 
	t_address	nIP;
	// Current IP, IP value of the current command at the moment of the fetch
	t_address	nCIP;
	// Return address, points to the next instruction after Call instraction
	t_address&	nRIP;	// Reference to AR1
	// Current stack pointer
	t_address&	nSP;	// Reference to AR2
	// Current stack frame 
	t_address&	nSF;	// Reference to AR3

	// Addressing registers pool
	t_address	anARPool[eAddressRegistersPoolSize];
	// General purpose registers pool
	uint8		aui8GPRPool[eGeneralPurposeRegisterPoolSize];

	// Code size (Code always should start from the 0 address)
	t_uoffset const	cnCodeSize;

	// Stack limits 
	t_uoffset const	cnStackLBound;
	t_uoffset const	cnStackUBound;

	// processor runs while this flag is true,
	// it stops its execution and returns when it is turned off
	volatile bool	bRun;

	//
	SCPUStateBase(t_uoffset cs, t_uoffset slb, t_uoffset sub);
	
	SCPUStateBase& operator=(SCPUStateBase const&);

	SCPUStateBase(SCPUStateBase const&) = delete;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Command Context
//	Contains all (decoded) info required for the single instruction execution
//
class CMemory;
struct SCommandContext
{
	// Cuurend command info
	SCommandInfo const&	tInfo;

	// CPU State Base
	SCPUStateBase&		tCPUState;

	// RAM
	CMemory&			oMemory;

	// Pointer to decoded operands
	// Points to either Address reg., GP reg. or Intermediate value when applicable, nullptr otherwise
	union UOperandPtr
	{
		void*		p;
		int8*		pi8;
		uint8*		pu8;
		int16*		pi16;
		uint16*		pu16;
		int32*		pi32;
		uint32*		pu32;
		int64*		pi64;
		uint64*		pu64;

		template <typename TOprSize> TOprSize* ptr() const
			{return reinterpret_cast<TOprSize*>(p);}

	}	tOpr[EOprIdx::Count];

	inline SCommandContext(SCommandInfo const&, SCPUStateBase&, CMemory&);

	SCommandContext(SCommandContext const&) = delete;
	void operator=(SCommandContext const&) = delete;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IExecutor interface
//	Base interface for all single instruction executers 
//
class IInstruction
{
public:
	virtual ~IInstruction();

	// Executes concrete instruction for the specified context
	virtual void Exec(SCommandContext& tCmdCtxt) = 0;
};

using IInstructionPtr = std::shared_ptr<IInstruction>;
using IInstructionWPtr = std::weak_ptr<IInstruction>;
using IInstructionUPtr = std::unique_ptr<IInstruction>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IDisassemble interface
//	Base interface for all single instruction disassemblers 
//
class IDisassemble
{
public:
	virtual ~IDisassemble();

	// Translates specifed command into its equivalent text representation
	virtual t_string Translate(SCommandInfo const& tCmd, bool bHexadecimal) = 0;
};

using IDisassemblePtr = std::shared_ptr<IDisassemble>;
using IDisassembleWPtr = std::weak_ptr<IDisassemble>;
using IDisassembleUPtr = std::unique_ptr<IDisassemble>;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	IAddressRecovery
//	Rverses specified address to its ooriginal symbol representation
//
class IAddressRecovery
{
public:
	virtual ~IAddressRecovery();

	// Translates sepcified address to its original symbol
	virtual t_string ToSymbol(t_address) const = 0;
};

// IAddressRecovery smart pointers
using IAddressRecoveryPtr = std::shared_ptr<IAddressRecovery>;
using IAddressRecoveryWPtr = std::weak_ptr<IAddressRecovery>;
using IAddressRecoveryUPtr = std::unique_ptr<IAddressRecovery>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Command base
// Serves as the base class for all command implementers
// It allows to define several command handlers altogether
//
class CCommandBase
{
public:
	//
	//	Instruction information
	//
	struct SInstructionInfo
	{
		SCommandMetaInfo	tMetaInfo;
		IInstruction*		pExec;
		IDisassemble*		pDisasm;

		inline SInstructionInfo();
	};
protected:
	//	Default constructor is for derived concrete commands
	CCommandBase();
public:
	virtual ~CCommandBase();

	// Non copiable nor movable
	CCommandBase(CCommandBase const&) = delete;
	CCommandBase(CCommandBase&&) = delete;
	void operator=(CCommandBase const&) = delete;
	void operator=(CCommandBase&&) = delete;

public:
	//
	//	Methods
	//
	inline t_size GetInstructionCount() const;
	SInstructionInfo GetInstruction(t_index nIdx) const;

	void SetAddressRecovery(IAddressRecoveryPtr pAddrRecovery);

protected:
	//
	//	Methods and Types for derived concrete commands
	//
	enum { OprSizeCount = int(EOprSize::Count) };

	//
	// Executor interface realization
	//

	//	Internal executor function pointer
	using FuncCmdExec = void(CCommandBase::*)(SCommandContext&);
	//using FuncCmdExec = std::function<void(CCommandBase::*)(SCommandContextBase&)>;

	class CInstructionImpl final : public IInstruction
	{
	public:
		CInstructionImpl(CCommandBase& oMaster, FuncCmdExec pFunc);
		~CInstructionImpl() override;

		void Exec(SCommandContext& tCmdCtxt) override;

	private:
		CCommandBase&	m_oMaster;
		FuncCmdExec		m_pfnExec;
	};

	// Version for executers differemtiated by the operand size (Byte, Word, DWord, QWord)
	class CInstructionImpl4 final : public IInstruction
	{
	public:
		CInstructionImpl4(CCommandBase& oMaster, FuncCmdExec pFunc[OprSizeCount]);
		~CInstructionImpl4() override;

		void Exec(SCommandContext& tCmdCtxt) override;

	private:
		CCommandBase&	m_oMaster;
		FuncCmdExec		m_apfnExec[OprSizeCount];
	};

	//
	// Disassemble interface realization
	//

	//	Internal disassembler function pointer
	using FuncCmdDisasm = t_string(CCommandBase::*)(SCommandInfo const&, bool bHexadecimal);
	//using FuncCmdDisasm = std::function<t_string(CCommandBase::*)(SCommandInfo const&)>;

	class CDisassembleImpl final : public IDisassemble
	{
	public:
		CDisassembleImpl(CCommandBase& oMaster, FuncCmdDisasm pfnDisasm);
		~CDisassembleImpl() override;

		t_string Translate(SCommandInfo const& tCmd, bool bHexadecimal) override;

	private:
		CCommandBase&	m_oMaster;
		FuncCmdDisasm	m_pfnDisasm;
	};


	// Registers and binds specified meber methods with specified command info
	// If method registered twice it will overwrite existing one
	// It is assumed that derived concrete commands will register their methods in their constructors
	void Register(SCommandMetaInfo const& tInfo, FuncCmdExec pfnExec, FuncCmdDisasm pfnDasm);
	// Version for executers differemtiated by the operand size (Byte, Word, DWord, QWord)
	void Register(SCommandMetaInfo const& tInfo, FuncCmdExec pfnExec[OprSizeCount], FuncCmdDisasm pfnDasm);

	// Generic disassemle funcion, suitable for the most instructions
	t_string DisAsm(SCommandInfo const&, bool bHexadecimal);
	// Disassembly partial helpers
	virtual t_string DisAsmCmd(SCommandInfo const& tCmd);
	virtual t_string DisAsmOprSize(SCommandInfo const& tCmd, bool bForce = false);
	virtual t_string DisAsmArgs(SCommandInfo const& tCmd, bool bHexadecimal);
	virtual t_string DisAsmArg(SCommandInfo const& tCmd, bool bHexadecimal, t_index nArgIdx);
	virtual t_string DisAsmImv(SCommandInfo const& tCmd, bool bHexadecimal);

	inline IAddressRecoveryPtr GetAddressRecovery() const;

private:
	using t_InstructionInfo = std::tuple<SCommandMetaInfo, IInstructionUPtr, IDisassembleUPtr>;
	using t_InstructionStore = std::vector<t_InstructionInfo>;

	t_InstructionStore		m_aInstructions;
	IAddressRecoveryWPtr	m_pAddressRecovery;
};

// CCommandBase smart pointers
using CCommandPtr = std::shared_ptr<CCommandBase>;
using CCommandWPtr = std::weak_ptr<CCommandBase>;
using CCommandUPtr = std::unique_ptr<CCommandBase>;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	SCommandInfo
//
inline SCommandInfo::SCommandInfo(SCommandMetaInfo const& mi) :
	tMetaInfo(mi),
	nExtension(0),
	eCdtnCode(ECndtnCode::None),
	eOprSize(EOprSize::Default),
	bOprSwitch1(false),
	bOprSwitch2(false),
	u64Imv(0)
{
	nRegIdx[EOprIdx::First] = 0;
	nRegIdx[EOprIdx::Second] = 0;
	nRegIdx[EOprIdx::Third] = 0;
}

//
//	SCommandContext
//
inline SCommandContext::SCommandContext(
	SCommandInfo const& info, SCPUStateBase& state, CMemory& mem) :
	tInfo(info), tCPUState(state), oMemory(mem)
{
	tOpr[0].p = nullptr;
	tOpr[1].p = nullptr;
	tOpr[2].p = nullptr;
}

//
//	CCommandBase 
//
inline CCommandBase::SInstructionInfo::SInstructionInfo() :
	pExec(nullptr), pDisasm(nullptr)
{
}

inline t_size CCommandBase::GetInstructionCount() const
{
	return m_aInstructions.size();
}

inline IAddressRecoveryPtr CCommandBase::GetAddressRecovery() const
{
	return m_pAddressRecovery.lock();
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_COMMAND_H

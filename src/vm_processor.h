#ifndef VM_PROCESSOR_H
#define VM_PROCESSOR_H

//
// Includes
//
#include "vm_code.h"
#include "vm_memory.h"
#include "vm_exception.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Interface for Input & Output Ports
//
class IPorts
{
public:
	virtual ~IPorts();

	virtual void In(t_index nPort, CValue& oValue) = 0;
	virtual void Out(t_index nPort, CValue const& oValue) = 0;
};

using IPortsPtr = std::shared_ptr<IPorts>;

//
//	Interface for Debuggers
//
class IDebugger
{
public:
	virtual ~IDebugger();

	virtual bool Start() = 0;
	virtual void End() = 0;
	virtual void Error() = 0;

	// Processor calls will call Break() upon breakpoint or trap from its execution loop
	// If Break() returns true CPU execution should continue
	// If Break() returns FALSE CPU execution should be stopped
	virtual bool Break() = 0;
};

using IDebuggerPtr = std::shared_ptr<IDebugger>;


//
//	Processor definition
//	Main processing unit
//
class CProcessor
{
public:
	//
	// Constants & Type definitions
	//

	enum
	{
		eAddressRegistersPoolSize = 8,
		eGeneralPurposeRegisterPoolSize = 64
	};

	class CFlags
	{
	public:
		enum
		{
			eMask_CF = 0x0001,
			eMask_PF = 0x0002,
			eMask_ZF = 0x0004,
			eMask_SF = 0x0008,
			eMask_OF = 0x0010,
			eMask_TF = 0x0080,
		};

		inline CFlags();

		inline bool getTrap() const;
		inline bool getCarry() const;
		inline bool getParity() const;
		inline bool getZero() const;
		inline bool getSign() const;
		inline bool getOverflow() const;
		inline uint16 getFlags() const;

		inline void setTrap(bool);
		inline void setCarry(bool);
		inline void setParity(bool);
		inline void setZero(bool);
		inline void setSign(bool);
		inline void setOverflow(bool);
		inline void setFlags(uint16);

		bool IsEqual() const;
		bool IsNotEqual() const;
		bool IsAbove() const;
		bool IsAboveOrEqual() const;
		bool IsBelow() const;
		bool IsBelowOrEqual() const;
		bool IsGreat() const;
		bool IsGreatOrEqual() const;
		bool IsLow() const;
		bool IsLowOrEqual() const;
		bool IsOverflow() const;
		bool IsNotOverflow() const;
		bool IsSigned() const;
		bool IsNotSigned() const;
		bool IsParity() const;
		bool IsNotParity() const;

	private:
		uint16	m_ui16Value;
	};

	struct SState
	{
		t_index		nPC;	// Program counter
		CFlags		oFlags;	// Flags
		volatile bool bRun;	// processor runs while this flag is true,
							// it stops its execution when it is turned off

		// Addressing regisotrs
		uint32		aui32ARPool[eAddressRegistersPoolSize];

		// General purpose registers pool
		uint8		aui8GPRPool[eGeneralPurposeRegisterPoolSize];

		// Stack helpers
		t_offset&	nSP;		// Stack pointer (reference to AR0)
		t_offset&	nSF;		// Stack frame (reference to AR1)
		// Stack limit 
		t_offset	nStackLBound;
		t_offset	nStackUBound;

		//
		SState();
		SState(SState const&);
		SState& operator=(SState const&);
	};

	// Interface for command execution
	struct SCommandContext;
	class IExecutor
	{
	public:
		virtual ~IExecutor();

		virtual void Nop(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Break(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Exit(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Execution control
		virtual void Jump(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Call(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Ret(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Assignment
		virtual void Assign(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Move(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Swap(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Logical
		virtual void And(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Or(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Xor(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Not(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Nand(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Nor(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Unsigned integral arithmetic
		virtual void Add(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Sub(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Mul(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Div(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Inc(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Dec(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Signeg integral arithmetic
		virtual void AddC(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void SubB(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void IMul(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void IDiv(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Neg(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Shift
		virtual void ShiftR(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void ShiftL(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void RotateR(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void RotateL(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void ShiftAR(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void ShiftAL(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void RotateCR(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void RotateCL(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Comparison
		virtual void Test(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Cmp(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Stack
		virtual void Push(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Pop(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void PushF(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void PopF(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Memory
		virtual void Load(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Store(SState& tMachineState, SCommandContext& tCommand) = 0;

		// Input & Output
		virtual void In(SState& tMachineState, SCommandContext& tCommand) = 0;
		virtual void Out(SState& tMachineState, SCommandContext& tCommand) = 0;
	};
	using IExecutorPtr = std::unique_ptr<IExecutor>;

	using EOpSize = EType; // operand size

	struct SArgument
	{
		enum EType
		{
			None,
			AR,		// Address register index
			GPR,	// General purpose register index
			Number,	// Numeric value
			Offset	// Offset
		};

		EType	eType;
		int64	nValue;

		inline SArgument();
	};


	struct SCommandContext
	{
		typedef void (IExecutor::* FnCommand)(SState& tMachineState, SCommandContext& tCommand);
		typedef bool (CFlags::* FnCondition)() const;

		FnCommand	pfnCommand;
		FnCondition	pfnCondition;

		EOpSize		eOpSize;

		SArgument	tArg1;
		SArgument	tArg2;
		SArgument	tArg3;

		t_index		nLine;

		inline SCommandContext();
	};

	// Decode filter
	class IDecodeFilter
	{
	public:
		virtual ~IDecodeFilter();

		// Preprocesses command
		// Returns true if command execution should be continued
		// Returns false if command should be skipped
		// Default implementation does nothing and always returns true
		virtual bool Preprocess(std::string& sCommand);
		// Parses argument string and returns it
		// Default implementation return empty argument which means not handled
		virtual SArgument ParseArgument(std::string const& sArgument);
	};
	using IDecodeFilterPtr = std::shared_ptr<IDecodeFilter>;

public:
	//
	//	Construction & Destruction
	//
	CProcessor();
	~CProcessor();

	// Not copiable
	CProcessor(CProcessor const&) = delete;
	void operator=(CProcessor const&) = delete;

public:
	//
	//	Methods
	//
	bool IsValid() const;
	void Init(CCodePtr pCode, CMemoryPtr pMemory, IPortsPtr pPorts, t_size nStackSize);
	
	void AttachDebugger(IDebuggerPtr pDebugger);
	void DetachDebugger();

	void SetDecodeFilter(IDecodeFilterPtr pDecodeFilter);

	void Run(CException* pError = nullptr);
	void Stop();
	inline bool IsRunning() const;

	inline SState const& GetState() const;
	void SetState(SState const&);

	inline SCommandContext const& GetCurrentCommandContext() const;
	inline std::string const& GetCurrentCommand() const;

	inline CFlags& GetFlags();
	inline void SetPC(t_index nNewPC);

protected:
	//
	//	Implementation
	//
	std::string Fetch();
	SCommandContext Decode(std::string& sCommand);
	void Execute(SCommandContext& tCommand);

	virtual IExecutorPtr CreateExecutor();

private:
	//
	//	Content
	//
	CCodePtr			m_pCode;
	CMemoryPtr			m_pMemory;
	IPortsPtr			m_pPorts;
	IDebuggerPtr		m_pDebugger;

	IDecodeFilterPtr	m_pDecodeFilter;
	IExecutorPtr		m_pExecutor;

	SState				m_tState;

	// Purpose of this members is to allow access them from outside spontaneously
	std::string			m_sCurrentCommand;
	SCommandContext		m_tCurrentCommandContext;
};

using CProcessorPtr = std::shared_ptr<CProcessor>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
inline CProcessor::CFlags::CFlags()
	: m_ui16Value(0)
{
}

inline bool CProcessor::CFlags::getTrap() const
{
	return (m_ui16Value & uint16(eMask_TF)) != 0;
}

inline bool CProcessor::CFlags::getCarry() const
{
	return (m_ui16Value & uint16(eMask_CF)) != 0;
}

inline bool CProcessor::CFlags::getParity() const
{
	return (m_ui16Value & uint16(eMask_PF)) != 0;
}

inline bool CProcessor::CFlags::getZero() const
{
	return (m_ui16Value & uint16(eMask_ZF)) != 0;
}

inline bool CProcessor::CFlags::getSign() const
{
	return (m_ui16Value & uint16(eMask_SF)) != 0;
}

inline bool CProcessor::CFlags::getOverflow() const
{
	return (m_ui16Value & uint16(eMask_OF)) != 0;
}

inline uint16 CProcessor::CFlags::getFlags() const
{
	return (m_ui16Value & uint16(0x007F));
}

inline void CProcessor::CFlags::setTrap(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_TF)) : (m_ui16Value & ~uint16(eMask_TF));
}

inline void CProcessor::CFlags::setCarry(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_CF)) : (m_ui16Value & ~uint16(eMask_CF));
}

inline void CProcessor::CFlags::setParity(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_PF)) : (m_ui16Value & ~uint16(eMask_PF));
}

inline void CProcessor::CFlags::setZero(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_ZF)) : (m_ui16Value & ~uint16(eMask_ZF));
}

inline void CProcessor::CFlags::setSign(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_SF)) : (m_ui16Value & ~uint16(eMask_SF));
}

inline void CProcessor::CFlags::setOverflow(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_OF)) : (m_ui16Value & ~uint16(eMask_OF));
}

inline void CProcessor::CFlags::setFlags(uint16 ui16Value)
{
	m_ui16Value = (ui16Value & uint16(0x007F)) | (m_ui16Value & uint16(0XFF80));
}

inline CProcessor::SArgument::SArgument()
	: eType(None), nValue(0)
{
}

inline CProcessor::SCommandContext::SCommandContext()
	: pfnCommand(nullptr), pfnCondition(nullptr), eOpSize(EOpSize::DWord), nLine(0)
{
}

inline bool CProcessor::IsRunning() const
{
	return m_tState.bRun;
}

inline CProcessor::SState const& CProcessor::GetState() const
{
	return m_tState;
}

inline CProcessor::SCommandContext const& CProcessor::GetCurrentCommandContext() const
{
	return m_tCurrentCommandContext;
}

inline std::string const& CProcessor::GetCurrentCommand() const
{
	return m_sCurrentCommand;
}

inline CProcessor::CFlags& CProcessor::GetFlags()
{
	return m_tState.oFlags;
}

inline void CProcessor::SetPC(t_index nNewPC)
{
	m_tState.nPC = nNewPC;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_PROCESSOR_H
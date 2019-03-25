#ifndef VASM_VM_PROCESSOR_H
#define VASM_VM_PROCESSOR_H

//
// Includes
//
#include <base_exception.h>
#include <core_definitions.h>
#include <core_command.h>
#include <core_memory.h>
#include <core_decoder.h>
#include <core_flags.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_address = core::t_address;
using t_uoffset = core::t_uoffset;

using EOpCode = core::EOpCode;
using EOprType = core::EOprType;
using EOprIdx = core::EOprIdx;
using EOprSize = core::EOprSize;
using EImvType = core::EImvType;
using ECndtnCode = core::ECndtnCode;

using SCommandMetaInfo = core::SCommandMetaInfo;
using SCommandInfo = core::SCommandInfo;
using SCommandContext = core::SCommandContext;

using CMemory = core::CMemory;
using CDecoder = core::CDecoder;
using IInstruction = core::IInstruction;

using CMemoryPtr = core::CMemoryPtr;
using CDecoderUPtr = core::CDecoderUPtr;
using CCommandLibraryPtr = core::CCommandLibraryPtr;

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor
//	The central processing unit (CPU)
//
class CProcessor
{
public:
	////////////////////////////////////////////////////////////////////////////
	//
	//	CPU Status
	//
	enum class EStatus
	{
		NotInitialized	= 0,	// CPU is not initialized and could not run
		Ready			= 0x01,	// CPU is initialized and readu to run
		Break			= 0x03, // CPU stopped due to debug break point
		Stopped			= 0x05, // CPU stopped due to user action
		Failed			= 0x08, // CPU stopped due to fault
		Running			= 0x10, // CPU currently is running
		Finished		= 0x20	// CPU reached EXIT command and finished its execution 
	};

	struct SStatus
	{
		EStatus				eStatus = EStatus::NotInitialized;
		base::CException	oErrorInfo; // In case of fail

		inline SStatus() = default;
		inline SStatus(SStatus const&) = default;
		inline SStatus(SStatus&&) = default;
		inline SStatus& operator=(SStatus const&) = default;
		inline SStatus& operator=(SStatus&&) = default;
	};
	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////
	//
	//	CPU State
	//
	struct SState : core::SCPUStateBase
	{
		//
		SState(t_uoffset nCodeSize = 0, t_uoffset nStackLBound = 0, t_uoffset nStackUBound = 0);
		SState& operator=(SState const&);
	};
	////////////////////////////////////////////////////////////////////////////

public:
	//
	//	Construction & Destruction
	//
	CProcessor();
	~CProcessor();

	// Non copiable, nor movable
	CProcessor(CProcessor const&) = delete;
	CProcessor(CProcessor&&) = delete;
	void operator=(CProcessor const&) = delete;
	void operator=(CProcessor&&) = delete;

public:
	//
	//	Methods
	//

	// Resets CPU to uninitialized state
	void Reset();

	// CPU assumes that code located between [0, nCodeSize) range and always checks PC
	// CPU assumes that stacl located between [MemSize-nStackSize, MemSize) range and always checks SP
	void Init(CCommandLibraryPtr pCmdLib,
			  CMemoryPtr pMemory,
			  t_uoffset nCodeSize,
			  t_uoffset nStackSize,
			  t_address nProgramStart);

	EStatus Run(bool bOnce = false);
	void Stop();

	inline SStatus const& Status() const;
	inline SState const& State() const;
	inline SCommandContext const& CurrentCommandContext() const;

	// For the debbugger
	inline SState& ChangeableState();
	inline bool CanRun() const;

protected:

	////////////////////////////////////////////////////////////////////////////
	//
	//	Extended command context 
	//	All pointers are temporary and should not kept
	//
	struct SCommandContextEx : public SCommandContext
	{
		// Current command info
		SCommandInfo	tCmdInfo;

		// Current instruction executer
		IInstruction*	pExec;

		inline SCommandContextEx(SState&, CMemory&);
	};
	using SCmdCtxtPtr = std::unique_ptr<SCommandContextEx>;
	////////////////////////////////////////////////////////////////////////////

protected:
	//
	//	Implementation
	//
	uchar const* Fetch() const; // Returns pointer to command
	void Decode(uchar const* pCmd, SCommandContextEx&);
	void Execute(SCommandContextEx&);

	inline SCommandContextEx& CurrentCommandContext();

private:
	//
	//	Content
	//
	CCommandLibraryPtr	m_pCmdLib;
	CMemoryPtr			m_pMemory;
	CDecoderUPtr		m_pDecoder;

	SStatus				m_tStatus;
	SState				m_tState;
	
	SCmdCtxtPtr			m_pCurrentCommand;
};

using CProcessorPtr = std::shared_ptr<CProcessor>;
using CProcessorWPtr = std::weak_ptr<CProcessor>;
using CProcessorUPtr = std::unique_ptr<CProcessor>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CProcessor::SCommandContextEx::SCommandContextEx(
	SState& tState, CMemory& oMemory) :
	SCommandContext(tCmdInfo, tState, oMemory)
{
	pExec = nullptr;
}

inline CProcessor::SStatus const& CProcessor::Status() const
{
	return m_tStatus;
}

inline CProcessor::SState const& CProcessor::State() const
{
	return m_tState;
}

inline CProcessor::SState& CProcessor::ChangeableState()
{
	if (m_tState.bRun)
		VASM_THROW_ERROR(t_csz("CPU State could not changed while processor is running"));
	return m_tState;
}

inline SCommandContext const& CProcessor::CurrentCommandContext() const
{
	VASM_CHECK_PTR(m_pCurrentCommand);
	return *m_pCurrentCommand;
}

inline CProcessor::SCommandContextEx& CProcessor::CurrentCommandContext()
{
	VASM_CHECK_PTR(m_pCurrentCommand);
	return *m_pCurrentCommand;
}

inline bool CProcessor::CanRun() const
{
	return (m_tStatus.eStatus == EStatus::Ready ||
			m_tStatus.eStatus == EStatus::Break ||
			m_tStatus.eStatus == EStatus::Stopped);
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_VM_PROCESSOR_H

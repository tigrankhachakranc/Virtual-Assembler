//
//	Component
//
#define __COMPONENT__ "Core"

//
// Includes
//
#include "command_library.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CCommandLibrary implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCommandLibrary::CCommandLibrary()
{
	// OpCode Zero is reserved for the Invalid instruction
	m_aInstructions.resize(1);
}

CCommandLibrary::~CCommandLibrary() = default;

void CCommandLibrary::SetAddressRecovery(IAddressRecoveryPtr p)
{
	m_pAddressRecovery = p;

	for (CCommandPtr pCmd : m_setCommands)
	{
		pCmd->SetAddressRecovery(p);
	}
}

void CCommandLibrary::Register(CCommandPtr pCommand)
{
	VASM_CHECK_PTR(pCommand);
	if (m_setCommands.find(pCommand) == m_setCommands.end())
	{
		m_setCommands.insert(pCommand);

		t_size nInstructionCount = pCommand->GetInstructionCount();
		for (t_index i = nInstructionCount; i > 0; --i)
		{
			SInstructionInfo tInfo = pCommand->GetInstruction(i-1);

			// Instruction  with Invalid OpCode is reserved
			if (tInfo.tMetaInfo.eOpCode == EOpCode::Invalid)
				VASM_THROW_ERROR(t_csz("Command Library: Invalid instruction could not be registered"));
			
			t_index idxOpCode = (t_index) tInfo.tMetaInfo.eOpCode;
			if (idxOpCode >= m_aInstructions.size())
				m_aInstructions.resize(idxOpCode + 1);

 			m_aInstructions[idxOpCode] = tInfo;
		}

		// Pass address recovery to command
		pCommand->SetAddressRecovery(m_pAddressRecovery);
	}
}

void CCommandLibrary::Unregister(CCommandPtr pCommand)
{
	auto it = m_setCommands.find(pCommand);
	if (it != m_setCommands.end())
	{

		t_size nInstructionCount = pCommand->GetInstructionCount();
		for (t_index i = 0; i < nInstructionCount; ++i)
		{
			SInstructionInfo tInfo = pCommand->GetInstruction(i);

			t_index idxOpCode = (t_index) tInfo.tMetaInfo.eOpCode;
			if (idxOpCode < m_aInstructions.size() &&
				m_aInstructions.at(idxOpCode).pExec == tInfo.pExec)
				m_aInstructions[idxOpCode] = SInstructionInfo();
		}

		m_setCommands.erase(it);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

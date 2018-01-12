//
//	Includes
//
#include "vm_debugger.h"

// STL
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CDebugger Implementation
//

CDebugger::CDebugger(CInterpreterPtr pMachine)
	: m_pMachine(pMachine), m_sBreakCmd("BREAK")
{

}

CDebugger::~CDebugger()
{

}


bool CDebugger::Start()
{
	if (m_pMachine != nullptr && m_pMachine->IsValid())
		return Break();
	return false;
}

void CDebugger::End()
{
	if (m_pMachine != nullptr)
	{
		CProcessor::SState const& tState = m_pMachine->GetProcessor()->GetState();
		PrintState(tState);
	}

	std::cout << "Finished!" << std::endl;
	std::cin.get();

	m_pMachine = nullptr;
}

void CDebugger::Error()
{
	if (m_pMachine != nullptr)
	{
		CProcessor::SState const& tState = m_pMachine->GetProcessor()->GetState();
		PrintState(tState);
	}

	std::cout << "Aborted!" << std::endl;
	std::cin.get();

	m_pMachine = nullptr;
}

bool CDebugger::Break()
{
	if (m_pMachine != nullptr)
	{
		CCodePtr pCode = m_pMachine->GetCode();
		CProcessorPtr pCPU = m_pMachine->GetProcessor();
		CProcessor::SState const& tState = pCPU->GetState();

		if (tState.nPC > 0)
		{
			auto it = m_mapCodeCache.find(tState.nPC - 1);
			if (it != m_mapCodeCache.end())
			{	// Breakpoint
				std::string const& sHitCmd = pCode->GetAt(tState.nPC - 1);
				if (sHitCmd == m_sBreakCmd)
				{	// Breakpoint hit, restore command
					pCode->ChangeAt(tState.nPC) = it->second;
					// And put pack PC
					pCPU->SetPC(tState.nPC - 1);
				}
				else
				{	// Breakpoint passed, restore break command
					pCode->ChangeAt(tState.nPC) = m_sBreakCmd;
					// Continue
					pCPU->GetFlags().setTrap(false);
					return true;
				}
			}
		}

		PrintState(tState);
	}
	else
	{
		std::cout << "Machine is not available" << std::endl;
	}

	while (m_pMachine != nullptr)
	{
		CCodePtr pCode = m_pMachine->GetCode();
		CProcessorPtr pCPU = m_pMachine->GetProcessor();
		CProcessor::SState const& tState = pCPU->GetState();

		char chCommand;
		std::cin >> chCommand;

		if (chCommand == 'g')
		{
			auto it = m_mapCodeCache.find(tState.nPC);
			if (it != m_mapCodeCache.end())
				pCPU->GetFlags().setTrap(true);
			else
				pCPU->GetFlags().setTrap(false);
			return true;
		}
		else if (chCommand == 't')
		{
			pCPU->GetFlags().setTrap(true);
			return true;
		}
		else if (chCommand == 'b')
		{
			t_size nLine;
			std::cin >> nLine;
			if (nLine < pCode->GetSize() &&
				m_mapCodeCache.find(nLine) == m_mapCodeCache.end())
			{
				std::string& sCmd = pCode->ChangeAt(nLine);
				m_mapCodeCache.insert(std::move(std::make_pair(nLine, std::move(sCmd))));
				sCmd = m_sBreakCmd;
			}
		}
		else if (chCommand == 'r')
		{
			t_size nLine;
			std::cin >> nLine;
			auto it = m_mapCodeCache.find(nLine);
			if (nLine < pCode->GetSize() && it != m_mapCodeCache.end())
			{
				std::string& sCmd = pCode->ChangeAt(nLine);
				sCmd = std::move(it->second);
				m_mapCodeCache.erase(it);
			}
		}
		else if (chCommand == 'x')
		{
			m_pMachine = nullptr;
		}
	}

	return false;
}

void CDebugger::PrintState(CProcessor::SState const& tState)
{
	std::cout << "Processor State: ------------------------"  << std::endl;
	std::cout << "PC = " << std::dec << tState.nPC << "  ";
	std::cout << "Flags: CF(" << tState.oFlags.getCarry() << ")  ";
	std::cout << "ZF(" << tState.oFlags.getZero() << ")  ";
	std::cout << "SF(" << tState.oFlags.getSign() << ")  ";
	std::cout << "PF(" << tState.oFlags.getParity() << ")  ";
	std::cout << "OF(" << tState.oFlags.getOverflow() << ")  ";
	std::cout << std::endl;

	
	std::cout << std::endl << "Address register:" << std::endl;
	std::cout << "A0 (SP) = " << std::hex << std::showbase << tState.aui32ARPool[0] << "  ";
	std::cout << "A1 (SF) = " << std::hex << std::showbase << tState.aui32ARPool[1] << "  ";
	std::cout << "A2 = " << std::hex << std::showbase << tState.aui32ARPool[2] << "  ";
	std::cout << "A3 = " << std::hex << std::showbase << tState.aui32ARPool[3] << "  ";
	std::cout << std::endl;
	std::cout << "A4 = " << std::hex << std::showbase << tState.aui32ARPool[4] << "  ";
	std::cout << "A5 = " << std::hex << std::showbase << tState.aui32ARPool[5] << "  ";
	std::cout << "A6 = " << std::hex << std::showbase << tState.aui32ARPool[6] << "  ";
	std::cout << "A7 = " << std::hex << std::showbase << tState.aui32ARPool[7] << "  ";
	std::cout << std::endl;

	std::cout << std::endl << "General purpose register:" << std::endl;
	std::cout << "R0 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[0]) << "  ";
	std::cout << "R8 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[8]) << "  ";
	std::cout << "R16 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[16]) << "  ";
	std::cout << "R24 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[24]) << "  ";
	std::cout << std::endl;
	std::cout << "R32 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[32]) << "  ";
	std::cout << "R40 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[40]) << "  ";
	std::cout << "R48 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[48]) << "  ";
	std::cout << "R56 = " << std::hex << std::showbase << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[56]) << "  ";
	std::cout << std::endl;

	CCodePtr pCode = m_pMachine->GetCode();
	std::cout << pCode->GetAt(tState.nPC) << std::endl;


	std::cout << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


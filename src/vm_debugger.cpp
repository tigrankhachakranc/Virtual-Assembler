//
//	Includes
//
#include "vm_debugger.h"

// STL
#include <iostream>
#include <iomanip>

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

void CDebugger::End(bool bSuccessfull)
{
	if (bSuccessfull)
		std::cout << std::endl << ">> Program execution finished!";
	else
		std::cout << std::endl << "!> Program execution stopped!";
	std::cin.get();

	m_pMachine = nullptr;
}

bool CDebugger::Error(CException const& e)
{
	if (m_pMachine != nullptr)
	{
		CProcessor::SState const& tState = m_pMachine->GetProcessor()->GetState();
		PrintState(tState);
	}

	std::cout << std::endl << "!> " << e.what();

	return false;
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
			t_index nPrevPC = tState.nPC - 1;
			auto it = m_mapCodeCache.find(nPrevPC);
			if (it != m_mapCodeCache.end())
			{	// Breakpoint
				std::string const& sHitCmd = pCode->GetAt(nPrevPC);
				if (sHitCmd == m_sBreakCmd)
				{	// Breakpoint hit, restore command
					pCode->ChangeAt(nPrevPC) = it->second;
					// And put pack PC
					pCPU->SetPC(nPrevPC);
				}
				else
				{	// Breakpoint passed, restore break command
					pCode->ChangeAt(nPrevPC) = m_sBreakCmd;

					if (m_chLastCommand == 'g')
					{	// Continue
						pCPU->GetFlags().setTrap(false);
						return true;
					}
				}
			}
		}

		PrintState(tState);
	}
	else
	{
		std::cout << std::endl << "!> Machine is not available";
	}

	while (m_pMachine != nullptr)
	{
		CCodePtr pCode = m_pMachine->GetCode();
		CProcessorPtr pCPU = m_pMachine->GetProcessor();
		CProcessor::SState const& tState = pCPU->GetState();

		std::cout << std::endl << "-> ";
		std::cin >> m_chLastCommand;

		if (m_chLastCommand == '?')
		{
			std::cout << "g                         Go: run program until finished or breakpoint reached" << std::endl;
			std::cout << "t [N=1]                   Trace: execute single instruction N times" << std::endl;
			std::cout << "j <line>                  Jump: set PC to the specified number of source code" << std::endl;
			std::cout << "b <line>                  Set breakpoint at line number of source code" << std::endl;
			std::cout << "r <line>                  Remove breakpoint from line number of source code" << std::endl;
			std::cout << "d [var]                   Dump content of the variable with name <var> (by default will dump all variables)" << std::endl;
			std::cout << "ds [size=64]              Dump stack with specified size of bytes" << std::endl;
			std::cout << "a [B|W|DW|QW] A|R?=<val>  Assign specifed value to the register" << std::endl;
			std::cout << "a <var>=<val>             Assign specifed value to the variable" << std::endl;
			std::cout << "st <Flag>                 Set specified flag" << std::endl;
			std::cout << "cl <Flag>                 Clear specified flag" << std::endl;
			std::cout << "x                         Stop program execution" << std::endl;
		}
		else if (m_chLastCommand == 'j')
		{
			t_size nSrcLine;
			std::cin >> nSrcLine;
			t_size nCodeLine = pCode->GetCodeLine(nSrcLine);
			if (nCodeLine < pCode->GetSize())
			{
				pCPU->SetPC(nCodeLine);
				PrintState(tState);
			}
			else
				std::cout << "!> Invalid source code line number.";
		}
		else if (m_chLastCommand == 'g')
		{
			auto it = m_mapCodeCache.find(tState.nPC);
			if (it != m_mapCodeCache.end())
				pCPU->GetFlags().setTrap(true);
			else
				pCPU->GetFlags().setTrap(false);
			return true;
		}
		else if (m_chLastCommand == 't')
		{
			pCPU->GetFlags().setTrap(true);
			return true;
		}
		else if (m_chLastCommand == 'b')
		{
			t_size nSrcLine;
			std::cin >> nSrcLine;
			t_size nCodeLine = pCode->GetCodeLine(nSrcLine);
			if (nCodeLine < pCode->GetSize() &&
				m_mapCodeCache.find(nCodeLine) == m_mapCodeCache.end())
			{
				std::string& sCmd = pCode->ChangeAt(nCodeLine);
				std::cout << ">> Breakpoint set at PC line #" << std::dec << nCodeLine << ": " << sCmd;

				m_mapCodeCache.insert(std::move(std::make_pair(nCodeLine, std::move(sCmd))));
				sCmd = m_sBreakCmd;
			}
			else
				std::cout << "!> Breakpoint not set.";
		}
		else if (m_chLastCommand == 'r')
		{
			t_size nSrcLine;
			std::cin >> nSrcLine;
			t_size nCodeLine = pCode->GetCodeLine(nSrcLine);
			auto it = m_mapCodeCache.find(nCodeLine);
			if (nCodeLine < pCode->GetSize() && it != m_mapCodeCache.end())
			{
				std::string& sCmd = pCode->ChangeAt(nCodeLine);
				sCmd = std::move(it->second);
				m_mapCodeCache.erase(it);

				std::cout << ">> Breakpoint removed from PC line #" << std::dec << nCodeLine;
			}
			else
				std::cout << "!> Breakpoint not removed.";
		}
		else if (m_chLastCommand == 'x')
		{
			m_pMachine = nullptr;
		}
	}

	return false;
}

void CDebugger::PrintState(CProcessor::SState const& tState)
{
	std::cout << std::endl;
	std::cout << "Processor status flags: ---------------------------------------"  << std::endl;
	std::cout << "CF:" << std::dec << tState.oFlags.getCarry() << "  ";
	std::cout << "PF:" << std::dec << tState.oFlags.getParity() << "  ";
	std::cout << "ZF:" << std::dec << tState.oFlags.getZero() << "  ";
	std::cout << "SF:" << std::dec << tState.oFlags.getSign() << "  ";
	std::cout << "OF:" << std::dec << tState.oFlags.getOverflow() << "  ";

	std::cout << std::endl;
	std::cout << "Address registers (0x): ---------------------------------------" << std::endl;
	std::cout << "A0 (SP) = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[0] << "  ";
	std::cout << "A2 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[2] << "  ";
	std::cout << "A4 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[4] << "  ";
	std::cout << "A6 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[6] << "  ";
	std::cout << std::endl;
	std::cout << "A1 (SF) = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[1] << "  ";
	std::cout << "A3 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[3] << "  ";
	std::cout << "A5 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[5] << "  ";
	std::cout << "A7 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.aui32ARPool[7] << "  ";

	std::cout << std::endl;
	std::cout << "General purpose registers (0x): --------------------------------------------------------------" << std::endl;
	std::cout << "R0  = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[0]) << "  ";
	std::cout << "R16 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[16]) << "  ";
	std::cout << "R32 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[32]) << "  ";
	std::cout << "R48 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[48]) << "  ";
	std::cout << std::endl;
	std::cout << "R8  = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[8]) << "  ";
	std::cout << "R24 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[24]) << "  ";
	std::cout << "R40 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[40]) << "  ";
	std::cout << "R56 = " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *reinterpret_cast<uint64 const*>(&tState.aui8GPRPool[56]) << "  ";
	std::cout << std::endl;

	CCodePtr pCode = m_pMachine->GetCode();
	std::cout << "Next command: --------------------------------------------------------------------------------" << std::endl;
	std::cout << "PC = " << std::dec << tState.nPC << ":  " << pCode->GetAt(tState.nPC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


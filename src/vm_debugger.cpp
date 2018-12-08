//
//	Includes
//
#include "vm_debugger.h"
#include "base_parser.h"

// STL
#include <iostream>
#include <iomanip>
#include <cctype>

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

		try
		{


			switch (m_chLastCommand)
			{
			case '?':
			{
				std::cout << "a [B|W|DW|QW] A|R? <val>  Assign specifed value to the register" << std::endl;
				std::cout << "a <var> <val>             Assign specifed value to the variable" << std::endl;
				std::cout << "b <line>                  Set Breakpoint at line number of source code" << std::endl;
				std::cout << "                          (by default will teset all breakpoints)" << std::endl;
				std::cout << "c <Flag>                  Clear specified flag" << std::endl;
				std::cout << "d [size=64]               Dump stack with specified size of bytes" << std::endl;
				std::cout << "f                         Dump Functions call stack" << std::endl;
				std::cout << "g                         Go: run program until finished or breakpoint reached" << std::endl;
				std::cout << "j <line>                  Jump: set PC to the specified number of source code" << std::endl;
				std::cout << "l <file_path>             Load source code from file" << std::endl;
				std::cout << "m [var]                   Dump content of the Memory specified by the variable with name <var>" << std::endl;
				std::cout << "                          (by default will dump all variables)" << std::endl;
				std::cout << "p                         Print current state" << std::endl;
				std::cout << "r                         Reset/Restart the program" << std::endl;
				std::cout << "s <Flag>                  Set specified flag" << std::endl;
				std::cout << "t [N=1]                   Trace: execute single instruction N times" << std::endl;
				std::cout << "u [line]                  Unset breakpoint from line number of source code" << std::endl;
				std::cout << "v <line> [N=10]           View N command(s) started from line number of source code" << std::endl;
				std::cout << "x                         Stop program execution and eXit" << std::endl;
				break;
			}
			case 'a':
			{
				std::string sToken;
				std::cin >> sToken;

				CProcessor::EOpSize eOpSize = CProcessor::EOpSize::DWord;

				// Parse operand size
				// keep operand size
				if (sToken.compare("B") == 0 || sToken.compare("b") == 0)
				{
					eOpSize = CProcessor::EOpSize::Byte;
					std::cin >> sToken;
				}
				else if (sToken.compare("W") == 0 || sToken.compare("w") == 0)
				{
					eOpSize = CProcessor::EOpSize::Word;
					std::cin >> sToken;
				}
				else if (sToken.compare("DW") == 0 || sToken.compare("dw") == 0)
				{
					eOpSize = CProcessor::EOpSize::Word;
					std::cin >> sToken;
				}
				else if (sToken.compare("QW") == 0 || sToken.compare("qw") == 0)
				{
					eOpSize = CProcessor::EOpSize::DWord;
					std::cin >> sToken;
				}

				if (sToken.empty() || std::isdigit(sToken.at(0)))
					throw base::CException("Missing register specifier.");

				// Is Register or Variable?
				char chFirst = sToken.at(0);
				if ((chFirst == 'A' || chFirst == 'R') && sToken.size() > 1 && base::CParser::IsNum(sToken, 1))
				{	// Register
					if (chFirst == 'A' && eOpSize != CProcessor::EOpSize::DWord)
						throw base::CException("Operand size is not applicable to address registers.");
					
					// get register index
					std::size_t nStrPos = 0;
					t_size nPoolSize = (chFirst == 'A') ? CProcessor::eAddressRegistersPoolSize : CProcessor::eGeneralPurposeRegisterPoolSize;
					sToken.erase(0, 1);
					t_size nRIdx = 0;
					try
					{
						nRIdx = (t_size) std::stoul(sToken, &nStrPos, 10);
					}
					catch (std::exception const&)
					{
					}

					if (nStrPos != sToken.size() || nRIdx >= nPoolSize)
						throw base::CException("Invalid register index specified.");

					// Read value
					CProcessor::SState oStateCopy = tState;
					if (chFirst == 'A')
					{
						uint32 nValue;
						std::cin >> nValue;
						oStateCopy.aui32ARPool[nRIdx] = nValue;
					}
					else switch (eOpSize)
					{
					case CProcessor::EOpSize::Byte:
					{
						int8 nValue;
						std::cin >> nValue;
						oStateCopy.aui8GPRPool[nRIdx] = (uint8) nValue;
						break;
					}
					case CProcessor::EOpSize::Word:
					{
						int16 nValue;
						std::cin >> nValue;
						reinterpret_cast<uint16&>(oStateCopy.aui8GPRPool[nRIdx]) = (uint16) nValue;
						break;
					}
					case CProcessor::EOpSize::DWord:
					{
						int32 nValue;
						std::cin >> nValue;
						reinterpret_cast<uint32&>(oStateCopy.aui8GPRPool[nRIdx]) = (uint32) nValue;
						break;
					}
					case CProcessor::EOpSize::QWord:
					{
						int64 nValue;
						std::cin >> nValue;
						reinterpret_cast<uint64&>(oStateCopy.aui8GPRPool[nRIdx]) = (uint64) nValue;
						break;
					}}

					pCPU->SetState(oStateCopy);
				}
				else
				{	// Lookup variable by name
					CMemoryManagerPtr pMemMan = m_pMachine->GetMemMan();
					CMemoryManager::SVarInfo tVarInfo = pMemMan->GetVariableInfo(sToken);
					if (tVarInfo.nOffset == 0)
						throw base::CException("Invalid variable name specified.");

					CValue oValue;
					if (tVarInfo.bString)
					{
						std::cin >> sToken;
						oValue = std::move(CValue(sToken));
					}
					else switch (tVarInfo.eType)
					{
					case EType::Byte:
					{
						int8 nValue;
						std::cin >> nValue;
						oValue = std::move(CValue((t_byte) nValue));
						break;
					}
					case EType::Word:
					{
						int16 nValue;
						std::cin >> nValue;
						oValue = std::move(CValue((t_word)nValue));
						break;
					}
					case EType::DWord:
					{
						int32 nValue;
						std::cin >> nValue;
						oValue = std::move(CValue((t_dword)nValue));
						break;
					}
					case EType::QWord:
					{
						int64 nValue;
						std::cin >> nValue;
						oValue = std::move(CValue((t_qword)nValue));
						break;
					}}

					if (!pMemMan->SetVariable(sToken, oValue))
						base::CException("Failed to assign variable.");
				}
				break;
			}
			case 'b':
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
				break;
			}
			case 'g':
			{
				auto it = m_mapCodeCache.find(tState.nPC);
				if (it != m_mapCodeCache.end())
					pCPU->GetFlags().setTrap(true);
				else
					pCPU->GetFlags().setTrap(false);
				return true;
			}
			case 'j':
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
				break;
			}
			case 'p':
			{
				PrintState(tState);
				return true;
			}
			case 't':
			{
				pCPU->GetFlags().setTrap(true);
				return true;
			}
			case 'u':
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
				break;
			}
			case 'x':
			{
				m_pMachine = nullptr;
				break;
			}
			default:
				std::cout << "!> Unknwons command: type '?' for help.";
				break;
			}
		}
		catch (std::exception const& e)
		{
			std::cout << "!> " << e.what() << std::endl << "!> Type '?' for usage.";
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


//
// Includes
//
#include "commander.h"
#include <base_utility.h>
#include <cl_compiler.h>

// STL
#include <fstream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace dev {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CStringComparator<EComparisonType::Equal_CI> compare;

using base::toStr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CCommander implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Static variables initialization
//
CCommander::t_mapCmdHandlers CCommander::ms_mapCmdHandlers = {
	{"?",		&CCommander::cmd_help},
	{"h",		&CCommander::cmd_help},
	{"help",	&CCommander::cmd_help},
	{"i",		&CCommander::cmd_info},
	{"info",	&CCommander::cmd_info},

	{"r",		&CCommander::cmd_run},
	{"run",		&CCommander::cmd_run},
	{"g",		&CCommander::cmd_run},
	{"go",		&CCommander::cmd_run},
	{"t",		&CCommander::cmd_step_in},
	{"trace",	&CCommander::cmd_step_in},
	{"n",		&CCommander::cmd_step_over},
	{"next",	&CCommander::cmd_step_over},
	{"so",		&CCommander::cmd_step_out},
	{"stepout",	&CCommander::cmd_step_out},
	{"pause",	&CCommander::cmd_stop},
	{"reset",	&CCommander::cmd_reset},

	{"bp",		&CCommander::cmd_bp_set},
	{"rbp",		&CCommander::cmd_bp_remove},

	{"set",		&CCommander::cmd_set},
	{"a",		&CCommander::cmd_assign},
	{"assign",	&CCommander::cmd_assign},
	{"w",		&CCommander::cmd_watch},
	{"watch",	&CCommander::cmd_watch},

	{"p",		&CCommander::cmd_print},
	{"print",	&CCommander::cmd_print},
	{"bt",		&CCommander::cmd_backtrace},
	{"d",		&CCommander::cmd_dump},
	{"dump",	&CCommander::cmd_dump},

	{"l",		&CCommander::cmd_load},
	{"load",	&CCommander::cmd_load},
	{"o",		&CCommander::cmd_open},
	{"open",	&CCommander::cmd_open},
	{"rl",		&CCommander::cmd_reload},
	{"reload",	&CCommander::cmd_reload},
	{"ro",		&CCommander::cmd_reopen},
	{"reopen",	&CCommander::cmd_reopen},
	{"x",		&CCommander::cmd_exit},
	{"exit",	&CCommander::cmd_exit}
};

//
//	C_Tors
//
CCommander::CCommander(std::istream& cin, std::ostream& cout) :
	m_cin(cin), m_cout(cout), m_bProcessing(false)
{
}

CCommander::~CCommander() = default;

//
//	Methods
//
void CCommander::Init(CMachinePtr pMachine)
{
	VASM_CHECK_X(!m_bProcessing, t_csz("Can't initialize commander while processing commands!"))
	m_pMachine = pMachine;
	VASM_CHECK_PTR(m_pMachine);

	m_pDebugger = std::make_shared<CDebugger>();
	m_pMachine->SetDebugger(m_pDebugger);
}

void CCommander::Start()
{
	VASM_CHECK_PTR(m_pMachine);
	VASM_CHECK_PTR(m_pDebugger);

	if (m_bProcessing)
	{
		m_cout << "Error: Debugger already started, attempting to start debugger twice!" << std::endl;
		return; // Already started!
	}

	t_string sCmdLine;
	m_bProcessing = true;

	if (m_pMachine->IsValid())
		PrintCurrentState();

	while (m_bProcessing)
	{
		m_cout << std::endl << "-> ";

		try
		{
			std::getline(m_cin, sCmdLine);
			if (sCmdLine.empty())
				continue;

			CCmdParser oParser(sCmdLine);
			t_string sCommand = std::move(oParser.ParseToken());

			auto it = ms_mapCmdHandlers.find(sCommand);
			if (it == ms_mapCmdHandlers.end())
				throw CError(toStr("Error: Unrecognized command '%1'", sCommand));

			fnCmdHandler pfn = it->second;
			pfn(this, oParser);
		}
		catch (base::CException const& err)
		{
			m_cout << err.GetErrorMsg();
		}
		catch (std::exception const& err)
		{
			m_cout << "Unexpected error: " << err.what();
		}
	}
}

void CCommander::Load(t_string sExecPath)
{
	if (m_pMachine == nullptr)
		VASM_THROW_ERROR(t_csz("Failed to load binary executable, commander is not initialized"));

	std::ifstream oBinFile(sExecPath, std::ios::in | std::ios::binary);
	if (oBinFile.fail())
		throw CError(base::toStr("Failed to open binary file '%1'", sExecPath));

	m_pMachine->Init(oBinFile);
	
	m_sProgram = std::move(sExecPath);
	m_aSourceCodes.clear();
}

void CCommander::Open(t_string sProgramSource)
{
	std::vector<t_string> aNames;
	aNames.push_back(std::move(sProgramSource));
	Open(std::move(aNames));
}

void CCommander::Open(std::vector<t_string> aSourceCodeNames)
{
	if (m_pMachine == nullptr)
		VASM_THROW_ERROR(t_csz("Failed to open sources, commander is not initialized"));

	std::stringstream oBinaryOutput;
	cl::CCompiler::Build(aSourceCodeNames, t_string(), oBinaryOutput);

	m_pMachine->Init(oBinaryOutput);

	m_sProgram.clear();
	m_aSourceCodes = std::move(aSourceCodeNames);
}


//
//	Command Handlers
//
void CCommander::cmd_help(CCmdParser&)
{
	m_cout << std::endl;
	m_cout << "?|h|help                                      Supported commands listing"										<< std::endl;
	m_cout << "i|info                                        Prints information about curent state and program being debugged"	<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "r|run         [-dbg yes|no]                   Runs program from curent IP (in debug (default) or nondebug mode)"	<< std::endl;
	m_cout << "              [-from location]                Sets current IP to specified source location and runs program"		<< std::endl;
	m_cout << "              [-to location]                  Stops execution when cotrol reaches to the specied source location"<< std::endl;
	m_cout << "              location format: [src:]line_num | func_name | [func_name.]label | 0xAddress"						<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "t|trace       [Num=1]                         Step In, executes Num (default=1) instructions and stops"			<< std::endl;
	m_cout << "n|next        [Num=1]                         Step Over, executes Num instructions by overcoming function calls"	<< std::endl;
	m_cout << "so|stepout                                    Step Out, executes until current function will return"				<< std::endl;
	m_cout << "pause                                         Pauses curent execution"											<< std::endl;
	m_cout << "reset                                         Resets CPU to initial state"										<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "bp            location                        Sets debug break point at specified source location"				<< std::endl;
	m_cout << "rbp           all | location                  Removes breakpoint (if any) from specifed source location"			<< std::endl;
	m_cout << "              see 'run' above for location format details"														<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "set           IP location                     Sets IP to the specified source code location, see run for details"<< std::endl;
	m_cout << "              A(idx) location | variable      Sets An to the specified source code location or variable address"	<< std::endl;
	m_cout << "              R(idx) [B|W|DW|QW|CH] value     Sets specified register to the specified numeric value"			<< std::endl;
	m_cout << "              CF|ZF|SF|OF 0|1                 Sets or clears specifeid flag in ther status Flags register"		<< std::endl;
	m_cout << "              Location format: [src:]line_num | func_name | [func_name.]label | 0xAddress"						<< std::endl;
	m_cout << "              Variable format: var_name | 0xAddress"																<< std::endl;
	m_cout << "              Value format: +/-DecimalNum | 0xHexValue  Register index should be aligned with the value size"	<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "a|assign      variable value [, ...]          Assigns value(s) to the specified variable or memory location"		<< std::endl;
	m_cout << "w|watch       [hex]                           Prints the watch list (variables and values)"						<< std::endl;
	m_cout << "              [hex] var_name [, ...]          Prints current value of the specified variable(s)"					<< std::endl;
	m_cout << "        add   var_name [, ...]                Adds specified variable(s) to the watch list"						<< std::endl;
	m_cout << "        rem   var_name [, ...]                Removes specified variable(s) from the watch list"					<< std::endl;
	m_cout << "        all   [hex]                           Prints values of all (defined) variables"							<< std::endl;
	m_cout << "              If 'hex' modifier is specifeid then all values are printed in the hexadecimal format"				<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "p|print       [dec]                           Prints CPU current state.   (If 'dec' is specified then values"	<< std::endl;
	m_cout << "        state [dec]                           Prints CPU current state.    will be printed in decimal form)"		<< std::endl;
	m_cout << "        cstack                                Prints function call stack (same as bt)"							<< std::endl;
	m_cout << "        sf    [+arg_offset] [-locals_offset]  Prints current stack frame within (SF - SP] range (by default)"	<< std::endl;
	m_cout << "        bp                                    Prints Break points list"											<< std::endl;
	m_cout << "        code  [-at An|location]               Prints code started from the current IP or specifieid location"	<< std::endl;
	m_cout << "              [-c|count instruction_count]    Specifies exact number of instructions to be printed"				<< std::endl;
	m_cout << "        data                                  Prints content of the data section"								<< std::endl;
	m_cout << "        memory -at An|0xAddress -sz Rn|count] Prints memory content at specified Address with Count bytes length"<< std::endl;
	m_cout << "                                              Address or Count could be specifed directly or through Register"	<< std::endl;
	m_cout << "        ports                                 Prints I/O Devices and Port ranges occupied by them"				<< std::endl;
	m_cout << "bt                                            Prints function call stack (backtrace), see also 'print cstack'"	<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "d|dump        -f|file file_path               Dumps entire memory content or part of it into specified file"		<< std::endl;
	m_cout << "              -format text | bin"																				<< std::endl;
	m_cout << "       memory [-at 0xAddress]"																					<< std::endl;
	m_cout << "              [-sz|size byte_count]"																				<< std::endl;
	m_cout << "       code                                   Dumps content of the code section"									<< std::endl;
	m_cout << "       data                                   Dumps content of the data section"									<< std::endl;
	m_cout << "       stack  [-l|depth Num]                  Dumps content of the stack with specified call depth (level)"		<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "l|load        exec_file_path                  Opens specified binary exectuavle file and loads into memory"		<< std::endl;
	m_cout << "o|open        source_file_name[,...]          Opens specifeid source file(s), compiles & loads them into memory"	<< std::endl;
	m_cout << "rl|reload                                     Reloads current binary exectuavle file into memroy and resets CPU" << std::endl;
	m_cout << "ro|reopen                                     Reopens current source file(s), recompiles & reloads into memory"	<< std::endl;
	m_cout << "x|exit                                        Exits the program"													<< std::endl;
}

void CCommander::cmd_info(CCmdParser& oParser)
{
	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	if (!m_pMachine->IsValid())
		m_cout << std::endl << "Machine is not initialized, no binary executable." << std::endl;
	else
	{
		if (!m_sProgram.empty())
		{
			m_cout << "Loaded from a precompiled binary executable" << std::endl;
			m_cout << "Current program: " << m_sProgram << std::endl;
		}
		else if (!m_aSourceCodes.empty())
		{
			m_cout << "Opened and compiled from source code(s):" << std::endl;
			for (auto const& sSrc : m_aSourceCodes)
				m_cout << "\t" << sSrc << std::endl;
		}
		else
		{
			m_cout << "No program or sources!" << std::endl;
		}

		switch (m_pDebugger->CPUStatus().eStatus)
		{
		case vm::CProcessor::EStatus::NotInitialized:
			m_cout << "VM status: Not initialized" << std::endl;
			break;
		case vm::CProcessor::EStatus::Ready:
			m_cout << "VM status: Ready" << std::endl;
			break;
		case vm::CProcessor::EStatus::Break:
			m_cout << "VM status: Hit at breakpoint" << std::endl;
			break;
		case vm::CProcessor::EStatus::Stopped:
			m_cout << "VM status: Stopped" << std::endl;
			break;
		case vm::CProcessor::EStatus::Failed:
			m_cout << "VM status: Failed" << std::endl;
			m_cout << "Fail reason: " << m_pDebugger->CPUStatus().oErrorInfo.sErrorMessage << std::endl;
			break;
		case vm::CProcessor::EStatus::Running:
			m_cout << "VM status: Running" << std::endl;
			break;
		case vm::CProcessor::EStatus::Finished:
			m_cout << "VM status: Finished" << std::endl;
			break;
		};
	}
}

void CCommander::cmd_run(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	bool isAsynch = false;
	bool isDebug = true;
	t_address nAddressFrom = core::cnInvalidAddress;
	t_address nAddressTo = core::cnInvalidAddress;

	oParser.SkipWhiteSpaces();
	while (!oParser.IsFinished())
	{
		t_string sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
		if (compare(sToken, t_csz("asynch")))
		{
			isAsynch = true;
			m_cout << "Warning: Asynchronous run is not supported!" << std::endl;
		}
		else if (compare(sToken, t_csz("-dbg")))
		{
			isDebug = oParser.ParseBoolean();
		}
		else if (compare(sToken, t_csz("-from")))
		{
			nAddressFrom = FetchAddress(oParser, EAddressHint::Code);
		}
		else if (compare(sToken, t_csz("-to")))
		{
			nAddressTo = FetchAddress(oParser, EAddressHint::Code);
		}
		else
		{
			throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
		}
	}

	if (nAddressFrom != core::cnInvalidAddress)
		m_pDebugger->ChangeIP(nAddressFrom);

	// Run program
	if (nAddressTo != core::cnInvalidAddress)
		m_pDebugger->RunTo(nAddressTo, !isDebug);
	else
		m_pDebugger->Run(!isDebug);

	PrintCurrentState();
}

void CCommander::cmd_step_in(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_size nCount = 1;
	oParser.SkipWhiteSpaces();
	if (!oParser.IsFinished())
	{
		t_string sToken;
		if (base::CParser::IsAlpha(oParser.PeekChar()))
		{
			sToken = std::move(oParser.ParseToken());
			if (!compare(sToken, t_csz("Num")) || oParser.GetChar(true) != t_char('='))
				throw CParserError(t_csz("Expecting [Num = count]"), oParser.GetPreviousPos(), sToken);
		}

		nCount = oParser.ParseNumber<t_size>();

		sToken = std::move(oParser.ParseToken());
		if (!sToken.empty())
			throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
	}

	m_pDebugger->StepIn(nCount);
	PrintCurrentState();
}

void CCommander::cmd_step_over(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_size nCount = 1;
	oParser.SkipWhiteSpaces();
	if (!oParser.IsFinished())
	{
		t_string sToken;
		if (base::CParser::IsAlpha(oParser.PeekChar()))
		{
			sToken = std::move(oParser.ParseToken());
			if (!compare(sToken, t_csz("Num")) || oParser.GetChar(true) != t_char('='))
				throw CParserError(t_csz("Expecting [Num = count]"), oParser.GetPreviousPos(), sToken);
		}

		nCount = oParser.ParseNumber<t_size>();

		sToken = std::move(oParser.ParseToken());
		if (!sToken.empty())
			throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
	}

	m_pDebugger->StepOver(nCount);
	PrintCurrentState();
}

void CCommander::cmd_step_out(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->StepOut();
	PrintCurrentState();
}

void CCommander::cmd_stop(CCmdParser& oParser)
{
	CheckMachine();

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->Stop();
}

void CCommander::cmd_reset(CCmdParser& oParser)
{
	CheckMachine();

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->ResetProgram();
}

void CCommander::cmd_bp_set(CCmdParser& oParser)
{
	CheckCPUStatus(false);
	t_address nAddress = FetchAddress(oParser, EAddressHint::Code);
	m_pDebugger->SetBreakPoint(nAddress);

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_bp_remove(CCmdParser& oParser)
{
	CheckCPUStatus(false);
	t_string sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
	if (compare(sToken, t_csz("all")))
	{
		m_pDebugger->RemoveAllBreakPoints();
	}
	else if (compare(sToken, t_csz("-at")))
	{
		t_index nIndex = oParser.ParseNumber<t_index>();
		m_pDebugger->RemoveBreakPointByIndex(nIndex);
	}
	else
	{
		oParser.RevertPreviousParse();

		t_address nAddress = FetchAddress(oParser, EAddressHint::Code);
		m_pDebugger->RemoveBreakPoint(nAddress);
	}

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_set(CCmdParser& oParser)
{
	CheckCPUStatus(true);
	t_string sToken = std::move(oParser.ParseName());

	if (compare(sToken, t_csz("IP")))
	{
		t_address nAddress = FetchAddress(oParser, EAddressHint::Code);
		if (!m_pDebugger->ChangeIP(nAddress))
			VASM_THROW_ERROR(t_csz("Failed to change IP Register"));
	}
	else if (compare(sToken, t_csz("CF")))
	{
		bool bFlag = oParser.ParseBoolean();
		core::CFlags oFlags = m_pDebugger->CPUState().oFlags;
		oFlags.setCarry(bFlag);
		m_pDebugger->ChangeFlags(oFlags);
	}
	else if (compare(sToken, t_csz("ZF")))
	{
		bool bFlag = oParser.ParseBoolean();
		core::CFlags oFlags = m_pDebugger->CPUState().oFlags;
		oFlags.setZero(bFlag);
		m_pDebugger->ChangeFlags(oFlags);
	}
	else if (compare(sToken, t_csz("SF")))
	{
		bool bFlag = oParser.ParseBoolean();
		core::CFlags oFlags = m_pDebugger->CPUState().oFlags;
		oFlags.setSign(bFlag);
		m_pDebugger->ChangeFlags(oFlags);
	}
	else if (compare(sToken, t_csz("OF")))
	{
		bool bFlag = oParser.ParseBoolean();
		core::CFlags oFlags = m_pDebugger->CPUState().oFlags;
		oFlags.setOverflow(bFlag);
		m_pDebugger->ChangeFlags(oFlags);
	}
	else if (sToken.front() == t_char('A') || sToken.front() == t_char('a'))
	{
		std::size_t nPos;
		sToken.erase(sToken.cbegin());
		t_index nRegIdx = (t_index) std::stoul(sToken, &nPos);
		if (nPos != sToken.size() || nRegIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
			throw CParserError(t_csz("Invalid address register index"), oParser.GetPreviousPos(), sToken);

		t_address nAddress = FetchAddress(oParser, EAddressHint::CodeAndData);
		m_pDebugger->ChangeRegister(CDebugger::ERegType::ADR, nRegIdx, {nAddress});
	}
	else
	{
		EValueType eType = EValueType::Unknown;
		core::EOprSize eOprSize = core::EOprSize::Default;
		t_index nRegIdx = g_ciInvalid;

		for (auto i = 0; i < 2 && !sToken.empty(); ++i)
		{
			if (sToken.front() == t_char('R') || sToken.front() == t_char('r'))
			{
				if (nRegIdx != g_ciInvalid)
					throw CParserError(t_csz("General purpose register should be specified once"), oParser.GetPreviousPos(), sToken);

				std::size_t nPos;
				sToken.erase(sToken.cbegin());
				if (sToken.empty() || !base::CParser::IsNum(sToken.front()))
					throw CParserError(t_csz("Invalid general purpose register specifier"), oParser.GetPreviousPos(), sToken);
				nRegIdx = (t_index) std::stoul(sToken, &nPos);
				if (nPos != sToken.size() || nRegIdx == g_ciInvalid)
					throw CParserError(t_csz("Invalid general purpose register specifier"), oParser.GetPreviousPos(), sToken);

				if (eType != EValueType::Unknown)
					break;
			}
			else
			{
				if (compare(sToken, t_csz("B")))
				{
					eType = EValueType::Byte;
					eOprSize = core::EOprSize::Byte;
				}
				else if (compare(sToken, t_csz("W")))
				{
					eType = EValueType::Word;
					eOprSize = core::EOprSize::Word;
				}
				else if (compare(sToken, t_csz("DW")))
				{
					eType = EValueType::DWord;
					eOprSize = core::EOprSize::DWord;
				}
				else if (compare(sToken, t_csz("QW")))
				{
					eType = EValueType::QWord;
					eOprSize = core::EOprSize::QWord;
				}
				else if (compare(sToken, t_csz("CH")))
				{
					eType = EValueType::Char;
					eOprSize = core::EOprSize::Byte;
				}
				else
				{
					oParser.RevertPreviousParse();
					break;
				}

				if (nRegIdx != g_ciInvalid)
					break;

				if (i == 1)
					throw CParserError(t_csz("Operand size should be specified once"), oParser.GetPreviousPos(), sToken);
			}

			sToken = std::move(oParser.ParseToken());
		}

		if (nRegIdx == g_ciInvalid)
			throw CParserError(t_csz("General purpose register specifier is missing"), oParser.GetPreviousPos(), sToken);
		if (nRegIdx < 4)
			throw CError(t_csz("Content of the DW R0 register is readonly and could not modified"));
		if (nRegIdx >= core::SCPUStateBase::eRegisterPoolSize)
			throw CParserError(t_csz("Invalid general purpose register index"), oParser.GetPreviousPos(), sToken);

		// Alignment check
		uchar nOprSizeInBytes = core::OperandSize(eOprSize);
		if (nOprSizeInBytes == 0)
			throw CError(t_csz("Invalid operand size"));
		else if (nRegIdx % nOprSizeInBytes != 0)
			throw CError(base::toStr("Unaligned register index '%1'", nRegIdx));

		if (eType == EValueType::Unknown)
			// Take default value type 
			eType = (EValueType) nOprSizeInBytes;

		CValue oValue = std::move(FetchValue(oParser, eType));
		m_pDebugger->ChangeRegister(CDebugger::ERegType::GP, nRegIdx, oValue);
	}

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_assign(CCmdParser& oParser)
{
	CheckCPUStatus(false);
	t_string sName = std::move(oParser.ParseName());

	vm::SVariableInfo tVarInfo = m_pDebugger->GetVariableInfo(sName);
	CValue oValue = FetchValue(oParser, tVarInfo.eType, tVarInfo.nSize);

	if (oValue.GetCount() > tVarInfo.nSize)
		throw CError(t_csz("Error: Provided value array size exceeds variable array size"));

	m_pDebugger->ChangeVariable(sName, std::move(oValue));

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_backtrace(CCmdParser& oParser)
{
	CheckCPUStatus(true);
	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	PrintStackBacktrace();
}


void CCommander::cmd_watch(CCmdParser& oParser)
{
	CheckCPUStatus(false);
	t_string sToken = std::move(oParser.IsFinished(true) ? t_string() : oParser.ParseName());

	bool isHexadecimal = false;
	if (sToken.empty())
	{
		m_cout << std::endl;
		PrintWatchList(m_aWatchlist, isHexadecimal);
	}
	else if (compare(sToken, t_csz("all")))
	{
		sToken = std::move(oParser.IsFinished(true) ? t_string() : oParser.ParseName());
		if (compare(sToken, t_csz("hex")))
			isHexadecimal = true;
		else if (!sToken.empty())
			throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

		m_cout << std::endl;
		m_cout << "Variables:  --------------------------------------------------------------------------------" << std::endl;
		for (auto const& tInfo : m_pDebugger->Variables())
			PrintVariable(tInfo, isHexadecimal);
	}
	else if (compare(sToken, t_csz("add")))
	{
		if (oParser.IsFinished(true))
			throw CParserError(t_csz("Expecting variable name(s)"), oParser.GetCurrentPos(), {});

		do
		{
			sToken = std::move(oParser.ParseName());
			// Check variable name
			auto const& tInfo = m_pDebugger->GetVariableInfo(sToken);
			VASM_UNUSED(tInfo);
			m_aWatchlist.insert(std::move(sToken));
		} while (!oParser.IsFinished(true));
	}
	else if (compare(sToken, t_csz("rem")))
	{
		if (oParser.IsFinished(true))
			throw CParserError(t_csz("Expecting variable name(s)"), oParser.GetCurrentPos(), {});

		do
		{
			sToken = std::move(oParser.ParseName());
			m_aWatchlist.erase(sToken);
			// Check variable name
			auto const& tInfo = m_pDebugger->GetVariableInfo(sToken);
			VASM_UNUSED(tInfo);
		} while (!oParser.IsFinished(true));
	}
	else if (compare(sToken, t_csz("hex")))
	{
		isHexadecimal = true;

		m_cout << std::endl;
		if (oParser.IsFinished(true))
			PrintWatchList(m_aWatchlist, isHexadecimal);
		else
		{
			std::set<t_string> aWatchList;
			do
			{
				sToken = std::move(oParser.ParseName());
				aWatchList.insert(std::move(sToken));
			} while (!oParser.IsFinished(true));
			PrintWatchList(aWatchList, isHexadecimal);
		}
	}
	else
	{
		std::set<t_string> aWatchList;
		do
		{
			aWatchList.insert(std::move(sToken));
			sToken = std::move(oParser.IsFinished(true) ? t_string() : oParser.ParseName());
		} while (!sToken.empty());

		m_cout << std::endl;
		PrintWatchList(aWatchList, isHexadecimal);
	}
}
void CCommander::cmd_print(CCmdParser& oParser)
{
	CheckCPUStatus(false);
	t_string sToken = std::move(oParser.IsFinished(true) ? t_string() : oParser.ParseName());

	if (sToken.empty())
	{
		PrintCurrentState(true);
	}
	else if (compare(sToken, t_csz("dec")))
	{
		if (!oParser.IsFinished(true))
			throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

		PrintCurrentState(false);
	}
	else if (compare(sToken, t_csz("state")))
	{
		bool bDecimal = false;
		sToken = std::move(oParser.ParseToken());
		if (compare(sToken, t_csz("dec")))
			bDecimal = true;
		if (!oParser.IsFinished(true))
			throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

		PrintCurrentState(!bDecimal);
	}
	else if (compare(sToken, t_csz("cstack")))
	{
		if (!oParser.IsFinished(true))
			throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

		CheckCPUStatus(true);
		PrintStackBacktrace();
	}
	else if (compare(sToken, t_csz("sf")))
	{
		CheckCPUStatus(true);

		// Parse stack frame boundary specifiers 
		core::t_uoffset nArgsumentsOffset = 0;
		core::t_uoffset nStackFrameOffset = 0;
		while (!oParser.IsFinished(true))
		{
			t_char chSign = oParser.GetChar();
			if (chSign == t_char('-'))
			{
				if (nStackFrameOffset != 0)
					throw CError(t_csz("Error: Stack frame offset specified twice"));
				nStackFrameOffset = oParser.ParseNumber<core::t_word>();
			}
			else if (chSign == t_char('+'))
			{
				if (nArgsumentsOffset != 0)
					throw CError(t_csz("Error: Arguments offset specified twice"));
				nArgsumentsOffset = oParser.ParseNumber<core::t_word>();
			}
			else
			{
				oParser.RevertPreviousParse();
				throw CParserError(t_csz("Unexpected token"),
								   oParser.GetCurrentPos(), std::move(oParser.ParseToken()));
			}
		}

		PrintCurrentStackFrame(nArgsumentsOffset, nStackFrameOffset);
	}
	else if (compare(sToken, t_csz("bp")))
	{
		if (!oParser.IsFinished(true))
			throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

		PrintBreakPoints();
	}
	else if (compare(sToken, t_csz("code")) || compare(sToken, t_csz("data")) ||
			 compare(sToken, t_csz("memory")) || compare(sToken, t_csz("mem")))
	{
		bool const isCode = compare(sToken, t_csz("code"));
		bool const isData = compare(sToken, t_csz("data"));
		t_address nAddress = core::cnInvalidAddress;
		t_count nCount = 0;

		oParser.SkipWhiteSpaces();
		while (!oParser.IsFinished(true))
		{
			sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
			if (compare(sToken, t_csz("-at")))
			{
				if (nAddress != core::cnInvalidAddress)
					throw CParserError(t_csz("Multiple starting addresses specified"), oParser.GetPreviousPos(), sToken);
				if (isCode)
					nAddress = FetchAddress(oParser, EAddressHint::Code, true);
				else if (isData)
					nAddress = FetchAddress(oParser, EAddressHint::Data, true);
				else
					nAddress = FetchAddress(oParser, EAddressHint::Memory, true);
				if (nAddress == core::cnInvalidAddress)
					throw CParserError(t_csz("Invalid code location"), oParser.GetPreviousPos(), sToken);
			}
			else if (compare(sToken, t_csz("-c")) || compare(sToken, t_csz("-count")) ||
					 compare(sToken, t_csz("-sz")) || compare(sToken, t_csz("-size")))
			{
				if (nCount != 0)
					throw CParserError(t_csz("Multiple counts specified"), oParser.GetPreviousPos(), sToken);

				sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
				if (sToken.empty())
					throw CParserError(t_csz("Expecting count specifier: numeric value [1..255] or non zero Register"), oParser.GetCurrentPos(), sToken);

				if (sToken.at(0) == t_char('R'))
				{
					for (t_index i = 0; i < core::SCPUStateBase::eRegisterPoolSize; ++i)
					{
						if (compare(sToken, toStr("R%1", i)))
						{	// Take count from register
							nCount = t_count(m_pDebugger->CPUState().reg<core::t_byte>(i));
							break;
						}
					}
				}
				else
				{
					oParser.RevertPreviousParse();
					nCount = t_count(oParser.ParseNumber<core::t_word, 0>());
				}

				if (nCount == 0)
					throw CParserError(t_csz("Invalid count specified: expecting numeric value [1..255] or non zero Register"), oParser.GetPreviousPos(), sToken);
			}
			else
				throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
		}

		if (isCode && nAddress == core::cnInvalidAddress)
			nAddress = m_pDebugger->CPUState().nIP; // Take current IP
		else if (isData && nAddress == core::cnInvalidAddress)
			nAddress = 0; // Means the beginning of data section

		if (nAddress == core::cnInvalidAddress)
			throw CError(t_csz("Starting address is not specified"));
		if (!isCode && !isData && nCount == 0)
			throw CError(t_csz("Meory size is not specified"));

		if (isCode)
			PrintCode(nAddress, nCount);
		else if (isData)
			PrintData(nAddress, nCount);
		else
			PrintMemory(nAddress, nCount);
	}
	else if (compare(sToken, t_csz("ports")))
	{
		if (!oParser.IsFinished(true))
			throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

		PrintPortsInfo();
	}
	else
		throw CParserError(t_csz("Unrecognized option"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_dump(CCmdParser& oParser)
{
	CheckCPUStatus(false);

	t_string sToken;
	t_string sFilePath;

	uint nSection = 0; // 0 - all, 1 - memory, 2 - code, 3 - data, 4 - stack
	bool bFormat = 1;  // 0 - binary, 1 - text
	t_address nAddress = core::cnInvalidAddress;
	t_size nSize = 0;

	oParser.SkipWhiteSpaces();
	if (oParser.PeekChar() != t_char('-'))
		sToken = std::move(oParser.ParseToken());

	if (sToken.empty() || compare(sToken, t_csz("all")))
		nSection = 0;
	else if (compare(sToken, t_csz("memory")) || compare(sToken, t_csz("mem")))
		nSection = 1;
	else if (compare(sToken, t_csz("code")))
		nSection = 2;
	else if (compare(sToken, t_csz("data")))
		nSection = 3;
	else if (compare(sToken, t_csz("stack")))
		nSection = 4;
	else if (!sToken.empty())
		throw CParserError(t_csz("Unknwon section"), oParser.GetPreviousPos(), sToken);

	oParser.SkipWhiteSpaces();
	while (!oParser.IsFinished())
	{
		sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
		if (compare(sToken, t_csz("-f")) || compare(sToken, t_csz("-file")))
		{
			if (!sFilePath.empty())
				throw CParserError(t_csz("Multiple files specified"), oParser.GetPreviousPos(), sToken);
			sFilePath = oParser.ParseString(false);
		}
		else if (compare(sToken, t_csz("-format")))
		{
			sToken = std::move(oParser.ParseName());
			if (compare(sToken, t_csz("bin")))
				bFormat = 0;
			else if (compare(sToken, t_csz("text")))
				bFormat = 1;
			else
				throw CError(toStr("Error: Unknown format specifier '%1'", sToken));
		}
		else if (compare(sToken, t_csz("-at")))
		{
			t_csz pszAddressType = nullptr;
			if (nAddress != core::cnInvalidAddress)
				throw CParserError(t_csz("Multiple addresses specified"), oParser.GetPreviousPos(), sToken);
			switch (nSection)
			{
			case 1: 
				nAddress = FetchAddress(oParser, EAddressHint::Memory);
				pszAddressType = t_csz("memory");
				break;
			//case 2:
			//	nAddress = FetchAddress(oParser, EAddressHint::Code);
			//	pszAddressType = t_csz("code");
			//	break;
			//case 3:
			//	nAddress = FetchAddress(oParser, EAddressHint::Data);
			//	pszAddressType = t_csz("data");
			//	break;
			default:
				throw CError(toStr("Invalid use of the argument '%1'", sToken));
			}
			if (nAddress == core::cnInvalidAddress)
				throw CParserError(base::toStr("Invalid %1 addresses specified", pszAddressType), oParser.GetPreviousPos(), sToken);
		}
		else if (compare(sToken, t_csz("-sz")) || compare(sToken, t_csz("-size")))
		{
			if (nSection == 1)
				nSize = oParser.ParseNumber<t_size, 0>();
			else
				throw CError(toStr("Error: Invalid use of the argument '%1'", sToken));
			if (nSize == 0)
				throw CParserError(t_csz("Invalid memory size specified"), oParser.GetPreviousPos(), sToken);
		}
		else if (compare(sToken, t_csz("-l")) || compare(sToken, t_csz("-depth")))
		{
			if (nSection == 4)
				nSize = oParser.ParseNumber<t_size>();
			else
				throw CError(toStr("Invalid use of the argument '%1'", sToken));
			if (nSize == 0)
				throw CParserError(t_csz("Invalid stack depth specified"), oParser.GetPreviousPos(), sToken);
		}
		else
		{
			throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
		}
	}

	if (sFilePath.empty())
		sFilePath = t_csz("dump.txt"); 
	if (nAddress == core::cnInvalidAddress)
		nAddress = 0; // Bu default start from the neginning

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CParserError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	// Open file
	std::ofstream oDumpFile(sFilePath, bFormat ? (std::ios_base::out | std::ios::binary | std::ios::trunc) : (std::ios_base::out));
	if (oDumpFile.fail())
		throw CError(base::toStr("Failed to open output dump file '%1'", sFilePath));

	if (!m_sProgram.empty())
		oDumpFile << "VASM " << (nSection == 1 ? "Memory " : "") << "Dump, Unit: " << m_sProgram << std::endl << std::endl;
	else if (!m_aSourceCodes.empty())
	{
		oDumpFile << "VASM " << (nSection == 1 ? "Memory " : "") << "Dump, Sources: ";
		for (auto const& sSrcName : m_aSourceCodes)
			oDumpFile << '\'' << sSrcName << "' ";
		oDumpFile << std::endl << std::endl;
	}

	switch (nSection)
	{
	case 0:
		m_pDebugger->Dump(oDumpFile, bFormat);
		break;
	case 1:
		m_pDebugger->DumpMemory(oDumpFile, nAddress, nSize, bFormat);
		break;
	case 2:
		m_pDebugger->DumpCode(oDumpFile, 0, 0, bFormat);
		break;
	case 3:
		m_pDebugger->DumpData(oDumpFile, nAddress, nSize, bFormat);
		break;
	case 4:
		m_pDebugger->DumpStack(oDumpFile, nSize, bFormat);
		break;
	default:
		VASM_CHECK(false);
	}

	m_cout << "Content successfully dumped to file: '" << sFilePath << "'";
}

void CCommander::cmd_load(CCmdParser& oParser)
{
	t_string sPath = std::move(oParser.ParseString(false));
	if (sPath.empty())
		throw CParserError(t_csz("Expecting binary executable path"), oParser.GetCurrentPos(), sPath);
	if (!oParser.IsFinished(true))
		throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

	Load(sPath);
	m_cout << std::endl << "Program successfully loaded!" << std::endl;
}

void CCommander::cmd_open(CCmdParser& oParser)
{
	std::vector<t_string> aSourceCodeNames;
	do
	{
		t_string sPath = std::move(oParser.ParseString(false));
		if (sPath.empty())
			throw CParserError(t_csz("Expecting source file name"), oParser.GetCurrentPos(), sPath);
		aSourceCodeNames.push_back(std::move(sPath));
	} while (oParser.GetChar(true) == t_char(','));

	if (!oParser.IsFinished(true))
		throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

	Open(std::move(aSourceCodeNames));
	m_cout << std::endl << "Program successfully opened!" << std::endl;
}

void CCommander::cmd_reload(CCmdParser& oParser)
{
	if (!oParser.IsFinished(true))
		throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());
	if (m_sProgram.empty())
		throw CError(t_csz("Unable to reload: Current binary executable path is empty"));

	Load(m_sProgram);
	m_cout << std::endl << "Program successfully reloaded!" << std::endl;
}

void CCommander::cmd_reopen(CCmdParser& oParser)
{
	if (!oParser.IsFinished(true))
		throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());
	if (m_aSourceCodes.empty())
		throw CError(t_csz("Unable to reopen: No source codes available"));

	Open(m_aSourceCodes);
	m_cout << std::endl << "Program successfully reopened!" << std::endl;
}

void CCommander::cmd_exit(CCmdParser& oParser)
{
	if (!oParser.IsFinished(true))
		throw CParserError(t_csz("Unexpected token"), oParser.GetCurrentPos(), oParser.ParseToken());

	m_cout << std::endl << "Good Bye!" << std::endl;
	m_bProcessing = false;
}


//
//	Internal routines
//

t_address CCommander::FetchAddress(
	CCmdParser& oParser, EAddressHint eHint, bool bFromRegister)
{
	t_address nAddress = core::cnInvalidAddress;

	if (oParser.IsFinished(true))
		throw CError(t_csz("Error: expecting code location specifier. See help for details."));

	t_char ch = oParser.PeekChar();
	if (CCmdParser::IsAlpha(ch))
	{
		t_string sToken = std::move(oParser.ParseToken());
		t_char chNext = oParser.GetChar(true);
		if (chNext == ':')
		{
			if (eHint == EAddressHint::Data)
				throw CError(t_csz("Error: Code addresses specifier is not allowed in this case"));

			t_string sSrcUnit = std::move(sToken);
			t_index nLineNum = oParser.ParseNumber<t_address>(0, &sToken);
			nAddress = m_pDebugger->ResolveAddressFromSource(nLineNum, sSrcUnit);
			if (nAddress == core::cnInvalidAddress)
				throw CError(toStr("Error: Invalid source:line_number '%1'", sSrcUnit + ':' + sToken));
		}
		else if (chNext == '.')
		{
			if (eHint == EAddressHint::Data)
				throw CError(t_csz("Error: Code addresses specifier is not allowed in this case"));

			t_string sFuncName = std::move(sToken);
			sToken = std::move(oParser.ParseName());
			nAddress = m_pDebugger->ResolveAddressFromFunction(sFuncName, sToken);
			if (nAddress == core::cnInvalidAddress)
				throw CError(toStr("Error: Invalid function.label '%1'", sFuncName + '.' + sToken));
		}
		else
		{
			oParser.RevertPreviousParse();

			if (bFromRegister)
			{
				if (compare(sToken, t_csz("RIP")))
				{	// Take address from the RIP
					if (eHint == EAddressHint::Data)
						throw CError(t_csz("Error: Code addresses specifier is not allowed in this case"));

					nAddress = m_pDebugger->CPUState().nRIP;
				}
				else for (t_index i = core::SCPUStateBase::eARBaseIndex; i < core::SCPUStateBase::eAddressRegistersPoolSize; ++i)
				{
					if (compare(sToken, toStr("A%1", i - core::SCPUStateBase::eARBaseIndex)))
					{	// Take address from Address register
						nAddress = m_pDebugger->CPUState().rega<t_address>(i);
						break;
					}
				}
			}

			if (eHint != EAddressHint::Data)
			{
				if (nAddress == core::cnInvalidAddress)
					// Try label name
					nAddress = m_pDebugger->ResolveAddressFromLabel(sToken);
				if (nAddress == core::cnInvalidAddress)
					// Try function name
					nAddress = m_pDebugger->ResolveAddressFromFunction(sToken);
			}
			if (eHint != EAddressHint::Code)
			{
				if (nAddress == core::cnInvalidAddress)
					// Try variable name
					nAddress = m_pDebugger->ResolveAddressFromVariable(sToken);
			}

			if (nAddress == core::cnInvalidAddress && eHint == EAddressHint::Code)
				throw CError(toStr("Error: Invalid label or function name '%1'", sToken));
			else if (nAddress == core::cnInvalidAddress)
				throw CError(toStr("Error: Invalid label, function or variable name '%1'", sToken));
		}
	}
	else if (ch == t_char('0'))
	{
		t_string sToken;
		nAddress = oParser.ParseNumber<t_address, 0>(0, &sToken);
		if (eHint == EAddressHint::Code && !m_pDebugger->CheckCodeAddress(nAddress))
			throw CError(toStr("Error: Invalid code address '%1'", sToken));
		else if (eHint == EAddressHint::Data && !m_pDebugger->CheckVariableAddress(nAddress))
			throw CError(toStr("Error: Invalid variable address '%1'", sToken));
		else if (eHint == EAddressHint::CodeAndData && !m_pDebugger->CheckCodeAddress(nAddress) &&
				 !m_pDebugger->CheckVariableAddress(nAddress))
			throw CError(toStr("Error: Invalid code or variable address '%1'", sToken));
	}
	else if (CCmdParser::IsNum(ch))
	{
		if (eHint == EAddressHint::Data)
			throw CError(t_csz("Error: Code addresses specifier is not allowed in this case"));

		t_string sToken;
		t_index nLineNum = oParser.ParseNumber<t_address>(0, &sToken);
		nAddress = m_pDebugger->ResolveAddressFromSource(nLineNum, {});
		if (nAddress == core::cnInvalidAddress)
			throw CError(toStr("Error: Invalid line number '%1'", sToken));
	}
	else
	{
		t_string sToken = std::move(oParser.ParseToken());
		throw CError(toStr("Error: Invalid code location specifer '%1'", sToken));
	}

	return nAddress;
}

CValue CCommander::FetchValue(CCmdParser& oParser, EValueType eType, t_count nCount)
{
	CValue oValue;

	if (nCount == 1)
	{
		switch (eType)
		{
		case EValueType::Byte:
			oValue = CValue(oParser.ParseNumberSafe<core::t_byte, 0>());
			break;
		case EValueType::Word:
			oValue = CValue(oParser.ParseNumberSafe<core::t_word, 0>());
			break;
		case EValueType::DWord:
			oValue = CValue(oParser.ParseNumberSafe<core::t_dword, 0>());
			break;
		case EValueType::QWord:
			oValue = CValue(oParser.ParseNumberSafe<core::t_qword, 0>());
			break;
		case EValueType::Char:
			oValue = CValue(oParser.ParseCharacter());
			break;
		default:
			VASM_THROW_ERROR(t_csz("Can't parse value with unknwon type"));
		}
	}
	else
	{
		t_char const chSeparator = t_char(',');
		switch (eType)
		{
		case EValueType::Byte:
		{
			core::t_byte_array aBytes;
			do
			{
				aBytes.push_back(oParser.ParseNumberSafe<core::t_byte, 0>(chSeparator));
			} while (!oParser.IsFinished(true));
			oValue = std::move(CValue(std::move(aBytes)));
			break;
		}
		case EValueType::Word:
		{
			core::t_word_array aWords;
			do
			{
				aWords.push_back(oParser.ParseNumberSafe<core::t_word, 0>(chSeparator));
			} while (!oParser.IsFinished(true));
			oValue = std::move(CValue(std::move(aWords)));
			break;
		}
		case EValueType::DWord:
		{
			core::t_dword_array aDWords;
			do
			{
				aDWords.push_back(oParser.ParseNumberSafe<core::t_dword, 0>(chSeparator));
			} while (!oParser.IsFinished(true));
			oValue = std::move(CValue(std::move(aDWords)));
			break;
		}
		case EValueType::QWord:
		{
			core::t_qword_array aQWords;
			do
			{
				aQWords.push_back(oParser.ParseNumberSafe<core::t_qword, 0>(chSeparator));
			} while (!oParser.IsFinished(true));
			oValue = std::move(CValue(std::move(aQWords)));
			break;
		}
		case EValueType::Char:
		{
			t_string aChars = oParser.ParseString();
			oValue = std::move(CValue(std::move(aChars)));
			break;
		}
		default:
			VASM_THROW_ERROR(t_csz("Can't parse value with unknwon type"));
		}
	}

	return std::move(oValue);
}

void CCommander::PrintCurrentState(bool const bHexadecimal) const
{
	std::ostream& os = m_cout;
	vm::CProcessor::SState const& tState = m_pDebugger->CPUState();
	vm::CProcessor::EStatus eStatus = m_pDebugger->CPUStatus().eStatus;

	if (eStatus == vm::CProcessor::EStatus::NotInitialized)
	{
		os << "Processor is not initialized!" << std::endl;
		return;
	}

	m_pDebugger->DumpState(os, bHexadecimal);

	if (!m_aWatchlist.empty())
		PrintWatchList(m_aWatchlist, bHexadecimal);

	if (eStatus == vm::CProcessor::EStatus::Finished)
	{
		os << "Program execution finished!" << std::endl;
	}
	else if (eStatus == vm::CProcessor::EStatus::Failed)
	{
		os << "Command: -------------------------------------------------------------------------" << std::endl;

		CDebugger::SCodeLineInfo tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nCIP);
		PrintCodeInfo(os, tCLInfo, false, true);
		PrintCommand(os, tState.nCIP);

		os << "----------------------------------------------------------------------------------" << std::endl;
		os << m_pDebugger->CPUStatus().oErrorInfo.sErrorMessage;
	}
	else if (!bool(uint(eStatus) & uint(vm::CProcessor::EStatus::Ready)))
	{
		os << "Unexpected CPU state!" << std::endl;
	}
	else if (tState.nIP >= tState.cnCodeSize)
	{
		os << "Program execution is out of control!" << std::endl;
	}
	else
	{
		CDebugger::SCodeLineInfo tCLInfo;
		if (tState.nCIP != tState.nIP)
		{
			os << "Prev command: --------------------------------------------------------------------" << std::endl;
			tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nCIP);
			PrintCodeInfo(os, tCLInfo, false, true);
			PrintCommand(os, tState.nCIP);
		}

		os << "Next command: --------------------------------------------------------------------" << std::endl;
		tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nIP);
		PrintCodeInfo(os, tCLInfo, false, true);
		PrintCommand(os, tState.nIP);
	}
}

void CCommander::PrintStackBacktrace() const
{
	auto aCodeLines = std::move(m_pDebugger->GetFunctionCallStack());

	m_cout << std::resetiosflags(0);
	m_cout << "Function call stack: -------------------------------------------------------------" << std::endl;
	for (auto const& tInfo : aCodeLines)
	{
		PrintCodeInfo(m_cout, tInfo, true, true);
	}
}

void CCommander::PrintPortsInfo() const
{
	auto aPortsInfo = std::move(m_pDebugger->GetPortsInfo());

	m_cout << std::resetiosflags(0);
	m_cout << "Port Ranges: ---------------------------------------------------------------------" << std::endl;
	for (auto const& tInfo : aPortsInfo)
	{
		m_cout << "[" << std::setfill('0') << std::setw(3) << tInfo.first.first;
		m_cout << " - " << std::setfill('0') << std::setw(3) << tInfo.first.second << "] ";
		m_cout << tInfo.second << std::endl;
	}
}

void CCommander::PrintBreakPoints() const
{
	auto aCodeLines = std::move(m_pDebugger->GetBreakPoints());

	m_cout << std::resetiosflags(0);
	m_cout << "Break Points: ----------------------------------------------------------------------------" << std::endl;
	for (auto const& tInfo : aCodeLines)
	{
		PrintCodeInfo(m_cout, tInfo, true, true);
	}
}

void CCommander::PrintCode(t_address const nBaseAddress, t_count nInstrCount) const
{
	CDebugger::SCodeLineInfo tCLInfo = m_pDebugger->GetCodeLineInfo(nBaseAddress);
	PrintCodeInfo(m_cout, tCLInfo, false, false);
	if (tCLInfo.nAddress == core::cnInvalidAddress)
		return; // No valid code

	t_address nCurrAddress = tCLInfo.nAddress; // Take validated address
	if (nInstrCount == 0)
	{	// Determine instructions count automatically
		auto nEndAddress = core::cnInvalidAddress;
		if (tCLInfo.nFuncSize > 0)
			// Take current function last line as the end address
			nEndAddress = tCLInfo.nAddress - tCLInfo.nRelOffset + tCLInfo.nFuncSize;

		// Starting from unknown location, go while instructions are valid 
		while (nCurrAddress != core::cnInvalidAddress && nCurrAddress < nEndAddress)
		{
			++nInstrCount;
			nCurrAddress = m_pDebugger->NextCodeAddress(nCurrAddress);
		}
		if (nCurrAddress == core::cnInvalidAddress)
			--nInstrCount; // Exclude last invalid command
	}

	// Start from validated address
	nCurrAddress = tCLInfo.nAddress;
	// Break at function boundaries to print code info
	auto nBreakAddress = tCLInfo.nAddress - tCLInfo.nRelOffset + tCLInfo.nFuncSize;
	while (nInstrCount > 0 && nCurrAddress != core::cnInvalidAddress &&
		   nCurrAddress < m_pDebugger->CPUState().cnCodeSize)
	{
		if (nCurrAddress >= nBreakAddress && nCurrAddress > tCLInfo.nAddress)
		{
			tCLInfo = m_pDebugger->GetCodeLineInfo(nCurrAddress);
			if (tCLInfo.nFuncSize > 0)
				PrintCodeInfo(m_cout, tCLInfo, false, false);
			nBreakAddress = tCLInfo.nAddress - tCLInfo.nRelOffset + tCLInfo.nFuncSize;
		}

		nCurrAddress = PrintCommand(m_cout, nCurrAddress);
		--nInstrCount;
	}
}

void CCommander::PrintData(t_address const nBaseAddress, t_count nSizeInBytes) const
{
	m_cout << "Data (0x): -----------------------------------------------------------------------" << std::endl;
	m_pDebugger->DumpData(m_cout, nBaseAddress, nSizeInBytes, true, false);
}

void CCommander::PrintMemory(t_address const nBaseAddress, t_count nSizeInBytes) const
{
	m_cout << "Memory (0x): ---------------------------------------------------------------------" << std::endl;
	m_pDebugger->DumpMemory(m_cout, nBaseAddress, nSizeInBytes, true);
}

void CCommander::PrintCurrentStackFrame(
	core::t_uoffset nArgumentsOffset, core::t_uoffset nStackFrameOffset) const
{
	auto const& tState = m_pDebugger->CPUState();
	t_address nCurrSP = (nStackFrameOffset) == 0 ? tState.nSP : tState.nSF - nStackFrameOffset;
	t_address nCurrSF = tState.nSF;
	auto tReturnLinkImage = m_pDebugger->GetReturnLinkFromStackFrame();

	// Round to 8 byte boundaries
	t_address nSPRounded = nCurrSP - nCurrSP % 8;
	t_address nSFRounded = nCurrSF + (nCurrSF % 8 == 0 ? 0 : (8 - nCurrSF % 8));

	std::ostream& os = m_cout;
	os << "Current Stack Frame:  allocated space (SF - SP) = " << std::dec << (tState.nSF - tState.nSP) << "  ----------------------------" << std::endl;
	os << "SP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nCurrSP;
	if (tReturnLinkImage.first == core::cnInvalidAddress)
		os << "   Ret IP = (unavailable)";
	else
		os << "   Ret IP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tReturnLinkImage.first;
	os << std::endl;
	os << "SF = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nCurrSF;
	if (tReturnLinkImage.first == core::cnInvalidAddress)
		os << "   Ret SF = (unavailable)";
	else
		os << "   Ret SF = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tReturnLinkImage.second;
	os << std::endl;

	core::CMemory const& oMemory = m_pDebugger->Memory();

	if (nArgumentsOffset != 0)
	{
		t_address nArgBegin = nCurrSF + 2 * sizeof(t_address);
		t_address nArgEnd = nArgBegin + nArgumentsOffset;;
		t_address nArgBeginRounded = nArgBegin - (nArgBegin % 8 == 0 ? 0 : (nArgBegin % 8));
		t_address nArgEndRounded = nArgEnd + (nArgEnd % 8 == 0 ? 0 : (8 - nArgEnd % 8));
		os << "Arguments   ----------------------------------------------------------------------" << std::endl;
		for (t_address nSP = nArgBeginRounded; nSP < nArgEndRounded && nSP < tState.cnStackLBound; nSP += 8)
		{
			os << "SF +" << std::dec << std::setfill(' ') << std::setw(3) << nSP - nCurrSF << ": ";
			for (auto i = 8; i > 0; --i)
			{
				t_address nCurr = nSP + t_address(i - 1);
				if (nCurr >= nArgEnd || nCurr < nArgBegin)
					os << ".. ";
				else
				{
					uint8 uValue;
					oMemory.ReadAt<uint8>(nCurr, uValue);
					os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +uValue << " ";
				}

				if (i == 5)
					os << " ";
			}
			os << std::endl;
		}

	}

	os << "Local Variables   ----------------------------------------------------------------" << std::endl;
	for (t_address nSP = nSFRounded - 1; nSP >= nSPRounded && nSP >= tState.cnStackUBound; --nSP)
	{
		if ((nSP + 1) % 8 == 0)
			os << "SF -" << std::dec << std::setfill(' ') << std::setw(3) << nCurrSF - (nSP - 7) << ": ";

		if (nSP < nCurrSP || nSP >= nCurrSF)
			os << ".. ";
		else
		{
			uint8 uValue;
			oMemory.ReadAt<uint8>(nSP, uValue);
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +uValue << " ";
		}

		if ((nSP + 1) % 8 == 5)
			os << " ";
		else if ((nSP + 1) % 8 == 1)
			os << std::endl;
	}
}

void CCommander::PrintWatchList(std::set<t_string> const& aWatchlist, bool const bHexadecimal) const
{
	m_cout << "Watch:  --------------------------------------------------------------------------" << std::endl;
	for (t_string const& sName : aWatchlist)
	{
		auto const& tInfo = m_pDebugger->GetVariableInfo(sName);
		PrintVariable(tInfo, bHexadecimal);
	}
}

void CCommander::PrintVariable(vm::SVariableInfo const& tInfo, bool const bHexadecimal) const
{
	std::ostream& os = m_cout;
	os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tInfo.nAddress;
	os << ": " << core::CValue::TypeToCStr(tInfo.eType) << ' ' << tInfo.sName;
	if (tInfo.nSize > 1)
		os << '(' << std::dec << std::setw(1) << tInfo.nSize << ')';

	core::CMemory const& oMemory = m_pDebugger->Memory();
	core::CValue oValue = std::move(oMemory.ReadValue(tInfo.nAddress, tInfo.eType, tInfo.nSize));
	os << " = " << (bHexadecimal ? std::hex : std::dec) << oValue << std::endl;
}

void CCommander::PrintCodeInfo(
	std::ostream& os, CDebugger::SCodeLineInfo const& tInfo,
	bool const bIncludeBaseAddress, bool const bIncludeOffset) const
{
	if (bIncludeBaseAddress)
		os << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tInfo.nAddress << ": ";
	else if (!tInfo.sFuncName.str().empty())
		os << "Func: ";
	else
		os << ": ";
	if (tInfo.sFuncName != nullptr)
		os << tInfo.sFuncName.str() << "  ";
	if (bIncludeOffset)
		os << "+0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tInfo.nRelOffset;
	if (tInfo.sUnitName != nullptr)
		os << "  Unit: " << tInfo.sUnitName.str() << ",  Line: " << std::setw(0) << std::dec << tInfo.nLineNum;
	os << std::endl;
}

t_address CCommander::PrintCommand(std::ostream& os, t_address const nCmdAddress) const
{
	std::string sBinaryCommand;
	t_address nNextAddress = nCmdAddress;
	std::string sCommand = std::move(m_pDebugger->GetDisassembledCommand(nNextAddress, &sBinaryCommand));
	os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nCmdAddress << ": "
		<< sCommand << t_csz("  \t# ") << sBinaryCommand << std::endl;
	return nNextAddress;
}

void CCommander::CheckMachine() const
{
	VASM_CHECK_PTR(m_pMachine);
	if (!m_pMachine->IsValid())
		VASM_THROW_ERROR(t_csz("Could not run: machine is not initialized"));
}

void CCommander::CheckCPUStatus(bool bCheckForReady) const
{
	CheckMachine();
	VASM_CHECK_PTR(m_pDebugger);
	vm::CProcessor::EStatus eStatus = m_pDebugger->CPUStatus().eStatus;
	if (eStatus == vm::CProcessor::EStatus::NotInitialized)
		VASM_THROW_ERROR(t_csz("Failed to complete command: CPU is not initialized"));
	else if (eStatus == vm::CProcessor::EStatus::Running)
		VASM_THROW_ERROR(t_csz("Failed to complete command: CPU currently is running"));
	else if (bCheckForReady && !bool(uint(eStatus) & uint(vm::CProcessor::EStatus::Ready)))
		VASM_THROW_ERROR(t_csz("Failed to complete command: CPU is not ready"));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace dev
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	{"bt",		&CCommander::cmd_backtrace},
	{"d",		&CCommander::cmd_dump},
	{"dump",	&CCommander::cmd_dump},
	{"p",		&CCommander::cmd_dump},
	{"print",	&CCommander::cmd_print},
	{"watch",	&CCommander::cmd_watch},

	{"l",		&CCommander::cmd_load},
	{"load",	&CCommander::cmd_load},
	{"o",		&CCommander::cmd_open},
	{"open",	&CCommander::cmd_open},
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
			CCmdParser oParser(sCmdLine);
			t_string sCommand = std::move(oParser.ParseToken());

			auto it = ms_mapCmdHandlers.find(sCommand);
			if (it == ms_mapCmdHandlers.end())
				throw CError(t_csz("Unrecognized command"), oParser.GetPreviousPos(), sCommand);

			fnCmdHandler pfn = it->second;
			pfn(this, oParser);
		}
		catch (base::CException const& err)
		{
			m_cout << err.GetErrorMsg(true);
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
		VASM_THROW_ERROR(base::toStr("Failed to open binary file '%1'", sExecPath));

	m_pMachine->Init(oBinFile);
	m_sProgram = sExecPath;
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

	std::stringstream oTemp;
	oTemp << t_csz("Compiled from:");
	for (auto const& temp : aSourceCodeNames)
		oTemp << " '" << temp << "'";
	m_sProgram = oTemp.str();
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
	m_cout << "r|run         [-dbg yes|no]                   Runs program from curent IP (either in debug or nondebug mode)"	<< std::endl;
	m_cout << "              [-from line_num|label|0xAddress]"																	<< std::endl;
	m_cout << "              [-to line_num|label|0xAddress]"																	<< std::endl;
	m_cout << "              [async]"																							<< std::endl;
	m_cout << "t|trace       [Num=1]                         Step In, executes Num instructions"								<< std::endl;
	m_cout << "n|next        [Num=1]                         Step Over, executes Num instructions by overcoming function calls"	<< std::endl;
	m_cout << "so|stepout                                    Step Out, Executes until current function will return"				<< std::endl;
	m_cout << "pause                                         Pauses curent execution"											<< std::endl;
	m_cout << "reset                                         Resets CPU to initial state"										<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "bp            [src:line_num|label|func_name]  Sets debug break point at specified position"						<< std::endl;
	m_cout << "rbp           [src:line_num|label|func_name]  Removes specified breakpoint"										<< std::endl;
	m_cout << "              [all]"																								<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "set           [IP src:line|label|func_name]   Sets specified value to the specified register"					<< std::endl;
	m_cout << "              [A(idx) src:line|label|func_name|var_name|0xAddress]"												<< std::endl;
	m_cout << "              [R(idx) [B|W|DW|QW|CH] +/-num|0xValue]"															<< std::endl;
	m_cout << "              [CF|ZF|SF|OF 0|1]"																					<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "a|assign      [var_name +/-num|0xValue[,...]  Assigns value to the specified variable or memory location"		<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "bt                                            Prints current backtrace, see also 'print callstack'"				<< std::endl;
	m_cout << "d|dump        -f|file file_path               Dumps entire memory content or part of it"							<< std::endl;
	m_cout << "              -format text|bin"																					<< std::endl;
	m_cout << "       memory [-at 0xAddress]"																					<< std::endl;
	m_cout << "              [-sz|size byte_count]"																				<< std::endl;
	m_cout << "       code   [-at src:line|label|func_name]  Dumps only code started from beginning or specified address"		<< std::endl;
	m_cout << "              [-sz|size byte_count]"																				<< std::endl;
	m_cout << "       data                                   Dumps data content only"											<< std::endl;
	m_cout << "       stack  [-depth Num=MAX]                Dumps the content of the stack with specified call depth"			<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "p|print [state]                               Prints CPU current state"											<< std::endl;
	m_cout << "        callstack                             Prints function call stack (same as backtrace)"					<< std::endl;
	m_cout << "        ports                                 Prints I/O Devices and Port ranges occupied by them"				<< std::endl;
	m_cout << "        code  [-sz|size instruction_count]    Prints code started from the current IP or specifieid address"		<< std::endl;
	m_cout << "              [-at srs:line|label|func_name|0xAddress]"															<< std::endl;
	m_cout << "        bp                                    Prints Break points list"											<< std::endl;
	m_cout << "        sf                                    Prints current stack frame"										<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "w|watch       [var_name [...]]                Prints current value of the specified variables"					<< std::endl;
	m_cout << "        add   var_name                        Ads specified variable to the watch list"							<< std::endl;
	m_cout << "        rem   var_name                        Removes specified variable from the watch list"					<< std::endl;
	m_cout << ""																												<< std::endl;
	m_cout << "l|load        exec_file_path                  Opens specified binary exectuavle file and loads into memory"		<< std::endl;
	m_cout << "o|open        source_file_name[,...]          Opens specifeid source file(s), compiles & loads them into memory"	<< std::endl;
	m_cout << "x|exit                                        Exits the program"													<< std::endl;
}

void CCommander::cmd_info(CCmdParser& oParser)
{
	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	if (!m_pMachine->IsValid())
		m_cout << std::endl << "Machine is not initialized, no binary executable." << std::endl;
	else
	{
		m_cout << "Current program: " << m_sProgram << std::endl;
		switch (m_pDebugger->CPUStatus().eStatus)
		{
		case vm::CProcessor::EStatus::NotInitialized:
			m_cout << "VM is not initialized" << std::endl;
			break;
		case vm::CProcessor::EStatus::Ready:
			m_cout << "VM is Ready" << std::endl;
			break;
		case vm::CProcessor::EStatus::Break:
			m_cout << "VM hit breakpoint" << std::endl;
			break;
		case vm::CProcessor::EStatus::Stopped:
			m_cout << "VM stopped" << std::endl;
			break;
		case vm::CProcessor::EStatus::Failed:
			m_cout << "VM is failed" << std::endl;
			m_cout << "Fail reason: " << m_pDebugger->CPUStatus().oErrorInfo.GetErrorMsg() << std::endl;
			break;
		case vm::CProcessor::EStatus::Running:
			m_cout << "VM currently is running" << std::endl;
			break;
		case vm::CProcessor::EStatus::Finished:
			m_cout << "VM is finished" << std::endl;
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
			nAddressFrom = FetchCodeAddress(oParser);
		}
		else if (compare(sToken, t_csz("-to")))
		{
			nAddressTo = FetchCodeAddress(oParser);
		}
		else
		{
			throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
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
	if (!m_aWatchlist.empty())
		PrintWatchList(m_aWatchlist);
}

void CCommander::cmd_step_in(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_size nCount = 1;
	oParser.SkipWhiteSpaces();
	if (!oParser.IsFinished())
	{
		t_string sToken = std::move(oParser.ParseToken());
		if (!compare(sToken, t_csz("Num")) || oParser.GetChar(true) != t_char('='))
			throw CError(t_csz("Expecting [Num = count]"), oParser.GetPreviousPos(), sToken);

		nCount = oParser.ParseNumber<t_size>();

		sToken = std::move(oParser.ParseToken());
		if (!sToken.empty())
			throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
	}

	m_pDebugger->StepIn(nCount);
	PrintCurrentState();
	if (!m_aWatchlist.empty())
		PrintWatchList(m_aWatchlist);
}

void CCommander::cmd_step_over(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_size nCount = 1;
	oParser.SkipWhiteSpaces();
	if (!oParser.IsFinished())
	{
		t_string sToken = std::move(oParser.ParseToken());
		if (!compare(sToken, t_csz("Num")) || oParser.GetChar(true) != t_char('='))
			throw CError(t_csz("Expecting [Num = count]"), oParser.GetPreviousPos(), sToken);

		nCount = oParser.ParseNumber<t_size>();

		sToken = std::move(oParser.ParseToken());
		if (!sToken.empty())
			throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
	}

	m_pDebugger->StepOver(nCount);
	PrintCurrentState();
	if (!m_aWatchlist.empty())
		PrintWatchList(m_aWatchlist);
}

void CCommander::cmd_step_out(CCmdParser& oParser)
{
	CheckCPUStatus(true);

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->StepOut();
	PrintCurrentState();
	if (!m_aWatchlist.empty())
		PrintWatchList(m_aWatchlist);
}

void CCommander::cmd_stop(CCmdParser& oParser)
{
	CheckMachine();

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->Stop();
}

void CCommander::cmd_reset(CCmdParser& oParser)
{
	CheckMachine();

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	m_pDebugger->ResetProgram();
}

void CCommander::cmd_bp_set(CCmdParser& oParser)
{
	CheckCPUStatus();

	t_address nAddress = FetchCodeAddress(oParser);
	m_pDebugger->SetBreakPoint(nAddress);

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_bp_remove(CCmdParser& oParser)
{
	CheckCPUStatus();

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

		t_address nAddress = FetchCodeAddress(oParser);
		m_pDebugger->RemoveBreakPoint(nAddress);
	}

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_set(CCmdParser& oParser)
{
	CheckCPUStatus();
	t_string sToken = std::move(oParser.ParseName());

	if (compare(sToken, t_csz("IP")))
	{
		t_address nAddress = FetchCodeAddress(oParser);
		if (!m_pDebugger->ChangeIP(nAddress))
			VASM_THROW_ERROR(t_csz("Failed to change IP Register"));
	}
	else if (sToken.front() == t_char('A') || sToken.front() == t_char('a'))
	{
		std::size_t nPos;
		sToken.erase(sToken.cbegin());
		t_index nRegIdx = (t_index) std::stoul(sToken, &nPos);
		if (nPos != sToken.size() || nRegIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
			throw CError(t_csz("Invalid address register index"), oParser.GetPreviousPos(), sToken);

		t_address nAddress = FetchCodeAddress(oParser);
		m_pDebugger->ChangeRegister(CDebugger::ERegType::ADR, nRegIdx, {nAddress});
	}
	else if (compare(sToken, t_csz("")))
	{
		std::size_t nPos;
		sToken.erase(sToken.cbegin());
		t_index nRegIdx = (t_index) std::stoul(sToken, &nPos);
		if (nPos != sToken.size() || nRegIdx >= core::SCPUStateBase::eGeneralPurposeRegisterPoolSize)
			throw CError(t_csz("Invalid general purpose register index"), oParser.GetPreviousPos(), sToken);

		EValueType eType = EValueType::Unknown;
		sToken = std::move(oParser.ParseToken());
		if (compare(sToken, t_csz("B")))
			eType = EValueType::Byte;
		else if (compare(sToken, t_csz("W")))
			eType = EValueType::Word;
		else if (compare(sToken, t_csz("DW")))
			eType = EValueType::DWord;
		else if (compare(sToken, t_csz("QW")))
			eType = EValueType::QWord;
		else if (compare(sToken, t_csz("CH")))
			eType = EValueType::Char;
		else
			oParser.RevertPreviousParse();

		CValue oValue = std::move(FetchValue(oParser, eType));
		m_pDebugger->ChangeRegister(CDebugger::ERegType::GP, nRegIdx, oValue);
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
	else
	{
		throw CError(t_csz("Unknown argument"), oParser.GetPreviousPos(), sToken);
	}

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_assign(CCmdParser& oParser)
{
	CheckCPUStatus();
	t_string sName = std::move(oParser.ParseName());

	vm::SVariableInfo tVarInfo = m_pDebugger->GetVariableInfo(sName);
	CValue oValue = FetchValue(oParser, tVarInfo.eType, tVarInfo.nSize);

	if (oValue.GetCount() > tVarInfo.nSize)
		throw CError(t_csz("Provided value array size exceeds variable array size"), oParser.GetCurrentPos(), sName);

	m_pDebugger->ChangeVariable(sName, std::move(oValue));

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_backtrace(CCmdParser& oParser)
{
	CheckCPUStatus();
	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	PrintStackBacktrace();
}

void CCommander::cmd_dump(CCmdParser& oParser)
{
	CheckCPUStatus();

	t_string sToken;
	t_string sFilePath;

	uint nSection = 0; // 0 - all 1 - memory, 2 - code, 3 - data, 4 - stack
	uint nFormat = 0;  // 0 - binary, 1 - text
	t_address nAddress = 0;
	t_size nSize = 0;

	oParser.SkipWhiteSpaces();
	if (oParser.PeekChar() != t_char('-'))
		sToken = std::move(oParser.ParseName());

	if (compare(sToken, t_csz("memory")))
		nSection = 1;
	else if (compare(sToken, t_csz("code")))
		nSection = 2;
	else if (compare(sToken, t_csz("data")))
		nSection = 3;
	else if (compare(sToken, t_csz("stack")))
		nSection = 4;
	else if (!sToken.empty())
		throw CError(t_csz("Unknwon section"), oParser.GetPreviousPos(), sToken);

	oParser.SkipWhiteSpaces();
	while (!oParser.IsFinished())
	{
		sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
		if (compare(sToken, t_csz("-f")) || compare(sToken, t_csz("-file")))
		{
			sFilePath = oParser.ParseString(false);
		}
		else if (compare(sToken, t_csz("-format")))
		{
			sToken = std::move(oParser.ParseName());
			if (compare(sToken, t_csz("bin")))
				nFormat = 0;
			else if (compare(sToken, t_csz("text")))
				nFormat = 1;
			else
				throw CError(t_csz("Unknown format specifier"), oParser.GetPreviousPos(), sToken);
		}
		else if (compare(sToken, t_csz("-at")))
		{
			if (nSection == 1 || nSection == 3)
				nAddress = oParser.ParseNumber<t_address, 16>();
			else if (nSection == 2)
				nAddress = FetchCodeAddress(oParser);
			else
				throw CError(t_csz("Invalid use of the argument"), oParser.GetPreviousPos(), sToken);
		}
		else if (compare(sToken, t_csz("-sz")) || compare(sToken, t_csz("-size")))
		{
			if (nSection > 0 && nSection < 4)
				nSize = oParser.ParseNumber<t_size, 0>();
			else
				throw CError(t_csz("Invalid use of the argument"), oParser.GetPreviousPos(), sToken);
		}
		else if (compare(sToken, t_csz("-depth")))
		{
			if (nSection == 4)
				nSize = oParser.ParseNumber<t_size>();
			else
				throw CError(t_csz("Invalid use of the argument"), oParser.GetPreviousPos(), sToken);
		}
		else
		{
			throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
		}
	}

	if (sFilePath.empty())
		throw CError(t_csz("File specifier is missing"), oParser.GetPreviousPos(), sToken);

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	// Open file
	std::ofstream oDumpFile(sFilePath, (nFormat == 0) ? (std::ios_base::out | std::ios::binary | std::ios::trunc) : (std::ios_base::out));
	if (oDumpFile.fail())
		VASM_THROW_ERROR(base::toStr("Failed to open output dump file '%1'", sFilePath));

	switch (nSection)
	{
	case 0:
		m_pDebugger->Dump(oDumpFile, nFormat);
	case 1:
		m_pDebugger->DumpMemory(oDumpFile, nAddress, nSize, nFormat);
		break;
	case 2:
		m_pDebugger->DumpCode(oDumpFile, nAddress, nSize, nFormat);
		break;
	case 3:
		m_pDebugger->DumpData(oDumpFile, nAddress, nSize, nFormat);
		break;
	case 4:
		m_pDebugger->DumpStack(oDumpFile, nSize, nFormat);
		break;
	default:
		VASM_ASSERT(false);
	}
}

void CCommander::cmd_print(CCmdParser& oParser)
{
	CheckCPUStatus();

	t_string sToken = std::move(oParser.ParseName());

	if (sToken.empty() || compare(sToken, t_csz("state")))
	{
		PrintCurrentState();
	}
	else if (compare(sToken, t_csz("callstack")))
	{
		PrintStackBacktrace();
	}
	else if (compare(sToken, t_csz("ports")))
	{
		PrintPortsInfo();
	}
	else if (compare(sToken, t_csz("bp")))
	{
		PrintBreakPoints();
	}
	else if (compare(sToken, t_csz("sf")))
	{
		PrintCurrentStackFrame();
	}
	else if (compare(sToken, t_csz("code")))
	{
		t_address nAddress = 0;
		t_size nSize = 0;

		oParser.SkipWhiteSpaces();
		while (!oParser.IsFinished())
		{
			sToken = std::move(oParser.ParseToken(base::CParser::IsSpace));
			if (compare(sToken, t_csz("-at")))
				nAddress = FetchCodeAddress(oParser);
			else if (compare(sToken, t_csz("-sz")) || compare(sToken, t_csz("-size")))
				nSize = oParser.ParseNumber<t_size, 0>();
			else
				throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
		}

		PrintCode(nAddress, nSize);
	}
	else
		throw CError(t_csz("Unknwon section"), oParser.GetPreviousPos(), sToken);

	sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);
}

void CCommander::cmd_watch(CCmdParser& oParser)
{
	CheckCPUStatus();
	t_string sToken = std::move(oParser.ParseName());

	if (sToken.empty())
	{
		PrintWatchList(m_aWatchlist);
	}
	else if (compare(sToken, t_csz("all")))
	{
		m_cout << std::endl;
		m_cout << "Variables:  --------------------------------------------------------------------------------" << std::endl;
		for (auto const& tInfo : m_pDebugger->Variables())
			PrintVariable(tInfo);
	}
	else if (compare(sToken, t_csz("add")))
	{
		sToken = std::move(oParser.ParseName());
		if (sToken.empty())
			throw CError(t_csz("Expecting variable name"), oParser.GetCurrentPos(), sToken);

		// Check variable name
		auto const& tInfo = m_pDebugger->GetVariableInfo(sToken);
		VASM_UNUSED(tInfo);

		m_aWatchlist.insert(std::move(sToken));
	}
	else if (compare(sToken, t_csz("rem")))
	{
		sToken = std::move(oParser.ParseName());
		if (sToken.empty())
			throw CError(t_csz("Expecting variable name"), oParser.GetCurrentPos(), sToken);

		// Check variable name
		auto const& tInfo = m_pDebugger->GetVariableInfo(sToken);
		VASM_UNUSED(tInfo);

		m_aWatchlist.erase(sToken);
	}
	else
	{
		std::set<t_string> aWatchList;
		do
		{
			aWatchList.insert(std::move(sToken));
			sToken = std::move(oParser.ParseName());
		} while (!sToken.empty());

		PrintWatchList(aWatchList);
	}
}

void CCommander::cmd_load(CCmdParser& oParser)
{
	t_string sPath = std::move(oParser.ParseString(false));
	if (sPath.empty())
		throw CError(t_csz("Expecting binary executable path"), oParser.GetCurrentPos(), sPath);
	Load(sPath);
}

void CCommander::cmd_open(CCmdParser& oParser)
{
	std::vector<t_string> aSourceCodeNames;
	do
	{
		t_string sPath = std::move(oParser.ParseString(false));
		if (sPath.empty())
			throw CError(t_csz("Expecting source file name"), oParser.GetCurrentPos(), sPath);
		aSourceCodeNames.push_back(std::move(sPath));
	} while (oParser.GetChar(true) == t_char(','));

	t_string sToken = std::move(oParser.ParseToken());
	if (!sToken.empty())
		throw CError(t_csz("Unexpected token"), oParser.GetPreviousPos(), sToken);

	Open(std::move(aSourceCodeNames));
}

void CCommander::cmd_exit(CCmdParser&)
{
	m_cout << std::endl << "Good Bye!" << std::endl;
	m_bProcessing = false;
}


//
//	Internal routines
//

t_address CCommander::FetchCodeAddress(CCmdParser& oParser)
{
	t_address nAddress = core::cnInvalidAddress;

	oParser.SkipWhiteSpaces();
	t_char ch = oParser.PeekChar();
	if (CCmdParser::IsAlpha(ch))
	{
		t_string sToken = std::move(oParser.ParseToken());
		t_char chNext = oParser.GetChar(true);
		if (chNext == ':')
		{
			t_string sSrcUnit = std::move(sToken);
			t_index nLineNum = oParser.ParseNumber<t_address>(0, &sToken);
			nAddress = m_pDebugger->ResolveAddressFromSource(nLineNum, sSrcUnit);
			if (nAddress == core::cnInvalidAddress)
				throw CError(t_csz("Invalid source:line_number"), oParser.GetPreviousPos(), sSrcUnit + ':' + sToken);
		}
		else if (chNext == '.')
		{
			t_string sFuncName = std::move(sToken);
			sToken = std::move(oParser.ParseName());
			nAddress = m_pDebugger->ResolveAddressFromFunction(sFuncName, sToken);
			if (nAddress == core::cnInvalidAddress)
				throw CError(t_csz("Invalid function.label"), oParser.GetPreviousPos(), sFuncName + '.' + sToken);
		}
		else
		{
			oParser.RevertPreviousParse();
			nAddress = m_pDebugger->ResolveAddressFromLabel(sToken);
			if (nAddress == core::cnInvalidAddress)
				nAddress = m_pDebugger->ResolveAddressFromFunction(sToken);
			if (nAddress == core::cnInvalidAddress)
				throw CError(t_csz("Invalid label or function name"), oParser.GetPreviousPos(), sToken);
		}
	}
	else if (ch == t_char('0'))
	{
		t_string sToken;
		nAddress = oParser.ParseNumber<t_address, 0>(0, &sToken);
		if (!m_pDebugger->CheckCodeAddress(nAddress))
			throw CError(t_csz("Invalid code address"), oParser.GetPreviousPos(), sToken);
	}
	else if (CCmdParser::IsNum(ch))
	{
		t_string sToken;
		t_index nLineNum = oParser.ParseNumber<t_address>(0, &sToken);
		nAddress = m_pDebugger->ResolveAddressFromSource(nLineNum, {});
		if (nAddress == core::cnInvalidAddress)
			throw CError(t_csz("Invalid line number"), oParser.GetPreviousPos(), sToken);
	}
	else
	{
		t_string sToken = std::move(oParser.ParseToken());
		throw CError(t_csz("Invalid code location specifer"), oParser.GetPreviousPos(), sToken);
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
		t_char chSeparator = 0;

		switch (eType)
		{
		case EValueType::Byte:
		{
			core::t_byte_array aBytes;
			do
			{
				aBytes.push_back(oParser.ParseNumberSafe<core::t_byte, 0>());
				chSeparator = oParser.GetChar(true);
			} while (chSeparator == t_char(','));
			oValue = std::move(CValue(std::move(aBytes)));
			break;
		}
		case EValueType::Word:
		{
			core::t_word_array aWords;
			do
			{
				aWords.push_back(oParser.ParseNumberSafe<core::t_word, 0>());
				chSeparator = oParser.GetChar(true);
			} while (chSeparator == t_char(','));
			oValue = std::move(CValue(std::move(aWords)));
			break;
		}
		case EValueType::DWord:
		{
			core::t_dword_array aDWords;
			do
			{
				aDWords.push_back(oParser.ParseNumberSafe<core::t_dword, 0>());
				chSeparator = oParser.GetChar(true);
			} while (chSeparator == t_char(','));
			oValue = std::move(CValue(std::move(aDWords)));
			break;
		}
		case EValueType::QWord:
		{
			core::t_qword_array aQWords;
			do
			{
				aQWords.push_back(oParser.ParseNumberSafe<core::t_qword, 0>());
				chSeparator = oParser.GetChar(true);
			} while (chSeparator == t_char(','));
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

		if (chSeparator != t_char('\0'))
			oParser.RevertPreviousParse();
	}

	return std::move(oValue);
}

void CCommander::PrintCurrentState() const
{
	std::ostream& os = m_cout;
	vm::CProcessor::SState const& tState = m_pDebugger->CPUState();
	vm::CProcessor::EStatus eStatus = m_pDebugger->CPUStatus().eStatus;

	if (eStatus == vm::CProcessor::EStatus::NotInitialized)
	{
		os << "Processor is not initialized!" << std::endl;
		return;
	}

	os << std::resetiosflags(0);
	os << std::endl;
	os << "----------------------------------------------------------------------------------" << std::endl;
	os << "Processor status flags: ";
	os << " CF(" << std::dec << tState.oFlags.getCarry() << ")";
	os << " ZF(" << std::dec << tState.oFlags.getZero() << ")";
	os << " SF(" << std::dec << tState.oFlags.getSign() << ")";
	os << " OF(" << std::dec << tState.oFlags.getOverflow() << ")";

	os << std::endl;
	os << "Address registers (0x): ----------------------------------------------------------" << std::endl;
	os << "IP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nIP  << "  ";
	//os << "CIP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nCIP << "  ";
	os << "RIP = " << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nRIP << "  ";
	os << std::endl;
	os << "SP = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eSPIndex] << "  ";
	os << " SF = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eSFIndex] << "  ";
	os << std::endl;
	os << "A4 = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eARBaseIndex + 0] << "  ";
	os << " A5 = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eARBaseIndex + 1] << "  ";
	os << std::endl;
	os << "A6 = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eARBaseIndex + 2] << "  ";
	os << " A7 = "  << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.anARPool[core::SCPUStateBase::eARBaseIndex + 3] << "  ";

	os << std::endl;
	os << "General purpose registers (0x): --------------------------------------------------" << std::endl;
	for (uint nRIdx = 0; nRIdx < core::SCPUStateBase::eGeneralPurposeRegisterPoolSize; nRIdx += 16)
	{
		os << "R" << std::dec << std::setfill('0') << std::setw(2) << nRIdx << ": ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 0] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 1] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 2] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 3] << " ";
		os << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 4] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 5] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 6] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 7] << " ";
		os << "  ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 8] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 9] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 10] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 11] << " ";
		os << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 12] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 13] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 14] << " ";
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << +tState.aui8GPRPool[nRIdx + 15] << " ";
		os << std::endl;
	}

	if (eStatus == vm::CProcessor::EStatus::Finished)
	{
		os << "Program execution finished!" << std::endl;
	}
	else if (eStatus == vm::CProcessor::EStatus::Failed)
	{
		os << "Command: -------------------------------------------------------------------------" << std::endl;

		CDebugger::SCodeLineInfo tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nCIP);
		if (tCLInfo.sFuncName != nullptr)
		{
			os	<< "Func: " << tCLInfo.sFuncName.str() << "  +0x" << std::hex << std::uppercase
				<< std::setfill('0') << std::setw(8) << tCLInfo.nRelOffset;
			if (tCLInfo.sUnitName != nullptr)
				os << ",  Unit: " << tCLInfo.sUnitName.str() << ",  Line: " << std::setw(0) << std::dec << tCLInfo.nLineNum;
			os << std::endl;
		}

		std::string sBinaryCommand;
		t_address nCmdAddress = tState.nCIP;
		std::string sCommand = std::move(m_pDebugger->GetDisassembledCommand(nCmdAddress, &sBinaryCommand));
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nCIP << ": "
			<< sCommand << "  #" << sBinaryCommand << std::endl;

		os << "----------------------------------------------------------------------------------" << std::endl;
		os << m_pDebugger->CPUStatus().oErrorInfo.GetErrorMsg(true);
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
		std::string sBinaryCommand;
		t_address nCmdAddress;
		std::string sCommand;
		CDebugger::SCodeLineInfo tCLInfo;

		if (tState.nCIP != tState.nIP)
		{
			os << "Prev command: --------------------------------------------------------------------" << std::endl;

			tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nCIP);
			if (tCLInfo.sFuncName != nullptr)
			{
				os	<< "Func: " << tCLInfo.sFuncName.str() << "  +0x" << std::hex << std::uppercase
					<< std::setfill('0') << std::setw(8) << tCLInfo.nRelOffset;
				if (tCLInfo.sUnitName != nullptr)
					os << ",  Unit: " << tCLInfo.sUnitName.str() << ",  Line: " << std::setw(0) << std::dec << tCLInfo.nLineNum;
				os << std::endl;
			}

			nCmdAddress = tState.nCIP;
			sCommand = std::move(m_pDebugger->GetDisassembledCommand(nCmdAddress, &sBinaryCommand));
			os	<< std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nCIP << ": "
				<< sCommand << "  #" << sBinaryCommand << std::endl;
		}

		os << "Next command: --------------------------------------------------------------------" << std::endl;

		tCLInfo = m_pDebugger->GetCodeLineInfo(tState.nIP);
		if (tCLInfo.sFuncName != nullptr)
		{
			os	<< "Func: " << tCLInfo.sFuncName.str() << "  +0x" << std::hex << std::uppercase
				<< std::setfill('0') << std::setw(8) << tCLInfo.nRelOffset;
			if (tCLInfo.sUnitName != nullptr)
				os << ",  Unit: " << tCLInfo.sUnitName.str() << ",  Line: " << std::setw(0) << std::dec << tCLInfo.nLineNum;
			os << std::endl;
		}

		nCmdAddress = tState.nIP;
		sCommand = std::move(m_pDebugger->GetDisassembledCommand(nCmdAddress, &sBinaryCommand));
		os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << tState.nIP << ": "
		   << sCommand << "  #" << sBinaryCommand << std::endl;
	}
}

void CCommander::PrintStackBacktrace() const
{
	auto aCodeLines = std::move(m_pDebugger->GetFunctionCallStack());

	m_cout << std::resetiosflags(0);
	m_cout << "Function call stack: ---------------------------------------------------------------------" << std::endl;
	for (auto const& tInfo : aCodeLines)
	{
		m_cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << "0x" << tInfo.nAddress << "  ";
		m_cout << tInfo.sUnitName.str() << " : " << tInfo.sFuncName.str();
		if (tInfo.nLineNum != g_ciInvalid)
			m_cout << "  Line: " << std::dec << tInfo.nLineNum;
		m_cout << std::endl;
	}
}

void CCommander::PrintPortsInfo() const
{
	auto aPortsInfo = std::move(m_pDebugger->GetPortsInfo());

	m_cout << std::resetiosflags(0);
	m_cout << "Port Ranges: -----------------------------------------------------------------------------" << std::endl;
	m_cout << std::setfill('0') << std::setw(3);
	for (auto const& tInfo : aPortsInfo)
	{
		m_cout << "[" << tInfo.first.first << " - " << tInfo.first.second << "] ";
		m_cout << tInfo.second << std::endl;
	}
}

void CCommander::PrintBreakPoints() const
{
	auto aCodeLines = std::move(m_pDebugger->GetBreakPoints());

	m_cout << std::resetiosflags(0);
	m_cout << "Break Points: ----------------------------------------------------------------------------" << std::endl;
	m_cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8);
	for (auto const& tInfo : aCodeLines)
	{
		m_cout << "0x" << tInfo.nAddress << "  ";
		m_cout << tInfo.sUnitName.str() << " : " << tInfo.sFuncName.str();
		if (tInfo.nLineNum != g_ciInvalid)
			m_cout << "  Line: " << std::dec << tInfo.nLineNum;
		m_cout << std::endl;
	}
}

void CCommander::PrintCode(t_address nBaseAddress, t_size nInstrCount) const
{
	CDebugger::SCodeLineInfo tCLInfo = m_pDebugger->GetCodeLineInfo(nBaseAddress);
	if (tCLInfo.sUnitName != nullptr)
		m_cout << "Unit: " << tCLInfo.sUnitName.str() << "  Line: " << tCLInfo.nLineNum << std::endl;
	if (tCLInfo.sFuncName != nullptr)
		m_cout << "Func: " << tCLInfo.sFuncName.str() << "  +0x: " << std::hex << std::uppercase
		<< std::setfill('0') << std::setw(8) << tCLInfo.nRelOffset << std::endl;

	t_address nCurrAddress = tCLInfo.nAddress;
	while (nInstrCount > 0 && nCurrAddress != core::cnInvalidAddress)
	{
		std::string sBinaryCommand;
		std::string sCommand = std::move(m_pDebugger->GetDisassembledCommand(nCurrAddress, &sBinaryCommand));
		m_cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nCurrAddress << ": "
				<< sCommand << "  #" << sBinaryCommand << std::endl;
	}
}

void CCommander::PrintCurrentStackFrame() const
{
	auto const& tState = m_pDebugger->CPUState();
	t_address nCurrSP = tState.nSP;
	t_address nCurrSF = tState.nSF;

	// Round to 8 byte boundaries
	t_address nSPRounded = nCurrSP - nCurrSP % 8;
	t_address nSFRounded = nCurrSF + (nCurrSF % 8 == 0 ? 0 : (8 - nCurrSF % 8));

	std::ostream& os = m_cout;
	os << std::endl;
	os << std::hex << std::uppercase << std::setfill('0') << std::setw(8);
	os << "Current Stack Frame:  " << "SP = " << nCurrSP << "  SF = " << nCurrSF << "  ----------------" << std::endl;

	core::CMemory const& oMemory = m_pDebugger->Memory();
	for (t_address nSP = nSPRounded; nSP < nSFRounded && nSP < tState.cnStackLBound; ++nSP)
	{
		if (nSP % 8 == 0)
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nSP << ":  ";

		if (nSP < nCurrSP || nSP > nCurrSF)
			os << "   ";
		else
		{
			uint8 uValue;
			oMemory.ReadAt<uint8>(nSP, uValue);
			os << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << uValue << " ";
		}

		if (nSP % 8 == 3)
			os << "  ";
		else if (nSP % 8 == 7)
			os << std::endl;
	}
}

void CCommander::PrintWatchList(std::set<t_string> const&) const
{
	m_cout << std::endl;
	m_cout << "Watch:  --------------------------------------------------------------------------------" << std::endl;
	for (t_string const& sName : m_aWatchlist)
	{
		auto const& tInfo = m_pDebugger->GetVariableInfo(sName);
		PrintVariable(tInfo);
	}
}

void CCommander::PrintVariable(vm::SVariableInfo const& tInfo) const
{
	std::ostream& os = m_cout;
	os << tInfo.sName << core::CValue::TypeToCStr(tInfo.eType);
	if (tInfo.nSize > 1)
		os << '(' << std::dec << std::setw(1) << tInfo.nSize << ')';

	core::CMemory const& oMemory = m_pDebugger->Memory();
	core::CValue oValue = std::move(oMemory.ReadValue(tInfo.nAddress, tInfo.eType, tInfo.nSize));
	os << ": " << oValue << std::endl;
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

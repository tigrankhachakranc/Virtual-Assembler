#ifndef VASM_DEV_COMMANDER_H
#define VASM_DEV_COMMANDER_H

//
// Includes
//
#include <vm_machine.h>
#include <vm_debugger.h>
#include <base_parser.h>

// STL
#include <iostream>
#include <functional>
#include <unordered_map>
#include <set>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace dev {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_address		= core::t_address;
using t_count		= core::t_count;

using EValueType	= core::EValueType;
using CValue		= core::CValue;

using CDebugger		= vm::CDebugger;
using CDebuggerPtr	= vm::CDebuggerPtr;

using CMachine		= vm::CMachine;
using CMachinePtr	= vm::CMachinePtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Command line handler
//
class CCommander
{
public:
	CCommander(std::istream& cin = std::cin, std::ostream& cout = std::cout);
	~CCommander();

	// Neither copiable, nor movable
	CCommander(CCommander const&) = delete;
	CCommander(CCommander&&) = delete;
	void operator=(CCommander const&) = delete;
	void operator=(CCommander&&) = delete;

public:
	//
	//	Methods
	//

	// Initializers
	void Init(CMachinePtr pMachine);

	// Starts command line processing
	void Start();

	// Loads Program binary executable
	void Load(t_string sExecPath);

	// Opens Program source code(s) compiles and loads
	void Open(t_string sProgramSource);
	void Open(std::vector<t_string> aSourceCodeNames);

public:
	//
	//	Error
	//
	using CCmdParser = base::CParser;
	using CParserError = CCmdParser::CError;
	using CError = base::CError;

protected:
	//
	//	Commadn handlers
	//
	virtual void cmd_help(CCmdParser& oParser);			// ?|h|help
	virtual void cmd_info(CCmdParser& oParser);			// i|info
	virtual void cmd_run(CCmdParser& oParser);			// r|run	[-dbg yes?no] [-from line_num|label|0xAddress] [-to line_num|label|0xAddress]
	virtual void cmd_step_in(CCmdParser& oParser);		// t|trace	[num=1]
	virtual void cmd_step_over(CCmdParser& oParser);	// n|next	[num=1]
	virtual void cmd_step_out(CCmdParser& oParser);		// so|stepout
	virtual void cmd_stop(CCmdParser& oParser);			// pause
	virtual void cmd_reset(CCmdParser& oParser);		// reset

	virtual void cmd_bp_set(CCmdParser& oParser);		// bp	[src:line_num|label|func_name]
	virtual void cmd_bp_remove(CCmdParser& oParser);	// rbp	[src:line_num|label|func_name] [-all yes|no]

	virtual void cmd_set(CCmdParser& oParser);			// set	[IP line_num|label|0xAddress] 
														//		[A(idx) src:line_num|label|func_name|var_name|0xAddress]
														//		[R(idx) [B|W|DW|QW] +/-num|0xValue]
														//		[CF|ZF|SF|OF 0|1]
	
	virtual void cmd_assign(CCmdParser& oParser);		// a|assign [var_name +/-num|0xValue[,...]
														//			[-at 0xAddress 0xValue[,...]]

	virtual void cmd_backtrace(CCmdParser& oParser);	// bt
	virtual void cmd_dump(CCmdParser& oParser);			// d|dump	stack [-depth num] [-size byte_count] [-f|file file_path]
														//			code [-at src:line_um|label|func_name] [-size inst_count] [-f|file file_path]
														//			data [var_name[,...]] [-f|file file_path]
														//			memory	[-at 0xAddress] [-size byte_count] [-f|file file_path]
		
	virtual void cmd_print(CCmdParser& oParser);		// p|print	[state (the default)]
														//			callstack (same as backtrace)
														//			ARs
														//			GRs
														//			Ports
														//			BPs
														//			Code [-at srs:line_num|label|func_name|0xAddress] [-size int_count]

	virtual void cmd_watch(CCmdParser& oParser);		// w|watch	[add|rem] var_name

	virtual void cmd_load(CCmdParser& oParser);			// l|load	exec_file_name
	virtual void cmd_open(CCmdParser& oParser);			// o|open	source_file_name[,...]
	virtual void cmd_reload(CCmdParser& oParser);		// rl|reload
	virtual void cmd_reopen(CCmdParser& oParser);		// ro|reopen
	virtual void cmd_exit(CCmdParser& oParser);			// x|exit

protected:
	//
	//	Implementation
	//
	enum class EAddressHint { Memory, Code, Data, CodeAndData };
	t_address FetchAddress(CCmdParser& oParser, EAddressHint eHint, bool bFromRegister = false);
	CValue FetchValue(CCmdParser& oParser, EValueType eType, t_count nCount = 1);
	
	void PrintCurrentState(bool const bHexadecimal = true) const;
	void PrintStackBacktrace() const;
	void PrintCurrentStackFrame(core::t_uoffset nArgumentsOffset = 0, core::t_uoffset nStackFrameOffset = 0) const;
	void PrintBreakPoints() const;
	void PrintWatchList(std::set<t_string> const&, bool const bHexadecimal) const;
	void PrintPortsInfo() const;
	void PrintCode(t_address const nBaseAddress, t_count nIstrCount) const;
	void PrintData(t_address const nBaseAddress, t_count nSizeInBytes) const;
	void PrintMemory(t_address const nBaseAddress, t_count nSizeInBytes) const;
	void PrintVariable(vm::SVariableInfo const& tVarInfo, bool const bHexadecimal) const;
	void PrintCodeInfo(std::ostream& os, CDebugger::SCodeLineInfo const& tInfo,
					   bool const bIncludeBaseAddress, bool const bIncludeOffset) const;
	t_address PrintCommand(std::ostream& os, t_address const nCmdAddress) const; // Returns next commands address

	void CheckMachine() const;
	void CheckCPUStatus(bool bCheckForReady) const;

private:
	//
	//	Contents
	//
	CMachinePtr		m_pMachine;
	CDebuggerPtr	m_pDebugger;

	std::istream&	m_cin;
	std::ostream&	m_cout;

	std::set<t_string>	m_aWatchlist;

	bool					m_bProcessing;
	t_string				m_sProgram;
	std::vector<t_string>	m_aSourceCodes;

	using fnCmdHandler		= std::function<void(CCommander*, CCmdParser&)>;
	using t_mapCmdHandlers	= std::unordered_map<t_string, fnCmdHandler>;

	static t_mapCmdHandlers	ms_mapCmdHandlers;
};

using CCommanderPtr = std::shared_ptr<CCommander>;
using CCommanderWPtr = std::weak_ptr<CCommander>;
using CCommanderUPtr = std::unique_ptr<CCommander>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace dev
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_DEV_COMMANDER_H
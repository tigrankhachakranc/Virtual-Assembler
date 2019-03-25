//
//	Includes
//
#include "command_parser.h"
#include "asm_parser.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::pair<int64, int64> SValuRangeFromType(EImvType eType);
std::pair<uint64, uint64> UValuRangeFromType(EImvType eType);

template <typename TSrcType>
void ReadjustInMemoryNumber(TSrcType& num, EImvType eTargetType);

CStringComparator<EComparisonType::Equal_CI> isEqual;

////////////////////////////////////////////////////////////////////////////////
//
//	Static memebers intiailization
//
////////////////////////////////////////////////////////////////////////////////

//
//	Command definitions
//
const CCommandParser::t_mapCommandDefinitions CCommandParser::ms_cmapCommands {
	//
	{t_csz("NOP"),	{EOpCode::NOP}},
	{t_csz("BREAK"),{EOpCode::BREAK}},
	{t_csz("EXIT"), {EOpCode::EXIT}},
	//
	// Execution control
	//
	{t_csz("JUMP"), {EOpCode::JUMPA, EOprType::AR}},
	{t_csz("JUMP"), {EOpCode::JUMPR, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("CALL"), {EOpCode::CALL,	 EOprType::AR}},
	{t_csz("RET"),  {EOpCode::RET}},
	{t_csz("RET"),  {EOpCode::RET2,  EOprType::GRIMV, EImvType::Num16}},
	// Conditional branching
	{t_csz("JEQ"),	{EOpCode::JUMPR, ECndtnCode::Equal, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JZ"),	{EOpCode::JUMPR, ECndtnCode::Equal, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNE"),	{EOpCode::JUMPR, ECndtnCode::NotEqual, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNZ"),	{EOpCode::JUMPR, ECndtnCode::NotEqual, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JAB"),	{EOpCode::JUMPR, ECndtnCode::Above, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNBE"),	{EOpCode::JUMPR, ECndtnCode::Above, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JAE"),	{EOpCode::JUMPR, ECndtnCode::AboveEqual, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNB"),	{EOpCode::JUMPR, ECndtnCode::AboveEqual, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JBL"),	{EOpCode::JUMPR, ECndtnCode::Below, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNAE"), {EOpCode::JUMPR, ECndtnCode::Below, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JBE"),	{EOpCode::JUMPR, ECndtnCode::BelowEqual, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNA"),	{EOpCode::JUMPR, ECndtnCode::BelowEqual, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JGR"),	{EOpCode::JUMPR, ECndtnCode::Great, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNLE"), {EOpCode::JUMPR, ECndtnCode::Great, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JGE"),	{EOpCode::JUMPR, ECndtnCode::GreatEqual, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNL"),	{EOpCode::JUMPR, ECndtnCode::GreatEqual, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JLO"),	{EOpCode::JUMPR, ECndtnCode::Low, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNGE"),	{EOpCode::JUMPR, ECndtnCode::Low, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JLE"),	{EOpCode::JUMPR, ECndtnCode::LowEqual, EOprType::GRIMV, EImvType::SNum16}},
	{t_csz("JNG"),	{EOpCode::JUMPR, ECndtnCode::LowEqual, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JOF"),	{EOpCode::JUMPR, ECndtnCode::Overflow, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNO"),	{EOpCode::JUMPR, ECndtnCode::NotOverflow, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JSN"),	{EOpCode::JUMPR, ECndtnCode::Signed, EOprType::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNS"),	{EOpCode::JUMPR, ECndtnCode::NotSigned, EOprType::GRIMV, EImvType::SNum16}},
	//	
	// Flags manipulation instructions
	//
	{t_csz("GFLR"),	{EOpCode::GFLR, EOprType::GR}},
	{t_csz("SFLR"),	{EOpCode::SFLR, EOprType::GR}},
	//
	// Memory access instructions
	//
	{t_csz("LOAD"),	{EOpCode::LOAD, EOprType::AGR, EOprType::AR, true}},
	{t_csz("STORE"),{EOpCode::STORE,EOprType::AGR, EOprType::AR, true}},
	{t_csz("LDREL"),{EOpCode::LDREL,EOprType::AGR, EOprType::AR, EOprType::IMV, EImvType::SNum32, true}},
	{t_csz("STREL"),{EOpCode::STREL,EOprType::AGR, EOprType::AR, EOprType::IMV, EImvType::SNum32, true}},
	//
	// Stack instructions
	//
	{t_csz("PUSHSF"), {EOpCode::PUSHSF}},
	{t_csz("POPSF"),  {EOpCode::POPSF}},
	{t_csz("PUSH"),	  {EOpCode::PUSHA,	EOprType::AR}},
	{t_csz("POP"),	  {EOpCode::POPA,	EOprType::AR}},
	{t_csz("PUSH"),	  {EOpCode::PUSHR,	EOprType::GR, true}},
	{t_csz("POP"),	  {EOpCode::POPR,	EOprType::GR, true}},
	{t_csz("PUSH"),	  {EOpCode::PUSHR,	EOprType::GR, EOprType::IMV, EImvType::Count, true}},
	{t_csz("POP"),	  {EOpCode::POPR,	EOprType::GR, EOprType::IMV, EImvType::Count, true}},
	//
	// I/O instructions
	//
	{t_csz("IN"),	{EOpCode::IN,  EOprType::GR, EOprType::GRIMV, EImvType::Port, true}},
	{t_csz("OUT"),	{EOpCode::OUT, EOprType::GR, EOprType::GRIMV, EImvType::Port, true}},
	//
	// Address calculation instructions
	//
	{t_csz("INC"),	{EOpCode::INC,  EOprType::AR, EOprType::GR}},
	{t_csz("DEC"),	{EOpCode::DEC,	EOprType::AR, EOprType::GR}},
	{t_csz("INC"),	{EOpCode::INC2, EOprType::AR, EOprType::IMV, EImvType::Num16}},
	{t_csz("DEC"),	{EOpCode::DEC2,	EOprType::AR, EOprType::IMV, EImvType::Num16}},
	//
	// Assignemnt instuctions
	//
	{t_csz("ASSIGN"), {EOpCode::ASSIGNA0, EOprType::AR}},
	{t_csz("ASSIGN"), {EOpCode::ASSIGNA4, EOprType::AR, EOprType::IMV, EImvType::Num32}},
	{t_csz("ASSIGN"), {EOpCode::ASSIGNR1, EOprType::GR, EOprType::IMV, EImvType::SNum8,  true}},
	{t_csz("ASSIGN"), {EOpCode::ASSIGNR2, EOprType::GR, EOprType::IMV, EImvType::SNum16, true}},
	{t_csz("ASSIGN"), {EOpCode::ASSIGNR4, EOprType::GR, EOprType::IMV, EImvType::SNum32, true}},
	{t_csz("ASSIGN"), {EOpCode::ASSIGNR8, EOprType::GR, EOprType::IMV, EImvType::SNum64, true}},
	//
	// Register manipulation instructions
	//
	{t_csz("MOV"),	{EOpCode::MOVE,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVE"),	{EOpCode::MOVE,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWAP"),	{EOpCode::SWAP,	EOprType::AGR, EOprType::AGR, true}},
	// Conditional Move
	{t_csz("MOVEQ"),  {EOpCode::MOVE, ECndtnCode::Equal,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVZ"),	  {EOpCode::MOVE, ECndtnCode::Equal,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNE"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNZ"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVAB"),  {EOpCode::MOVE, ECndtnCode::Above,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNBE"), {EOpCode::MOVE, ECndtnCode::Above,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVAE"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNB"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVBL"),  {EOpCode::MOVE, ECndtnCode::Below,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNAE"), {EOpCode::MOVE, ECndtnCode::Below,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVBE"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNA"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVGR"),  {EOpCode::MOVE, ECndtnCode::Great,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNLE"), {EOpCode::MOVE, ECndtnCode::Great,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVGE"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNL"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVLO"),  {EOpCode::MOVE, ECndtnCode::Low,			EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNGE"), {EOpCode::MOVE, ECndtnCode::Low,			EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVLE"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNG"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVOF"),  {EOpCode::MOVE, ECndtnCode::Overflow,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNO"),  {EOpCode::MOVE, ECndtnCode::NotOverflow,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVSN"),  {EOpCode::MOVE, ECndtnCode::Signed,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("MOVNS"),  {EOpCode::MOVE, ECndtnCode::NotSigned,	EOprType::AGR, EOprType::AGR, true}},
	// Conditional Swap
	{t_csz("SWPEQ"),  {EOpCode::SWAP, ECndtnCode::Equal,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPZ"),	  {EOpCode::SWAP, ECndtnCode::Equal,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNE"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNZ"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPAB"),  {EOpCode::SWAP, ECndtnCode::Above,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNBE"), {EOpCode::SWAP, ECndtnCode::Above,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPAE"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNB"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPBL"),  {EOpCode::SWAP, ECndtnCode::Below,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNAE"), {EOpCode::SWAP, ECndtnCode::Below,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPBE"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNA"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPGR"),  {EOpCode::SWAP, ECndtnCode::Great,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNLE"), {EOpCode::SWAP, ECndtnCode::Great,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPGE"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNL"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPLO"),  {EOpCode::SWAP, ECndtnCode::Low,			EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNGE"), {EOpCode::SWAP, ECndtnCode::Low,			EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPLE"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNG"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPOF"),  {EOpCode::SWAP, ECndtnCode::Overflow,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNO"),  {EOpCode::SWAP, ECndtnCode::NotOverflow,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPSN"),  {EOpCode::SWAP, ECndtnCode::Signed,		EOprType::AGR, EOprType::AGR, true}},
	{t_csz("SWPNS"),  {EOpCode::SWAP, ECndtnCode::NotSigned,	EOprType::AGR, EOprType::AGR, true}},
	//
	// Comparison instructions
	//
	{t_csz("CMP"),	{EOpCode::CMP,	EOprType::AGR, EOprType::AGR, true}},
	{t_csz("TEST"),	{EOpCode::TEST,	EOprType::AGR, EOprType::AGR, true}},
	// Conditional Set
	{t_csz("SET"),	 {EOpCode::SET, ECndtnCode::None,		 EOprType::GR, true}},
	{t_csz("STEQ"),	 {EOpCode::SET, ECndtnCode::Equal,		 EOprType::GR, true}},
	{t_csz("STZ"),	 {EOpCode::SET, ECndtnCode::Equal,		 EOprType::GR, true}},
	{t_csz("STNE"),  {EOpCode::SET, ECndtnCode::NotEqual,	 EOprType::GR, true}},
	{t_csz("STNZ"),  {EOpCode::SET, ECndtnCode::NotEqual,	 EOprType::GR, true}},
	{t_csz("STAB"),	 {EOpCode::SET, ECndtnCode::Above,		 EOprType::GR, true}},
	{t_csz("STNBE"), {EOpCode::SET, ECndtnCode::Above,		 EOprType::GR, true}},
	{t_csz("STAE"),  {EOpCode::SET, ECndtnCode::AboveEqual,  EOprType::GR, true}},
	{t_csz("STNB"),  {EOpCode::SET, ECndtnCode::AboveEqual,  EOprType::GR, true}},
	{t_csz("STBL"),	 {EOpCode::SET, ECndtnCode::Below,		 EOprType::GR, true}},
	{t_csz("STNAE"), {EOpCode::SET, ECndtnCode::Below,		 EOprType::GR, true}},
	{t_csz("STBE"),  {EOpCode::SET, ECndtnCode::BelowEqual,  EOprType::GR, true}},
	{t_csz("STNA"),  {EOpCode::SET, ECndtnCode::BelowEqual,  EOprType::GR, true}},
	{t_csz("STGR"),	 {EOpCode::SET, ECndtnCode::Great,		 EOprType::GR, true}},
	{t_csz("STNLE"), {EOpCode::SET, ECndtnCode::Great,		 EOprType::GR, true}},
	{t_csz("STGE"),  {EOpCode::SET, ECndtnCode::GreatEqual,  EOprType::GR, true}},
	{t_csz("STNL"),  {EOpCode::SET, ECndtnCode::GreatEqual,	 EOprType::GR, true}},
	{t_csz("STLO"),	 {EOpCode::SET, ECndtnCode::Low,		 EOprType::GR, true}},
	{t_csz("STNGE"), {EOpCode::SET, ECndtnCode::Low,		 EOprType::GR, true}},
	{t_csz("STLE"),  {EOpCode::SET, ECndtnCode::LowEqual,	 EOprType::GR, true}},
	{t_csz("STNG"),  {EOpCode::SET, ECndtnCode::LowEqual,	 EOprType::GR, true}},
	{t_csz("STOF"),	 {EOpCode::SET, ECndtnCode::Overflow,	 EOprType::GR, true}},
	{t_csz("STNO"),  {EOpCode::SET, ECndtnCode::NotOverflow, EOprType::GR, true}},
	{t_csz("STSN"),	 {EOpCode::SET, ECndtnCode::Signed,		 EOprType::GR, true}},
	{t_csz("STNS"),	 {EOpCode::SET, ECndtnCode::NotSigned,	 EOprType::GR, true}},
	//
	// Logical Instructions
	//
	{t_csz("AND"),	{EOpCode::AND,	EOprType::GR, EOprType::GR, true}},
	{t_csz("OR"),	{EOpCode::OR,	EOprType::GR, EOprType::GR, true}},
	{t_csz("XOR"),	{EOpCode::XOR,	EOprType::GR, EOprType::GR, true}},
	{t_csz("NAND"),	{EOpCode::NAND,	EOprType::GR, EOprType::GR, true}},
	{t_csz("NOR"),	{EOpCode::NOR,	EOprType::GR, EOprType::GR, true}},
	{t_csz("NOT"),	{EOpCode::NOT,	EOprType::GR, true}},
	//
	// Shift instructions
	//
	{t_csz("SHL"),	{EOpCode::SHL,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("SHR"),	{EOpCode::SHR,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("ROL"),	{EOpCode::ROL,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("ROR"),	{EOpCode::ROR,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("SAL"),	{EOpCode::SAL,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("SAR"),	{EOpCode::SAR,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("RCL"),	{EOpCode::RCL,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	{t_csz("RCR"),	{EOpCode::RCR,	EOprType::GR, EOprType::GRIMV, EImvType::Count, true}},
	//
	// Unsigned integral arithmetic instructions
	//
	{t_csz("ADD"),	{EOpCode::ADD,	EOprType::GR, EOprType::GR, true}},
	{t_csz("SUB"),	{EOpCode::SUB,	EOprType::GR, EOprType::GR, true}},
	{t_csz("MUL"),	{EOpCode::MUL,	EOprType::GR, EOprType::GR, true}},
	{t_csz("DIV"),	{EOpCode::DIV,	EOprType::GR, EOprType::GR, true}},
	//
	// Signed integral arithmetic instructions
	//
	{t_csz("ADC"),	{EOpCode::ADC,	EOprType::GR, EOprType::GR, true}},
	{t_csz("SBB"),	{EOpCode::SBB,	EOprType::GR, EOprType::GR, true}},
	{t_csz("IMUL"),	{EOpCode::IMUL,	EOprType::GR, EOprType::GR, true}},
	{t_csz("IDIV"),	{EOpCode::IDIV,	EOprType::GR, EOprType::GR, true}},
	{t_csz("NEG"),	{EOpCode::NEG,	EOprType::GR, true}},
	{t_csz("CAST"),	{EOpCode::CAST,	EOprType::GR, true}}
};

//
//	Operand size keywords
//
const CCommandParser::t_mapOperandSizeKeywords CCommandParser::ms_cmapOprSizeKeywords = {
	{t_csz("B"),  EOprSize::Byte},
	{t_csz("b"),  EOprSize::Byte},
	{t_csz("W"),  EOprSize::Word},
	{t_csz("w"),  EOprSize::Word},
	{t_csz("DW"), EOprSize::DWord},
	{t_csz("dw"), EOprSize::DWord},
	{t_csz("QW"), EOprSize::QWord},
	{t_csz("qw"), EOprSize::QWord}
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	CCommandParser Implementation
//
////////////////////////////////////////////////////////////////////////////////
void CCommandParser::Parse(SCommand& tCommand)
{
	tCommand.eOpCode = EOpCode::Invalid;

	// Parse command name
	t_string sName = std::move(ParseToken());
	if (sName.empty() || sName.front() == s_cchComment)
		return; // No command, Skip

	// Lookup instruction by name
	auto itCmdRange = ms_cmapCommands.equal_range(sName);
	if (itCmdRange.first == itCmdRange.second)
	{	// Unrecognized command name
		throw CError(t_csz("Unrecognized command name"), GetCurrentPos(), sName);
	}

	// Command found
	bool bOprSizeParsed = false;
	// Parse next token, it could be either operand size, operand or end of exp.
	t_string sToken = std::move(ParseToken());

	// Look for operand size definition
	auto itOprSz = ms_cmapOprSizeKeywords.find(sToken);
	if (itOprSz != ms_cmapOprSizeKeywords.end())
	{
		bOprSizeParsed = true;
		tCommand.eOprSize = itOprSz->second;

		// Parse next token
		sToken = std::move(ParseToken());
	}

	// Special handling for the CAST command
	if (itCmdRange.first->second.eOpCode == EOpCode::CAST)
	{
		itOprSz = ms_cmapOprSizeKeywords.find(sToken);
		if (!bOprSizeParsed || itOprSz == ms_cmapOprSizeKeywords.end())
			throw CError(t_csz("CAST command requires  source and target operand sizes"), GetCurrentPos(), sName);

		// Source perand size already kept in tis regular place
		// Keep target operasnd size in the place of third arg and you have to remember that it is in the third arg
		tCommand.aArguments[EOprIdx::Third].u8Val = (uint8) itOprSz->second;

		// Parse next token
		sToken = std::move(ParseToken());
	}

	// Null keyword is reserved only for ASSIGN commands
	bool bAcceptNull = (itCmdRange.first->second.eOpCode == EOpCode::ASSIGNA0);

	// Parse arguments if any
	tCommand.nArgCount = 0;
	do
	{
		if (sToken.empty() || sToken.front() == s_cchComment)
			break;

		// Parse argument
		SArgument& tArg = tCommand.aArguments[tCommand.nArgCount];
		tArg = ParseArgument(std::move(sToken), bAcceptNull);

		// Parse next token
		sToken = std::move(ParseToken());

		if (tArg.eType == EArgType::None)
			break;

		// Increase argument count
		++tCommand.nArgCount;

		if (sToken.size() == 1 && sToken.front() == s_cchArgSep)
		{
			// Parse next aregument, it should not be empty or comment
			sToken = std::move(ParseToken());
			if (sToken.empty() || sToken.front() == s_cchComment)
				throw CError("Invalid command: expecting valid argument", GetCurrentPos(), sToken);
			if (tCommand.nArgCount == EOprIdx::Count)
				throw CError("Invalid command: Too many arguments", GetCurrentPos(), sToken);
		}
	} while (tCommand.nArgCount < EOprIdx::Count);

	if (!sToken.empty() && sToken.front() != s_cchComment)
		throw CError("Invalid command: extra characters", GetCurrentPos(), sToken);

	// Find corresponding command (OpCode)
	t_mapCommandDefinitions::const_iterator it;
	for (it = itCmdRange.first; it != itCmdRange.second; ++it)
	{
		SCommandDefinition const& tCmdInfo = it->second;

		// Match operand size
		if (bOprSizeParsed && !tCmdInfo.hasOprSize)
			continue; // Skip

		// Match argument count
		if (tCommand.nArgCount != tCmdInfo.nArgCount)
			continue; // Skip

		// Match argument types
		t_index nArg = 0;
		for (; nArg < tCommand.nArgCount; ++nArg)
		{
			SArgument const& tArg = tCommand.aArguments[nArg];
			// Check argument type compastibility
			if (!bool(uchar(tArg.eType) & uchar(tCmdInfo.eOperands[nArg]) & uchar(0x0F)))
				break;

			// When IMV type is specifed also match by sign
			if (tArg.eType == EArgType::SNUM && tCmdInfo.eImvType != EImvType::None)
			{
				// There is implicit rule that all unsigned types are odd and signed ones are even numbers
				if (uchar(tCmdInfo.eImvType) % 2 != 0)
					break;
			}
		}
		if (nArg != tCommand.nArgCount)
			continue; // Skip

		// If Operand size parsed and there is IMV type, also match by the IMV Type 
		if (tCmdInfo.hasOprSize && tCmdInfo.eImvType >= EImvType::Num8 && tCmdInfo.eImvType <= EImvType::SNum64)
		{
			EOprSize eTargetOprSz = EOprSize::Count;
			switch (tCmdInfo.eImvType)
			{
			case EImvType::Num8:
			case EImvType::SNum8:
				eTargetOprSz = EOprSize::Byte;
				break;
			case EImvType::Num16:
			case EImvType::SNum16:
				eTargetOprSz = EOprSize::Word;
				break;
			case EImvType::Num32:
			case EImvType::SNum32:
				eTargetOprSz = EOprSize::DWord;
				break;
			case EImvType::Num64:
			case EImvType::SNum64:
				eTargetOprSz = EOprSize::QWord;
				break;
			default:
				break;
			}

			if (tCommand.eOprSize != eTargetOprSz)
				continue; // Skip
		}

		break; // Command found, do not continue!
	}

	if (it == itCmdRange.second)
		// Command not found
		throw CError(t_csz("Incomplete or ill formed command"), GetCurrentPos(), sName);

	// Complete command
	tCommand.eOpCode = it->second.eOpCode;
	tCommand.eCdtnCode = it->second.eCndtnCode;
	tCommand.eImvType = it->second.eImvType;
	if (it->second.hasOprSize && !bOprSizeParsed)
		tCommand.eOprSize = EOprSize::Default;

	// Check arguments compatibility
	for (uchar nArg = 0; nArg < tCommand.nArgCount; ++nArg)
	{
		SArgument& tArg = tCommand.aArguments[nArg];
		switch (tArg.eType)
		{
		case EArgType::AR:
		{
			if (tArg.nIdx >= core::SCPUStateBase::eAddressRegistersPoolSize)
				throw CError(base::toStr("Invalid address register index ", tArg.nIdx), GetCurrentPos(), {});
			break;
		}
		case EArgType::GR:
		{
			if (tArg.nIdx >= core::SCPUStateBase::eGeneralPurposeRegisterPoolSize)
				throw CError(base::toStr("Invalid general purpose register index ", tArg.nIdx), GetCurrentPos(), {});
			break;
		}
		case EArgType::VAR:
		{
			if (tCommand.eImvType != EImvType::Num32)
				throw CError("Incorrect variable name usage", GetCurrentPos(), {});
			break;
		}
		case EArgType::FUNC:
		{
			if (tCommand.eImvType != EImvType::Num32)
				throw CError("Incorrect function name usage", GetCurrentPos(), {});
			break;
		}
		case EArgType::LBL:
		{
			if (tCommand.eImvType != EImvType::Num16 && tCommand.eImvType != EImvType::SNum16)
				throw CError("Incorrect label name usage", GetCurrentPos(), {});
			break;
		}
		case EArgType::NUM:
		{
			std::pair<uint64, uint64> parRange = UValuRangeFromType(tCommand.eImvType);
			if (tArg.u64Val > parRange.second)
				throw CError("Numeric value exceeds allowed range", GetCurrentPos(), {});

			// Readjust number in-memory representation
			ReadjustInMemoryNumber(tArg.u64Val, tCommand.eImvType);
			break;
		}
		case EArgType::SNUM:
		{
			std::pair<int64, int64> parRange = SValuRangeFromType(tCommand.eImvType);
			if (tArg.i64Val < parRange.first || tArg.i64Val > parRange.second)
				throw CError("Numeric value exceeds allowed range", GetCurrentPos(), {});

			// Readjust number in-memory representation
			ReadjustInMemoryNumber(tArg.i64Val, tCommand.eImvType);
			break;
		}}

		// Special handling for the PUSH/POP Rx command
		if ((tCommand.eOpCode == EOpCode::PUSHR || tCommand.eOpCode == EOpCode::POPR) && tCommand.nArgCount == 1)
		{
			// Push Rx command has default second argument Count =0 , complete it
			tCommand.nArgCount = 2;
			tCommand.aArguments[1].eType = EArgType::NUM;
			tCommand.aArguments[1].u8Val = 0;
			tCommand.eImvType = EImvType::Count;
		}
	}
}

SArgument CCommandParser::ParseArgument(t_string sToken, bool bAcceptNull)
{
	SArgument tArg;
	t_mapSymbolTable::const_iterator it;

	char chFirst = sToken.front();
	if ((chFirst == 'A' || chFirst == 'R') &&
		base::CParser::IsNum(sToken, 1))
	{	// Register
		// get index
		sToken.erase(0, 1);
		std::size_t nStrPos = 0;
		try
		{
			tArg.nIdx = (t_index) std::stoul(sToken, &nStrPos, 10);
		}
		catch (std::exception const&)
		{
		}

		if (nStrPos != sToken.size())
			throw CError("Invalid register index", GetCurrentPos(), sToken);

		if (chFirst == 'A')
			tArg.eType = EArgType::AR;
		else // (chFirst == 'R')
			tArg.eType = EArgType::GR;
	}
	else if (isEqual(sToken, t_csz("RIP")))
	{
		tArg.eType = EArgType::AR;
		tArg.nIdx = (t_index) core::SCPUStateBase::SCPUStateBase::eRIPIndex;
	}
	else if (isEqual(sToken, t_csz("SP")))
	{
		tArg.eType = EArgType::AR;
		tArg.nIdx = (t_index) core::SCPUStateBase::SCPUStateBase::eSPIndex;
	}
	else if (isEqual(sToken, t_csz("SF")) || isEqual(sToken, t_csz("BP")))
	{
		tArg.eType = EArgType::AR;
		tArg.nIdx = (t_index) core::SCPUStateBase::SCPUStateBase::eSFIndex;
	}
	else if (bAcceptNull && isEqual(sToken, t_csz("null")))
	{	// Null is reserved only for assign command
		tArg.eType = EArgType::None;
	}
	else if (chFirst == '+' || chFirst == '-')
	{	// Signed numeric
		tArg.eType = EArgType::SNUM;

		// Convert will determine number format (hexadecimal, 10 base & etc)
		size_t nStrPos = 0;
		try
		{
			tArg.i64Val = std::stoll(sToken, &nStrPos, 0);
		}
		catch (std::exception const& e)
		{
			throw CError(base::toStr("Invalid numeric value -> %1", e.what()), GetCurrentPos(), sToken);
		}
		if (nStrPos != sToken.size())
			throw CError(t_csz("Invalid numeric value"), GetCurrentPos(), sToken);
	}
	else if (base::CParser::IsNum(chFirst))
	{	// Numeric
		tArg.eType = EArgType::NUM;

		// Check if it is hexadecimal
		size_t nStrPos = 0;
		try
		{
			tArg.u64Val = std::stoull(sToken, &nStrPos, 0);
		}
		catch (std::exception const& e)
		{
			throw CError(base::toStr("Invalid numeric value -> %1", e.what()), GetCurrentPos(), sToken);
		}
		if (nStrPos != sToken.size())
			throw CError(t_csz("Invalid numeric value"), GetCurrentPos(), sToken);
	}
	else if ((it = m_mapLabelTable.find(&sToken)) != m_mapLabelTable.end())
	{	// Label name
		tArg.eType = EArgType::LBL;
		tArg.nIdx = it->second;
	}
	else if ((it = m_mapFunctionTable.find(&sToken)) != m_mapFunctionTable.end())
	{	// Function name
		tArg.eType = EArgType::FUNC;
		tArg.nIdx = it->second;
	}
	else if ((it = m_mapVariableTable.find(&sToken)) != m_mapVariableTable.end())
	{	// Variable name
		tArg.eType = EArgType::VAR;
		tArg.nIdx = it->second;
	}
	else
		throw CError("Invalid argument", GetCurrentPos(), sToken);

	return tArg;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Helper functions
//
////////////////////////////////////////////////////////////////////////////////

std::pair<int64, int64> SValuRangeFromType(EImvType eType)
{
	switch (eType)
	{
	case EImvType::Num8:
	case EImvType::SNum8:
		return {INT8_MIN, INT8_MAX};
	case EImvType::Num16:
	case EImvType::SNum16:
		return {INT16_MIN, INT16_MAX};
	case EImvType::Num32:
	case EImvType::SNum32:
		return {INT32_MIN, INT32_MAX};
	case EImvType::Num64:
	case EImvType::SNum64:
		return {INT64_MIN, INT64_MAX};
	case EImvType::Count:
		return {INT8_MIN, INT8_MAX};
	case EImvType::Port:
		return {-2048i16, 0x07FFi16};
	default:
		break;
	}
	return{0, 0};
}

std::pair<uint64, uint64> UValuRangeFromType(EImvType eType)
{
	switch (eType)
	{
	case EImvType::Num8:
		return {0, (uint64) UINT8_MAX};
	case EImvType::SNum8:
		return {0, (uint64) INT8_MAX};
	case EImvType::Num16:
		return {0, (uint64) UINT16_MAX};
	case EImvType::SNum16:
		return {0, (uint64) INT16_MAX};
	case EImvType::Num32:
		return {0, (uint64) UINT32_MAX};
	case EImvType::SNum32:
		return {0, (uint64) INT32_MAX};
	case EImvType::Num64:
		return {0, (uint64) INT64_MAX};
	case EImvType::SNum64:
		return {0, UINT64_MAX};
	case EImvType::Count:
		return {0, (uint64) UINT8_MAX};
	case EImvType::Port:
		return {0, (uint64) 0x0FFFi16};
	default:
		break;
	}
	return{0, 0};
}

template <typename TSrcType>
void ReadjustInMemoryNumber(TSrcType& num, EImvType eTargetType)
{
	switch (eTargetType)
	{
	case EImvType::Num8:
		reinterpret_cast<uint8&>(num) = static_cast<uint8>(num);
		break;
	case EImvType::SNum8:
		reinterpret_cast<int8&>(num) = static_cast<int8>(num);
		break;
	case EImvType::Num16:
		reinterpret_cast<uint16&>(num) = static_cast<uint16>(num);
		break;
	case EImvType::SNum16:
		reinterpret_cast<int16&>(num) = static_cast<int16>(num);
		break;
	case EImvType::Num32:
		reinterpret_cast<uint32&>(num) = static_cast<uint32>(num);
		break;
	case EImvType::SNum32:
		reinterpret_cast<int32&>(num) = static_cast<int32>(num);
		break;
	case EImvType::Num64:
		reinterpret_cast<uint64&>(num) = static_cast<uint64>(num);
		break;
	case EImvType::SNum64:
		reinterpret_cast<int64&>(num) = static_cast<int64>(num);
		break;
	case EImvType::Count:
		reinterpret_cast<uint8&>(num) = static_cast<uint8>(num);
		break;
	case EImvType::Port:
		reinterpret_cast<uint16&>(num) = static_cast<uint16>(num);
		break;
	default:
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

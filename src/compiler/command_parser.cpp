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
const CCommandParser::t_mapCommandDefinitions CCommandParser::ms_cmapCommands{
	//
	{t_csz("NOP"),	{EOpCode::NOP}},
	{t_csz("BREAK"),{EOpCode::BREAK}},
	{t_csz("EXIT"), {EOpCode::EXIT}},
	{t_csz("END"), {EOpCode::EXIT}},
	//
	// Execution control
	//
	{t_csz("JUMP"), {EOpCode::JUMPA, EOprTypeEx::AR}},
	{t_csz("JUMP"), {EOpCode::JUMPR, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("CALL"), {EOpCode::CALLA, EOprTypeEx::AR}},
	//{t_csz("CALL"), {EOpCode::CALLR, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("RET"),  {EOpCode::RET}},
	// Conditional branching
	{t_csz("JEQ"),	{EOpCode::JUMPR, ECndtnCode::Equal, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JZO"),	{EOpCode::JUMPR, ECndtnCode::Equal, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNE"),	{EOpCode::JUMPR, ECndtnCode::NotEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNZ"),	{EOpCode::JUMPR, ECndtnCode::NotEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JAB"),	{EOpCode::JUMPR, ECndtnCode::Above, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNBE"),	{EOpCode::JUMPR, ECndtnCode::Above, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JAE"),	{EOpCode::JUMPR, ECndtnCode::AboveEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNB"),	{EOpCode::JUMPR, ECndtnCode::AboveEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JBL"),	{EOpCode::JUMPR, ECndtnCode::Below, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNAE"), {EOpCode::JUMPR, ECndtnCode::Below, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JBE"),	{EOpCode::JUMPR, ECndtnCode::BelowEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNA"),	{EOpCode::JUMPR, ECndtnCode::BelowEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JGR"),	{EOpCode::JUMPR, ECndtnCode::Great, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNLE"), {EOpCode::JUMPR, ECndtnCode::Great, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JGE"),	{EOpCode::JUMPR, ECndtnCode::GreatEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNL"),	{EOpCode::JUMPR, ECndtnCode::GreatEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JLO"),	{EOpCode::JUMPR, ECndtnCode::Low, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNGE"),	{EOpCode::JUMPR, ECndtnCode::Low, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JLE"),	{EOpCode::JUMPR, ECndtnCode::LowEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	{t_csz("JNG"),	{EOpCode::JUMPR, ECndtnCode::LowEqual, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JOF"),	{EOpCode::JUMPR, ECndtnCode::Overflow, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNO"),	{EOpCode::JUMPR, ECndtnCode::NotOverflow, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JSN"),	{EOpCode::JUMPR, ECndtnCode::Signed, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	{t_csz("JNS"),	{EOpCode::JUMPR, ECndtnCode::NotSigned, EOprTypeEx::GRIMV, EImvType::SNum16}},
	//
	// Memory access instructions
	//
	{t_csz("LOD"),	{EOpCode::LOAD, EOprTypeEx::AR, EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("LOD"),	{EOpCode::LOAD, EOprTypeEx::AR, EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("LOAD"),	{EOpCode::LOAD, EOprTypeEx::GR, EOprTypeEx::AR, eHasOprSize}},
	{t_csz("LOAD"),	{EOpCode::LOAD, EOprTypeEx::GR, EOprTypeEx::AR, eHasOprSize}},
	{t_csz("STO"),	{EOpCode::STORE,EOprTypeEx::AR, EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("STO"),	{EOpCode::STORE,EOprTypeEx::AR, EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("STORE"),{EOpCode::STORE,EOprTypeEx::AR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STORE"),{EOpCode::STORE,EOprTypeEx::AR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("LDREL"),{EOpCode::LDREL,EOprTypeEx::AR, EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::SNum32, eFixedOprSizeDWord}},
	{t_csz("LDREL"),{EOpCode::LDREL,EOprTypeEx::GR, EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::SNum32, eHasOprSize}},
	{t_csz("STREL"),{EOpCode::STREL,EOprTypeEx::AR, EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::SNum32, eFixedOprSizeDWord}},
	{t_csz("STREL"),{EOpCode::STREL,EOprTypeEx::AR, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum32, eHasOprSize}},
	//
	// Stack instructions
	//
	{t_csz("PUSHSF"), {EOpCode::PUSHSF}},
	{t_csz("PUSHSF"), {EOpCode::PUSHSF2,EOprTypeEx::GRIMV, EImvType::Num16}},
	{t_csz("POPSF"),  {EOpCode::POPSF}},
	{t_csz("POPSF"),  {EOpCode::POPSF2,	EOprTypeEx::GRIMV, EImvType::Num16}},
	{t_csz("PUSH"),	  {EOpCode::PUSH,	EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("PUSH"),	  {EOpCode::PUSH,	EOprTypeEx::GR, eHasOprSize}},
	{t_csz("PUSH"),	  {EOpCode::PUSH,	EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::Count, eHasOprSize}},
	{t_csz("POP"),	  {EOpCode::POP,	EOprTypeEx::AR, eFixedOprSizeDWord}},
	{t_csz("POP"),	  {EOpCode::POP,	EOprTypeEx::GR, eHasOprSize}},
	{t_csz("POP"),	  {EOpCode::POP,	EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::Count, eHasOprSize}},
	//
	// Assignemnt instuctions
	//
	{t_csz("MVI"),	{EOpCode::MOVIA, EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::Num32}},
	{t_csz("MOVI"),	{EOpCode::MOVIA, EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::Num32}},
	{t_csz("MVI"),	{EOpCode::MOVI1, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum8,  eOprSizeVital}},
	{t_csz("MOVI"),	{EOpCode::MOVI1, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum8,  eOprSizeVital}},
	{t_csz("MVI"),	{EOpCode::MOVI2, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum16, eOprSizeVital}},
	{t_csz("MOVI"),	{EOpCode::MOVI2, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum16, eOprSizeVital}},
	{t_csz("MVI"),	{EOpCode::MOVI4, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum32, eOprSizeVital}},
	{t_csz("MOVI"),	{EOpCode::MOVI4, EOprTypeEx::GR, EOprTypeEx::IMV, EImvType::SNum32, eOprSizeVital}},
	//
	// Register manipulation instructions
	//
	{t_csz("MOV"),	{EOpCode::MOVE,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVE"),	{EOpCode::MOVE,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOV"),	{EOpCode::MOVE,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVE"),	{EOpCode::MOVE,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOV"),	{EOpCode::MOVE,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVE"),	{EOpCode::MOVE,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWP"),	{EOpCode::SWAP,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWAP"),	{EOpCode::SWAP,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWP"),	{EOpCode::SWAP,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWAP"),	{EOpCode::SWAP,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWP"),	{EOpCode::SWAP,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWAP"),	{EOpCode::SWAP,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	// Conditional Move
	{t_csz("MOVEQ"),  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVEQ"),  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVEQ"),  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVZ"),	  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVZ"),	  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVZ"),	  {EOpCode::MOVE, ECndtnCode::Equal,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNE"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNE"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNE"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNZ"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNZ"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNZ"),  {EOpCode::MOVE, ECndtnCode::NotEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVAB"),  {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVAB"),  {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVAB"),  {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNBE"), {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNBE"), {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNBE"), {EOpCode::MOVE, ECndtnCode::Above,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVAE"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVAE"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVAE"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNB"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNB"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNB"),  {EOpCode::MOVE, ECndtnCode::AboveEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVBL"),  {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVBL"),  {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVBL"),  {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNAE"), {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNAE"), {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNAE"), {EOpCode::MOVE, ECndtnCode::Below,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVBE"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVBE"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVBE"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNA"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNA"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNA"),  {EOpCode::MOVE, ECndtnCode::BelowEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVGR"),  {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVGR"),  {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVGR"),  {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNLE"), {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNLE"), {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNLE"), {EOpCode::MOVE, ECndtnCode::Great,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVGE"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVGE"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVGE"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNL"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNL"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNL"),  {EOpCode::MOVE, ECndtnCode::GreatEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVLO"),  {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVLO"),  {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVLO"),  {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNGE"), {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNGE"), {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNGE"), {EOpCode::MOVE, ECndtnCode::Low,			EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVLE"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVLE"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVLE"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNG"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNG"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNG"),  {EOpCode::MOVE, ECndtnCode::LowEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVOF"),  {EOpCode::MOVE, ECndtnCode::Overflow,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVOF"),  {EOpCode::MOVE, ECndtnCode::Overflow,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVOF"),  {EOpCode::MOVE, ECndtnCode::Overflow,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNO"),  {EOpCode::MOVE, ECndtnCode::NotOverflow,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNO"),  {EOpCode::MOVE, ECndtnCode::NotOverflow,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNO"),  {EOpCode::MOVE, ECndtnCode::NotOverflow,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVSN"),  {EOpCode::MOVE, ECndtnCode::Signed,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVSN"),  {EOpCode::MOVE, ECndtnCode::Signed,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVSN"),  {EOpCode::MOVE, ECndtnCode::Signed,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("MOVNS"),  {EOpCode::MOVE, ECndtnCode::NotSigned,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("MOVNS"),  {EOpCode::MOVE, ECndtnCode::NotSigned,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("MOVNS"),  {EOpCode::MOVE, ECndtnCode::NotSigned,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	// Conditional Swap
	{t_csz("SWPEQ"),  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPEQ"),  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPEQ"),  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPZ"),	  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPZ"),	  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPZ"),	  {EOpCode::SWAP, ECndtnCode::Equal,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNE"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNE"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNE"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNZ"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNZ"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNZ"),  {EOpCode::SWAP, ECndtnCode::NotEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPAB"),  {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPAB"),  {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPAB"),  {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNBE"), {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNBE"), {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNBE"), {EOpCode::SWAP, ECndtnCode::Above,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPAE"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPAE"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPAE"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNB"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNB"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNB"),  {EOpCode::SWAP, ECndtnCode::AboveEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPBL"),  {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPBL"),  {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPBL"),  {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNAE"), {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNAE"), {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNAE"), {EOpCode::SWAP, ECndtnCode::Below,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPBE"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPBE"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPBE"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNA"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNA"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNA"),  {EOpCode::SWAP, ECndtnCode::BelowEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPGR"),  {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPGR"),  {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPGR"),  {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNLE"), {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNLE"), {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNLE"), {EOpCode::SWAP, ECndtnCode::Great,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPGE"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPGE"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPGE"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNL"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNL"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNL"),  {EOpCode::SWAP, ECndtnCode::GreatEqual,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPLO"),  {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPLO"),  {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPLO"),  {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNGE"), {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNGE"), {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNGE"), {EOpCode::SWAP, ECndtnCode::Low,			EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPLE"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPLE"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPLE"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNG"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNG"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNG"),  {EOpCode::SWAP, ECndtnCode::LowEqual,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPOF"),  {EOpCode::SWAP, ECndtnCode::Overflow,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPOF"),  {EOpCode::SWAP, ECndtnCode::Overflow,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPOF"),  {EOpCode::SWAP, ECndtnCode::Overflow,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNO"),  {EOpCode::SWAP, ECndtnCode::NotOverflow,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNO"),  {EOpCode::SWAP, ECndtnCode::NotOverflow,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNO"),  {EOpCode::SWAP, ECndtnCode::NotOverflow,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPSN"),  {EOpCode::SWAP, ECndtnCode::Signed,		EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPSN"),  {EOpCode::SWAP, ECndtnCode::Signed,		EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPSN"),  {EOpCode::SWAP, ECndtnCode::Signed,		EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("SWPNS"),  {EOpCode::SWAP, ECndtnCode::NotSigned,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("SWPNS"),  {EOpCode::SWAP, ECndtnCode::NotSigned,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("SWPNS"),  {EOpCode::SWAP, ECndtnCode::NotSigned,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	//	
	// Flags manipulation instructions
	//
	{t_csz("GETF"),	 {EOpCode::GETF, EOprTypeEx::GR}},
	{t_csz("SETF"),	 {EOpCode::SETF, EOprTypeEx::GR}},
	{t_csz("CLC"),	 {EOpCode::CLC}},
	{t_csz("STC"),	 {EOpCode::STC}},
	{t_csz("CMC"),	 {EOpCode::CMC}},
	// Conditional Set
	{t_csz("SET"),	 {EOpCode::SET, ECndtnCode::None,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STEQ"),  {EOpCode::SET, ECndtnCode::Equal,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STZ"),	 {EOpCode::SET, ECndtnCode::Equal,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNE"),  {EOpCode::SET, ECndtnCode::NotEqual,	 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNZ"),  {EOpCode::SET, ECndtnCode::NotEqual,	 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STAB"),  {EOpCode::SET, ECndtnCode::Above,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNBE"), {EOpCode::SET, ECndtnCode::Above,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STAE"),  {EOpCode::SET, ECndtnCode::AboveEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNB"),  {EOpCode::SET, ECndtnCode::AboveEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SETBL"),  {EOpCode::SET, ECndtnCode::Below,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNAE"), {EOpCode::SET, ECndtnCode::Below,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STBE"),  {EOpCode::SET, ECndtnCode::BelowEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNA"),  {EOpCode::SET, ECndtnCode::BelowEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STGR"),  {EOpCode::SET, ECndtnCode::Great,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNLE"), {EOpCode::SET, ECndtnCode::Great,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STGE"),  {EOpCode::SET, ECndtnCode::GreatEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNL"),  {EOpCode::SET, ECndtnCode::GreatEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STLO"),  {EOpCode::SET, ECndtnCode::Low,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNGE"), {EOpCode::SET, ECndtnCode::Low,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STLE"),  {EOpCode::SET, ECndtnCode::LowEqual,	 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNG"),  {EOpCode::SET, ECndtnCode::LowEqual,	 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STOF"),  {EOpCode::SET, ECndtnCode::Overflow,	 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNO"),  {EOpCode::SET, ECndtnCode::NotOverflow, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STSN"),  {EOpCode::SET, ECndtnCode::Signed,		 EOprTypeEx::GR, eHasOprSize}},
	{t_csz("STNS"),  {EOpCode::SET, ECndtnCode::NotSigned,	 EOprTypeEx::GR, eHasOprSize}},
	//
	// Comparison instructions
	//
	{t_csz("CMP"),	{EOpCode::CMP,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("CMP"),	{EOpCode::CMP,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("CMP"),	{EOpCode::CMP,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	{t_csz("TEST"),	{EOpCode::TEST,	EOprTypeEx::AR,  EOprTypeEx::AGR, eFixedOprSizeDWord}},
	{t_csz("TEST"),	{EOpCode::TEST,	EOprTypeEx::AGR, EOprTypeEx::AR,  eFixedOprSizeDWord}},
	{t_csz("TEST"),	{EOpCode::TEST,	EOprTypeEx::GR,  EOprTypeEx::GR,  eHasOprSize}},
	//
	// Logical Instructions
	//
	{t_csz("AND"),	{EOpCode::AND,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("OR"),	{EOpCode::OR,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("XOR"),	{EOpCode::XOR,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NAND"),	{EOpCode::NAND,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NOR"),	{EOpCode::NOR,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NOT"),	{EOpCode::NOT,	EOprTypeEx::GR, eHasOprSize}},
	//
	// Shift instructions
	//
	{t_csz("SHL"),	{EOpCode::SHL,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("SHR"),	{EOpCode::SHR,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("SAL"),	{EOpCode::SAL,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("SAR"),	{EOpCode::SAR,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("ROL"),	{EOpCode::ROL,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("ROR"),	{EOpCode::ROR,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("RCL"),	{EOpCode::RCL,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	{t_csz("RCR"),	{EOpCode::RCR,	EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Count, eHasOprSize}},
	//
	// Signed & Unsigned integral arithmetic instructions
	//
	{t_csz("ADD"),	{EOpCode::ADD,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADC"),	{EOpCode::ADC,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUB"),	{EOpCode::SUB,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBB"),	{EOpCode::SBB,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEG"),	{EOpCode::NEG,	EOprTypeEx::GR, eHasOprSize}},
	//
	{t_csz("ADDEQ"),  {EOpCode::ADD, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDZ"),	  {EOpCode::ADD, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNE"),  {EOpCode::ADD, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNZ"),  {EOpCode::ADD, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDAB"),  {EOpCode::ADD, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNBE"), {EOpCode::ADD, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDAE"),  {EOpCode::ADD, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNB"),  {EOpCode::ADD, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDBL"),  {EOpCode::ADD, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNAE"), {EOpCode::ADD, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDBE"),  {EOpCode::ADD, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNA"),  {EOpCode::ADD, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDGR"),  {EOpCode::ADD, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNLE"), {EOpCode::ADD, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDGE"),  {EOpCode::ADD, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNL"),  {EOpCode::ADD, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDLO"),  {EOpCode::ADD, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNGE"), {EOpCode::ADD, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDLE"),  {EOpCode::ADD, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNG"),  {EOpCode::ADD, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDOF"),  {EOpCode::ADD, ECndtnCode::Overflow,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNO"),  {EOpCode::ADD, ECndtnCode::NotOverflow, EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDSN"),  {EOpCode::ADD, ECndtnCode::Signed,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADDNS"),  {EOpCode::ADD, ECndtnCode::NotSigned,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	//
	{t_csz("ADCEQ"),  {EOpCode::ADC, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCZ"),	  {EOpCode::ADC, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNE"),  {EOpCode::ADC, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNZ"),  {EOpCode::ADC, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCAB"),  {EOpCode::ADC, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNBE"), {EOpCode::ADC, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCAE"),  {EOpCode::ADC, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNB"),  {EOpCode::ADC, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCBL"),  {EOpCode::ADC, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNAE"), {EOpCode::ADC, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCBE"),  {EOpCode::ADC, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNA"),  {EOpCode::ADC, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCGR"),  {EOpCode::ADC, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNLE"), {EOpCode::ADC, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCGE"),  {EOpCode::ADC, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNL"),  {EOpCode::ADC, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCLO"),  {EOpCode::ADC, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNGE"), {EOpCode::ADC, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCLE"),  {EOpCode::ADC, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNG"),  {EOpCode::ADC, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCOF"),  {EOpCode::ADC, ECndtnCode::Overflow,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNO"),  {EOpCode::ADC, ECndtnCode::NotOverflow, EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCSN"),  {EOpCode::ADC, ECndtnCode::Signed,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("ADCNS"),  {EOpCode::ADC, ECndtnCode::NotSigned,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	//
	{t_csz("SUBEQ"),  {EOpCode::SUB, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBZ"),	  {EOpCode::SUB, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNE"),  {EOpCode::SUB, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNZ"),  {EOpCode::SUB, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBAB"),  {EOpCode::SUB, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNBE"), {EOpCode::SUB, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBAE"),  {EOpCode::SUB, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNB"),  {EOpCode::SUB, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBBL"),  {EOpCode::SUB, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNAE"), {EOpCode::SUB, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBBE"),  {EOpCode::SUB, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNA"),  {EOpCode::SUB, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBGR"),  {EOpCode::SUB, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNLE"), {EOpCode::SUB, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBGE"),  {EOpCode::SUB, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNL"),  {EOpCode::SUB, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBLO"),  {EOpCode::SUB, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNGE"), {EOpCode::SUB, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBLE"),  {EOpCode::SUB, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNG"),  {EOpCode::SUB, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBOF"),  {EOpCode::SUB, ECndtnCode::Overflow,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNO"),  {EOpCode::SUB, ECndtnCode::NotOverflow, EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBSN"),  {EOpCode::SUB, ECndtnCode::Signed,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SUBNS"),  {EOpCode::SUB, ECndtnCode::NotSigned,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	//
	{t_csz("SBBEQ"),  {EOpCode::SBB, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBZ"),	  {EOpCode::SBB, ECndtnCode::Equal,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNE"),  {EOpCode::SBB, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNZ"),  {EOpCode::SBB, ECndtnCode::NotEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBAB"),  {EOpCode::SBB, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNBE"), {EOpCode::SBB, ECndtnCode::Above,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBAE"),  {EOpCode::SBB, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNB"),  {EOpCode::SBB, ECndtnCode::AboveEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBBL"),  {EOpCode::SBB, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNAE"), {EOpCode::SBB, ECndtnCode::Below,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBBE"),  {EOpCode::SBB, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNA"),  {EOpCode::SBB, ECndtnCode::BelowEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBGR"),  {EOpCode::SBB, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNLE"), {EOpCode::SBB, ECndtnCode::Great,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBGE"),  {EOpCode::SBB, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNL"),  {EOpCode::SBB, ECndtnCode::GreatEqual,  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBLO"),  {EOpCode::SBB, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNGE"), {EOpCode::SBB, ECndtnCode::Low,		  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBLE"),  {EOpCode::SBB, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNG"),  {EOpCode::SBB, ECndtnCode::LowEqual,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBOF"),  {EOpCode::SBB, ECndtnCode::Overflow,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNO"),  {EOpCode::SBB, ECndtnCode::NotOverflow, EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBSN"),  {EOpCode::SBB, ECndtnCode::Signed,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("SBBNS"),  {EOpCode::SBB, ECndtnCode::NotSigned,	  EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	//
	{t_csz("NEGEQ"),  {EOpCode::NEG, ECndtnCode::Equal,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGZ"),	  {EOpCode::NEG, ECndtnCode::Equal,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNE"),  {EOpCode::NEG, ECndtnCode::NotEqual,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNZ"),  {EOpCode::NEG, ECndtnCode::NotEqual,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGAB"),  {EOpCode::NEG, ECndtnCode::Above,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNBE"), {EOpCode::NEG, ECndtnCode::Above,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGAE"),  {EOpCode::NEG, ECndtnCode::AboveEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNB"),  {EOpCode::NEG, ECndtnCode::AboveEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGBL"),  {EOpCode::NEG, ECndtnCode::Below,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNAE"), {EOpCode::NEG, ECndtnCode::Below,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGBE"),  {EOpCode::NEG, ECndtnCode::BelowEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNA"),  {EOpCode::NEG, ECndtnCode::BelowEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGGR"),  {EOpCode::NEG, ECndtnCode::Great,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNLE"), {EOpCode::NEG, ECndtnCode::Great,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGGE"),  {EOpCode::NEG, ECndtnCode::GreatEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNL"),  {EOpCode::NEG, ECndtnCode::GreatEqual,  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGLO"),  {EOpCode::NEG, ECndtnCode::Low,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNGE"), {EOpCode::NEG, ECndtnCode::Low,		  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGLE"),  {EOpCode::NEG, ECndtnCode::LowEqual,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNG"),  {EOpCode::NEG, ECndtnCode::LowEqual,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGOF"),  {EOpCode::NEG, ECndtnCode::Overflow,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNO"),  {EOpCode::NEG, ECndtnCode::NotOverflow, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGSN"),  {EOpCode::NEG, ECndtnCode::Signed,	  EOprTypeEx::GR, eHasOprSize}},
	{t_csz("NEGNS"),  {EOpCode::NEG, ECndtnCode::NotSigned,	  EOprTypeEx::GR, eHasOprSize}},
	//
	// Address calculation instructions
	//
	{t_csz("INC"),	{EOpCode::ADD,	EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INC"),	{EOpCode::INC,	EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::Num16}},
	{t_csz("DEC"),	{EOpCode::SUB,	EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DEC"),	{EOpCode::DEC,	EOprTypeEx::AR, EOprTypeEx::IMV, EImvType::Num16}},
	//
	{t_csz("INCEQ"),  {EOpCode::ADD, ECndtnCode::Equal,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCZ"),	  {EOpCode::ADD, ECndtnCode::Equal,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNE"),  {EOpCode::ADD, ECndtnCode::NotEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNZ"),  {EOpCode::ADD, ECndtnCode::NotEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCAB"),  {EOpCode::ADD, ECndtnCode::Above,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNBE"), {EOpCode::ADD, ECndtnCode::Above,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCAE"),  {EOpCode::ADD, ECndtnCode::AboveEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNB"),  {EOpCode::ADD, ECndtnCode::AboveEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCBL"),  {EOpCode::ADD, ECndtnCode::Below,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNAE"), {EOpCode::ADD, ECndtnCode::Below,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCBE"),  {EOpCode::ADD, ECndtnCode::BelowEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNA"),  {EOpCode::ADD, ECndtnCode::BelowEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCGR"),  {EOpCode::ADD, ECndtnCode::Great,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNLE"), {EOpCode::ADD, ECndtnCode::Great,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCGE"),  {EOpCode::ADD, ECndtnCode::GreatEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNL"),  {EOpCode::ADD, ECndtnCode::GreatEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCLO"),  {EOpCode::ADD, ECndtnCode::Low,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNGE"), {EOpCode::ADD, ECndtnCode::Low,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCLE"),  {EOpCode::ADD, ECndtnCode::LowEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNG"),  {EOpCode::ADD, ECndtnCode::LowEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCOF"),  {EOpCode::ADD, ECndtnCode::Overflow,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNO"),  {EOpCode::ADD, ECndtnCode::NotOverflow, EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCSN"),  {EOpCode::ADD, ECndtnCode::Signed,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("INCNS"),  {EOpCode::ADD, ECndtnCode::NotSigned,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	//
	{t_csz("DECEQ"),  {EOpCode::SUB, ECndtnCode::Equal,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECZ"),	  {EOpCode::SUB, ECndtnCode::Equal,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNE"),  {EOpCode::SUB, ECndtnCode::NotEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNZ"),  {EOpCode::SUB, ECndtnCode::NotEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECAB"),  {EOpCode::SUB, ECndtnCode::Above,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNBE"), {EOpCode::SUB, ECndtnCode::Above,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECAE"),  {EOpCode::SUB, ECndtnCode::AboveEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNB"),  {EOpCode::SUB, ECndtnCode::AboveEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECBL"),  {EOpCode::SUB, ECndtnCode::Below,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNAE"), {EOpCode::SUB, ECndtnCode::Below,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECBE"),  {EOpCode::SUB, ECndtnCode::BelowEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNA"),  {EOpCode::SUB, ECndtnCode::BelowEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECGR"),  {EOpCode::SUB, ECndtnCode::Great,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNLE"), {EOpCode::SUB, ECndtnCode::Great,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECGE"),  {EOpCode::SUB, ECndtnCode::GreatEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNL"),  {EOpCode::SUB, ECndtnCode::GreatEqual,  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECLO"),  {EOpCode::SUB, ECndtnCode::Low,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNGE"), {EOpCode::SUB, ECndtnCode::Low,		  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECLE"),  {EOpCode::SUB, ECndtnCode::LowEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNG"),  {EOpCode::SUB, ECndtnCode::LowEqual,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECOF"),  {EOpCode::SUB, ECndtnCode::Overflow,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNO"),  {EOpCode::SUB, ECndtnCode::NotOverflow, EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECSN"),  {EOpCode::SUB, ECndtnCode::Signed,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	{t_csz("DECNS"),  {EOpCode::SUB, ECndtnCode::NotSigned,	  EOprTypeEx::AR, EOprTypeEx::GR, eFixedOprSizeDWord}},
	//
	// Multiplicatin & Division
	//
	{t_csz("MUL"),	{EOpCode::MUL,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("IMUL"),	{EOpCode::IMUL,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("DIV"),	{EOpCode::DIV,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	{t_csz("IDIV"),	{EOpCode::IDIV,	EOprTypeEx::GR, EOprTypeEx::GR, eHasOprSize}},
	//
	// Type cast instructions
	//
	{t_csz("CAST"),	{EOpCode::CAST,	EOprTypeEx::GR, eHasOprSize}},
	//
	// I/O instructions
	//
	{t_csz("IN"),	{EOpCode::IN,  EOprTypeEx::GR, EOprTypeEx::GRIMV, EImvType::Port, eHasOprSize}},
	{t_csz("OUT"),	{EOpCode::OUT, EOprTypeEx::GRIMV, EOprTypeEx::GR, EImvType::Port, eHasOprSize}},
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

		// Source operand size already kept in its regular place
		// Keep target operasnd size in the place of third arg and you have to remember that it is in the third arg
		tCommand.aArguments[EOprIdx::Third].u8Val = (uint8) itOprSz->second;

		// Parse next token
		sToken = std::move(ParseToken());
	}

	// Parse arguments if any
	tCommand.nArgCount = 0;
	do
	{
		if (sToken.empty() || sToken.front() == s_cchComment)
			break;

		// Parse argument
		SArgument& tArg = tCommand.aArguments[tCommand.nArgCount];
		tArg = ParseArgument(std::move(sToken));

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
		if (bOprSizeParsed && !tCmdInfo.HasOprSize())
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
		// This part is required to parse MovI Rn, Num commands
		if (tCmdInfo.IsOprSizeVital() && !tCmdInfo.HasOprSize() &&
			tCmdInfo.eImvType >= EImvType::Num8 && tCmdInfo.eImvType <= EImvType::SNum64)
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
	if (!bOprSizeParsed)
		tCommand.eOprSize = it->second.DefaultOprSize();

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
			if (tArg.nIdx >= core::SCPUStateBase::eRegisterPoolSize)
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
	}

	// Special handling for the PUSH/POP commands
	if ((tCommand.eOpCode == EOpCode::PUSH || tCommand.eOpCode == EOpCode::POP) &&
		tCommand.nArgCount == 1 && tCommand.aArguments[EOprIdx::First].eType == EArgType::GR)
	{
		// Push Rx command has default second argument Count = 0, complete it
		tCommand.nArgCount = 2;
		tCommand.aArguments[EOprIdx::Second].eType = EArgType::NUM;
		tCommand.aArguments[EOprIdx::Second].u8Val = 0;
		tCommand.eImvType = EImvType::Count;
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
		{
			tArg.eType = EArgType::AR;
			tArg.nIdx += core::SCPUStateBase::eARBaseIndex;
		}
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

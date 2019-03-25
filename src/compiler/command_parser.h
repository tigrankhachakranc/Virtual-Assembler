#ifndef VASM_CL_COMMAND_PARSER_H
#define VASM_CL_COMMAND_PARSER_H

//
//	Includes
//
#include "intermediate.h"
#include <base_parser.h>

// STL
#include <map>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_mapSymbolTable = std::unordered_map<CStringRef, t_index, std::hash<t_string>, std::equal_to<t_string> >;

////////////////////////////////////////////////////////////////////////////////
//
//	CommandParser definition
//
class CCommandParser : public base::CParser
{
	typedef base::CParser Base;

public:
	inline CCommandParser(
		t_string const& sCommand,
		t_mapSymbolTable const& mapVariableTable,
		t_mapSymbolTable const& mapFunctionTable,
		t_mapSymbolTable const& mapLabelTable);

public:
	//
	//	Methods
	//

	// Parses current string and returns true if succeed, false if the string is empty or comment
	// If command is unrecognized or ill formed then throws CError
	void Parse(SCommand&);

protected:
	//
	//	Implementation
	//
	SArgument ParseArgument(t_string sToken, bool bAcceptNull);

protected:
	//
	//	Contents
	//
	t_mapSymbolTable const&	m_mapVariableTable;
	t_mapSymbolTable const&	m_mapFunctionTable;
	t_mapSymbolTable const&	m_mapLabelTable;

protected:
	//
	//	Static contents
	//
	struct SCommandDefinition
	{
		EOpCode		eOpCode;
		ECndtnCode	eCndtnCode;
		EImvType	eImvType;
		uchar		nArgCount;
		EOprType	eOperands[EOprIdx::Count];
		bool		hasOprSize;


		inline SCommandDefinition(EOpCode);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprType, EImvType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprType, EOprType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, EOprType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, EOprType, EImvType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, EOprType, EOprType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, EOprType, EOprType, EImvType, bool hasOprSize = false);
		inline SCommandDefinition(EOpCode, EOprType, EOprType, EOprType, EImvType, bool hasOprSize = false);
	};


	// Command definitions
	using t_mapCommandDefinitions = std::multimap<t_string, SCommandDefinition, CStringComparator<EComparisonType::Less_CI>>;
	static const t_mapCommandDefinitions ms_cmapCommands;

	using t_mapOperandSizeKeywords = std::map<t_string, EOprSize>;
	static const t_mapOperandSizeKeywords ms_cmapOprSizeKeywords;

public:
	//
	// Static constants
	//
	static const char s_cchComment	= '#';
	static const char s_cchArgSep	= ',';
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	CCommandParser
//
inline CCommandParser::CCommandParser(
	t_string const& sCommand, 
	t_mapSymbolTable const& mapVarTable,
	t_mapSymbolTable const& mapFuncTable,
	t_mapSymbolTable const& mapLblTable) :
	Base(sCommand),
	m_mapVariableTable(mapVarTable),
	m_mapFunctionTable(mapFuncTable),
	m_mapLabelTable(mapLblTable)
{
}

//
//	CCommandParser::SCommandDefinition
//
inline CCommandParser::SCommandDefinition::SCommandDefinition(EOpCode opc) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(EImvType::None), nArgCount(0),
	eOperands{EOprType::None, EOprType::None, EOprType::None}, hasOprSize(false)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprType oprt1, bool oprsz) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(EImvType::None), nArgCount(1),
	eOperands{oprt1, EOprType::None, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprType oprt1, EImvType imv, bool oprsz) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(imv), nArgCount(1),
	eOperands{oprt1, EOprType::None, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprType oprt1, EOprType oprt2, bool oprsz) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(EImvType::None), nArgCount(2),
	eOperands{oprt1, oprt2, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprType oprt1, bool oprsz) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(EImvType::None), nArgCount(1),
	eOperands{oprt1, EOprType::None, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprType oprt1, EImvType imv, bool oprsz) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(1),
	eOperands{oprt1, EOprType::None, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprType oprt1, EOprType oprt2, bool oprsz) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(EImvType::None), nArgCount(2),
	eOperands{oprt1, oprt2, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprType oprt1, EOprType oprt2, EImvType imv, bool oprsz) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(2),
	eOperands{oprt1, oprt2, EOprType::None}, hasOprSize(oprsz)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprType oprt1, EOprType oprt2, EOprType oprt3, EImvType imv, bool oprsz) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(3),
	eOperands{oprt1, oprt2, oprt3}, hasOprSize(oprsz)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VASM_CL_COMMAND_PARSER_H

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
	using Base = base::CParser;

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
	SArgument ParseArgument(t_string sToken, bool bAcceptNull = false);

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
	enum
	{
		// Hints for the operand size 
		eOprSizeApplicable = 0x10,
		eOprSizeVital = 0x20,
		eHasOprSize = eOprSizeApplicable | uchar(EOprSize::Default),
		eDefaultOprSize = uchar(EOprSize::Default),
		// Fixed operand sizes used to fill in OprSize field of the extension 
		eFixedOprSizeByte = uchar(EOprSize::Byte),
		eFixedOprSizeWord = uchar(EOprSize::Word),
		eFixedOprSizeDWord = uchar(EOprSize::DWord),
	};

	struct SCommandDefinition
	{
		EOpCode		eOpCode;
		ECndtnCode	eCndtnCode;
		EImvType	eImvType;
		uchar		eOprSizeHint;
		uchar		nArgCount;
		EOprTypeEx	eOperands[EOprIdx::Count];

		inline SCommandDefinition(EOpCode);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprTypeEx, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprTypeEx, EImvType, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, ECndtnCode, EOprTypeEx, EOprTypeEx, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, EOprTypeEx, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, EOprTypeEx, EImvType, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, EOprTypeEx, EOprTypeEx, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, EOprTypeEx, EOprTypeEx, EImvType, uchar eOprSizeHint = eDefaultOprSize);
		inline SCommandDefinition(EOpCode, EOprTypeEx, EOprTypeEx, EOprTypeEx, EImvType, uchar eOprSizeHint = eDefaultOprSize);

		inline bool HasOprSize() const;
		inline bool IsOprSizeVital() const;
		inline EOprSize DefaultOprSize() const;
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
	eOperands{EOprTypeEx::None, EOprTypeEx::None, EOprTypeEx::None}, eOprSizeHint((uchar)EOprSize::Default)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprTypeEx oprt1, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(EImvType::None), nArgCount(1),
	eOperands{oprt1, EOprTypeEx::None, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprTypeEx oprt1, EImvType imv, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(imv), nArgCount(1),
	eOperands{oprt1, EOprTypeEx::None, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, ECndtnCode cndtn, EOprTypeEx oprt1, EOprTypeEx oprt2, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(cndtn), eImvType(EImvType::None), nArgCount(2),
	eOperands{oprt1, oprt2, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprTypeEx oprt1, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(EImvType::None), nArgCount(1),
	eOperands{oprt1, EOprTypeEx::None, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprTypeEx oprt1, EImvType imv, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(1),
	eOperands{oprt1, EOprTypeEx::None, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprTypeEx oprt1, EOprTypeEx oprt2, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(EImvType::None), nArgCount(2),
	eOperands{oprt1, oprt2, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprTypeEx oprt1, EOprTypeEx oprt2, EImvType imv, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(2),
	eOperands{oprt1, oprt2, EOprTypeEx::None}, eOprSizeHint(oprszhint)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	EOpCode opc, EOprTypeEx oprt1, EOprTypeEx oprt2, EOprTypeEx oprt3, EImvType imv, uchar oprszhint) :
	eOpCode(opc), eCndtnCode(ECndtnCode::None), eImvType(imv), nArgCount(3),
	eOperands{oprt1, oprt2, oprt3}, eOprSizeHint(oprszhint)
{
}

inline bool CCommandParser::SCommandDefinition::HasOprSize() const
{
	return bool(eOprSizeHint & eOprSizeApplicable);
}

inline bool CCommandParser::SCommandDefinition::IsOprSizeVital() const
{
	return bool(eOprSizeHint & eOprSizeVital);
}

inline EOprSize CCommandParser::SCommandDefinition::DefaultOprSize() const
{
	return EOprSize(eOprSizeHint & 0x03);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VASM_CL_COMMAND_PARSER_H

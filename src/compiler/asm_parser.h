#ifndef VASM_CL_ASM_PARSER_H
#define VASM_CL_ASM_PARSER_H

//
// Includes
//
#include "input.h"
#include "intermediate.h"
#include <core_value.h>
#include <base_exception.h>

// STL
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_mapSymbolTable = std::unordered_map<CStringRef, t_index, std::hash<t_string>, std::equal_to<t_string> >;

////////////////////////////////////////////////////////////////////////////////
//
//	Assembly Parser 
//	Parses VASM Program and outputs intermediate structures representing Code, Data & related info
//
class CAsmParser
{
public:
	CAsmParser(CInput& oInput, SUnit& tUnit);
	~CAsmParser();

public:
	//
	//	Parser Error
	//
	class CError : public base::CException
	{
		typedef base::CException Base;
	public:
		inline CError();
		inline CError(t_csz psErrMsg, t_index nLine, t_index nPos, t_string const& sToken);
		inline CError(t_string const& sErrMsg, t_index nLine, t_index nPos, t_string const& sToken);
		inline ~CError() = default;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

	public:
		inline t_index LineNumber() const;
		inline t_index Position() const;
		inline t_string const& Token() const;
		inline t_string const& ErrorMsg() const;

	private:
		t_index		m_nLineNumber;
		t_index		m_nPosition;
		t_string	m_sToken;
	};

public:
	//
	//	Methods
	//

	// Parses Input and outputs parsed unit
	void Parse();

private:
	//
	//	Implementation
	//
	void ParseVariables();
	void ParseFnctions();
	void ParseFunctionBody(SFunction& tFunction);

private:
	// Keyword definitions
	enum class ESection
	{
		Invalid,
		Code,
		Data,
		Stack,
		Main,
	};

private:
	//
	//	Contents
	//
	CInput&	m_oInput;
	SUnit&	m_tUnit;

	// Symbol lookup tables
	t_mapSymbolTable m_mapVariableTable;
	t_mapSymbolTable m_mapFunctionTable;

private:
	//
	// Static constants
	//
	static const char s_cchSection;	// '.'
	static const char s_cchLabel;	// ':';

	// Language keywords
	using t_mapSections = std::map<t_string, ESection>;
	static const t_mapSections ms_mapSections;

	using t_mapVariableKeywords = std::map<t_string, EValueType>;
	static const t_mapVariableKeywords ms_mapVariableKeywords;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	CAsmParser::CError
//
inline CAsmParser::CError::CError() :
	m_nLineNumber(g_ciInvalid), m_nPosition(g_ciInvalid)
{
}

inline CAsmParser::CError::CError(
	t_csz psErrMsg, t_index nLine, t_index nPos, t_string const& sToken) :
	Base(t_string(psErrMsg)), m_nLineNumber(nLine), m_nPosition(nPos), m_sToken(sToken)
{
}

inline CAsmParser::CError::CError(
	t_string const& sErrMsg, t_index nLine, t_index nPos, t_string const& sToken) :
	Base(sErrMsg), m_nLineNumber(nLine), m_nPosition(nPos), m_sToken(sToken)
{
}

inline t_index CAsmParser::CError::LineNumber() const
{
	return m_nLineNumber;
}

inline t_index CAsmParser::CError::Position() const
{
	return m_nPosition;
}

inline t_string const& CAsmParser::CError::Token() const
{
	return m_sToken;
}

inline t_string const& CAsmParser::CError::ErrorMsg() const
{
	return m_sErrorMsg;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_ASM_PARSER_H

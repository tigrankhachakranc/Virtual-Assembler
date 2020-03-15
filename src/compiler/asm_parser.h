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
	class CError : public base::CError
	{
		using Base = base::CError;
	public:
		inline CError(t_csz const pcszErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken);
		inline CError(t_string const& sErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken);
		inline CError(t_string&& sErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken);
		~CError() override;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

		void operator=(CError const&) = delete;
		void operator=(CError&&) = delete;

	public:
		inline t_index LineNumber() const;
		inline t_index Position() const;
		inline t_string const& Token() const;
		inline t_string const& ErrorMsg() const;

	private:
		t_index	const	m_cnLineNumber;
		t_index	const	m_cnPosition;
		t_string const	m_csToken;

		static t_csz const s_FixedError;
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
inline CAsmParser::CError::CError(
	t_csz const pcszErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken) :
	Base(pcszErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_cnLineNumber(cnLine), m_cnPosition(cnPos), m_csToken(sToken)
{
}

inline CAsmParser::CError::CError(
	t_string const& sErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken) :
	Base(sErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_cnLineNumber(cnLine), m_cnPosition(cnPos), m_csToken(sToken)
{
}

inline CAsmParser::CError::CError(
	t_string&& sErrMsg, t_index const cnLine, t_index const cnPos, t_string const& sToken) :
	Base(sErrMsg, Reserved, VASM_ERR_INFO(s_FixedError)),
	m_cnLineNumber(cnLine), m_cnPosition(cnPos), m_csToken(sToken)
{
}

inline t_index CAsmParser::CError::LineNumber() const
{
	return m_cnLineNumber;
}

inline t_index CAsmParser::CError::Position() const
{
	return m_cnPosition;
}

inline t_string const& CAsmParser::CError::Token() const
{
	return m_csToken;
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

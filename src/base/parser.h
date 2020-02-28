#ifndef BASE_PARSER_H
#define BASE_PARSER_H

//
//	Includes
//
#include "definitions.h"
#include "exception.h"

// STL
#include <cctype>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Basic parser class definition
//	In fact this is Lexer and devides its input string into tokens
//
class CParser
{
public:
	//
	//	Parser Error
	//
	class CError : public base::CException
	{
		typedef base::CException Base;
	public:
		inline CError();
		inline CError(t_csz psErrMsg, t_index nPos, t_string const& sToken = t_string());
		inline CError(t_string const& sErrMsg, t_index nPos, t_string const& sToken = t_string());
		~CError() override;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

	public:
		inline t_index Position() const;
		inline t_string const& Token() const;
		inline t_string const& ErrorMsg() const;

		t_string GetErrorMsg(bool bDetailed = false) const override;

	private:
		t_index		m_nPosition;
		t_string	m_sToken;
	};

public:
	inline CParser(t_string const& sInput);

	inline t_index GetCurrentPos() const;
	inline t_index GetPreviousPos() const;
	inline t_string const& Input() const;
	
	inline bool IsFinished() const;
	inline bool IsFinished(bool bSkipWhiteSpaces);
	inline bool IsWhiteSpace() const;

	void Skip(int nCharCount);
	void SkipWhiteSpaces();

	inline t_char PeekChar(int nOffset = 0) const;
	t_char GetChar(bool bSkipEmptySpaces = false);

	using fnIsDelimiter = std::function<bool(t_char)>;
	t_string ParseToken(fnIsDelimiter = nullptr);
	t_string ParseTillEnd(t_char chStopper = 0, bool bTrimExtraSpaces = false);

	t_string ParseName();
	t_char ParseCharacter(t_char const chDelimiter = 0);
	t_string ParseString(bool bStrict = true, t_char chOpeningQuote = t_char('"'), t_char chClosingQuote = 0);

	// Parses numerical value accordingly to the specified target type
	template <typename TNumType, int Radix = 10>
	TNumType ParseNumber(t_char const chDelimiter = 0, t_string* psToken = nullptr);
	// Parses number (either signed or unsiged) based on its source sign
	// disregarding target type, then safely casts to the target type
	template <typename TNumType, int Radix = 10>
	TNumType ParseNumberSafe(t_char const chDelimiter = 0, t_string* psToken = nullptr);

	bool ParseBoolean(t_char const chDelimiter = 0);

	void RevertPreviousParse();

public:
	// Static helpers
	inline static bool IsSpace(t_char const ch);
	inline static bool IsAlphaNum(t_char const ch);
	inline static bool IsAlpha(t_char const ch);
	inline static bool IsNum(t_char const ch);
	inline static bool IsHexNum(t_char const ch);

	static uchar GetNumValue(t_char const ch);
	static uchar GetHexNumValue(t_char const ch);

	static bool IsNum(t_string const& str, size_t nStartPos = 0);

protected:
	inline void FixCurrentPos();
	inline void SetCurrentPos(t_index nPos);

	template <typename TNumType>
	TNumType ParseNumberInternal(int Radix, t_char const chDelimiter, t_string* psToken);

private:
	t_index m_nPos;
	t_index m_nPrevPos;
	t_string const& m_sInput;

	static char  (CParser::* const pfn0)(int, t_char const, t_string*);
	static int8  (CParser::* const pfn1)(int, t_char const, t_string*);
	static uint8 (CParser::* const pfn2)(int, t_char const, t_string*);
	static int16 (CParser::* const pfn3)(int, t_char const, t_string*);
	static uint16(CParser::* const pfn4)(int, t_char const, t_string*);
	static int32 (CParser::* const pfn5)(int, t_char const, t_string*);
	static uint32(CParser::* const pfn6)(int, t_char const, t_string*);
	static int64 (CParser::* const pfn7)(int, t_char const, t_string*);
	static uint64(CParser::* const pfn8)(int, t_char const, t_string*);
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CParser::CParser(t_string const& sInput)
	: m_nPos(0), m_nPrevPos(0), m_sInput(sInput)
{
}

inline t_index CParser::GetCurrentPos() const
{
	return m_nPos;
}

inline t_index CParser::GetPreviousPos() const
{
	return m_nPrevPos;
}

inline t_string const& CParser::Input() const
{
	return m_sInput;
}

bool CParser::IsFinished() const
{
	return (m_nPos >= m_sInput.size());
}

bool CParser::IsFinished(bool bSkipWhiteSpaces)
{
	if (bSkipWhiteSpaces)
		SkipWhiteSpaces();
	return (m_nPos >= m_sInput.size());
}

inline bool CParser::IsWhiteSpace() const
{
	return (m_nPos < m_sInput.size() && IsSpace(m_sInput[m_nPos]));
}

inline t_char CParser::PeekChar(int nOffset) const
{
	char ch = 0;
	if (m_nPos + nOffset < m_sInput.size())
		ch = m_sInput.at(m_nPos + nOffset);
	return ch;
}

inline void CParser::FixCurrentPos()
{
	m_nPrevPos = m_nPos;
}

inline void CParser::SetCurrentPos(t_index nPos)
{
	m_nPos = nPos;
}

template <typename TNumType, int Radix>
TNumType CParser::ParseNumber(t_char const chDelimiter, t_string* psToken)
{
	return ParseNumberInternal<TNumType>(Radix, chDelimiter, psToken);
}

template <typename TNumType, int Radix>
TNumType CParser::ParseNumberSafe(t_char const chDelimiter, t_string* psToken)
{
	SkipWhiteSpaces();
	TNumType val = (PeekChar() == '-') ?
		static_cast<TNumType>(ParseNumberInternal<TCleanType<TNumType>::signed_type> (Radix, chDelimiter, psToken)):
		static_cast<TNumType>(ParseNumberInternal<TCleanType<TNumType>::usigned_type>(Radix, chDelimiter, psToken));
	return val;
}

//
//	Static routines
//
inline bool CParser::IsSpace(t_char const ch)
{
	return (std::isspace(ch) != 0);
}

inline bool CParser::IsAlphaNum(t_char const ch)
{
	return ((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch >= '0' && ch <= '9') ||
			(ch == '_') || (ch == '+') || (ch == '-'));
}

inline bool CParser::IsAlpha(t_char const ch)
{
	return ((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch == '_'));
}

inline bool CParser::IsNum(t_char const ch)
{
	return ((ch >= '0' && ch <= '9') || (ch == '+') || (ch == '-'));
}

inline bool CParser::IsHexNum(t_char const ch)
{
	return ((ch >= 'a' && ch <= 'f') ||
			(ch >= 'A' && ch <= 'F') ||
			(ch >= '0' && ch <= '9'));
}


//
//	CError
//
inline CParser::CError::CError() :
	m_nPosition(g_ciInvalid)
{
}

inline CParser::CError::CError(
	t_csz psErrMsg, t_index nPos, t_string const& sToken) :
	Base(t_string(psErrMsg)), m_nPosition(nPos), m_sToken(sToken)
{
}

inline CParser::CError::CError(
	t_string const& sErrMsg, t_index nPos, t_string const& sToken) :
	Base(sErrMsg), m_nPosition(nPos), m_sToken(sToken)
{
}

inline t_index CParser::CError::Position() const
{
	return m_nPosition;
}

inline t_string const& CParser::CError::Token() const
{
	return m_sToken;
}

inline t_string const& CParser::CError::ErrorMsg() const
{
	return m_sErrorMsg;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_PARSER_H

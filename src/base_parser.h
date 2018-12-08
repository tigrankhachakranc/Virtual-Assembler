#ifndef BASE_PARSER_H
#define BASE_PARSER_H

//
//	Includes
//
#include "base_definitions.h"
#include "base_exception.h"

// STL
#include <cctype>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Parser base definition
//
class CParser
{
public:
	inline CParser(std::string const& sInput);

	inline t_index GetCurrentPos() const;
	inline t_index GetPreviousPos() const;
	inline std::string const& GetInput() const;
	
	inline bool IsFinished() const;
	inline bool IsWhiteSpace() const;

	inline void Skip(int nCharCount);
	void SkipWhiteSpaces();

	inline char PeekChar(int nOffset = 0) const;
	char GetChar(bool bSkipEmptySpaces = false);

	std::string ParseToken();
	std::string ParseTillEnd(char chStopper = 0, bool bTrimExtraSpaces = false);

	uchar ParseCharacter(char const chDelimiter = 0);
	std::string ParseString();

	template <typename TNumType>
	TNumType ParseNumber(char const chDelimiter = 0);
	
	void RevertPreviousParse();

public:
	// Static helpers
	static inline bool IsAlphaNum(char const ch);
	static inline bool IsAlpha(char const ch);
	static inline bool IsNum(char const ch);
	static inline bool IsHexNum(char const ch);

	static inline uchar GetNumValue(char const ch);
	static inline uchar GetHexNumValue(char const ch);

	static bool IsNum(std::string const& str, size_t nStartPos = 0);

protected:
	inline void FixCurrentPos();
	inline void SetCurrentPos(t_index nPos);

private:
	t_index m_nPos;
	t_index m_nPrevPos;
	std::string const& m_sInput;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
inline CParser::CParser(std::string const& sInput)
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

inline std::string const& CParser::GetInput() const
{
	return m_sInput;
}

bool CParser::IsFinished() const
{
	return (m_nPos >= m_sInput.size());
}

inline bool CParser::IsWhiteSpace() const
{
	return (m_nPos < m_sInput.size() && std::isspace(m_sInput[m_nPos]) != 0);
}

inline char CParser::PeekChar(int nOffset) const
{
	char ch = 0;
	if (m_nPos + nOffset < m_sInput.size())
		ch = m_sInput.at(m_nPos + nOffset);
	return ch;
}

inline void CParser::Skip(int nCharCount)
{
	if (m_nPos + nCharCount < m_sInput.size())
		m_nPos += nCharCount;
	else
		m_nPos = (t_index) m_sInput.size();
}

inline void CParser::FixCurrentPos()
{
	m_nPrevPos = m_nPos;
}

inline void CParser::SetCurrentPos(t_index nPos)
{
	m_nPos = nPos;
}

inline bool CParser::IsAlphaNum(char const ch)
{
	return ((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch >= '0' && ch <= '9') ||
			(ch == '_'));
}

inline bool CParser::IsAlpha(char const ch)
{
	return ((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch == '_'));
}

inline bool CParser::IsNum(char const ch)
{
	return (ch >= '0' && ch <= '9');
}

inline bool CParser::IsHexNum(char const ch)
{
	return ((ch >= 'a' && ch <= 'f') ||
			(ch >= 'A' && ch <= 'F') ||
			(ch >= '0' && ch <= '9'));
}

inline uchar CParser::GetNumValue(char const ch)
{
	uchar chValue = 0;
	if (ch >= '0' && ch <= '9')
		chValue = (uchar) ch - '0';
	return chValue;
}

inline uchar CParser::GetHexNumValue(char const ch)
{
	uchar chValue = 0;
	if (ch >= 'a' && ch <= 'f')
		chValue = (uchar) ch - 'a' + 10;
	else if (ch >= 'A' && ch <= 'F')
		chValue = (uchar) ch - 'A' + 10;
	else if (ch >= '0' && ch <= '9')
		chValue = (uchar) ch - '0';
	return chValue;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_PARSER_H

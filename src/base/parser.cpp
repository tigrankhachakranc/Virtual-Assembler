//
//	Includes
//
#include "parser.h"
#include "utility.h"

// STL
#include <limits>
#include <cstdlib>
#include <type_traits>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CParser::CError Implementation
//
////////////////////////////////////////////////////////////////////////////////
CParser::CError::~CError() = default;

t_string CParser::CError::GetErrorMsg(bool bDetailed) const
{
	t_string sError;

	if (bDetailed)
	{
		std::stringstream oBuff(std::ios_base::out);
		oBuff << "Parsing error: ";

		std::stringstream::pos_type nStrmPos = oBuff.tellp();
		if (!m_sErrorMsg.empty())
			oBuff << m_sErrorMsg;
		else
			oBuff << Base::what();

		if (oBuff.tellp() == nStrmPos)
			oBuff << "Unknown.";
		else
			oBuff << ".";

		if (m_nPosition != g_ciInvalid)
			oBuff << "  Position: " << m_nPosition;
		if (m_sToken.empty())
			oBuff << "  Token: " << m_sToken;

		oBuff << std::endl;
		sError = oBuff.str();
	}
	else
	{
		sError = m_sErrorMsg.empty() ? Base::what() : m_sErrorMsg;
	}

	return std::move(sError);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CParser Implementation
//
////////////////////////////////////////////////////////////////////////////////

void CParser::Skip(int nCharCount)
{
	if (m_nPos + nCharCount < m_sInput.size())
		m_nPos += nCharCount;
	else
		m_nPos = (t_index) m_sInput.size();
}

void CParser::SkipWhiteSpaces()
{
	while (IsWhiteSpace())
		++m_nPos;
}

t_char CParser::PeekChar(int nOffset) const
{
	char ch = 0;
	if (m_nPos + nOffset < m_sInput.size())
		ch = m_sInput.at(m_nPos + nOffset);
	return ch;
}

t_char CParser::GetChar(bool bSkipEmptySpaces)
{
	char ch = 0;

	if (bSkipEmptySpaces)
		SkipWhiteSpaces();

	FixCurrentPos();

	if (!IsFinished())
	{
		ch = m_sInput.at(m_nPos);
		++m_nPos;
	}
	return ch;
}


t_string CParser::ParseToken(fnIsDelimiter pfnDelimiter)
{
	t_string sToken;

	SkipWhiteSpaces();
	FixCurrentPos();

	if (pfnDelimiter)
	{
		while (!pfnDelimiter(PeekChar()))
			sToken.push_back(GetChar(false));
	}
	else
	{
		while (IsAlphaNum(PeekChar()))
			sToken.push_back(GetChar(false));
	}

	if (GetCurrentPos() == GetPreviousPos() && !IsFinished())
		sToken.push_back(GetChar(false));

	return std::move(sToken);
}

t_string CParser::ParseTillEnd(t_char chStopper, bool bTrimSpaces)
{
	t_string sText;

	FixCurrentPos();
	bool isPrevSpace = false;
	while (!IsFinished())
	{
		t_char chCurr = GetChar(false);

		if (chStopper != 0 && chCurr == chStopper)
			break;

		if (bTrimSpaces && std::isspace(chCurr))
		{
			isPrevSpace = true;
			continue; // skip white spaces
		}

		if (isPrevSpace && !sText.empty())
			sText.push_back(' '); // put back single space if not the first

		sText.push_back(chCurr);
		isPrevSpace = false;
	}

	return std::move(sText);
}

t_string CParser::ParseName()
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting name");

	if (!IsAlpha(PeekChar()))
		throw CException(toStr("Unexpected character '%1': Expecting name", t_string(1, PeekChar())));

	return std::move(ParseToken());
}

template <typename TNumType>
TNumType CParser::ParseNumberInternal(int nRadix, t_char const chDelimiter, t_string* psToken)
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting numeric value");

	FixCurrentPos();

	TNumType nNumber = 0;
	size_t nPos = GetCurrentPos();
	t_char const* pchStartPos = Input().c_str() + nPos;
	t_char* pchLastPos = nullptr;

	if (std::is_signed<TNumType>::value)
	{
		int64 i64Number = std::strtoll(pchStartPos, &pchLastPos, nRadix);
		if (pchLastPos <= pchStartPos ||
			i64Number > (int64) std::numeric_limits<TNumType>::max() ||
			i64Number < (int64) std::numeric_limits<TNumType>::min())
			throw CException("Invalid numeric value");
		nNumber = (TNumType) i64Number;
	}
	else
	{
		uint64 ui64Number = std::strtoull(pchStartPos, &pchLastPos, nRadix);
		if (pchLastPos <= pchStartPos ||
			ui64Number > (uint64) std::numeric_limits<TNumType>::max())
			throw CException("Invalid numeric value");
		nNumber = (TNumType) ui64Number;
	}

	SetCurrentPos(nPos + (pchLastPos - pchStartPos));

	if (chDelimiter != 0)
	{	// Ensure for delimiter
		t_char chCurr = GetChar(true);
		if (chCurr != 0 && chCurr != chDelimiter)
			throw CException("Expecting delimiter");
	}
	else if (!IsFinished() && !IsWhiteSpace())
	{	// Should be white space or end of string
		throw CException("Invalid numeric value");
	}

	if (psToken != nullptr && pchLastPos <= pchStartPos)
		*psToken = std::move(t_string(pchStartPos, pchLastPos - pchStartPos));

	return nNumber;
}

// Force compiler to instantiate template functions
char  (CParser::* const CParser::pfn0)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<char>;
int8  (CParser::* const CParser::pfn1)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<int8>;
uint8 (CParser::* const CParser::pfn2)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<uint8>;
int16 (CParser::* const CParser::pfn3)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<int16>;
uint16(CParser::* const CParser::pfn4)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<uint16>;
int32 (CParser::* const CParser::pfn5)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<int32>;
uint32(CParser::* const CParser::pfn6)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<uint32>;
int64 (CParser::* const CParser::pfn7)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<int64>;
uint64(CParser::* const CParser::pfn8)(int, t_char const, t_string*) = &CParser::ParseNumberInternal<uint64>;

t_char CParser::ParseCharacter(t_char const chDelimiter)
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting character value");

	uchar chValue = 0;

	FixCurrentPos();
	t_char chCurr = PeekChar();
	if (chCurr == '\'')
	{	// Character symbol
		Skip(1);
		chCurr = GetChar(false);
		if (chCurr == '\\')
			chCurr = GetChar(false);
		if (chCurr == 0)
			throw CException("Unexpected end of line: Invalid character specification");

		chValue = (uchar) chCurr;

		chCurr = GetChar(false);
		if (chCurr != '\'')
			throw CException("Expecting charcter closing quote (')");
	}
	else if (chCurr == '0' && PeekChar(1) == 'x')
	{	// Hexadecimal value
		Skip(2);
		t_char ch1 = GetChar(false);
		t_char ch2 = GetChar(false);
		if (!IsHexNum(ch1) || !IsHexNum(ch2))
			throw CException("Expecting hexadecimal character value");

		chValue = GetHexNumValue(ch1) * 16 + GetHexNumValue(ch2);
	}
	else
	{
		size_t nPos = GetCurrentPos();
		t_char const* pchStartPos = Input().c_str() + nPos;
		t_char* pchLastPos = nullptr;

		int nNumber = std::strtol(pchStartPos, &pchLastPos, 0);

		if (pchLastPos <= pchStartPos ||
			nNumber > std::numeric_limits<uchar>::max() ||
			nNumber < std::numeric_limits<char>::min())
			throw CException("Invalid character value");

		SetCurrentPos(nPos + (pchLastPos - pchStartPos));

		chValue = (t_char) nNumber;
	}

	if (chDelimiter != 0)
	{	// Ensure for delimiter
		chCurr = GetChar(true);
		if (chCurr != 0 && chCurr != chDelimiter)
			throw CException("Expecting delimiter");
	}
	else if (!IsFinished() && !IsWhiteSpace())
	{	// Should be white space or end of string
		throw CException("Invalid character value");
	}

	return chValue;
}

t_string CParser::ParseString(
	bool bStrict, t_char chOpeningQuote, t_char chClosingQuote)
{
	bool bEnclosed = false;
	t_char chCurr = 0;

	VASM_ASSERT(!bStrict || chOpeningQuote != 0);
	if (chClosingQuote == 0)
		chClosingQuote = chOpeningQuote;
	
	SkipWhiteSpaces();
	if (bStrict || PeekChar() == chOpeningQuote)
	{
		chCurr = GetChar(true);
		if (chCurr != chOpeningQuote)
			throw CException(base::toStr("Expecting string literal opening quote %1", t_string(1, chOpeningQuote)));
		bEnclosed = true;
	}

	FixCurrentPos();
	t_string sLiteral;
	while (true)
	{
		chCurr = GetChar(false);
		if (chCurr == '\\') // Escaper
			chCurr = GetChar(false); // skip symbol
		else if (bEnclosed && chCurr == chClosingQuote)
			break;
		else if (!bEnclosed && IsSpace(chCurr))
			break;

		if (bEnclosed && chCurr == 0)
			throw CException(base::toStr("Expecting string literal closing quote %1", t_string(1, chClosingQuote)));

		sLiteral.push_back(chCurr);
	}

	return std::move(sLiteral);
}

bool CParser::ParseBoolean(t_char const chDelimiter)
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting Boolean value");

	FixCurrentPos();

	bool bValue;
	CStringComparator<EComparisonType::Equal_CI> compare;
	t_string sToken = std::move(ParseToken());
	if (sToken == t_csz("1") || compare(sToken, t_csz("true")) ||
		compare(sToken, t_csz("yes")) || compare(sToken, t_csz("on")))
		bValue = true;
	else if (sToken == t_csz("0") || compare(sToken, t_csz("false")) ||
			 compare(sToken, t_csz("no")) || compare(sToken, t_csz("off")))
		bValue = false;
	else
		throw CException("Unexpected token: Expecting Boolean value");

	if (chDelimiter != 0)
	{	// Ensure for delimiter
		t_char chCurr = GetChar(true);
		if (chCurr != 0 && chCurr != chDelimiter)
			throw CException("Expecting delimiter");
	}

	return bValue;
}

void CParser::RevertPreviousParse()
{
	m_nPos = m_nPrevPos;
}

bool CParser::IsNum(t_string const& str, size_t nPos)
{
	bool bResult = nPos < str.size() ? IsNum(str[nPos]) : false;
	for (++nPos; nPos < str.size() && bResult; ++nPos)
		bResult &= IsNum(str[nPos]);
	return bResult;
}

//
//	Static routines
//
uchar CParser::GetNumValue(t_char const ch)
{
	uchar chValue = 0;
	if (ch >= '0' && ch <= '9')
		chValue = (uchar) ch - '0';
	return chValue;
}

uchar CParser::GetHexNumValue(t_char const ch)
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
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
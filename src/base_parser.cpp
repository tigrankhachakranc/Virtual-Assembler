//
//	Includes
//
#include "base_parser.h"

// STL
#include <limits>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CParser Implementation
//
////////////////////////////////////////////////////////////////////////////////
void CParser::SkipWhiteSpaces()
{
	while (IsWhiteSpace())
		++m_nPos;
}

char CParser::GetChar(bool bSkipEmptySpaces)
{
	char ch = 0;

	if (bSkipEmptySpaces)
		SkipWhiteSpaces();
	
	if (!IsFinished())
	{
		ch = m_sInput.at(m_nPos);
		++m_nPos;
	}
	return ch;
}


std::string CParser::ParseToken()
{
	std::string sToken;

	SkipWhiteSpaces();
	FixCurrentPos();

	while (IsAlphaNum(PeekChar()))
		sToken.push_back(GetChar(false));

	if (GetCurrentPos() == GetPreviousPos() && !IsFinished())
		sToken.push_back(GetChar(false));

	return std::move(sToken);
}

std::string CParser::ParseTillEnd(char chStopper, bool bTrimSpaces)
{
	std::string sText;

	FixCurrentPos();
	bool isPrevSpace = false;
	while (!IsFinished())
	{
		char chCurr = GetChar(false);

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


template <typename TNumType>
TNumType CParser::ParseNumber(char const chDelimiter)
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting numeric value.");

	FixCurrentPos();

	int64 nNumber = 0;
	size_t nPos = GetCurrentPos();
	char const* pchStartPos = GetInput().c_str() + nPos;
	char* pchLastPos = nullptr;

	nNumber = std::strtoll(pchStartPos, &pchLastPos, 0);

	if (pchLastPos <= pchStartPos ||
		nNumber > (int64) std::numeric_limits<TNumType>::max() ||
		nNumber < (int64) std::numeric_limits<TNumType>::min())
		throw CException("Invalid numeric value.");

	SetCurrentPos(nPos + (pchLastPos - pchStartPos));

	//int nBase = 10;
	//bool bSigned = false;

	//char chCurr = PeekChar();
	//if (chCurr == '-')
	//{
	//	bSigned = true;
	//	Skip(1);
	//	if (!IsNum(PeekChar()))
	//		throw CException("Invalid numeric value.");
	//}
	//else if (chCurr == '0' && PeekChar(1) == 'x')
	//{
	//	Skip(2);
	//	nBase = 16;
	//}

	//size_t nPos = GetCurrentPos();
	//int64 nNumber = 0;
	//if (nBase == 16)
	//{
	//	while (IsHexNum(PeekChar()))
	//	{
	//		uchar chVal = GetHexNumValue(GetChar(false));
	//		nNumber = nNumber * nBase + chVal;
	//	}
	//}
	//else
	//{
	//	while (IsNum(PeekChar()))
	//	{
	//		uchar chVal = GetNumValue(GetChar(false));
	//		nNumber = nNumber * nBase + chVal;
	//	}
	//}

	//if (bSigned)
	//	nNumber *= -1;

	//if (nPos == GetCurrentPos() || IsAlpha(PeekChar()))
	//	throw CException("Invalid numeric value.");
	//else if (nBase == 16)
	//{
	//	if (GetCurrentPos() - nPos > 2 * sizeof(TNumType))
	//		throw CException("Hexadecimal numeric value exceeds allowed number of symbols.");
	//}
	//else if (nNumber < (int64) std::numeric_limits<TNumType>::min() ||
	//		 nNumber > (int64) std::numeric_limits<TNumType>::max())
	//	throw CException("Numeric value exceeds allowed range.");

	if (chDelimiter != 0)
	{	// Ensure for delimiter
		char chCurr = GetChar(true);
		if (chCurr != 0 && chCurr != chDelimiter)
			throw CException("Expecting delimiter.");
	}
	else if (!IsFinished() && !IsWhiteSpace())
	{	// Should be white space or end of string
		throw CException("Invalid numeric value.");
	}

	return static_cast<TNumType>(nNumber);
}

// Force compiler to instantiate template functions
static int8	  (CParser::* pfn1)(char const chDelimiter) = &CParser::ParseNumber<int8>;
static uint8  (CParser::* pfn2)(char const chDelimiter) = &CParser::ParseNumber<uint8>;
static int16  (CParser::* pfn3)(char const chDelimiter) = &CParser::ParseNumber<int16>;
static uint16 (CParser::* pfn4)(char const chDelimiter) = &CParser::ParseNumber<uint16>;
static int32  (CParser::* pfn5)(char const chDelimiter) = &CParser::ParseNumber<int32>;
static uint32 (CParser::* pfn6)(char const chDelimiter) = &CParser::ParseNumber<uint32>;
static int64  (CParser::* pfn7)(char const chDelimiter) = &CParser::ParseNumber<int64>;
static uint64 (CParser::* pfn8)(char const chDelimiter) = &CParser::ParseNumber<uint64>;

uchar CParser::ParseCharacter(char const chDelimiter)
{
	SkipWhiteSpaces();
	if (IsFinished())
		throw CException("Unexpected end of line: Expecting character value.");

	uchar chValue = 0;

	FixCurrentPos();
	char chCurr = PeekChar();
	if (chCurr == '\'')
	{	// Character symbol
		Skip(1);
		chCurr = GetChar(false);
		if (chCurr == '\\')
			chCurr = GetChar(false);
		if (chCurr == 0)
			throw CException("Unexpected end of line: Invalid character specification.");

		chValue = (uchar) chCurr;

		chCurr = GetChar(false);
		if (chCurr != '\'')
			throw CException("Expecting charcter closing quote (').");
	}
	else if (chCurr == '0' && PeekChar(1) == 'x')
	{	// Hexadecimal value
		Skip(2);
		char ch1 = GetChar(false);
		char ch2 = GetChar(false);
		if (!IsHexNum(ch1) || !IsHexNum(ch2))
			throw CException("Expecting hexadecimal character value.");

		chValue = GetHexNumValue(ch1) * 16 + GetHexNumValue(ch2);
	}
	else
	{
		size_t nPos = GetCurrentPos();
		char const* pchStartPos = GetInput().c_str() + nPos;
		char* pchLastPos = nullptr;

		int nNumber = std::strtol(pchStartPos, &pchLastPos, 0);

		if (pchLastPos <= pchStartPos || 
			nNumber > std::numeric_limits<uchar>::max() ||
			nNumber < std::numeric_limits<char>::min())
			throw CException("Invalid character value.");

		SetCurrentPos(nPos + (pchLastPos - pchStartPos));

		if (nNumber < 0)
			chValue = (uchar) ((char) nNumber);
		else
			chValue = (uchar) nNumber;
	}

	if (chDelimiter != 0)
	{	// Ensure for delimiter
		chCurr = GetChar(true);
		if (chCurr != 0 && chCurr != chDelimiter)
			throw CException("Expecting delimiter.");
	}
	else if (!IsFinished() && !IsWhiteSpace())
	{	// Should be white space or end of string
		throw CException("Invalid character value.");
	}

	return chValue;
}

std::string CParser::ParseString()
{
	char chCurr = GetChar(true);
	if (chCurr != '"')
		throw CException("Expecting string literal opening quote (\").");

	FixCurrentPos();
	std::string sLiteral;
	while (true)
	{
		chCurr = GetChar(false);
		if (chCurr == '"')
			break;
		else if (chCurr == '\\')
			chCurr = GetChar(false); // skip symbol

		if (chCurr == 0)
			throw CException("Expecting string literal closing quote (\").");
		
		sLiteral.push_back(chCurr);
	}

	return std::move(sLiteral);
}

void CParser::RevertPreviousParse()
{
	m_nPos = m_nPrevPos;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VASM_CL_INPUT_H
#define VASM_CL_INPUT_H

//
// Includes
//
#include <base_definitions.h>
#include <base_parser.h>

// STL
#include <string>
#include <istream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Represents the input
//	Wraps input stream, tracks line numbers
//
class CInput
{
public:
	inline CInput(t_string sInputUnitName, std::istream& oInput);
	inline ~CInput();

	// Returns true if there is input to read, false otherwise (EOF reached after skipping empty lines)
	inline bool IsInput(bool bSkipEmptyLinesAndComments = true);

	// Reads single line from the input stream and returns it
	inline t_string GetLine();

	// Puts already read line back and will return it with next GetLine call
	// This function limited for single line, repeted call will diregard previous Line
	inline void PutBack(t_string sLine);

	// Returns the line number of the most recent GetLineCall
	inline t_index GetLineNumber() const;

	// Returns source unit name
	inline t_string const& UnitName() const;

	// Auxiliary methods
	inline void SkipEmptyLinesAndComments();

	// istram methods replication
	inline bool eof() const;
	inline bool good() const;

private:
	//
	//	Implementation
	//

private:
	//
	//	Contents
	//
	t_index			m_nLine;
	std::istream&	m_oInput;
	t_string		m_sUnitName;

	t_string		m_sLastLine;

public:
	//
	// Static constants
	//
	static const char s_cchComment = '#';
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CInput::CInput(t_string sInputUnitName, std::istream& oInput) :
	m_nLine(0), m_oInput(oInput), m_sUnitName(std::move(sInputUnitName))
{
}

inline CInput::~CInput() = default;

inline bool CInput::IsInput(bool bSkipEmptyLinesAndComments)
{
	bool isInput = false;
	if (!m_sLastLine.empty())
		isInput = true;
	else if (!bSkipEmptyLinesAndComments)
		isInput = m_oInput.good();
	else while (m_oInput.good())
	{
		std::getline(m_oInput, m_sLastLine);
		++m_nLine;

		// Find comments and eliminate
		base::CParser oParser(m_sLastLine);
		oParser.SkipWhiteSpaces();
		if (oParser.IsFinished() || oParser.PeekChar() == s_cchComment)
			continue; // Skip empty lines & comments

		isInput = true;
		break;
	}
	return isInput;
}

inline t_string CInput::GetLine()
{
	t_string sLine;
	if (!m_sLastLine.empty())
	{
		sLine = std::move(m_sLastLine);
	}
	else if (m_oInput.good())
	{
		std::getline(m_oInput, sLine);
		++m_nLine;
	}
	return std::move(sLine);
}

inline void CInput::PutBack(t_string sLine)
{
	if (sLine.empty())
		return; // Do nothing if line is empty

	if (!m_sLastLine.empty())
		VASM_THROW_ERROR(t_csz("Input: Can't put back more than one line"));

	m_sLastLine = std::move(sLine);
}

inline t_index CInput::GetLineNumber() const
{
	return m_nLine;
}

inline t_string const& CInput::UnitName() const
{
	return m_sUnitName;
}

inline bool CInput::eof() const
{
	return m_oInput.eof();
}

inline bool CInput::good() const
{
	return m_oInput.good();
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_INPUT_H

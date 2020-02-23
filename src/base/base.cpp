//
//	Includes
//
#include <base_global.h>
#include "definitions.h"
#include "exception.h"

// STL
#include <sstream>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VASM_BASE_EXPORT t_index const g_ciInvalid = ~t_index(0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CException Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CException::~CException() = default;

t_string CException::GetErrorMsg(bool bDetailed) const
{
	t_string sError;

	if (bDetailed)
	{
		std::stringstream oBuff(std::ios_base::out);
		oBuff << "Error: ";

		std::stringstream::pos_type nStrmPos = oBuff.tellp();
		if (!m_sErrorMsg.empty())
			oBuff << m_sErrorMsg;
		else
			oBuff << Base::what();

		if (oBuff.tellp() == nStrmPos)
			oBuff << "Unknown." << std::endl;
		else
			oBuff << "." << std::endl;

		if (m_cszComponent != nullptr)
			oBuff << "Component: " << m_cszComponent << "  ";

		if (m_cnLine >= 0 && m_cszFile != nullptr)
			oBuff << "File: " << m_cszFile << "  Line: " << m_cnLine << std::endl;

		sError = oBuff.str();
	}
	else
	{
		sError = m_sErrorMsg.empty() ? Base::what() : m_sErrorMsg;
	}

	return std::move(sError);
}

char const* CException::what() const
{
	if (!m_sErrorMsg.empty())
		return m_sErrorMsg.c_str();
	return Base::what();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

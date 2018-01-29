#ifndef VM_EXCEPTION_H
#define VM_EXCEPTION_H

//
//	Includes
//
#include "vm_definitions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	VM Exception definition
//
class CException : public base::CException
{
	typedef base::CException Base;

public:
	inline CException();
	inline CException(char const* const, t_index nLine, std::string const& sCommand);
	inline CException(std::string const& sErrMsg, t_index nLine, std::string const& sCommand);
	inline CException(std::string&& sErrMsg, t_index nLine, std::string const& sCommand);

	inline CException(std::exception const& o, t_index nLine, std::string const& sCommand);
	inline CException(base::CException const& o, t_index nLine, std::string const& sCommand);
	inline CException(base::CException&& o, t_index nLine, std::string const& sCommand);

	inline CException(CException const& o);
	inline CException(CException&& o);

	inline CException& operator=(CException const&);
	inline CException& operator=(CException&&);

	inline t_index GetCodeLine() const;
	inline std::string const& GetCommand() const;

	inline void Clear();

protected:
	t_index		m_nCodeLine;
	std::string	m_sCommand;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
inline CException::CException()
	: m_nCodeLine(0)
{
}

inline CException::CException(
	char const* const sErrMsg, t_index nLine, std::string const& sCommand)
	: Base(sErrMsg), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(
	std::string const& sErrMsg, t_index nLine, std::string const& sCommand)
	: Base(sErrMsg), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(std::string&& sErrMsg, t_index nLine, std::string const& sCommand)
	: Base(std::move(sErrMsg)), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(std::exception const& o, t_index nLine, std::string const& sCommand)
	: Base(o), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(base::CException const& o, t_index nLine, std::string const& sCommand)
	: Base(o), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(base::CException&& o, t_index nLine, std::string const& sCommand)
	: Base(std::move(o)), m_nCodeLine(nLine), m_sCommand(sCommand)
{
}

inline CException::CException(CException const& o)
	: Base(o), m_nCodeLine(o.m_nCodeLine), m_sCommand(o.m_sCommand)
{
}

inline CException::CException(CException&& o)
	: Base(std::move(o)), m_nCodeLine(o.m_nCodeLine), m_sCommand(std::move(o.m_sCommand))
{
}

inline CException& CException::operator=(CException const& o)
{
	Base::operator=(o);
	m_nCodeLine = o.m_nCodeLine;
	m_sCommand = o.m_sCommand;
	return *this;
}

inline CException& CException::operator=(CException&& o)
{
	Base::operator=(o);
	m_nCodeLine = o.m_nCodeLine;
	m_sCommand = std::move(o.m_sCommand);
	return *this;
}

inline t_index CException::GetCodeLine() const
{
	return m_nCodeLine;
}

inline std::string const& CException::GetCommand() const
{
	return m_sCommand;
}

inline void CException::Clear()
{
	m_nCodeLine = 0;
	m_sCommand.clear();
	Base::Clear();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespacevm vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_EXCEPTION_H

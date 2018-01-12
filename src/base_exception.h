#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

//
//	Includes
//
#include <exception>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Exception base definition
//
class CException : public std::exception
{
	typedef std::exception Base;

public:
	inline CException();

	inline CException(char const* const&);
	inline CException(std::string const& sErrMsg);
	inline CException(std::string&& sErrMsg);
	inline CException(std::exception const&);

	inline CException(CException const& o);
	inline CException(CException&& o);

	inline CException& operator=(CException const&);
	inline CException& operator=(std::exception const&);

	inline std::string const& GetErrorMsg() const;
	char const* what() const override;

	inline bool IsEmpty() const;
	inline void Clear();

protected:
	std::string	m_sErrorMsg;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
inline CException::CException()
{
}

inline CException::CException(char const* const& sErrMsg)
	: m_sErrorMsg(sErrMsg)
{
}

inline CException::CException(std::string const& sErrMsg)
	: m_sErrorMsg(sErrMsg)
{
}

inline CException::CException(std::string&& sErrMsg)
	: m_sErrorMsg(std::move(sErrMsg))
{
}

inline CException::CException(std::exception const& o)
	: m_sErrorMsg(o.what())
{
}

inline CException::CException(CException const& o)
	: Base(o), m_sErrorMsg(o.m_sErrorMsg)
{
}

inline CException::CException(CException&& o)
	: Base(o), m_sErrorMsg(std::move(o.m_sErrorMsg))
{
}

inline CException& CException::operator=(CException const& o)
{
	Base::operator=(o);
	m_sErrorMsg = o.m_sErrorMsg;
	return *this;
}

inline CException& CException::operator=(std::exception const& o)
{
	Base::operator=(std::exception());
	m_sErrorMsg = std::move(std::string(o.what()));
}

inline std::string const& CException::GetErrorMsg() const
{
	return m_sErrorMsg;
}

inline char const* CException::what() const
{
	if (!m_sErrorMsg.empty())
		return m_sErrorMsg.c_str();
	return Base::what();
}

inline bool CException::IsEmpty() const
{
	return m_sErrorMsg.empty();
}

inline void CException::Clear()
{
	m_sErrorMsg.clear();
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_EXCEPTION_H

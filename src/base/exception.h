#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

//
//	Includes
//
#include <base_definitions.h>

// STL
#include <stdexcept>
#include <exception>
#include <string>
#include <cassert>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Exception base definition
//
class CException : public std::exception
{
	typedef std::exception Base;

public:
	inline CException();
	~CException() override;

	inline CException(t_csz psErrMsg, t_csz pcszFile = t_csz(nullptr), int nLine = -1);
	inline CException(t_string const& sErrMsg, t_csz pcszFile = t_csz(nullptr), int nLine = -1);
	inline CException(t_string&& sErrMsg, t_csz pcszFile = t_csz(nullptr), int nLine = -1);

	inline CException(std::exception const&);
	inline CException(CException const& o);
	inline CException(CException&& o);

	inline CException& operator=(std::exception const&);
	inline CException& operator=(CException const&);
	inline CException& operator=(CException&&);

	virtual t_string GetErrorMsg(bool bDetailed = false) const;
	char const* what() const override;

	inline bool IsEmpty() const;
	inline void Clear();

protected:
	int const	m_cnLine;
	t_csz const	m_cszFile;
	t_string	m_sErrorMsg;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CException::CException() :
	Base(), m_cnLine(-1), m_cszFile(nullptr)
{
}

inline CException::CException(t_csz psErrMsg, t_csz pcszFile, int nLine) :
	Base(psErrMsg, 1), m_cnLine(nLine), m_cszFile(pcszFile)
{
}

inline CException::CException(t_string const& sErrMsg, t_csz pcszFile, int nLine) :
	Base("", 1), m_cnLine(nLine), m_cszFile(pcszFile), m_sErrorMsg(sErrMsg)
{
}

inline CException::CException(t_string&& sErrMsg, t_csz pcszFile, int nLine) :
	Base("", 1), m_cnLine(nLine), m_cszFile(pcszFile), m_sErrorMsg(sErrMsg)
{
}

inline CException::CException(std::exception const& o) :
	Base(o), m_cnLine(-1), m_cszFile(nullptr)
{
}

inline CException::CException(CException const& o) :
	Base(o), m_cnLine(o.m_cnLine), m_cszFile(o.m_cszFile), m_sErrorMsg(o.m_sErrorMsg)
{
}

inline CException::CException(CException&& o) :
	Base(o), m_cnLine(o.m_cnLine), m_cszFile(o.m_cszFile), m_sErrorMsg(std::move(o.m_sErrorMsg))
{
}

inline CException& CException::operator=(std::exception const& o)
{
	if (this != &o)
	{
		Base::operator=(o);
		m_sErrorMsg.clear();
	}
	return *this;
}

inline CException& CException::operator=(CException const& o)
{
	if (this != &o)
	{
		Base::operator=(o);
		m_sErrorMsg = o.m_sErrorMsg;
	}
	return *this;
}

inline CException& CException::operator=(CException&& o)
{
	if (this != &o)
	{
		Base::operator = (o);
		m_sErrorMsg = std::move(o.m_sErrorMsg);
	}
	return *this;
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
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Throw helpers
//
#define VASM_SRC_LINE								t_csz(__FILE__), int(__LINE__)
#define VASM_THROW_BAD_ALLOC( _allocsize_ )			throw ::vasm::base::CException(t_csz("Not enough memopry"), VASM_SRC_LINE )
#define VASM_THROW_INVALID_CONDITION()				throw ::vasm::base::CException(t_csz("Should not happen"), VASM_SRC_LINE )
#define VASM_THROW_INVALID_INDEX( _idx_, _size_ )	throw ::vasm::base::CException(t_csz("Invalid index"), VASM_SRC_LINE )
#define VASM_THROW_INVALID_POINTER()				throw ::vasm::base::CException(t_csz("Invalid pointer"), VASM_SRC_LINE )
#define VASM_THROW_ERROR( _message_ )				throw ::vasm::base::CException( _message_, VASM_SRC_LINE )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Runtime check helpers
//
#if defined NDEBUG
#	define VASM_ASSERT(_cndtn_)		static_cast<void>(false && (_cndtn_))
#	define VASM_VERIFY(_cndtn_)		((void)(_expression_))
#else
#	define VASM_ASSERT(_cndtn_)		assert((bool) (_cndtn_))
#	define VASM_VERIFY(_cndtn_)		VASM_ASSERT(_cndtn_)
#endif

#define VASM_CHECK(_cndtn_)							if( !bool(_cndtn_) ) { VASM_ASSERT(false); VASM_THROW_INVALID_CONDITION(); }
#define VASM_CHECK_X(_cndtn_, _message_)			if( !bool(_cndtn_) ) { VASM_ASSERT(false); VASM_THROW_ERROR(_message_); }
#define VASM_CHECK_IDX(_idx_, _size_)				if( _idx_ < 0 || _idx_ >= _size_ ) { VASM_ASSERT(false); VASM_THROW_INVALID_INDEX(_idx_,_size_); }
#define VASM_CHECK_IDX_X(_idx_, _size_, _message_)	if( _idx_ < 0 || _idx_ >= _size_ ) { VASM_ASSERT(false); VASM_THROW_ERROR(_message_); }
#define VASM_CHECK_PTR(_ptr_)						if( _ptr_ == nullptr ) { VASM_ASSERT(false); VASM_THROW_INVALID_POINTER(); }
#define VASM_CHECK_NEW_PTR(_ptr_)					if( _ptr_ == nullptr ) { VASM_ASSERT(false); VASM_THROW_BAD_ALLOC(sizeof(*_ptr_)); }
#define VASM_CHECK_NEW_PTRN(_ptr_, _count_)			if( _ptr_ == nullptr ) { VASM_ASSERT(false); VASM_THROW_BAD_ALLOC(sizeof(*_ptr_) * _count_); }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Other helpers
//
#define VASM_UNUSED(x)	(void)x;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_EXCEPTION_H

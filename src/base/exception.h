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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Exception base definition
//	Detailed error message includes information about source, line and component
//	Required for debugging to help identify source of problems
//	This class mostly is suitable to throw when program exception situation happened
//  This isnot suitable to handle errors caused by user actions which are normal from programs point of view
//
class CException : public std::exception
{
	using Base = std::exception;

public:
	struct SInfo
	{
		int		nLine;
		t_csz	cszFile;
		t_csz	cszComponent;
		t_csz	cszFixedErrorMsg;

		inline SInfo(t_csz const pcszFixedErrMsg = nullptr, int const cnLine = -1, 
					 t_csz const pcszFile = nullptr, t_csz const pcszComponent = nullptr);

		inline bool IsEmpty() const;
	};

public:
	inline CException();
	~CException() override;

	inline CException(t_csz const psFixedErrMsg,
					  int const cnLine = -1,
					  t_csz const pcszFile = t_csz(nullptr),
					  t_csz const pcszComponent = t_csz(nullptr));
	explicit inline CException(SInfo const& tInfo);
	inline CException(std::exception const& o);

	inline CException(CException const& o) = default;
	inline CException(CException&& o) = default;

	void operator=(CException const&) = delete;
	void operator=(CException&&) = delete;

	inline t_string GetErrorMsg() const;
	char const* what() const override;

	inline bool IsEmpty() const;
	inline int Line() const;
	inline t_csz File() const;
	inline t_csz Component() const;

	inline SInfo const& Info() const;

protected:
	virtual t_string FormatErrorMsg(bool bFinal) const;

private:
	SInfo const	 m_tInfo;
	static t_csz const s_cszDefaultErrorMsg;
};
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CError is designed to  carry error situations caused by user actions
//	Its detailed user message is user friendly an doesn't contains source code info
//
class CError : public base::CException
{
	using Base = base::CException;
public:
	//
	// Some generic error codes
	//
	enum
	{
		Unspecified = 0,
		BadArguments,
		FileOpen,
		FileRead,
		FileWrite,

		Reserved = 100
	};

	//
	//	Disposable error information
	//
	struct SInfo : Base::SInfo
	{
		int			nErrorCode = 0;
		t_string	sErrorMessage;

		inline SInfo(t_string const& sErrMsg = t_string(), int const cnErrCode = Unspecified, Base::SInfo const& = {});
		inline SInfo(t_string&& sErrMsg, int const cnErrCode = Unspecified, Base::SInfo const& = {});
		inline SInfo(Base::SInfo const& tBaseInfo);

		inline bool IsEmpty() const;
	};

public:
	inline CError();
	~CError() override;

	inline CError(int const cnErrorCode, Base::SInfo const& = {});
	inline CError(t_csz const pcszErrMsg, int const cnErrorCode = Unspecified, Base::SInfo const& = {});
	inline CError(t_string const& sErrMsg, int const cnErrorCode = Unspecified, Base::SInfo const& = {});
	inline CError(t_string&& sErrMsg, int const cnErrorCode = Unspecified, Base::SInfo const& = {});
	explicit inline CError(SInfo const& tErrInfo);
	explicit inline CError(SInfo&& tErrInfo);

	inline CError(CError const&) = default;
	inline CError(CError&&) = default;

	void operator=(CError const&) = delete;
	void operator=(CError&&) = delete;

public:
	inline int ErrorCode() const;
	inline SInfo GetInfo(bool const bFormatted = false) const;

protected:
	t_string FormatErrorMsg(bool bFinal) const override;

protected:
	int			m_nErrorCode;
	t_string	m_sErrorMsg;

	static t_csz const s_cszDefaultError;
	static t_csz const s_cszUserError;
};
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CException
//
inline CException::SInfo::SInfo(
	t_csz const errMsg, int const line, t_csz const file, t_csz const component) :
	nLine(line), cszFile(file), cszComponent(component), cszFixedErrorMsg(errMsg)
{
}

inline bool CException::SInfo::IsEmpty() const
{
	return (cszFixedErrorMsg == nullptr);
}

inline CException::CException() :
	CException(SInfo(s_cszDefaultErrorMsg , -1, nullptr, nullptr))
{
}

inline CException::CException(t_csz pcszFixedErrMsg, int cnLine, t_csz pcszFile, t_csz pcszComponent) :
	CException(SInfo(pcszFixedErrMsg, cnLine, pcszFile, pcszComponent))
{
}

inline CException::CException(SInfo const& tInfo) :
	Base(), m_tInfo(tInfo)
{
}

inline CException::CException(std::exception const& o) :
	Base(o), m_tInfo(o.what())
{
}

inline t_string CException::GetErrorMsg() const
{
	return std::move(FormatErrorMsg(true));
}

inline bool CException::IsEmpty() const
{
	return (m_tInfo.cszFixedErrorMsg == nullptr && m_tInfo.nLine == -1 && m_tInfo.cszFile == nullptr);
}

inline int CException::Line() const
{
	return m_tInfo.nLine;
}

inline t_csz CException::File() const
{
	return m_tInfo.cszFile;
}

inline t_csz CException::Component() const
{
	return m_tInfo.cszComponent;
}

inline CException::SInfo const& CException::Info() const
{
	return m_tInfo;
}

//
//	CError
//
inline CError::SInfo::SInfo(t_string const& sErrMsg, int const cnErrCode, Base::SInfo const& tBaseInfo) :
	Base::SInfo(tBaseInfo), nErrorCode(cnErrCode), sErrorMessage(sErrMsg)
{
}

inline CError::SInfo::SInfo(t_string&& sErrMsg, int const cnErrCode, Base::SInfo const& tBaseInfo) :
	Base::SInfo(tBaseInfo), nErrorCode(cnErrCode), sErrorMessage(sErrMsg)
{
}

inline CError::SInfo::SInfo(Base::SInfo const& tBaseInfo) :
	Base::SInfo(tBaseInfo), nErrorCode(Unspecified), sErrorMessage()
{
}

inline bool CError::SInfo::IsEmpty() const
{
	return (Base::SInfo::IsEmpty() && sErrorMessage.empty());
}

inline CError::CError() :
	Base(s_cszDefaultError), m_nErrorCode(Unspecified)
{
}

inline CError::CError(int const cnErrCode, Base::SInfo const& tBaseInfo) :
	CError(t_string(), cnErrCode, tBaseInfo)
{
}

inline CError::CError(t_csz pcszErrMsg, int const cnErrCode, Base::SInfo const& tBaseInfo) :
	CError(t_string(pcszErrMsg), cnErrCode, tBaseInfo)
{
}

inline CError::CError(t_string const& sErrMsg, int const cnErrCode, Base::SInfo const& tInfo) :
	Base(tInfo.IsEmpty() ? Base::SInfo(s_cszUserError) : tInfo),
	m_nErrorCode(cnErrCode), m_sErrorMsg(sErrMsg)
{
}

inline CError::CError(t_string&& sErrMsg, int const cnErrCode, Base::SInfo const& tInfo) :
	Base(tInfo.IsEmpty() ? Base::SInfo(s_cszUserError) : tInfo),
	m_nErrorCode(cnErrCode), m_sErrorMsg(sErrMsg)
{
}

inline CError::CError(SInfo const& tErrInfo) :
	Base(tErrInfo), m_nErrorCode(tErrInfo.nErrorCode), m_sErrorMsg(tErrInfo.sErrorMessage)
{
}

inline CError::CError(SInfo&& tErrInfo) :
	Base(tErrInfo), m_nErrorCode(tErrInfo.nErrorCode), m_sErrorMsg(tErrInfo.sErrorMessage)
{
}

inline int CError::ErrorCode() const
{
	return m_nErrorCode;
}

inline CError::SInfo CError::GetInfo(bool const bFormatted) const
{
	return bFormatted ? SInfo(GetErrorMsg(), m_nErrorCode, Base::Info()) :
						SInfo(m_sErrorMsg, m_nErrorCode, Base::Info());
}
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __COMPONENT__
#	define __COMPONENT__ nullptr
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Throw helpers
//
#define VASM_SRC_LINE								int(__LINE__), t_csz(__FILE__), t_csz(__COMPONENT__)
#define VASM_ERR_INFO( _fixed_error_message_ )		::vasm::base::CException::SInfo(t_csz(_fixed_error_message_), VASM_SRC_LINE)
#define VASM_THROW_BAD_ALLOC( _allocsize_ )			throw ::vasm::base::CException(VASM_ERR_INFO("Not enough memopry"))
#define VASM_THROW_INVALID_CONDITION()				throw ::vasm::base::CException(VASM_ERR_INFO("Invalid condition"))
#define VASM_THROW_INVALID_INDEX( _idx_, _size_ )	throw ::vasm::base::CException(VASM_ERR_INFO("Invalid index"))
#define VASM_THROW_INVALID_POINTER()				throw ::vasm::base::CException(VASM_ERR_INFO("Invalid pointer"))
#define VASM_THROW_ERROR( _fixed_error_message_ )	throw ::vasm::base::CException(VASM_ERR_INFO(_fixed_error_message_))
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
#define VASM_ASSERT_PTR(_ptr_)		VASM_ASSERT(_ptr_ != nullptr)

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
#define VASM_UNUSED(x)	(void)(x);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_EXCEPTION_H

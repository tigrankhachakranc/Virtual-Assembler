#ifndef VASM_CL_ENCODER_H
#define VASM_CL_ENCODER_H

//
// Includes
//
#include "intermediate.h"
#include <core_command_library.h>
#include <base_exception.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_address = core::t_address;
using t_uoffset = core::t_uoffset;

using CCommandLibraryPtr = core::CCommandLibraryPtr;

using t_aCode = std::vector<uint8>;
using t_aRelocTable = std::vector<t_uoffset>;

////////////////////////////////////////////////////////////////////////////////
//
//	Command Encoder
//
class CEncoder final
{
public:
	CEncoder(t_aCode& aCodeBuffer,
			 t_aRelocTable& aSymblRelocTbl,
			 t_aRelocTable& aLblRelocTbl,
			 std::vector<t_index> const& aVarToSymblIdxMapping,
			 std::vector<t_index> const& aFuncToSymblIdxMapping,
			 CCommandLibraryPtr pCmdLib);
	~CEncoder();

	// Non copiable nor movable
	CEncoder(CEncoder const&) = delete;
	CEncoder(CEncoder&&) = delete;
	void operator=(CEncoder const&) = delete;
	void operator=(CEncoder&&) = delete;

public:
	//
	//	Error
	//
	class CError : public base::CException
	{
		typedef base::CException Base;
	public:
		inline CError();
		inline CError(t_csz psErrMsg, t_index nLine, t_csz pszCommandName);
		inline CError(t_string const& sErrMsg, t_index nPos, t_csz pszCommandName);
		inline ~CError() = default;

		inline CError(CError const&) = default;
		inline CError(CError&&) = default;

	public:
		inline t_index LineNumber() const;
		inline t_csz CommandName() const;
		inline t_string const& ErrorMsg() const;

	private:
		t_index	m_nLineNumber;
		t_csz	m_pszCommandName;
	};

public:
	//
	//	Methods
	//

	// Encodes specified command into the code buffer at the specified offset and returns encoded length
	// Note that Code buffer should contain enough amount of memory required to Encode specified command
	t_uoffset Encode(SCommand const& tCmd, t_uoffset nCodeOffset);

private:
	t_aCode&					m_aCodeBuffer;
	t_aRelocTable&				m_aSymblRelocTbl;
	t_aRelocTable&				m_aLblRelocTbl;
	CCommandLibraryPtr			m_pCmdLibrary;
	std::vector<t_index> const&	m_aVarToSymblIdxMapping;
	std::vector<t_index> const&	m_aFuncToSymblIdxMapping;
};

// CCommandBase smart pointers
using CEncoderPtr = std::shared_ptr<CEncoder>;
using CEncoderWPtr = std::weak_ptr<CEncoder>;
using CEncoderUPtr = std::unique_ptr<CEncoder>;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////

//
//	CEncoder::CError
//
inline CEncoder::CError::CError() :
	Base(), m_nLineNumber(g_ciInvalid), m_pszCommandName(nullptr)
{
}

inline CEncoder::CError::CError(
	t_csz psErrMsg, t_index nLine, t_csz pszCommandName) :
	Base(t_string(psErrMsg)), m_nLineNumber(nLine), m_pszCommandName(pszCommandName)
{
}

inline CEncoder::CError::CError(
	t_string const& sErrMsg, t_index nLine, t_csz pszCommandName) :
	Base(sErrMsg), m_nLineNumber(nLine), m_pszCommandName(pszCommandName)
{
}

inline t_index CEncoder::CError::LineNumber() const
{
	return m_nLineNumber;
}

inline t_csz CEncoder::CError::CommandName() const
{
	return m_pszCommandName;
}

inline t_string const& CEncoder::CError::ErrorMsg() const
{
	return m_sErrorMsg;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_ENCODER_H

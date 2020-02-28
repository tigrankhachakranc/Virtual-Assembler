#ifndef VASM_CORE_VALUE_H
#define VASM_CORE_VALUE_H

//
// Includes
//
#include <vasm_global.h>
#include <base_definitions.h>
#include <base_exception.h>

// STL
#include <memory>
#include <functional>
#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Generic Value type
//
using t_byte = uint8;
using t_word = uint16;
using t_dword = uint32;
using t_qword = uint64;

using t_byte_array = std::vector<t_byte>;
using t_word_array = std::vector<t_word>;
using t_dword_array = std::vector<t_dword>;
using t_qword_array = std::vector<t_qword>;

//
//	Value Types
//
enum class EValueType : uint8
{
	Unknown = 0,
	Byte = sizeof(t_byte),
	Word = sizeof(t_word),
	DWord = sizeof(t_dword),
	QWord = sizeof(t_qword),
	Char,
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Value definition
//	serves as data carrier
//
class CValue
{
public:
	inline CValue(EValueType eType = EValueType::Unknown) noexcept; // constructs null value, with specified type
	CValue(EValueType eType, t_size nCount, void const* pData = nullptr);

	CValue(t_byte bData) noexcept;
	CValue(t_word wData) noexcept;
	CValue(t_dword dwData) noexcept;
	CValue(t_qword qwData) noexcept;
	CValue(t_char chData) noexcept;

	CValue(t_byte_array abData);
	CValue(t_word_array awData);
	CValue(t_dword_array adwbData);
	CValue(t_qword_array aqwData);
	
	CValue(t_string sData);
	CValue(t_cstring cszData, t_size nSize);

	CValue(CValue const& o);
	CValue(CValue&& o) noexcept;

	~CValue() noexcept;

	CValue& operator=(CValue const&);
	CValue& operator=(CValue&&) noexcept;

	// Acessor Methods
	inline EValueType GetType() const noexcept;
	inline t_size GetCount() const noexcept; // returns number of contained elements
	inline t_size GetSize() const noexcept;	// returns total size of allocated memory
	t_size GetElementSize() const noexcept;	// returns size of single element (size of type)

	inline bool IsNull() const noexcept;
	inline bool IsValid() const noexcept;
	inline bool IsString() const noexcept;

	inline operator t_byte const&() const noexcept;
	inline operator t_word const&() const noexcept;
	inline operator t_dword const&() const noexcept;
	inline operator t_qword const&() const noexcept;
	inline operator t_char const&() const noexcept;
	inline operator t_csz() const noexcept;
	inline operator bool() const noexcept;

	inline operator void const*() const noexcept;

	template <typename DataType>
	inline DataType const& operator[](t_index idx) const noexcept;

public:
	//
	//	Static helpers
	//
	static t_csz TypeToCStr(EValueType eType, bool bLongVersion = false) noexcept;

private:
	union SData
	{
		t_byte		b;
		t_word		w;
		t_dword		dw;
		t_qword		qw;
		t_char		ch;

		t_byte*		pb;
		t_word*		pw;
		t_dword*	pdw;
		t_qword*	pqw;
		t_char*		psz;

	};

	EValueType	m_eType;
	t_size		m_nCount;
	SData		m_tData;
};

using CValueArray = std::vector<CValue>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline template helpers
//
////////////////////////////////////////////////////////////////////////////////

// Value type from native data type
template<typename TDataType>
inline EValueType ValueType()
{
	return EValueType::Unknown;
}

template<>
inline EValueType ValueType<t_byte>()
{
	return EValueType::Byte;
}

template<>
inline EValueType ValueType<t_word>()
{
	return EValueType::Word;
}

template<>
inline EValueType ValueType<t_dword>()
{
	return EValueType::DWord;
}

template<>
inline EValueType ValueType<t_qword>()
{
	return EValueType::QWord;
}

template<>
inline EValueType ValueType<t_char>()
{
	return EValueType::Char;
}

// Sise of ValueType
inline uchar SizeOfValueType(EValueType e)
{
	return (e == EValueType::Char) ? sizeof(t_char) : t_size(e);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CValue::CValue(EValueType eType) noexcept
	: m_eType(eType), m_nCount(0)
{
	m_tData.qw = 0;
}

inline EValueType CValue::GetType() const noexcept
{
	return m_eType;
}

inline t_size CValue::GetCount() const noexcept
{
	return m_nCount;
}

inline t_size CValue::GetSize() const noexcept
{
	return m_nCount * GetElementSize();
}

inline bool CValue::IsNull() const noexcept
{
	return (m_nCount == 0);
}

inline bool CValue::IsValid() const noexcept
{
	return m_eType != EValueType::Unknown;
}


inline bool CValue::IsString() const noexcept
{
	return (m_eType == EValueType::Char && m_nCount > 1);
}

inline CValue::operator t_byte const&() const noexcept
{
	return (m_nCount > 1) ? *m_tData.pb : m_tData.b;
}

inline CValue::operator t_word const&() const noexcept
{
	return (m_nCount > 1) ? *m_tData.pw : m_tData.w;
}

inline CValue::operator t_dword const&() const noexcept
{
	return (m_nCount > 1) ? *m_tData.pdw : m_tData.dw;
}

inline CValue::operator t_qword const&() const noexcept
{
	return (m_nCount > 1) ? *m_tData.pqw : m_tData.qw;
}

inline CValue::operator t_char const&() const noexcept
{
	return (m_nCount > 1) ? *m_tData.psz : m_tData.ch;
}

inline CValue::operator t_csz() const noexcept
{
	return (IsString() ? m_tData.psz : nullptr);
}

inline CValue::operator bool() const noexcept
{
	return (m_nCount > 1) ? bool(*m_tData.pqw) : bool(m_tData.qw);
}

inline CValue::operator void const*() const noexcept
{
	void const* p = nullptr;
	if (m_nCount == 1)
		p = reinterpret_cast<void const*>(&m_tData.b);
	else if (m_nCount > 1)
		p = reinterpret_cast<void const*>(m_tData.pb);
	return p;
}

template <>
inline t_byte const& CValue::operator[]<t_byte>(t_index idx) const noexcept
{
	VASM_ASSERT(m_eType == EValueType::Byte && m_nCount > 1);
	return m_tData.pb[idx];
}

template <>
inline t_word const& CValue::operator[]<t_word>(t_index idx) const noexcept
{
	VASM_ASSERT(m_eType == EValueType::Word && m_nCount > 1);
	return m_tData.pw[idx];
}

template <>
inline t_dword const& CValue::operator[]<t_dword>(t_index idx) const noexcept
{
	VASM_ASSERT(m_eType == EValueType::DWord && m_nCount > 1);
	return m_tData.pdw[idx];
}

template <>
inline t_qword const& CValue::operator[]<t_qword>(t_index idx) const noexcept
{
	VASM_ASSERT(m_eType == EValueType::QWord && m_nCount > 1);
	return m_tData.pqw[idx];
}

template <>
inline t_char const& CValue::operator[]<t_char>(t_index idx) const noexcept
{
	VASM_ASSERT(m_eType == EValueType::Char && m_nCount > 1);
	return m_tData.psz[idx];
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ostream>

////////////////////////////////////////////////////////////////////////////////
//
//	Serialization operators
//
std::ostream& operator<<(std::ostream& os, vasm::core::CValue const& oValue);
////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CPRE_VALUE_H

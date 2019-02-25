#ifndef VM_DEFINITIONS_H
#define VM_DEFINITIONS_H

//
//	Includes
//
#include "base_definitions.h"
#include "base_exception.h"

// STL
#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_byte = uint8;
using t_word = uint16;
using t_dword = uint32;
using t_qword = uint64;

using t_byte_array = std::vector<t_byte>;
using t_word_array = std::vector<t_word>;
using t_dword_array = std::vector<t_dword>;
using t_qword_array = std::vector<t_qword>;


//
//	VM memory types
//
enum class EType : uint8
{
	Byte = sizeof(t_byte),
	Word = sizeof(t_word),
	DWord = sizeof(t_dword),
	QWord = sizeof(t_qword)
};

//
//	Value definition
//	serves as data carrier
//
class CValue
{
public:
	inline CValue(EType eType = EType::Byte); // constructs null value, with specified type
	CValue(EType eType, t_size nCount, void const* pData = nullptr);
	
	CValue(t_byte bData);
	CValue(t_word wData);
	CValue(t_dword dwData);
	CValue(t_qword qwData);

	CValue(t_byte_array abData);
	CValue(t_word_array awData);
	CValue(t_dword_array adwbData);
	CValue(t_qword_array aqwData);

	CValue(std::string sData);
	CValue(char const* cszData, t_size nSize);

	CValue(CValue const& o);
	CValue(CValue&& o);

	~CValue();

	CValue& operator=(CValue const&);
	CValue& operator=(CValue&&);

	// Acessor Methods
	inline EType GetType() const;
	inline t_size GetCount() const; // returns number of contained elements
	inline t_size GetSize() const;	// returns total size of allocated memory
	t_size GetElementSize() const;	// returns size of single element (size of type)
	
	inline bool IsNull() const;
	inline bool IsString() const;

	inline operator t_byte const&() const;
	inline operator t_word const&() const;
	inline operator t_dword const&() const;
	inline operator t_qword const&() const;

	inline operator void const*() const;

private:
	union SData
	{
		t_byte		b;
		t_word		w;
		t_dword		dw;
		t_qword		qw;
		
		t_byte*		pb;
		t_word*		pw;
		t_dword*	pdw;
		t_qword*	pqw;
	};

	EType	m_eType;
	bool	m_bString;
	t_size  m_nCount;
	SData	m_tData;
};

using t_aVariables = std::vector< std::pair<std::string, CValue> >;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline implementations
//
inline CValue::CValue(EType eType)
	: m_eType(eType), m_bString(false), m_nCount(0)
{
	m_tData.qw = 0;
}

inline EType CValue::GetType() const
{
	return m_eType;
}

inline t_size CValue::GetCount() const
{
	return m_nCount;
}

inline t_size CValue::GetSize() const
{
	return m_nCount * GetElementSize();
}

inline bool CValue::IsNull() const
{
	return (m_nCount == 0);
}

inline bool CValue::IsString() const
{
	return m_bString;
}

inline CValue::operator t_byte const&() const
{
	return (m_nCount > 1) ? *m_tData.pb : m_tData.b;
}

inline CValue::operator t_word const&() const
{
	return (m_nCount > 1) ? *m_tData.pw : m_tData.w;
}

inline CValue::operator t_dword const&() const
{
	return (m_nCount > 1) ? *m_tData.pdw : m_tData.dw;
}

inline CValue::operator t_qword const&() const
{
	return (m_nCount > 1) ? *m_tData.pqw : m_tData.qw;
}

inline CValue::operator void const*() const
{
	void const* p = nullptr;
	if (m_nCount == 1)
		p = reinterpret_cast<void const*>(&m_tData.b);
	else if (m_nCount > 1)
		p = reinterpret_cast<void const*>(m_tData.pb);
	return p;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespacevm base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_DEFINITIONS_H

//
//	Component
//
#define __COMPONENT__ "Core"

//
//	Includes
//
#include "value.h"
#include <base_exception.h>

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CValue::CValue(EValueType eType, t_size nCount, void const* pData)
	: m_eType(eType), m_nCount(nCount)
{
	m_tData.qw = 0;
	if (nCount > 1)
	{
		size_t nActualSize = 0;
		switch (m_eType)
		{
		case EValueType::Byte:
			m_tData.pb = new t_byte[m_nCount];
			nActualSize = m_nCount * sizeof(t_byte);
			break;
		case EValueType::Word:
			m_tData.pw = new t_word[m_nCount];
			nActualSize = m_nCount * sizeof(t_word);
			break;
		case EValueType::DWord:
			m_tData.pdw = new t_dword[m_nCount];
			nActualSize = m_nCount * sizeof(t_dword);
			break;
		case EValueType::QWord:
			m_tData.pqw = new t_qword[m_nCount];
			nActualSize = m_nCount * sizeof(t_qword);
			break;
		case EValueType::Char:
			m_nCount += (static_cast<t_csz>(pData)[nCount - 1] == t_char(0)) ? 0 : 1; // One more for the the trailing '\0'
			m_tData.psz = new t_char[m_nCount];
			nActualSize = nCount * sizeof(t_char);
			m_tData.psz[m_nCount-1] = t_char(0);
			break;
		default:
			VASM_THROW_ERROR("CValue: Unknown value type");
		}

		if (pData == nullptr)
			std::memset((void*) m_tData.pb, 0, nActualSize);
		else
			std::memcpy((void*) m_tData.pb, (void*) pData, nActualSize);
	}
	else
	{
		if (nCount == 1 && pData != nullptr)
		{
			switch (m_eType)
			{
			case EValueType::Byte:
				m_tData.b = *((t_byte const*) pData);
				break;
			case EValueType::Word:
				m_tData.w = *((t_word const*) pData);
				break;
			case EValueType::DWord:
				m_tData.dw = *((t_dword const*) pData);
				break;
			case EValueType::QWord:
				m_tData.qw = *((t_qword const*) pData);
				break;
			case EValueType::Char:
				m_tData.ch = *((t_char const*) pData);
				break;
			default:
				VASM_THROW_ERROR("CValue: Unknown value type");
			}
		}
	}
}

CValue::CValue(t_byte bData) noexcept
	: m_eType(EValueType::Byte), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.b = bData;
}

CValue::CValue(t_word wData) noexcept
	: m_eType(EValueType::Word), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.w = wData;
}

CValue::CValue(t_dword dwData) noexcept
	: m_eType(EValueType::DWord), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.dw = dwData;
}

CValue::CValue(t_qword qwData) noexcept
	: m_eType(EValueType::QWord), m_nCount(1)
{
	m_tData.qw = qwData;
}

CValue::CValue(t_char chData) noexcept
	: m_eType(EValueType::Char), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.ch = chData;
}

CValue::CValue(t_byte_array abData)
	: m_eType(EValueType::Byte), m_nCount(abData.size())
{
	m_tData.qw = 0;
	if (m_nCount == 1)
	{
		m_tData.b = abData.at(0);
	}
	else if (m_nCount > 1)
	{
		m_tData.pb = new t_byte[m_nCount];
		std::memcpy((void*) m_tData.pb, (void*) abData.data(), m_nCount);
	}
}

CValue::CValue(t_word_array awData)
	: m_eType(EValueType::Word), m_nCount(awData.size())
{
	m_tData.qw = 0;
	if (m_nCount == 1)
	{
		m_tData.w = awData.at(0);
	}
	else if (m_nCount > 1)
	{
		m_tData.pw = new t_word[m_nCount];
		std::memcpy((void*) m_tData.pw, (void*) awData.data(), m_nCount * sizeof(t_word));
	}
}

CValue::CValue(t_dword_array adwData)
	: m_eType(EValueType::DWord), m_nCount(adwData.size())
{
	m_tData.qw = 0;
	if (m_nCount == 1)
	{
		m_tData.dw = adwData.at(0);
	}
	else if (m_nCount > 1)
	{
		m_tData.pdw = new t_dword[m_nCount];
		std::memcpy((void*) m_tData.pdw, (void*) adwData.data(), m_nCount * sizeof(t_dword));
	}
}

CValue::CValue(t_qword_array aqwData)
	: m_eType(EValueType::QWord), m_nCount(aqwData.size())
{
	m_tData.qw = 0;
	if (m_nCount == 1)
	{
		m_tData.qw = aqwData.at(0);
	}
	else if (m_nCount > 1)
	{
		m_tData.pqw = new t_qword[m_nCount];
		std::memcpy((void*) m_tData.pqw, (void*) aqwData.data(), m_nCount * sizeof(t_qword));
	}
}

CValue::CValue(t_string sData)
	: CValue(sData.data(), sData.size())
{
}

CValue::CValue(t_cstring cszData, t_size nSize)
	: m_eType(EValueType::Char), m_nCount(0)
{
	m_tData.qw = 0;
	if (cszData != nullptr && nSize > 0)
	{
		m_nCount = (cszData[nSize-1] == t_char(0)) ?  nSize : nSize + 1;
		m_tData.psz = new t_char[m_nCount];
		std::memcpy((void*) m_tData.psz, (void*) cszData, nSize * sizeof(t_char));
		m_tData.psz[m_nCount - 1] = t_char(0); // last '\0'
	}
}

CValue::CValue(CValue const& o)
	: CValue(o.m_eType, o.m_nCount, static_cast<void const*>(o))
{
}

CValue::CValue(CValue&& o) noexcept
{
	m_eType = o.m_eType;
	m_nCount = o.m_nCount;
	m_tData.qw = o.m_tData.qw;

	// make other null
	o.m_nCount = 0;
	o.m_tData.qw = 0;
}

CValue::~CValue() noexcept
{
	if (m_nCount > 1)
		delete[] m_tData.pb;
	m_tData.qw = 0;
	m_nCount = 0;
}

CValue& CValue::operator=(CValue const& o)
{
	if (&o != this)
	{
		CValue oCopy(o.m_eType, o.m_nCount, static_cast<void const*>(o));
		operator=(std::move(oCopy));
	}
	return *this;
}

CValue& CValue::operator=(CValue&& o) noexcept
{
	if (&o != this)
	{
		if (m_nCount > 1)
			delete[] m_tData.pb;

		m_eType = o.m_eType;
		m_nCount = o.m_nCount;
		m_tData.qw = o.m_tData.qw;

		// make other null to not release any allocated memory
		o.m_nCount = 0;
		o.m_tData.qw = 0;
	}
	return *this;
}

t_size CValue::GetElementSize() const noexcept
{
	int nElemSize = 0;
	switch (m_eType)
	{
	case EValueType::Byte:
		nElemSize = sizeof(t_byte);
		break;
	case EValueType::Word:
		nElemSize = sizeof(t_word);
		break;
	case EValueType::DWord:
		nElemSize = sizeof(t_dword);
		break;
	case EValueType::QWord:
		nElemSize = sizeof(t_qword);
		break;
	case EValueType::Char:
		nElemSize = sizeof(t_char);
		break;
	default:
		//VASM_THROW_ERROR("CValue: Invalid value element size");
		break;
	}
	return nElemSize;
}

//
//	Static routines
//
t_csz CValue::TypeToCStr(EValueType eType, bool bLongVersion) noexcept
{
	switch (eType)
	{
	case EValueType::Byte:
		return bLongVersion ? t_csz("Byte") : t_csz("B ");
	case EValueType::Word:
		return bLongVersion ? t_csz("Word") : t_csz("W ");
	case EValueType::DWord:
		return bLongVersion ? t_csz("DWord") : t_csz("DW");
	case EValueType::QWord:
		return bLongVersion ? t_csz("QWord") : t_csz("QW");
	case EValueType::Char:
		return bLongVersion ? t_csz("Char") : t_csz("CH");
	default:
		break;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//
//	Serialization operators
//
std::ostream& operator<<(std::ostream& os, vasm::core::CValue const& oValue)
{
	vasm::t_size nCount = oValue.GetCount();
	if (nCount == 1)
	{
		switch (oValue.GetType())
		{
		case vasm::core::EValueType::Byte:
			os << +static_cast<vasm::core::t_byte>(oValue);
			break;
		case vasm::core::EValueType::Word:
			os << +static_cast<vasm::core::t_word>(oValue);
			break;
		case vasm::core::EValueType::DWord:
			os << +static_cast<vasm::core::t_dword>(oValue);
			break;
		case vasm::core::EValueType::QWord:
			os << +static_cast<vasm::core::t_qword>(oValue);
			break;
		case vasm::core::EValueType::Char:
			os << static_cast<vasm::t_char>(oValue);
			break;
		default:
			break;
		}
	}
	else if (nCount > 1)
	{
		switch (oValue.GetType())
		{
		case vasm::core::EValueType::Byte:
		{
			os << "{";
			for (vasm::t_index i = 0; i < nCount; ++i)
			{
				if (i > 0)
					os << ", ";
				os << +(*(&static_cast<vasm::core::t_byte const&>(oValue) + i));
			}
			os << "}";
			break;
		}
		case vasm::core::EValueType::Word:
		{
			os << "{";
			for (vasm::t_index i = 0; i < nCount; ++i)
			{
				if (i > 0)
					os << ", ";
				os << +(*(&static_cast<vasm::core::t_word const&>(oValue) + i));
			}
			os << "}";
			break;
		}
		case vasm::core::EValueType::DWord:
		{
			os << "{";
			for (vasm::t_index i = 0; i < nCount; ++i)
			{
				if (i > 0)
					os << ", ";
				os << +(*(&static_cast<vasm::core::t_dword const&>(oValue) + i));
			}
			os << "}";
			break;
		}
		case vasm::core::EValueType::QWord:
		{
			os << "{";
			for (vasm::t_index i = 0; i < nCount; ++i)
			{
				if (i > 0)
					os << ", ";
				os << +(*(&static_cast<vasm::core::t_qword const&>(oValue) + i));
			}
			os << "}";
			break;
		}
		case vasm::core::EValueType::Char:
		{
			os << '"' << static_cast<vasm::t_csz>(oValue) << '"';
			break;
		}
		default:
			break;
		}
	}
	return os;
}

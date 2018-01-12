//
//	Includes
//
#include "vm_definitions.h"

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CValue::CValue(EType eType, t_size nCount, void const* pData)
	: m_eType(eType), m_bString(false), m_nCount(nCount)
{
	if (nCount > 1)
	{
		size_t nActualSize = 0;
		switch (m_eType)
		{
		case EType::Byte:
			m_tData.pb = new t_byte[m_nCount];
			nActualSize = m_nCount * sizeof(t_byte);
			break;
		case EType::Word:
			m_tData.pw = new t_word[m_nCount];
			nActualSize = m_nCount * sizeof(t_word);
			break;
		case EType::DWord:
			m_tData.pdw = new t_dword[m_nCount];
			nActualSize = m_nCount * sizeof(t_dword);
			break;
		case EType::QWord:
			m_tData.pqw = new t_qword[m_nCount];
			nActualSize = m_nCount * sizeof(t_qword);
			break;
		default:
			break;
		}

		if (pData == nullptr)
			std::memset((void*) m_tData.pb, 0, nActualSize);
		else
			std::memcpy((void*) m_tData.pb, (void*) pData, nActualSize);
	}
	else
	{
		m_tData.qw = 0;
		if (nCount == 1 && pData != nullptr)
		{
			switch (m_eType)
			{
			case EType::Byte:
				m_tData.b = *((t_byte const*) pData);
				break;
			case EType::Word:
				m_tData.w = *((t_word const*) pData);
				break;
			case EType::DWord:
				m_tData.dw = *((t_dword const*) pData);
				break;
			case EType::QWord:
				m_tData.qw = *((t_qword const*) pData);
				break;
			}
		}
	}
}

CValue::CValue(t_byte bData)
	: m_eType(EType::Byte), m_bString(false), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.b = bData;
}

CValue::CValue(t_word wData)
	: m_eType(EType::Word), m_bString(false), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.w = wData;
}

CValue::CValue(t_dword dwData)
	: m_eType(EType::DWord), m_bString(false), m_nCount(1)
{
	m_tData.qw = 0;
	m_tData.dw = dwData;
}

CValue::CValue(t_qword qwData)
	: m_eType(EType::QWord), m_bString(false), m_nCount(1)
{
	m_tData.qw = qwData;
}

CValue::CValue(t_byte_array abData)
	: m_eType(EType::Byte), m_bString(false), m_nCount(abData.size())
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
	: m_eType(EType::Word), m_bString(false), m_nCount(awData.size())
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
	: m_eType(EType::DWord), m_bString(false), m_nCount(adwData.size())
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
	: m_eType(EType::QWord), m_bString(false), m_nCount(aqwData.size())
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

CValue::CValue(std::string sData)
	: CValue(sData.c_str(), sData.size())
{
}

CValue::CValue(char const* cszData, t_size nSize)
	: m_eType(EType::Byte), m_bString(true), m_nCount(nSize + 1)
{
	if (cszData != nullptr)
	{
		m_tData.pb = new t_byte[m_nCount];
		std::memcpy((void*) m_tData.pb, (void*) cszData, nSize);
		m_tData.pb[nSize] = 0; // last '\0'
	}
	else
	{
		m_tData.qw = 0;
		m_nCount = 0;
	}
}

CValue::CValue(CValue const& o)
	: CValue(o.m_eType, o.m_nCount, static_cast<void const*>(o))
{
	m_bString = o.m_bString;
}

CValue::CValue(CValue&& o)
{
	m_eType = o.m_eType;
	m_nCount = o.m_nCount;
	m_tData.pqw = o.m_tData.pqw;
	m_bString = o.m_bString;

	// make other null
	o.m_nCount = 0;
	o.m_tData.pqw = nullptr;
}

CValue::~CValue()
{
	if (m_nCount > 1)
		delete m_tData.pb;
	m_tData.qw = 0;
	m_nCount = 0;
}

CValue& CValue::operator=(CValue const& o)
{
	if (&o != this)
	{
		if (m_nCount > 1)
			delete m_tData.pb;

		CValue oCopy(o.m_eType, o.m_nCount, static_cast<void const*>(o));
		m_eType = oCopy.m_eType;
		m_nCount = oCopy.m_nCount;
		m_tData.pqw = oCopy.m_tData.pqw;
		m_bString = o.m_bString;

		// make copy null to not release any allocated memory
		oCopy.m_nCount = 0;
		oCopy.m_tData.pqw = nullptr;
	}
	return *this;
}

CValue& CValue::operator=(CValue&& o)
{
	if (&o != this)
	{
		if (m_nCount > 1)
			delete m_tData.pb;

		m_eType = o.m_eType;
		m_nCount = o.m_nCount;
		m_tData.pqw = o.m_tData.pqw;
		m_bString = o.m_bString;

		// make other null to not release any allocated memory
		o.m_nCount = 0;
		o.m_tData.pqw = nullptr;
	}
	return *this;
}

t_size CValue::GetElementSize() const
{
	int nElemSize = 0;
	switch (m_eType)
	{
	case EType::Byte:
		nElemSize = sizeof(t_byte);
		break;
	case EType::Word:
		nElemSize = sizeof(t_word);
		break;
	case EType::DWord:
		nElemSize = sizeof(t_dword);
		break;
	case EType::QWord:
		nElemSize = sizeof(t_qword);
		break;
	default:
		throw base::CException("Fatal error: Invalid value element size");
	}
	return nElemSize;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

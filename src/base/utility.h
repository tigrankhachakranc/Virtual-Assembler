#ifndef VASM_BASE_UTILITY_H
#define VASM_BASE_UTILITY_H

//
// Includes
//
#include <base_definitions.h>
#include <base_exception.h>

// STL
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <locale>
#include <sstream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	String formatting helpers
//
inline t_string toStr(t_csz pcsz, int nArg1)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	return str;
}

inline t_string toStr(t_csz pcsz, uint nArg1)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	return str;
}

inline t_string toStr(t_csz pcsz, int nArg1, int nArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	str.replace(str.find(t_csz("%2")), 2, std::to_string(nArg2));
	return str;
}

inline t_string toStr(t_csz pcsz, uint nArg1, uint nArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	str.replace(str.find(t_csz("%2")), 2, std::to_string(nArg2));
	return str;
}

inline t_string toStr(t_csz pcsz, t_string const& sArg1)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, sArg1);
	return str;
}

inline t_string toStr(t_csz pcsz, int nArg1, t_string const& sArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	str.replace(str.find(t_csz("%2")), 2, sArg2);
	return str;
}

inline t_string toStr(t_csz pcsz, uint nArg1, t_string const& sArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, std::to_string(nArg1));
	str.replace(str.find(t_csz("%2")), 2, sArg2);
	return str;
}

inline t_string toStr(t_csz pcsz, t_string const& sArg1, int nArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, sArg1);
	str.replace(str.find(t_csz("%2")), 2, std::to_string(nArg2));
	return str;
}

inline t_string toStr(t_csz pcsz, t_string const& sArg1, uint nArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, sArg1);
	str.replace(str.find(t_csz("%2")), 2, std::to_string(nArg2));
	return str;
}

inline t_string toStr(t_csz pcsz, t_string const& sArg1, t_string const& sArg2)
{
	t_string str(pcsz);
	str.replace(str.find(t_csz("%1")), 2, sArg1);
	str.replace(str.find(t_csz("%2")), 2, sArg2);
	return str;
}

inline t_string toStr(int8 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << uint8(nArg);
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(uint8 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << nArg;
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(int16 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << uint16(nArg);
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(uint16 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		if (bHex)
			buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << nArg;
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(int32 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << uint32(nArg);
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(uint32 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << nArg;
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(int64 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << uint64(nArg);
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}

inline t_string toStr(uint64 nArg, bool bHex = false)
{
	if (bHex && nArg != 0)
	{
		std::stringstream buff(std::ios_base::out);
		buff << t_csz("0x") << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << nArg;
		return std::move(buff.str());
	}
	return std::move(std::to_string(nArg));
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Range comparision
//
template <typename T> using TRange = std::pair<T, T>;

template <typename T>
struct TRangeLessThanComparator : std::binary_function<TRange<T>, TRange<T>, bool>
{
	using arg_type = TRange<T>;

	inline bool operator()(arg_type const& val1, arg_type const& val2) const
	{
		return val1.second < val2.first;
	}
};

template <typename T>
struct TRangeEqualComparator : std::binary_function<TRange<T>, TRange<T>, bool>
{
	using arg_type = TRange<T>;

	inline bool operator()(arg_type const& val1, arg_type const& val2) const
	{
		return val1.first == val2.first && val1.second < val2.second;
	}
};

template <typename T>
struct TRangeHash : std::unary_function<TRange<T>, t_size>
{
	inline result_type operator()(argument_type const& val) const
	{
		return std::hash( t_size(val.first) | (t_size(val.second) << (sizeof(t_size) / 2)));
	}
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	String operations
//

//
//	TStringRef
//
template <typename TString>
class TStringRef
{
public:
	inline TStringRef(TString const* pstr = nullptr);
	inline TStringRef(TStringRef const&) = default;

	inline TStringRef& operator=(nullptr_t);
	inline TStringRef& operator=(TString const*);
	inline TStringRef& operator=(TStringRef const&) = default;

	inline bool operator==(nullptr_t);
	inline bool operator!=(nullptr_t);

	inline operator TString const&() const;

	inline TString const& str() const;
	inline TString const* ptr() const;

private:
	TString m_str;
};

//
//	EComparisonType
//
enum class EComparisonType
{
	Less,		// case sensitive less than 
	More,		// case sensitive more than
	Equal,		// case sensitive equal
	Less_CI,	// case insensitive less than 
	More_CI,	// case insensitive more than
	Equal_CI	// case insensitive equal
};


//
//	TStringComparator
//
template <typename TString, EComparisonType ComparisonType>
struct TStringComparator : std::binary_function<TString, TString, bool>
{
	using value_type = TString;
	using data_type = typename TString::value_type;
	using data_cptr_type = typename TString::const_pointer;

	inline bool operator()(value_type const& v1, value_type const& v2) const;
	inline bool operator()(value_type const& v1, data_cptr_type v2) const;

	std::locale oLocale;
};

//
//	TStringRefComparator
//
template <typename TString, EComparisonType ComparisonType>
struct TStringRefComparator : std::binary_function<TStringRef<TString>, TStringRef<TString>, bool>
{
	using value_type = TStringRef<TString>;
	using data_cptr_type = typename TString::const_pointer;

	inline bool operator()(value_type v1, value_type v2) const;
	inline bool operator()(value_type v1, data_cptr_type v2) const;

	TStringComparator<TString, ComparisonType> tStrCmp;
};

//
//	Inline Implementations
//

template <typename TString>
inline TStringRef<TString>::TStringRef(TString const* pstr) :
	m_str(*pstr)
{
}

template <typename TString>
inline TStringRef<TString>& TStringRef<TString>::operator=(nullptr_t)
{
	m_str.clear();
	return *this;
}

template <typename TString>
inline TStringRef<TString>& TStringRef<TString>::operator=(TString const* pstr)
{
	m_str = *pstr;
	return *this;
}

template <typename TString>
inline bool TStringRef<TString>::operator==(nullptr_t)
{
	return (m_str.empty());
}

template <typename TString>
inline bool TStringRef<TString>::operator!=(nullptr_t)
{
	return (!m_str.empty());
}

template <typename TString>
inline TStringRef<TString>::operator TString const&() const
{
	//VASM_CHECK_PTR(m_pstr);
	return m_str;
}

template <typename TString>
inline TString const& TStringRef<TString>::str() const
{
	//VASM_CHECK_PTR(m_pstr);
	return m_str;
}

template <typename TString>
inline TString const* TStringRef<TString>::ptr() const
{
	return &m_str;
}

template <typename TString, EComparisonType ComparisonType>
inline bool TStringComparator<TString, ComparisonType>::operator()(
	value_type const& v1, value_type const& v2) const
{
	bool bRes = false;
	switch (ComparisonType)
	{
	case EComparisonType::Less:
		bRes = (v1 < v2);
		break;
	case EComparisonType::More:
		bRes = (v1 > v2);
		break;
	case EComparisonType::Equal:
		bRes = (v1 == v2);
		break;
	case EComparisonType::Less_CI:
		bRes = std::lexicographical_compare(
			v1.begin(), v1.end(), v2.begin(), v2.end(),
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) < std::tolower(ch2, oLocale));
		});
		break;
	case EComparisonType::More_CI:
		bRes = std::lexicographical_compare(
			v1.begin(), v1.end(), v2.begin(), v2.end(),
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) > std::tolower(ch2, oLocale));
		});
		break;
	case EComparisonType::Equal_CI:
		bRes = std::equal(
			v1.begin(), v1.end(), v2.begin(), v2.end(),
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) == std::tolower(ch2, oLocale));
		});
		break;
	}
	return bRes;
}

template <typename TString, EComparisonType ComparisonType>
inline bool TStringComparator<TString, ComparisonType>::operator()(
	value_type const& v1, data_cptr_type v2) const
{
	bool bRes = false;
	switch (ComparisonType)
	{
	case EComparisonType::Less:
		bRes = (v1 < v2);
		break;
	case EComparisonType::More:
		bRes = (v1 > v2);
		break;
	case EComparisonType::Equal:
		bRes = (v1 == v2);
		break;
	case EComparisonType::Less_CI:
	{
		data_cptr_type v2_end = v2;
		while (v2_end != data_type(0))
			++v2_end;
		bRes = std::lexicographical_compare(
			v1.begin(), v1.end(), v2, v2_end,
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) < std::tolower(ch2, oLocale));
		});
		break;
	}
	case EComparisonType::More_CI:
	{
		data_cptr_type v2_end = v2;
		while (v2_end != data_type(0))
			++v2_end;
		bRes = std::lexicographical_compare(
			v1.begin(), v1.end(), v2, v2_end,
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) > std::tolower(ch2, oLocale));
		});
		break;
	}
	case EComparisonType::Equal_CI:
	{
		bRes = std::equal(
			v1.begin(), v1.end(), v2,
			[this](TString::value_type ch1, TString::value_type ch2)
		{
			return (std::tolower(ch1, oLocale) == std::tolower(ch2, oLocale));
		});
		break;
	}}
	return bRes;
}

template <typename TString, EComparisonType ComparisonType>
inline bool TStringRefComparator<TString, ComparisonType>::operator()(value_type v1, value_type v2) const
{
	return tStrCmp(v1.str(), v2.str());
}

template <typename TString, EComparisonType ComparisonType>
inline bool TStringRefComparator<TString, ComparisonType>::operator()(value_type v1, data_cptr_type v2) const
{
	return tStrCmp(v1.str(), v2);
}


//
//	function EndsWith
//	Case Insensitive Implementation of endsWith()
//	It checks if the string 'mainStr' ends with given string 'toMatch'
//
inline bool EndsWith(t_string const& sSource, t_string const& sTarget)
{
	bool bResult = false;
	if (sSource.size() >= sTarget.size())
	{
		auto itTarget = sTarget.begin();
		auto itSource = std::next(sSource.begin(), sSource.size() - sTarget.size());
		bResult = std::all_of(itSource, sSource.end(), [&itTarget](char const& c)
		{
			return ::tolower(c) == ::tolower(*(itTarget++));
		});
	}
	return bResult;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Helper templates
//	Cleans and explicily adds signed/unsigned modifier from the type 
//
template <typename TDataType>
struct TCleanType
{
};

template <> struct TCleanType<int8>
{
	typedef int8	signed_type;
	typedef uint8	usigned_type;
};

template <> struct TCleanType<uint8>
{
	typedef int8	signed_type;
	typedef uint8	usigned_type;
};

template <> struct TCleanType<int16>
{
	typedef int16	signed_type;
	typedef uint16	usigned_type;
};

template <> struct TCleanType<uint16>
{
	typedef int16	signed_type;
	typedef uint16	usigned_type;
};

template <> struct TCleanType<int32>
{
	typedef int32	signed_type;
	typedef uint32	usigned_type;
};

template <> struct TCleanType<uint32>
{
	typedef int32	signed_type;
	typedef uint32	usigned_type;
};

template <> struct TCleanType<int64>
{
	typedef int64	signed_type;
	typedef uint64	usigned_type;
};

template <> struct TCleanType<uint64>
{
	typedef int64	signed_type;
	typedef uint64	usigned_type;
};
////////////////////////////////////////////////////////////////////////////////

// Force instantiation
template class  TStringRef<t_string>;
template struct TStringComparator<t_string, EComparisonType::Equal_CI>;
template struct TStringComparator<t_string, EComparisonType::Less_CI>;
template struct TStringRefComparator<t_string, EComparisonType::Equal_CI>;
template struct TStringRefComparator<t_string, EComparisonType::Less_CI>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using CStringRef = base::TStringRef<t_string>;
using EComparisonType = base::EComparisonType;

template <EComparisonType ComparisonType>
using CStringComparator = base::TStringComparator<t_string, ComparisonType>;

template <EComparisonType ComparisonType>
using CStringRefComparator = base::TStringRefComparator<t_string, ComparisonType>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // VASM_BASE_UTILITY_H

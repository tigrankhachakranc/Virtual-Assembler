#ifndef VASM_CORE_FLAGS_H
#define VASM_CORE_FLAGS_H

//
// Includes
//
#include <base_definitions.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	CPU Flags
//
class CFlags
{
public:
	enum : uint16
	{
		eMask_CF = 0x0001, 
		eMask_ZF = 0x0002,
		eMask_SF = 0x0004,
		eMask_OF = 0x0008,

		eMask_ALL = 0x000F
	};

	inline CFlags();

	inline bool getCarry() const;
	inline bool getZero() const;
	inline bool getSign() const;
	inline bool getOverflow() const;
	inline uint16 getFlags() const;

	inline void setCarry(bool);
	inline void setZero(bool);
	inline void setSign(bool);
	inline void setOverflow(bool);
	inline void setFlags(uint16);

	inline void clearCarry();
	inline void clearZero();
	inline void clearSign();
	inline void clearOverflow();
	inline void clearFlags();

	inline bool isEqual() const;
	inline bool isNotEqual() const;
	inline bool isAbove() const;
	inline bool isAboveOrEqual() const;
	inline bool isBelow() const;
	inline bool isBelowOrEqual() const;
	inline bool isGreat() const;
	inline bool isGreatOrEqual() const;
	inline bool isLow() const;
	inline bool isLowOrEqual() const;
	inline bool isOverflow() const;
	inline bool isNotOverflow() const;
	inline bool isSigned() const;
	inline bool isNotSigned() const;

	inline bool isCC(ECndtnCode eCC) const;

private:
	uint16	m_ui16Value;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CFlags::CFlags()
	: m_ui16Value(0)
{
}

inline bool CFlags::getCarry() const
{
	return (m_ui16Value & eMask_CF) != 0;
}

inline bool CFlags::getZero() const
{
	return (m_ui16Value & eMask_ZF) != 0;
}

inline bool CFlags::getSign() const
{
	return (m_ui16Value & eMask_SF) != 0;
}

inline bool CFlags::getOverflow() const
{
	return (m_ui16Value & eMask_OF) != 0;
}

inline uint16 CFlags::getFlags() const
{
	return (m_ui16Value & eMask_ALL);
}

inline void CFlags::setCarry(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_CF)) : (m_ui16Value & ~uint16(eMask_CF));
}

inline void CFlags::setZero(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_ZF)) : (m_ui16Value & ~uint16(eMask_ZF));
}

inline void CFlags::setSign(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_SF)) : (m_ui16Value & ~uint16(eMask_SF));
}

inline void CFlags::setOverflow(bool b)
{
	m_ui16Value = b ? (m_ui16Value | uint16(eMask_OF)) : (m_ui16Value & ~uint16(eMask_OF));
}

inline void CFlags::setFlags(uint16 ui16Value)
{
	m_ui16Value = (ui16Value & eMask_ALL);
}

inline void CFlags::clearCarry()
{
	m_ui16Value &= ~uint16(eMask_CF);
}

inline void CFlags::clearZero()
{
	m_ui16Value &= ~uint16(eMask_ZF);
}

inline void CFlags::clearSign()
{
	m_ui16Value &= ~uint16(eMask_SF);
}

inline void CFlags::clearOverflow()
{
	m_ui16Value &= ~uint16(eMask_OF);
}

inline void CFlags::clearFlags()
{
	m_ui16Value = 0;
}

inline bool CFlags::isEqual() const
{
	return (getZero() == true);
}

inline bool CFlags::isNotEqual() const
{
	return (getZero() == false);
}

inline bool CFlags::isAbove() const
{
	return (getCarry() == false && getZero() == false);
}

inline bool CFlags::isAboveOrEqual() const
{
	return (getCarry() == false);
}

inline bool CFlags::isBelow() const
{
	return (getCarry() == true);
}

inline bool CFlags::isBelowOrEqual() const
{
	return (getCarry() == true || getZero() == true);
}

inline bool CFlags::isGreat() const
{
	return (getZero() == false && getSign() == getOverflow());
}

inline bool CFlags::isGreatOrEqual() const
{
	return (getSign() == getOverflow());
}

inline bool CFlags::isLow() const
{
	return (getSign() != getOverflow());
}

inline bool CFlags::isLowOrEqual() const
{
	return (getZero() == true && getSign() != getOverflow());
}

inline bool CFlags::isOverflow() const
{
	return (getOverflow() == true);
}

inline bool CFlags::isNotOverflow() const
{
	return (getOverflow() == false);
}

inline bool CFlags::isSigned() const
{
	return (getSign() == true);
}

inline bool CFlags::isNotSigned() const
{
	return (getSign() == false);
}

inline bool CFlags::isCC(ECndtnCode eCC) const
{
	bool bRes = false;
	switch (eCC)
	{
	case ECndtnCode::Equal:
		bRes = isEqual();
		break;
	case ECndtnCode::NotEqual:
		bRes = isNotEqual();
		break;
	case ECndtnCode::Above:
		bRes = isAbove();
		break;
	case ECndtnCode::AboveEqual:
		bRes = isAboveOrEqual();
		break;
	case ECndtnCode::Below:
		bRes = isBelow();
		break;
	case ECndtnCode::BelowEqual:
		bRes = isBelowOrEqual();
		break;
	case ECndtnCode::Great:
		bRes = isGreat();
		break;
	case ECndtnCode::GreatEqual:
		bRes = isGreatOrEqual();
		break;
	case ECndtnCode::Low:
		bRes = isLow();
		break;
	case ECndtnCode::LowEqual:
		bRes = isLowOrEqual();
		break;
	case ECndtnCode::Overflow:
		bRes = isOverflow();
		break;
	case ECndtnCode::NotOverflow:
		bRes = isNotOverflow();
		break;
	case ECndtnCode::Signed:
		bRes = isSigned();
		break;
	case ECndtnCode::NotSigned:
		bRes = isNotSigned();
		break;
	default:
		break;
	}
	return bRes;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace casm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_FLAGS_H
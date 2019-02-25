//
//	Includes
//
#include "io_devices.h"
#include <base_exception.h>
#include <base_utility.h>

// STL
#include <cstdio>
#include <iostream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CConsoleDevice Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-Tors
//
CStreamDevice::CStreamDevice(
	t_csz cszName, t_port nPortBase, std::istream& in, std::ostream& out) :
	m_in(in), m_out(out), m_eMode(EMode::Char), m_pcszName(cszName)
{
	m_aSlots[0] = {nPortBase, EValueType::DWord};
	m_aSlots[1] = {nPortBase + sizeof(uint32), EValueType::QWord};
}

CStreamDevice::~CStreamDevice() = default;

//
//	IDevice interface
//
t_string CStreamDevice::GetName() const
{
	return std::move(t_string(m_pcszName));
}

t_size CStreamDevice::GetSlotCount() const
{
	return (sizeof(m_aSlots) / sizeof(SSlotInfo));
}

IDevice::SSlotInfo CStreamDevice::GetSlotInfo(t_index nSlot) const
{
	SSlotInfo tInfo;
	if (nSlot >= 0 && nSlot < GetSlotCount())
		tInfo = m_aSlots[nSlot];
	return tInfo;
}

void CStreamDevice::In(t_index nSlot, CValue& oValue)
{
	if (nSlot == 0)
	{	// Mode
		oValue = uint32(m_eMode);
	}
	else if (nSlot == 1)
	{
		switch (m_eMode)
		{
		case EMode::Char:
		{
			t_char ch;
			m_in.get(ch);
			oValue = CValue(ch);
			break;
		}
		case EMode::Number:
		{
			switch (oValue.GetType())
			{
			case EValueType::Byte:
			{
				t_byte val;
				m_in >> val;
				oValue = CValue(val);
				break;
			}
			case EValueType::Word:
			{
				t_word val;
				m_in >> val;
				oValue = CValue(val);
				break;
			}
			case EValueType::DWord:
			{
				t_dword val;
				m_in >> val;
				oValue = CValue(val);
				break;
			}
			case EValueType::QWord:
			{
				t_qword val;
				m_in >> val;
				oValue = CValue(val);
				break;
			}
			default:
			{
				t_char ch;
				m_in.get(ch);
				oValue = CValue(ch);
				break;
			}}
			break;
		}
		case EMode::Boolean:
		{
			bool val;
			m_in >> val;
			oValue = CValue(t_byte(val));
			break;
		}
		default:
			VASM_ASSERT(false);
			break;
		}
	}
}

void CStreamDevice::Out(t_index nSlot, CValue const& oValue)
{
	if (nSlot == 0)
	{	// Mode
		uint nMode = oValue;
		switch (nMode)
		{
		case 0:
			FlushOut();
			break;
		case 1:
			m_eMode = EMode::Char;
			m_oBuffer << std::noboolalpha;
			break;
		case 2:
			m_eMode = EMode::Number;
			m_oBuffer << std::noboolalpha;
			break;
		case 3:
			m_eMode = EMode::Boolean;
			m_oBuffer << std::boolalpha;
			break;
		case 4:
			m_oBuffer << std::dec;
			break;
		case 5:
			m_oBuffer << std::hex << std::nouppercase;
			break;
		case 6:
			m_oBuffer << std::hex << std::uppercase;
			break;
		case 7:
			m_oBuffer << std::setw(static_cast<uint32>(oValue));
			break;
		case 8:
			m_oBuffer << std::setfill(static_cast<char>(oValue));
			break;
		case 9:
			m_oBuffer << std::scientific;
			break;
		case 10:
			m_oBuffer << std::defaultfloat;
			break;
		case 11:
			m_oBuffer << std::showbase;
			break;
		case 12:
			m_oBuffer << std::noshowbase;
			break;
		}
	}
	else if (nSlot == 1)
	{
		switch (m_eMode)
		{
		case EMode::Char:
			m_oBuffer << static_cast<t_char>(oValue);
			break;
		case EMode::Number:
			m_oBuffer << oValue;
			break;
		case EMode::Boolean:
			m_oBuffer << static_cast<bool>(oValue);
			break;
		default:
			break;
		}
	}
}

//
//	Internal routines
//
void CStreamDevice::FlushOut()
{
	m_out << std::move(m_oBuffer.str());
	m_out.flush();
	m_oBuffer.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CConsoleDevice Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CConsoleDevice::CConsoleDevice(std::istream& cin, std::ostream& cout) :
	CStreamDevice(t_csz("Console"), t_port(4), cin, cout)
{
}

CConsoleDevice::~CConsoleDevice() = default;

void CConsoleDevice::In(t_index nSlot, CValue& oValue)
{
	if (nSlot == 1)
		ostrm() << std::endl << "(VM) << ";
	CStreamDevice::In(nSlot, oValue);
}

void CConsoleDevice::FlushOut()
{
	ostrm() << std::endl << "(VM) >> ";
	CStreamDevice::FlushOut();
	ostrm() << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


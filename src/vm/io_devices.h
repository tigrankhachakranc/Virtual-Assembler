#ifndef VASM_VM_IO_DEVICES_H
#define VASM_VM_IO_DEVICES_H

//
// Includes
//
#include <core_io_controller.h>

// STL
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_port		= core::t_port;
using t_byte		= core::t_byte;
using t_word		= core::t_word;
using t_dword		= core::t_dword;
using t_qword		= core::t_qword;

using CValue		= core::CValue;
using CValueArray	= core::CValueArray;
using EValueType	= core::EValueType;
using IDevice		= core::IDevice;
using IDevicePtr	= core::IDevicePtr;

////////////////////////////////////////////////////////////////////////////////
//
//	Stream Device
//	Slot 0: Get/Set I/O mode
//	Slot 1: Get/Set value
//
//	IO Modes are:
//	0:	Flush out buffer
//	1:	Single character
//	2:	Numeric value
//	3:	Boolean value (std::boolalpha)
//	4:	Sets number format to Decimal (std::dec)
//	5:	Sets number format to lowercase Hexadecimal (std::hex, std::lowercase)
//	6:	Sets number format to uppercase Hexadecimal (std::hex, std::uppercase)
//	7:	Number width (std::setw)
//	8:	Numeric fill (srd::setfill)
//	9:	Set real numbers format to scientific (std::scientific)
//	10:	Set real numbers format to default (std::defaultfloat)
//	11:	Show number base (std::showbase)
//	12:	Hide number base (std::showbase)
//
class CStreamDevice : public IDevice
{
public:
	CStreamDevice(t_csz cszName, t_port nBase, std::istream& in, std::ostream& out);
	~CStreamDevice() override;

public:
	enum class EMode
	{
		Char,
		Number,
		Boolean
	};

public:
	//
	//	IDevice Interface
	//
	t_string  GetName() const override;

	t_size    GetSlotCount() const override;
	SSlotInfo GetSlotInfo(t_index nSlot) const override;

	void In(t_index nSlot, CValue& oValue) override;
	void Out(t_index nSlot, CValue const& oValue) override;

protected:
	//
	//	Implementation
	//
	virtual void FlushOut();

	inline std::stringstream& Buffer();
	inline std::stringstream const& Buffer() const;

	inline std::istream& istrm();
	inline std::ostream& ostrm();

private:
	//
	//	Contents
	//
	std::istream&		m_in;
	std::ostream&		m_out;

	EMode				m_eMode;
	std::stringstream	m_oBuffer;

	t_csz		m_pcszName;
	SSlotInfo	m_aSlots[2];
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Console Device
//	Occupied ports: [4-7], [8-15]
//	Represents std::in & std::out, 
//
class CConsoleDevice : public CStreamDevice
{
public:
	CConsoleDevice(std::istream& cin, std::ostream& cout);
	~CConsoleDevice() override;

protected:
	void In(t_index nSlot, CValue& oValue) override;
	void FlushOut() override;
};
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline std::stringstream& CStreamDevice::Buffer()
{
	return m_oBuffer;
}

inline std::stringstream const& CStreamDevice::Buffer() const
{
	return m_oBuffer;
}

inline std::istream& CStreamDevice::istrm()
{
	return m_in;
}

inline std::ostream& CStreamDevice::ostrm()
{
	return m_out;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_IO_CONTROLLER_H

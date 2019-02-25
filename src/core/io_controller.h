#ifndef VASM_CORE_IO_CONTROLLER_H
#define VASM_CORE_IO_CONTROLLER_H

//
// Includes
//
#include "value.h"
#include <base_utility.h>

// STL
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Definition for the port address
//
using t_port = uint16;	// Port index
using t_PortRange = base::TRange<t_port>;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IO Device
//	Interface to Handle IN & OUT requests
//
class IDevice
{
public:
	virtual ~IDevice();

	// Slot is the specific port index relative to the single Device
	// Wile Ports are continous bytes slots are discrete, each with its size
	struct SSlotInfo
	{
		// Base port index of the slot
		t_port		nBase = 0;
		// Value type determines Port range counted from the Base
		EValueType	eType = EValueType::Unknown;

		inline SSlotInfo() = default;
		inline SSlotInfo(SSlotInfo const&) = default;
	};

	virtual t_string GetName() const = 0;

	virtual t_size    GetSlotCount() const = 0;
	virtual SSlotInfo GetSlotInfo(t_index nSlot) const = 0;

	// Handles Input from the Device operations
	virtual void In(t_index nSlot, CValue& oValue)  = 0;
	// Handles Output to the Device operations
	virtual void Out(t_index nSlot, CValue const& oValue)  = 0;
};

using IDevicePtr = std::shared_ptr<IDevice>;
using IDeviceCPtr = std::shared_ptr<IDevice const>;
using IDeviceWPtr = std::weak_ptr<IDevice>;
using IDeviceUPtr = std::unique_ptr<IDevice>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	IO Controller
//	Keeps IO Devices and dispatches IN & OUT requests to IO Devices
//
class CIOController final
{
public:
	CIOController();
	~CIOController();

	// Neither copiable, nor movable
	CIOController(CIOController const&) = delete;
	CIOController(CIOController&&) = delete;
	void operator=(CIOController const&) = delete;
	void operator=(CIOController&&) = delete;

public:
	//
	//	Methods
	//
	std::vector<std::pair<t_PortRange, IDeviceCPtr>> GetPorts() const;

	// Registers specified device
	// Throws when specified Device could not be registered (i.e. because of port range conflict)
	void Register(IDevicePtr pDevice);
	// Unregisters specified Device
	// Returns true if the device is found and false if it not registered
	bool Unregister(IDevicePtr pDevice);

	// Dispatches Input from the Device operations
	// Throws in case of error
	void In(t_port nPort, CValue& oValue);
	// Dispatches Output to the Device operations
	// Throws in case of error
	void Out(t_port nPort, CValue const& oValue);

private:
	//
	//	Implementation
	//
	using t_PortComparator = base::TRangeLessThanComparator<t_port>;
	struct SSlotInfo : public IDevice::SSlotInfo
	{
		t_index		nSlot;
		IDevicePtr	pDevice;

		inline SSlotInfo() = default;
		inline SSlotInfo(SSlotInfo const&) = default;
		inline SSlotInfo(IDevice::SSlotInfo const&);
	};
	using t_PortMap = std::map<t_PortRange, SSlotInfo, t_PortComparator>;

	// Finds Slot and returns its reference fro the specified Port and Value type
	// If not found or Port does not correspond to the specified type then trows error
	SSlotInfo const& Lookup(t_port const nPort, EValueType const eType) const;

private:
	//
	//	Content
	//
	t_PortMap	m_mapPorts;
};

using CIOControllerPtr = std::shared_ptr<CIOController>;
using CIOControllerWPtr = std::weak_ptr<CIOController>;
using CIOControllerUPtr = std::unique_ptr<CIOController>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CIOController::SSlotInfo::SSlotInfo(IDevice::SSlotInfo const& o) :
	IDevice::SSlotInfo(o), nSlot(0), pDevice(nullptr)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_IO_CONTROLLER_H

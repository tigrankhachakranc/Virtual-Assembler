//
//	Component
//
#define __COMPONENT__ "Core"

//
//	Includes
//
#include "io_controller.h"
#include <base_exception.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IDevice implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IDevice::~IDevice() = default;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CIOController implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CIOController::CIOController() = default;
CIOController::~CIOController() = default;

std::vector<std::pair<t_PortRange, IDeviceCPtr>> CIOController::GetPorts() const
{
	std::vector< std::pair<t_PortRange, IDeviceCPtr>> aPorts;
	aPorts.reserve(m_mapPorts.size());
	for (auto const& tItem : m_mapPorts)
		aPorts.push_back({tItem.first, tItem.second.pDevice});
	return std::move(aPorts);
}

void CIOController::Register(IDevicePtr pDevice)
{
	VASM_CHECK_PTR(pDevice);

	t_size nCount = pDevice->GetSlotCount();
	for (t_index i = 0; i < nCount; ++i)
	{
		SSlotInfo tInfo = pDevice->GetSlotInfo(i);

		t_PortRange pr(tInfo.nBase, (t_port) (tInfo.nBase + t_port(SizeOfValueType(tInfo.eType)) - 1));
		auto it = m_mapPorts.find(pr);
		if (it != m_mapPorts.end())
			VASM_THROW_ERROR(base::toStr(t_csz("Failed to register Device '%1': Port #%2 already occupied"), pDevice->GetName(), tInfo.nBase));
	
		tInfo.nSlot = i;
		tInfo.pDevice = pDevice;

		m_mapPorts.insert({pr, tInfo});
	}
}

bool CIOController::Unregister(IDevicePtr pDevice)
{
	bool bRemoved = false;
	auto it = m_mapPorts.begin();
	while (it != m_mapPorts.end())
	{
		if (it->second.pDevice == pDevice)
		{
			it = m_mapPorts.erase(it);
			bRemoved = true;
		}
		else
			++it;
	}
	return bRemoved;
}

void CIOController::In(t_port nPort, CValue& oValue)
{
	SSlotInfo const& tInfo = Lookup(nPort, oValue.GetType());
	tInfo.pDevice->In(tInfo.nSlot, oValue);
}

void CIOController::Out(t_port nPort, CValue const& oValue)
{
	SSlotInfo const& tInfo = Lookup(nPort, oValue.GetType());
	tInfo.pDevice->Out(tInfo.nSlot, oValue);
}

CIOController::SSlotInfo const& CIOController::Lookup(
	t_port const nPort, EValueType const eType) const
{
	t_PortRange pr(nPort, (t_port) (nPort + t_port(eType) - 1));
	auto it = m_mapPorts.find(pr);
	if (it == m_mapPorts.end())
		VASM_THROW_ERROR(base::toStr(t_csz("IO Controller: Port #%1 not found"), nPort));

	SSlotInfo const& tSlot = it->second;
	if (nPort != tSlot.nBase)
		VASM_THROW_ERROR(base::toStr(t_csz("IO Controller: Port #%1 doesn't matches to the target"), nPort));
	if (SizeOfValueType(eType) > SizeOfValueType(tSlot.eType))
		VASM_THROW_ERROR(base::toStr(t_csz("IO Controller: Port #%1 data type doesn't matches to the target data type"), nPort));

	return tSlot;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//
//	Component
//
#define __COMPONENT__ "Core"

//
//	Includes
//
#include "address_recovery.h"

// STL
#include <iomanip>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CAddressRecovery Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-tors
//
CAddressRecovery::CAddressRecovery(t_AddrssToSymbolMap&& map) :
	m_mapSymbols(map)
{
}

CAddressRecovery::~CAddressRecovery() = default;

//
//	Methods
//
t_string CAddressRecovery::ToSymbol(core::t_address nAddress) const
{
	t_string sSymbol;
	auto it = m_mapSymbols.find(nAddress);
	if (it != m_mapSymbols.end())
		sSymbol = it->second;
	else
	{
		std::stringstream temp;
		temp << std::hex << std::uppercase << std::setw(8) << "0x" << nAddress;
		sSymbol = temp.str();
	}
	return sSymbol;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


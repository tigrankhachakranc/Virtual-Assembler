//
//	Includes
//
#include "vm_memory_manager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CMemoryManager implementation
//
CMemoryManager::CMemoryManager()
	: m_nDataMarker(0)
{
}

CMemoryManager::~CMemoryManager() = default;

void CMemoryManager::Init(t_size nMemSize)
{
	if (nMemSize == 0)
		base::CException("Failed to initialize memory manager: empty memory is requested.");
	else if (nMemSize > CMemory::MaxSize)
		base::CException("Failed to initialize memory manager: requested memory is too big.");

	// Round specified size
	t_size nReminder = nMemSize % CMemory::MinStackSize;
	if (nReminder != 0)
		nMemSize += nReminder;

	m_nDataMarker = 1; // first byte is reserved for null
	m_mapVariables.clear();
	
	m_pRAM = CMemoryPtr(new CMemory(nMemSize));
}

bool CMemoryManager::IsValid() const
{
	return (m_pRAM != nullptr);
}

void CMemoryManager::AddVariable(std::string const& sName, CValue const& oValue)
{
	if (!IsValid())
		base::CException("Failed to declare variable: memory manager is not initialized.");
	else if (sName.empty())
		base::CException("Failed to declare variable: variable name could not be empty.");
	else if (m_mapVariables.find(sName) != m_mapVariables.end())
		base::CException("Failed to declare variable: variable already defined.");

	CMemory const& oRAM = Memory();

		// Round data marker to elemen boundary
	t_size nReminder = m_nDataMarker % oValue.GetElementSize();
	if (nReminder != 0)
		m_nDataMarker += nReminder;

	SVarInfo tVarInfo(oValue, m_nDataMarker);
	m_nDataMarker += tVarInfo.nSize;
	if (m_nDataMarker > oRAM.GetSize())
		base::CException("Failed to declare variable: Size is out of available memory bounds.");

	if (tVarInfo.nSize > 0)
	{
		std::memcpy( (void*) &oRAM[tVarInfo.nOffset], (void*) static_cast<void const*>(oValue), tVarInfo.nSize);
		m_mapVariables.insert(std::make_pair(sName, tVarInfo));
	}
}

t_size CMemoryManager::GetVariableOffset(std::string const& sName) const
{
	t_size nOffset = 0;
	auto it = m_mapVariables.find(sName);
	if (it != m_mapVariables.end())
		nOffset = it->second.nOffset;
	return nOffset;
}

CValue CMemoryManager::GetVariable(std::string const& sName) const
{
	CValue oValue;
	CMemory const& oRAM = Memory();
	auto it = m_mapVariables.find(sName);
	if (it != m_mapVariables.end())
	{
		SVarInfo const& tInfo = it->second;

		if (tInfo.bString)
		{	// special case for string
			oValue = std::move(CValue((char const*) &oRAM[tInfo.nOffset], tInfo.nSize - 1));
		}
		else
		{
			t_size nCount = tInfo.nSize / t_size(tInfo.eType);
			oValue = std::move(CValue(tInfo.eType, nCount, &oRAM[tInfo.nOffset]));
		}
	}
	return oValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

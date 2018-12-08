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

void CMemoryManager::Init(t_aVariables const& aVariables, t_size const nStackSize)
{
	m_nDataMarker = 1; // first byte is reserved for null
	m_mapVariables.clear();

	// Calculate total data size
	t_size nMemSize = aVariables.empty() ? 0 : 1;
	for (auto const& tItem : aVariables)
	{
		// Round to elemen boundary
		t_size nVarSize = tItem.second.GetElementSize();
		t_size nReminder = nMemSize % nVarSize;
		if (nReminder != 0)
			nMemSize += nVarSize - nReminder;
		nMemSize += tItem.second.GetSize();
	}

	// Adjust specified stack size
	if (nStackSize == 0)
		m_nStackSize = t_size(CMemory::DefaultStackSize);
	else if (nStackSize > CMemory::MaxStackSize)
		base::CException("Failed to initialize memory manager: requested stack size is too big.");
	else
	{
		m_nStackSize = nStackSize;
		t_size nReminder = nStackSize % CMemory::MinStackSize;
		if (nReminder != 0)
			m_nStackSize += CMemory::MinStackSize - nReminder;
	}
	nMemSize += m_nStackSize;

	if (nMemSize > CMemory::MaxSize)
		base::CException("Failed to initialize memory manager: requested memory is too big.");

	m_pRAM = CMemoryPtr(new CMemory(nMemSize));

	for (auto const& tItem : aVariables)
		AddVariable(tItem.first, tItem.second);
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
	t_size nVarSize = oValue.GetElementSize();
	t_size nReminder = m_nDataMarker % nVarSize;
	if (nReminder != 0)
		m_nDataMarker += nVarSize - nReminder;

	SVarInfo tVarInfo(oValue, m_nDataMarker);
	m_nDataMarker += tVarInfo.nSize;
	if (m_nDataMarker > oRAM.GetSize() - m_nStackSize)
		base::CException("Failed to declare variable: Offset is out of available memory bounds.");

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

bool CMemoryManager::SetVariable(std::string const& sName, CValue const& oValue)
{
	bool bOk = false;
	CMemory const& oRAM = Memory();
	auto it = m_mapVariables.find(sName);
	if (it != m_mapVariables.end())
	{
		SVarInfo const& tVarInfo = it->second;
		SVarInfo tVarInfo2(oValue, tVarInfo.nOffset);
		if (tVarInfo != tVarInfo2)
			base::CException("Failed to assign variable: type mismatch.");

		if (tVarInfo.nSize > 0)
		{
			std::memcpy((void*)&oRAM[tVarInfo.nOffset], (void*) static_cast<void const*>(oValue), tVarInfo.nSize);
			bOk = true;
		}
	}
	return bOk;
}

CMemoryManager::SVarInfo CMemoryManager::GetVariableInfo(std::string const& sName) const
{
	SVarInfo tInfo;
	auto it = m_mapVariables.find(sName);
	if (it != m_mapVariables.end())
		tInfo = it->second;
	return tInfo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

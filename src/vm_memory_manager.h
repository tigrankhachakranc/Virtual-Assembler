#ifndef VM_MEMORY_MANAGER_H
#define VM_MEMORY_MANAGER_H

//
// Includes
//
#include "vm_memory.h"

// STL
#include <map>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Meory Manager definition
//	Controls memory and holds variables reference maps
//
class CMemoryManager
{
public:
	CMemoryManager();
	~CMemoryManager();

	// Neither copiable, nor movable
	CMemoryManager(CMemoryManager const&) = delete;
	CMemoryManager(CMemoryManager&&) = delete;
	void operator=(CMemoryManager const&) = delete;
	void operator=(CMemoryManager&&) = delete;

	// Initialization interface
	void Init(t_size nSize = CMemory::DefaultSize);
	// Returns true if initialized
	bool IsValid() const;

	// Access to the memory content
	inline CMemory& Memory();
	inline CMemory const& Memory() const;
	inline CMemoryPtr GetMemory() const;

	// Variable support
	void AddVariable(std::string const& sName, CValue const& oValue);
	t_size GetVariableOffset(std::string const& sName) const;
	CValue GetVariable(std::string const& sName) const;

private:
	//
	//	Content
	//
	struct SVarInfo
	{
		EType	eType;
		bool	bString;
		t_size	nSize;
		t_size	nOffset;

		inline SVarInfo();
		inline SVarInfo(CValue const& oVal, t_size offset);
	};

	using t_mapVariables = std::unordered_map<std::string, SVarInfo>;

	t_size			m_nDataMarker;
	CMemoryPtr		m_pRAM;
	t_mapVariables	m_mapVariables;
};

using CMemoryManagerPtr = std::shared_ptr<CMemoryManager>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CMemory& CMemoryManager::Memory()
{
	if (m_pRAM == nullptr)
		throw base::CException("Fatal error: uninitialized memory access.");
	return *m_pRAM;
}

inline CMemory const& CMemoryManager::Memory() const
{
	if (m_pRAM == nullptr)
		throw base::CException("Fatal error: uninitialized memory access.");
	return *m_pRAM;
}

inline CMemoryPtr CMemoryManager::GetMemory() const
{
	return m_pRAM;
}

inline CMemoryManager::SVarInfo::SVarInfo()
	: eType(EType::Byte), bString(false), nSize(0), nOffset(0)
{
}

inline CMemoryManager::SVarInfo::SVarInfo(
	CValue const& oVal, t_size offset)
	: eType(oVal.GetType()), bString(oVal.IsString()),
	  nSize(oVal.GetSize()), nOffset(offset)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VM_MEMORY_MANAGER_H
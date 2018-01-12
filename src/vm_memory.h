#ifndef VM_MEMORY_H
#define VM_MEMORY_H

//
// Includes
//
#include "vm_definitions.h"

// STL
#include <vector>
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Memory definition
//	Represents the concept of poor memory
//
class CMemory
{
public:
	// Contants
	enum
	{
		DefaultSize			= 0x00010000,
		MaxSize				= 0x01000000,
		
		DefaultStackSize	= 0x00001000,
		MinStackSize		= 0x00000100,
		MaxStackSize		= 0x00100000
	};

public:
	inline CMemory(t_size nMemSize = 0);
	inline ~CMemory() = default;

	// Neither copiable, nor movable
	CMemory(CMemory const&) = delete;
	CMemory(CMemory&&) = delete;
	void operator=(CMemory const&) = delete;
	void operator=(CMemory&&) = delete;

	// Return memory size
	inline t_size GetSize() const;

	// Accessing methods
	template <typename Type>
	inline Type& At(t_index);

	template <typename Type>
	inline Type const& At(t_index) const;

	template <typename Type>
	inline Type const& cAt(t_index) const;

	inline uchar& operator[](t_index);
	inline uchar const& operator[](t_index) const;

protected:
	// Initialization interface for derived classes
	inline void Resize(t_size nMemSize, t_byte i8DefaultFill = 0);

private:
	std::vector<uchar>	m_aBuffer;
};

using CMemoryPtr = std::shared_ptr<CMemory>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CMemory::CMemory(t_size nMemSize)
{
	if (nMemSize > 0)
		Resize(nMemSize);
}

inline t_size CMemory::GetSize() const
{
	return static_cast<t_size>(m_aBuffer.size());
}

template <typename Type>
inline Type& CMemory::At(t_index idx)
{
	if (idx == 0) // null is reserved
		throw base::CException("Segmentation fault.");
	else if (idx > m_aBuffer.size() - sizeof(Type))
		throw base::CException("Runtime error: access out of memory bounds.");
	return reinterpret_cast<Type&>(m_aBuffer[idx]);
}

template <typename Type>
inline Type const& CMemory::At(t_index idx) const
{
	if (idx == 0) // null is reserved
		throw base::CException("Segmentation fault.");
	else if (idx > m_aBuffer.size() - sizeof(Type))
		throw base::CException("Runtime error: out of memory bounds read.");
	return reinterpret_cast<Type const&>(m_aBuffer[idx]);
}

template <typename Type>
inline Type const& CMemory::cAt(t_index idx) const
{
	return At<Type>(idx);
}

inline uchar& CMemory::operator[](t_index idx)
{
	return m_aBuffer[idx];
}

inline uchar const& CMemory::operator[](t_index idx) const
{
	return m_aBuffer[idx];
}

inline void CMemory::Resize(t_size nMemSize, t_byte i8DefaultFill)
{
	m_aBuffer.resize(nMemSize, i8DefaultFill);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_MEMORY_H
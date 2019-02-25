#ifndef VASM_CORE_MEMORY_H
#define VASM_CORE_MEMORY_H

//
// Includes
//
#include "value.h"
#include <core_definitions.h>
#include <base_exception.h>

// STL
#include <vector>
#include <memory>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Memory 
//	Represents poor memory concept
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
	inline CMemory(t_uoffset nMemSize = 0, uchar u8DefaultFill = 0);
	inline ~CMemory() = default;

	// Neither copiable, nor movable
	CMemory(CMemory const&) = delete;
	CMemory(CMemory&&) = delete;
	void operator=(CMemory const&) = delete;
	void operator=(CMemory&&) = delete;

	// Return memory size
	inline t_uoffset GetSize() const;
	// Resized memory to the specified size
	inline void Resize(t_uoffset nNewSize, uchar u8DefaultFill = 0);

	// Accessing methods
	template <typename Type>
	inline Type& operator[](t_address);

	template <typename Type = t_byte>
	inline Type const& operator[](t_address) const;

	template <typename Type>
	inline void ReadAt(t_address, Type& val) const;

	template <typename Type>
	inline void WriteAt(t_address, Type const& val);

	template <typename Type>
	inline void Read(t_address, Type* pTrgt, t_uoffset nCount) const;

	template <typename Type>
	inline void Write(t_address, Type const* pSrc, t_uoffset nCount);

	inline CValue ReadValue(t_address, EValueType eType, t_size nCount) const;
	inline void WriteValue(t_address, CValue const&);

	inline void Fill(std::vector<uchar> const& aBuffer, t_address nBaseAddress = 0);

private:
	std::vector<uchar>	m_aBuffer;
};

// Pointer definition
using CMemoryPtr = std::shared_ptr<CMemory>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CMemory::CMemory(t_uoffset nMemSize, uchar u8DefaultFill)
{
	if (nMemSize > 0)
		Resize(nMemSize, u8DefaultFill);
}

inline t_uoffset CMemory::GetSize() const
{
	return static_cast<t_uoffset>(m_aBuffer.size());
}

inline void CMemory::Resize(t_uoffset nMemSize, uchar u8DefaultFill)
{
	m_aBuffer.resize((t_size) nMemSize, u8DefaultFill);
}

template <typename Type>
inline Type& CMemory::operator[](t_address addr)
{
	if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory access.");
	else if (addr > m_aBuffer.size() - sizeof(Type))
		VASM_THROW_ERROR("Out of memory bounds access.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory access.");
	return reinterpret_cast<Type&>(m_aBuffer[addr]);
}

template <typename Type>
inline Type const& CMemory::operator[](t_address addr) const
{
	if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory const access.");
	else if (addr > m_aBuffer.size() - sizeof(Type))
		VASM_THROW_ERROR("Out of memory bounds const access.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory const access.");
	return reinterpret_cast<Type const&>(m_aBuffer[addr]);
}

template <typename Type>
inline void CMemory::ReadAt(t_address addr, Type& val) const
{
	if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory read.");
	else if (addr > m_aBuffer.size() - sizeof(Type))
		VASM_THROW_ERROR("Out of memory bounds read.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory read.");
	val = reinterpret_cast<Type const&>(m_aBuffer[addr]);
}

template <typename Type>
inline void CMemory::WriteAt(t_address addr, Type const& val)
{
	if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory write.");
	else if (addr > m_aBuffer.size() - sizeof(Type))
		VASM_THROW_ERROR("Out of memory bounds write.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory write.");
	reinterpret_cast<Type&>(m_aBuffer[addr]) = val;
}

template <typename Type>
inline void CMemory::Read(t_address addr, Type* pTrgt, t_uoffset nCount) const
{
	size_t nCountBytes = size_t(nCount) * sizeof(Type);
	if (nCount == 0)
		VASM_THROW_ERROR("Zero bytes memory read.");
	else if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory read.");
	else if (addr + nCountBytes > m_aBuffer.size())
		VASM_THROW_ERROR("Out of memory bounds read.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory read.");
	std::memcpy(pTrgt, &m_aBuffer[addr], nCountBytes);
}

template <typename Type>
inline void CMemory::Write(t_address addr, Type const* pTrgt, t_uoffset nCount)
{
	size_t nCountBytes = size_t(nCount) * sizeof(Type);
	if (nCount == 0)
		VASM_THROW_ERROR("Zero bytes memory write.");
	else if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory write.");
	else if (addr + nCountBytes > m_aBuffer.size() )
		VASM_THROW_ERROR("Out of memory bounds write.");
	else if (addr % sizeof(Type) != 0)
		VASM_THROW_ERROR("Unaligned memory write.");
	std::memcpy(&m_aBuffer[addr], pTrgt, nCountBytes);
}

inline CValue CMemory::ReadValue(
	t_address addr, EValueType eType, t_size nCount) const
{
	t_uoffset nElemSize = SizeOfValueType(eType);
	t_size nCountBytes = nElemSize * nCount;

	if (nElemSize == 0)
		VASM_THROW_ERROR("Memory read with invalid value type.");
	else if (nCount == 0)
		VASM_THROW_ERROR("Zero bytes memory read.");
	else if (addr == 0) // null is reserved
		VASM_THROW_ERROR("Nullptr memory read.");
	else if (addr + nCountBytes > m_aBuffer.size())
		VASM_THROW_ERROR("Out of memory bounds read.");
	else if (addr % nElemSize != 0)
		VASM_THROW_ERROR("Unaligned memory read.");
	
	return CValue(eType, nCount, &m_aBuffer[addr]);
}

inline void CMemory::WriteValue(t_address addr, CValue const& oValue)
{
	switch (oValue.GetType())
	{
	case EValueType::Byte:
		Write<uint8>(addr, &static_cast<uint8 const&>(oValue), oValue.GetCount());
		break;
	case EValueType::Word:
		Write<uint16>(addr, &static_cast<uint16 const&>(oValue), oValue.GetCount());
		break;
	case EValueType::DWord:
		Write<uint32>(addr, &static_cast<uint32 const&>(oValue), oValue.GetCount());
		break;
	case EValueType::QWord:
		Write<uint64>(addr, &static_cast<uint64 const&>(oValue), oValue.GetCount());
		break;
	case EValueType::Char:
		Write<t_char>(addr, &static_cast<t_char const&>(oValue), oValue.GetCount());
		break;
	default:
		VASM_THROW_ERROR("Memory write with invalid value type.");
	}
}

inline void CMemory::Fill(
	std::vector<uchar> const& aBuffer, t_address nBaseAddr)
{
	if (nBaseAddr + aBuffer.size() > m_aBuffer.size())
		VASM_THROW_ERROR("Out of memory bounds fill.");
	std::memcpy(&m_aBuffer[nBaseAddr], aBuffer.data(), aBuffer.size());
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CORE_MEMORY_H
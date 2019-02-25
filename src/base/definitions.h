#ifndef VASM_BASE_DEFINITIONS_H
#define VASM_BASE_DEFINITIONS_H

//
//	Includes
//
#include <base_global.h>

// STL
#include <cstdint>
#include <cstddef>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Global type definitions
//
using uint	= unsigned int;
using uchar	= unsigned char;

using uint8	 = std::uint8_t;
using int8	 = std::int8_t;
using uint16 = std::uint16_t;
using int16	 = std::int16_t;
using uint32 = std::uint32_t;
using int32	 = std::int32_t;
using uint64 = std::uint64_t;
using int64	 = std::int64_t;

using float32 = float;
using float64 = double;

using t_index = uint32;
using t_size  = uint32;

using t_csz  = char const*;
using t_char = char;
using t_string = std::basic_string<t_char>;
using t_cstring = t_char const*;

extern VASM_BASE_EXPORT t_index const g_ciInvalid;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespacevm vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VASM_BASE_DEFINITIONS_H

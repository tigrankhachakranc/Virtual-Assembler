#ifndef VASM_CL_INTERMEDIATE_H
#define VASM_CL_INTERMEDIATE_H

//
// Includes
//
#include <core_value.h>
#include <core_definitions.h>
#include <core_command.h>
#include <base_utility.h>

// STL
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Intermediate structors representing content of CODE, DATA and related sections
//
////////////////////////////////////////////////////////////////////////////////

using t_byte		= core::t_byte;
using t_word		= core::t_word;
using t_dword		= core::t_dword;
using t_qword		= core::t_qword;

using t_byte_array	= core::t_byte_array;
using t_word_array	= core::t_word_array;
using t_dword_array	= core::t_dword_array;
using t_qword_array	= core::t_qword_array;

using t_count		= core::t_count;
using t_uoffset		= core::t_uoffset;
using t_soffset		= core::t_soffset;
using EValueType	= core::EValueType;
using CValue		= core::CValue;
using EOpCode		= core::EOpCode;
using ECndtnCode	= core::ECndtnCode;
using EOprSize		= core::EOprSize;
using EOprType		= core::EOprType;
using EOprIdx		= core::EOprIdx;
using EImvType		= core::EImvType;

////////////////////////////////////////////////////////////////////////////////
//
//	Variable
//
struct SVariable
{
	t_string	sName;
	CValue		tValue;
};

using t_aVariables = std::vector<SVariable>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Argument type
//
enum class EArgType
{
	None = 0,	 // No operand
	AR	 = 0x01, // Address register index
	GR	 = 0x02, // General purpose register index
	VAR	 = 0x04, // Variable index in variables table
	FUNC = 0x14, // Function index in functions table
	LBL  = 0x24, // Label index in local symbol table
	NUM  = 0x34, // Intermediate value (byte)
	SNUM = 0x44	 // Intermediate value (word)
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Argument Info
//
struct SArgument
{
	EArgType	eType = EArgType::None;

	union 
	{
		t_index	nIdx;

		int8	i8Val;
		uint8	u8Val;
		int16	i16Val;
		uint16	u16Val;
		int32	i32Val;
		uint32	u32Val;
		int64	i64Val;
		uint64	u64Val = 0;
	};
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Parsed command 
//
struct SCommand
{
	EOpCode		eOpCode;	// Operation code

	ECndtnCode	eCdtnCode;	// Condition code (when applicable)
	EOprSize	eOprSize;	// Operands size (when applicable)
	EImvType	eImvType;	// Intermediate value type (when applicable)

	uint8		nArgCount;	// Argument count
	SArgument	aArguments[EOprIdx::Count];

	t_index		nLineNumber; // Original line number

	inline SCommand();
};
using t_aCommands = std::vector<SCommand>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Label
//
struct SLabel
{
	t_string	sName;
	t_index		nIndex; // Index of the command pointed by the label
};

using t_aLabels = std::vector<SLabel>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Parsed function
//
struct SFunction
{
	t_string	sName;
	t_aLabels	aLabels;
	t_aCommands	aCommands;

	t_index		nBaseLine = 0;	// Funtion base line number within the src unit
	t_count		nSizeLine = 0;	// Function size in line numbers within the src unit

	inline SFunction() = default;
	inline SFunction(SFunction const&) = default;
	inline SFunction(SFunction&&) = default;
	inline SFunction(t_string const& sName);
	inline SFunction(t_string&& sName);
};

using t_aFunctions = std::vector<SFunction>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Parsed Unit 
//
struct SUnit
{
	t_string		sName;
	t_aVariables	aVariables;
	t_aFunctions	aFunctions;

	t_uoffset		nStackSize = 0;
	t_index			nMainFuncIdx = g_ciInvalid;
};

using t_aFunctions = std::vector<SFunction>;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline SCommand::SCommand() :
	eOpCode(EOpCode::Invalid),
	eCdtnCode(ECndtnCode::None),
	eOprSize(EOprSize::Default),
	eImvType(EImvType::None),
	nArgCount(0),
	aArguments{{}, {}, {}},
	nLineNumber(g_ciInvalid)
{
}

inline SFunction::SFunction(t_string const& str) :
	sName(str)
{
}

inline SFunction::SFunction(t_string&& str) :
	sName(str)
{
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VASM_CL_INTERMEDIATE_H

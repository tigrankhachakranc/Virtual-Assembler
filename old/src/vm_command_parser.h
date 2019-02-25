#ifndef VM_COMMAND_PARSER_H
#define VM_COMMAND_PARSER_H

//
//	Includes
//
#include "base_parser.h"
#include "vm_processor.h"
#include "vm_exception.h"

// STL
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CommandParser definition
//
class CCommandParser : public base::CParser
{
	typedef base::CParser Base;

public:
	inline CCommandParser(std::string const& sCommand);

	void Parse(CProcessor::SCommandContext& tCmdCtxt,
			   CProcessor::IDecodeFilter* pDecodeFilter);

private:
	// Command definitions
	enum class EArgType : uint8
	{
		None	= 0,

		AR		= 1, // address register
		GR		= 2, // general purpose register
		NUM		= 4, // numeric value

		// helper flags
		SameR	= 8,  // Same type as previous one
		Optnl	= 10, // Optional flag
		Offset	= 20, // number as +/- offset flag
		Signed	= 40, // positive flag

		// auxiliary type combinations
		AGR		= AR | GR,  // address or generic register
		GRN		= GR | NUM, // generic register or number 
		NOFF	= NUM | Offset, // number or offset
		SNUM	= NUM | Signed, // number or offset

		OGRN	= GRN | Optnl,
	};

	struct SCommandDefinition
	{
		using FnCommand = CProcessor::SCommandContext::FnCommand;
		FnCommand pfnCommand;

		bool hasOpSize;

		EArgType eArg1;
		EArgType eArg2;
		EArgType eArg3;

		using FnCondition = CProcessor::SCommandContext::FnCondition;
		FnCondition pfnCondition;

		inline SCommandDefinition(FnCommand pfnCmd, bool opSize,
								  EArgType arg1 = EArgType::None,
								  EArgType arg2 = EArgType::None,
								  EArgType arg3 = EArgType::None,
								  FnCondition pfnCndtn = nullptr);
	};

	CProcessor::SArgument ParseArgument(
		EArgType eArgType, CProcessor::SArgument::EType ePrevArg,
		CProcessor::IDecodeFilter* pDecodeFilter);

private:
	// Command definitions
	using CCommandDefinitionMap = std::map<std::string, SCommandDefinition>;
	static const CCommandDefinitionMap ms_mapCommands;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
inline CCommandParser::CCommandParser(std::string const& sCommand)
	: Base(sCommand)
{
}

inline CCommandParser::SCommandDefinition::SCommandDefinition(
	FnCommand pfnCmd, bool opSize,
	EArgType arg1, EArgType arg2, 
	EArgType arg3, FnCondition pfnCndtn)
	: pfnCommand(pfnCmd), hasOpSize(opSize), eArg1(arg1),
	  eArg2(arg2), eArg3(arg3), pfnCondition(pfnCndtn)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_COMMAND_PARSER_H

//
//	Includes
//
#include "vm_command_parser.h"

// STL
#include <locale>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	Command definitions
//
////////////////////////////////////////////////////////////////////////////////
const CCommandParser::CCommandDefinitionMap CCommandParser::ms_mapCommands {
	std::make_pair(std::string("NOP"), CCommandParser::SCommandDefinition 
		{&CProcessor::IExecutor::Nop, false}),
	std::make_pair(std::string("BREAK"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Break, false}),
	// Execution control
	std::make_pair(std::string("CALL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Call, false, EArgType::NUM}),
	std::make_pair(std::string("RET"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Ret, false}),
	std::make_pair(std::string("JMP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF}),
	std::make_pair(std::string("JUMP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF}),
	// Conditional branching
	std::make_pair(std::string("JE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsEqual}),
	std::make_pair(std::string("JZ"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsEqual}),
	std::make_pair(std::string("JNE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsNotEqual}),
	std::make_pair(std::string("JNZ"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsNotEqual}),
	std::make_pair(std::string("JA"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsAbove}),
	std::make_pair(std::string("JNBE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsAbove}),
	std::make_pair(std::string("JAE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsAboveOrEqual}),
	std::make_pair(std::string("JNB"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsAboveOrEqual}),
	std::make_pair(std::string("JB"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsBelow}),
	std::make_pair(std::string("JNAE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsBelow}),
	std::make_pair(std::string("JBE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsBelowOrEqual}),
	std::make_pair(std::string("JNA"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsBelowOrEqual}),
	std::make_pair(std::string("JG"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsGreat}),
	std::make_pair(std::string("JNLE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsGreat}),
	std::make_pair(std::string("JGE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsGreatOrEqual}),
	std::make_pair(std::string("JNL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsGreatOrEqual}),
	std::make_pair(std::string("JL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsLow}),
	std::make_pair(std::string("JNGE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsLow}),
	std::make_pair(std::string("JLE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsLowOrEqual}),
	std::make_pair(std::string("JNG"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsLowOrEqual}),
	std::make_pair(std::string("JO"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsOverflow}),
	std::make_pair(std::string("JNO"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsNotOverflow}),
	std::make_pair(std::string("JS"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsSigned}),
	std::make_pair(std::string("JNS"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsNotSigned}),
	std::make_pair(std::string("JP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsParity}),
	std::make_pair(std::string("JNP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Jump, false, EArgType::NOFF, EArgType::None, EArgType::None, &CProcessor::CFlags::IsNotParity}),
	// Assignment
	std::make_pair(std::string("ASSIGN"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Assign, true, EArgType::AGR, EArgType::SNUM}),
	std::make_pair(std::string("MOV"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Move, true, EArgType::AGR, EArgType::AGR}),
	std::make_pair(std::string("MOVE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Move, true, EArgType::AGR, EArgType::AGR}),
	std::make_pair(std::string("SWAP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Swap, true, EArgType::AGR, EArgType::SameR}),
	// Logical
	std::make_pair(std::string("AND"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::And, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("OR"), CCommandParser::SCommandDefinition 
		{&CProcessor::IExecutor::Or, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("XOR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Xor, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("NOT"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Not, true, EArgType::GR}),
	std::make_pair(std::string("NAND"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Nand, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("NOR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Nor, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	// Unsigned integral arithmetic
	std::make_pair(std::string("ADD"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Add, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("SUB"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Sub, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("MUL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Mul, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("DIV"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Div, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("INC"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Inc, true, EArgType::AGR, EArgType::OGRN}),
	std::make_pair(std::string("DEC"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Dec, true, EArgType::AGR, EArgType::OGRN}),
	// Signeg integral arithmetic
	std::make_pair(std::string("ADC"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::AddC, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("SBB"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::SubB, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("IMUL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::IMul, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("IDIV"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::IDiv, true, EArgType::GR, EArgType::GR, EArgType::GR}),
	std::make_pair(std::string("NEG"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Neg, true, EArgType::AGR}),
	// Shift
	std::make_pair(std::string("SHR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::ShiftR, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("SHL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::ShiftL, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("ROR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::RotateR, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("ROL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::RotateL, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("SAR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::ShiftAR, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("SAL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::ShiftAL, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("RCR"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::RotateCR, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("RCL"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::RotateCL, true, EArgType::GR, EArgType::GRN}),
	// Comparison
	std::make_pair(std::string("TEST"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Test, true, EArgType::AGR, EArgType::SameR}),
	std::make_pair(std::string("CMP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Cmp, true, EArgType::AGR, EArgType::SameR}),
	// Stack
	std::make_pair(std::string("PUSH"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Push, true, EArgType::AGR}),
	std::make_pair(std::string("POP"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Pop, true, EArgType::AGR}),
	std::make_pair(std::string("PUSHF"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::PushF, false}),
	std::make_pair(std::string("POPF"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::PopF, false}),
	// Memory
	std::make_pair(std::string("LOAD"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Load, true, EArgType::AGR, EArgType::AR}),
	std::make_pair(std::string("STORE"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Store, true, EArgType::AGR, EArgType::AR}),
	// Input & Output
	std::make_pair(std::string("IN"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::In, true, EArgType::GR, EArgType::GRN}),
	std::make_pair(std::string("OUT"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Out, true, EArgType::GR, EArgType::GRN}),
	// Exit
	std::make_pair(std::string("EXIT"), CCommandParser::SCommandDefinition
		{&CProcessor::IExecutor::Exit, false})
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CCommandParser Implementation
//
////////////////////////////////////////////////////////////////////////////////
void CCommandParser::Parse(
	CProcessor::SCommandContext& tCmdCtxt,
	CProcessor::IDecodeFilter* pDecodeFilter)
{
	// Parse command name
	std::string sToken = std::move(ParseToken());
	// convert to uppercase
	std::locale oLocale;
	for (char& ch : sToken)
		ch = std::toupper(ch, oLocale);

	// Lookup command
	auto itCmd = ms_mapCommands.find(sToken);
	if (itCmd == ms_mapCommands.cend())
		throw base::CException("Unknwon command name.");
	SCommandDefinition const& tCmd = itCmd->second;

	// Keep command
	tCmdCtxt.pfnCommand = tCmd.pfnCommand;
	tCmdCtxt.pfnCondition = tCmd.pfnCondition;

	// Parse operand size extension if applicable
	if (tCmd.hasOpSize)
	{
		sToken = std::move(ParseToken());
		if (sToken.empty())
			throw base::CException("Incomplete command.");

		// keep operand size
		if (sToken.compare("B") == 0 || sToken.compare("b") == 0)
			tCmdCtxt.eOpSize = CProcessor::EOpSize::Byte;
		else if (sToken.compare("W") == 0 || sToken.compare("w") == 0)
			tCmdCtxt.eOpSize = CProcessor::EOpSize::Word;
		else if (sToken.compare("DW") == 0 || sToken.compare("dw") == 0)
			tCmdCtxt.eOpSize = CProcessor::EOpSize::DWord;
		else if (sToken.compare("QW") == 0 || sToken.compare("qw") == 0)
			tCmdCtxt.eOpSize = CProcessor::EOpSize::QWord;
		else
		{	// since operand size is optional determine if it is not an argument and report error
			SkipWhiteSpaces();
			if (!IsFinished() && PeekChar() != ',')
				throw base::CException("Invalid operand size extension specified.");
			// put back parsed token since it assumed to be argument
			RevertPreviousParse();
		}
	}

	// Parse arguments if any
	if (tCmd.eArg1 != EArgType::None)
	{
		// Parse first argument
		tCmdCtxt.tArg1 = ParseArgument(tCmd.eArg1, CProcessor::SArgument::None, pDecodeFilter);

		if (tCmd.eArg2 != EArgType::None)
		{
			// Parse second argument
			tCmdCtxt.tArg2 = ParseArgument(tCmd.eArg2, tCmdCtxt.tArg1.eType, pDecodeFilter);

			if (tCmd.eArg3 != EArgType::None)
				// Parse third argument
				tCmdCtxt.tArg3 = ParseArgument(tCmd.eArg3, tCmdCtxt.tArg2.eType, pDecodeFilter);
		}
	}
	
	if (GetChar(true) != 0)
		throw base::CException("Invalid command: extra characters.");
}

CProcessor::SArgument CCommandParser::ParseArgument(
	EArgType eArgType, CProcessor::SArgument::EType ePrevArg,
	CProcessor::IDecodeFilter* pDecodeFilter)
{
	CProcessor::SArgument tArg;
	if (ePrevArg != CProcessor::SArgument::None)
	{	// sequential arguments should be seperated by comma
		char chComma = GetChar(true);
		bool isOptional = (uint(eArgType) & uint(EArgType::Optnl)) != 0;
		if (chComma == 0 && isOptional)
			return tArg; // OK, optional argument is not specified
		else if (chComma != ',')
			throw base::CException("Invalid command: arguments should be separated by comma.");
	}

	// Read argument
	std::string sToken = std::move(ParseToken());
	if (sToken.empty())
		throw base::CException("Incomplete command: argument is missing.");

	// Allow decode filter to parse first
	if (sToken.compare("ZERO") == 0 || sToken.compare("Zero") == 0)
	{	// Zero could not be hooked
		tArg.eType = CProcessor::SArgument::Zero;
		tArg.nValue = 0;
	}
	else if (pDecodeFilter != nullptr)
		tArg = pDecodeFilter->ParseArgument(sToken);

	// Argument could be either A|R+Number or just number
	char chFirst = sToken.at(0);
	if (tArg.eType != CProcessor::SArgument::None)
	{	// Argument parsed by the filter
		// Ensure its validness
		switch (tArg.eType)
		{
		case CProcessor::SArgument::AR:
			if ((uint(eArgType) & uint(EArgType::AR)) == 0)
				throw base::CException("Invalid argument specified: address register is not expected.");
			break;
		case CProcessor::SArgument::GPR:
			if ((uint(eArgType) & uint(EArgType::GR)) == 0)
				throw base::CException("Invalid argument specified: generic register is not expected.");
			break;
		case CProcessor::SArgument::Number:
			if ((uint(eArgType) & uint(EArgType::NUM)) == 0)
				throw base::CException("Invalid argument specified: number is not expected.");
			break;
		case CProcessor::SArgument::Offset:
			if ((uint(eArgType) & uint(EArgType::Offset)) == 0)
				throw base::CException("Invalid argument specified: offset is not expected.");
			break;
		case CProcessor::SArgument::Zero:
			if ((uint(eArgType) & (uint(EArgType::AR) | uint(EArgType::GR) | uint(EArgType::NUM) | uint(EArgType::SameR))) == 0)
				throw base::CException("Invalid argument specified: ZERO is not expected.");
			break;
		default:
			throw base::CException("Invalid argument specified.");
			break;
		}
	}
	else if ((chFirst == '+' || chFirst == '-') || (chFirst >= '0' && chFirst <= '9'))
	{	// Numeric
		if ((uint(eArgType) & uint(EArgType::NUM)) == 0)
			throw base::CException("Invalid argument specified: numberic is not expected.");
		if ((uint(eArgType) & uint(EArgType::Offset)) == 0 &&
			(uint(eArgType) & uint(EArgType::Signed)) == 0 &&
			(chFirst == '+' || chFirst == '-'))
			throw base::CException("Invalid argument specified: signed number is not expected.");

		if ((uint(eArgType) & uint(EArgType::Offset)) != 0 &&
			(chFirst == '+' || chFirst == '-'))
			tArg.eType = CProcessor::SArgument::Offset;
		else
			tArg.eType = CProcessor::SArgument::Number;

		// Check if it is hexadecimal
		size_t nStrPos = 0;
		if ((chFirst == '+' || chFirst == '-'))
			tArg.nValue = std::stoll(sToken, &nStrPos, 0);
		else
			tArg.uValue = std::stoull(sToken, &nStrPos, 0);
		if (nStrPos != sToken.size())
			throw base::CException("Invalid number specified.");
	}
	else if (chFirst == 'A' || chFirst == 'R')
	{	// Register

		// get index
		sToken.erase(0, 1);
		std::size_t nStrPos = 0;
		try
		{
			tArg.nValue = std::stoi(sToken, &nStrPos, 10);
		}
		catch (std::exception const&)
		{ }

		if (nStrPos != sToken.size())
			throw base::CException("Invalid register index specified.");

		if (eArgType == EArgType::SameR)
		{
			if (ePrevArg == CProcessor::SArgument::AR)
				eArgType = EArgType::AR;
			else if (ePrevArg == CProcessor::SArgument::GPR)
				eArgType = EArgType::GR;
			else
				throw base::CException("Invalid argument specified: first argument should be register.");
		}

		if (chFirst == 'A')
		{
			if (!static_cast<bool>(uint(eArgType) & uint(EArgType::AR)))
				// This should be address register
				throw base::CException("Invalid argument specified: addrerss register is not expected.");
			if (tArg.nValue < 0 || tArg.nValue >= CProcessor::eAddressRegistersPoolSize)
				throw base::CException("Invalid address register index.");

			tArg.eType = CProcessor::SArgument::AR;
		}
		else // (chFirst == 'R')
		{
			if ((uint(eArgType) & uint(EArgType::GR)) == 0)
				// This should be generic register
				throw base::CException("Invalid argument specified: general purpose register is not expected.");
			if (tArg.nValue < 0 || tArg.nValue >= CProcessor::eGeneralPurposeRegisterPoolSize)
				throw base::CException("Invalid general purpose register index.");

			tArg.eType = CProcessor::SArgument::GPR;
		}
	}
	else
		throw base::CException("Unknown argument specified.");

	return tArg;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

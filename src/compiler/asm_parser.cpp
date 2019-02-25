//
// Includes
//
#include "asm_parser.h"
#include "command_parser.h"

#include <base_parser.h>
#include <base_utility.h>
#include <core_memory.h>

// STL
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CAsmParser Implementation
//
////////////////////////////////////////////////////////////////////////////////

//
//	Static memebers intialization
//
const char CAsmParser::s_cchSection = '.';
const char CAsmParser::s_cchLabel	= ':';

const CAsmParser::t_mapSections CAsmParser::ms_mapSections {
	std::make_pair(t_string("code"), CAsmParser::ESection::Code),
	std::make_pair(t_string("Code"), CAsmParser::ESection::Code),
	std::make_pair(t_string("CODE"), CAsmParser::ESection::Code),
	std::make_pair(t_string("data"), CAsmParser::ESection::Data),
	std::make_pair(t_string("Data"), CAsmParser::ESection::Data),
	std::make_pair(t_string("DATA"), CAsmParser::ESection::Data),
	std::make_pair(t_string("stack"), CAsmParser::ESection::Stack),
	std::make_pair(t_string("Stack"), CAsmParser::ESection::Stack),
	std::make_pair(t_string("STACK"), CAsmParser::ESection::Stack),
	std::make_pair(t_string("main"), CAsmParser::ESection::Main),
	std::make_pair(t_string("Main"), CAsmParser::ESection::Main),
	std::make_pair(t_string("MAIN"), CAsmParser::ESection::Main),
};

const CAsmParser::t_mapVariableKeywords CAsmParser::ms_mapVariableKeywords {
	std::make_pair(t_string("byte"), EValueType::Byte),
	std::make_pair(t_string("Byte"), EValueType::Byte),
	std::make_pair(t_string("BYTE"), EValueType::Byte),
	std::make_pair(t_string("word"), EValueType::Word),
	std::make_pair(t_string("Word"), EValueType::Word),
	std::make_pair(t_string("WORD"), EValueType::Word),
	std::make_pair(t_string("dword"), EValueType::DWord),
	std::make_pair(t_string("DWord"), EValueType::DWord),
	std::make_pair(t_string("DWORD"), EValueType::DWord),
	std::make_pair(t_string("qword"), EValueType::QWord),
	std::make_pair(t_string("QWord"), EValueType::QWord),
	std::make_pair(t_string("QWORD"), EValueType::QWord),
	std::make_pair(t_string("char"), EValueType::Char),
	std::make_pair(t_string("Char"), EValueType::Char),
	std::make_pair(t_string("CHAR"), EValueType::Char),
};


//
//	C_Tors
//
CAsmParser::CAsmParser(CInput& oInput, SUnit& tUnit) :
	m_oInput(oInput), m_tUnit(tUnit)
{
}

CAsmParser::~CAsmParser() = default;

//
//	Parsing routines
//

void CAsmParser::Parse()
{
	CInput& oInput = m_oInput;
	SUnit& oUnit = m_tUnit;
	oUnit.sName = oInput.UnitName();
	std::unordered_set<ESection> oSectionTestPlace;
	
	while (oInput.IsInput())
	{
		t_string sLine = std::move(oInput.GetLine());
		
		try
		{
		// Parse section
			base::CParser oParser(sLine);
			t_string sToken = std::move(oParser.ParseToken());
			if (sToken.size() != 1 || sToken.front() != s_cchSection)
				throw CError("Invalid token", oInput, oParser.GetCurrentPos(), sToken);

			ESection eSection = ESection::Invalid;
			sToken = std::move(oParser.ParseToken());
			auto const it = ms_mapSections.find(sToken);
			if (it != ms_mapSections.end())
				eSection = it->second;

			if (oSectionTestPlace.count(eSection) > 0)
				throw CError("Section duplication", oInput, oParser.GetCurrentPos(), sToken);

			switch (eSection)
			{
			case ESection::Code:
			{
				// Parse functins
				oSectionTestPlace.insert(eSection);
				ParseFnctions();
				break;
			}
			case ESection::Data:
			{
				// Parse variables
				if (oSectionTestPlace.count(ESection::Code) > 0)
					throw CError("Data section could not follow code section", oInput, oParser.GetCurrentPos(), sToken);

				oSectionTestPlace.insert(eSection);
				ParseVariables();
				break;
			}
			case ESection::Stack:
			{
				// Parse stack size declaration
				char ch = oParser.GetChar(true);
				if (ch != '=')
					throw CError("Ill formed stack section, it should look like '.Stack = <num_size>'", oInput, oParser.GetCurrentPos(), sToken);

				// Parse stack size
				oUnit.nStackSize = oParser.ParseNumber<t_uoffset, 0>();

				if (oUnit.nStackSize < t_size(core::CMemory::MinStackSize) ||
					oUnit.nStackSize > t_size(core::CMemory::MaxStackSize))
					throw CError("Bad stack size, stack should be in range [256b - 1MB]", oInput, oParser.GetCurrentPos(), sToken);

				// Round specified size
				t_uoffset nReminder = oUnit.nStackSize % t_size(core::CMemory::MinStackSize);
				if (nReminder != 0)
					oUnit.nStackSize += t_uoffset(core::CMemory::MinStackSize) - nReminder;

				oSectionTestPlace.insert(eSection);
				break;
			}
			case ESection::Main:
			{
				// Parse main entry point declaration
				if (oSectionTestPlace.count(ESection::Code) == 0)
					throw CError("Main section should follow code section", oInput, oParser.GetCurrentPos(), sToken);

				char ch = oParser.GetChar(true);
				if (ch != '=')
					throw CError("Ill formed main section, it should look like '.Main = <func_name>'", oInput, oParser.GetCurrentPos(), sToken);

				t_string sMainFunc = std::move(oParser.ParseToken());
				for (t_index i = 0; i < oUnit.aFunctions.size(); ++i)
				{
					if (oUnit.aFunctions.at(i).sName == sMainFunc)
					{
						oUnit.nMainFuncIdx = i;
						break;
					}
				}

				if (oUnit.nMainFuncIdx == g_ciInvalid)
					throw CError("Specified main function not found", oInput, oParser.GetCurrentPos(), sMainFunc);

				oSectionTestPlace.insert(eSection);
				break;
			}
			default:
				throw CError("Invalid section", oInput, oParser.GetCurrentPos(), sToken);
			}
		}
		catch (base::CParser::CError const& err)
		{
			throw CError(err.ErrorMsg(), oInput, err.Position(), err.Token());
		}
	}
}

void CAsmParser::ParseVariables()
{
	CInput& oInput = m_oInput;
	SUnit& oUnit = m_tUnit;

	while (oInput.IsInput())
	{
		// Eliminate comments
		t_string sLine = std::move(oInput.GetLine());

		base::CParser oParser(sLine);
		t_string sToken = std::move(oParser.ParseToken());

		auto const it = ms_mapVariableKeywords.find(sToken);
		if (it == ms_mapVariableKeywords.end())
		{	// Stop variables processing and pass control back
			oInput.PutBack(std::move(sLine));
			break;
		}

		// Variable declaration
		CValue oValue(it->second);

		// Parse variable name
		sToken = std::move(oParser.ParseToken());
		if (sToken.empty())
			throw CError("Expected variale name", oInput, oParser.GetCurrentPos(), sToken);

		t_size nSize = 1;
		t_string sVarName = std::move(sToken);
		char chAfterName = oParser.GetChar(true);

		if (chAfterName == '[')
		{	// Array specifier
			if (oParser.PeekChar() == ']')
				nSize = 0; // Dynamic size
			else
				nSize = oParser.ParseNumber<t_count>(']');
			if (nSize > 0x00010000)
				throw CError("Bad array size", oInput, oParser.GetCurrentPos(), sToken);

			char ch = oParser.GetChar(true);
			if (ch == '=')
			{	// Parse initializer
				switch (oValue.GetType())
				{
				case EValueType::Byte:
				{
					t_byte_array aValues;
					aValues.reserve(nSize);
					do
					{
						t_byte n = oParser.ParseNumberSafe<t_byte, 0>(t_char(','));
						aValues.push_back(n);
						if (nSize > 0 && aValues.size() > nSize)
							throw CError("Initializer elements count exceeds array size", oInput, oParser.GetCurrentPos(), sToken);
					} while (!oParser.IsFinished());
					
					oValue = std::move(CValue(aValues));
					break;
				}
				case EValueType::Word:
				{
					t_word_array aValues;
					aValues.reserve(nSize);
					do
					{
						t_word n = oParser.ParseNumberSafe<t_word, 0>(t_char(','));
						aValues.push_back(n);
						if (nSize > 0 && aValues.size() > nSize)
							throw CError("Initializer elements count exceeds array size", oInput, oParser.GetCurrentPos(), sToken);
					} while (!oParser.IsFinished());

					oValue = std::move(CValue(aValues));
					break;
				}
				case EValueType::DWord:
				{
					t_dword_array aValues;
					aValues.reserve(nSize);
					do
					{
						t_dword n = oParser.ParseNumberSafe<t_dword, 0>(t_char(','));
						aValues.push_back(n);
						if (nSize > 0 && aValues.size() > nSize)
							throw CError("Initializer elements count exceeds array size", oInput, oParser.GetCurrentPos(), sToken);
					} while (!oParser.IsFinished());

					oValue = std::move(CValue(aValues));
					break;
				}
				case EValueType::QWord:
				{
					t_qword_array aValues;
					aValues.reserve(nSize);
					do
					{
						t_qword n = oParser.ParseNumberSafe<t_qword, 0>(t_char(','));
						aValues.push_back(n);
						if (nSize > 0 && aValues.size() > nSize)
							throw CError("Initializer elements count exceeds array size", oInput, oParser.GetCurrentPos(), sToken);
					} while (!oParser.IsFinished());

					oValue = std::move(CValue(aValues));
					break;
				}
				case EValueType::Char:
				{
					t_string sLiteral = std::move(oParser.ParseString());
					if (nSize > 0 && sLiteral.size() >= nSize) // Also take into account '\0'
						throw CError("String literal size exceeds array size", oInput, oParser.GetCurrentPos(), sToken);
					oValue = std::move(CValue(sLiteral));
					break;
				}
				default:
					throw CError("Invalid variable type", oInput, oParser.GetCurrentPos(), sToken);
				}
			} //fi (ch == '=')
			else if (ch != 0)
				throw CError("Unknown token after variable definition", oInput, oParser.GetCurrentPos(), sToken);
			else if (nSize == 0)
				throw CError("Zero sized array is not allowed", oInput, oParser.GetCurrentPos(), sToken);
		} // fi (']')
		else if (chAfterName == '=')
		{	// Single value intializer
			switch (oValue.GetType())
			{
			case EValueType::Byte:
			{
				t_byte n = oParser.ParseNumberSafe<t_byte, 0>();
				oValue = std::move(CValue(n));
				break;
			}
			case EValueType::Word:
			{
				t_word n = oParser.ParseNumberSafe<t_word, 0>();
				oValue = std::move(CValue(n));
				break;
			}
			case EValueType::DWord:
			{
				t_dword n = oParser.ParseNumberSafe<t_dword, 0>();
				oValue = std::move(CValue(n));
				break;
			}
			case EValueType::QWord:
			{
				t_qword n = oParser.ParseNumberSafe<t_qword, 0>();
				oValue = std::move(CValue(n));
				break;
			}
			case EValueType::Char:
			{
				t_char ch = oParser.ParseCharacter();
				oValue = std::move(CValue(ch));
				break;
			}
			default:
				throw CError("Invalid variable type", oInput, oParser.GetCurrentPos(), sToken);
			}
		} // fi '='
		else if (chAfterName == ';')
			; // Variable declaration
		else if (chAfterName == 0)
		{
			switch (oValue.GetType())
			{
			case EValueType::Byte:
			case EValueType::Word:
			case EValueType::DWord:
			case EValueType::QWord:
			case EValueType::Char:
				oValue = std::move(CValue(oValue.GetType(), 1));
				break;
			default:
				throw CError("Invalid variable type", oInput, oParser.GetCurrentPos(), sToken);
			}
		}
		else
			throw CError("Unknown token after variable declaration", oInput, oParser.GetCurrentPos(), sToken);

		auto itVar = m_mapVariableTable.find(CStringRef(&sToken));
		if (itVar == m_mapVariableTable.end())
		{	// Define/Declare variable
			oUnit.aVariables.push_back({std::move(sVarName), std::move(oValue)});
			m_mapVariableTable.insert({&oUnit.aVariables.back().sName, oUnit.aVariables.size() - 1});
		}
		else
		{
			t_index nVarIdx = itVar->second;
			if (oUnit.aVariables.at(nVarIdx).tValue.GetType() != oValue.GetType())
				throw CError("Variable type does not match with its previous declaration", oInput, oParser.GetCurrentPos(), sVarName);
			if (oValue.IsNull())
				; // Do nothing, just repeated variable declaration
			else if (oUnit.aVariables.at(nVarIdx).tValue.IsNull())
				oUnit.aVariables[nVarIdx].tValue = std::move(oValue); // Overwite variable declaration with its definition 
		else
			throw CError("Variable with the specified name already defined", oInput, oParser.GetCurrentPos(), sVarName);
		}
	} //fi (!eof)
}

void CAsmParser::ParseFnctions()
{
	CInput& oInput = m_oInput;
	SUnit& oUnit = m_tUnit;

	while (oInput.IsInput())
	{
		// eliminate comments
		t_string sLine = std::move(oInput.GetLine());

		base::CParser oParser(sLine);
		t_string sToken = std::move(oParser.ParseToken());

		CStringComparator<EComparisonType::Equal_CI> isEqual;
		if (!isEqual(sToken, t_csz("Func")))
		{	// Stop Functions processing and pass control back
			oInput.PutBack(std::move(sLine));
			break;
		}

		// Parse function name
		sToken = std::move(oParser.ParseToken());
		if (sToken.empty())
			throw CError("Expected functon name", oInput, oParser.GetCurrentPos(), sToken);

		// Function declaration/definition
		// Get or Create function 
		t_index nFuncLocation = g_ciInvalid;
		auto const it = m_mapFunctionTable.find(&sToken);
		if (it != m_mapFunctionTable.cend())
			nFuncLocation = it->second;
		else
		{
			nFuncLocation = oUnit.aFunctions.size();
			oUnit.aFunctions.push_back({std::move(sToken)});
			m_mapFunctionTable.insert({&oUnit.aFunctions.back().sName, nFuncLocation});
		}

		SFunction& tFunction = oUnit.aFunctions.at(nFuncLocation);

		sToken = std::move(oParser.ParseToken());
		bool isDefinition = (sToken.size() == 1 && sToken.front() == ':');
		bool isDeclaration = (sToken.size() == 1 && sToken.front() == ';');
		if (!isDefinition && !isDeclaration)
			throw CError("Function name should end with ':' or ';'", oInput, oParser.GetCurrentPos(), sToken);

		sToken = std::move(oParser.ParseToken());
		if (!sToken.empty() && sToken.front() != CInput::s_cchComment)
			throw CError(isDefinition ? "Invalid token follows function definition" : "Invalid token follows function declaration", oInput, oParser.GetCurrentPos(), sToken);

		if (isDefinition)
		{
			if (!tFunction.aCommands.empty())
				throw CError(base::toStr("Function '%1' already defined", tFunction.sName), oInput, oParser.GetCurrentPos(), {});

			tFunction.nBaseLine = oInput.GetLineNumber();
			ParseFunctionBody(tFunction);
			tFunction.nSizeLine = oInput.GetLineNumber() - tFunction.nBaseLine;

			if (tFunction.aCommands.empty())
				throw CError(base::toStr("Empty Function definition '%1'", tFunction.sName), oInput, oParser.GetCurrentPos(), {});
		}
	}
}

void CAsmParser::ParseFunctionBody(SFunction& tFunction)
{
	CInput& oInput = m_oInput;
	t_mapSymbolTable mapLabelTable;

	std::vector< std::pair<t_string, t_index> > aRawCommands;

	// At first go and parse only Labels and collect commands to travere them later
	while (oInput.IsInput())
	{
		// eliminate comments
		t_string sLine = std::move(oInput.GetLine());

		SLabel tLabel;
		t_string sCommand;
		base::CParser oParser(sLine);

		// Parse name, it could be either command name or label
		t_string sToken = std::move(oParser.ParseToken());

		CStringComparator<EComparisonType::Equal_CI> isEqual;
		if (isEqual(sToken, t_csz("EndF")))
		{	// Stop Functions processing 
			break;
		}

		// Analyze next character
		oParser.SkipWhiteSpaces();
		char chNext = oParser.PeekChar();
		if (chNext == s_cchLabel)
		{
			// Keep label
			tLabel.sName = std::move(sToken);
			tLabel.nIndex = (t_index) aRawCommands.size();
			tFunction.aLabels.push_back(std::move(tLabel));
			mapLabelTable.insert({&tFunction.aLabels.back().sName, tFunction.aLabels.size() - 1});

			// Skip label
			oParser.GetChar();
			oParser.SkipWhiteSpaces();
			chNext = oParser.PeekChar();
		}

		if (!oParser.IsFinished() && chNext != CCommandParser::s_cchComment)
		{
			if (oParser.GetCurrentPos() > 0)
				sLine.erase(0, oParser.GetCurrentPos());

			// Kepp command line
			aRawCommands.push_back({std::move(sLine), std::move(oInput.GetLineNumber())});
		}
	}

	// Now go through raw commands and parse them
	for (auto const& tRawCmd : aRawCommands)
	{
		SCommand tCommand;
		CCommandParser oParser(tRawCmd.first, m_mapVariableTable, m_mapFunctionTable, mapLabelTable);

		try
		{
			// Parse command
			oParser.Parse(tCommand);
			// Opcode should be valid because empty lines and comments already skipped
			VASM_CHECK(tCommand.eOpCode != EOpCode::Invalid);
			// Keep parsed command
			tCommand.nLineNumber = tRawCmd.second;
			tFunction.aCommands.push_back(tCommand);
		}
		catch (CCommandParser::CError& e)
		{
			throw CError(e.ErrorMsg(), oInput, e.Position(), e.Token());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

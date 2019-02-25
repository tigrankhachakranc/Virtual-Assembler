//
// Includes
//
#include "vm_preprocessor.h"
#include "base_parser.h"
#include "vm_memory.h"

// STL
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char Preprocessor::s_cchComment = '#';
const char Preprocessor::s_cchLabel = ':';

////////////////////////////////////////////////////////////////////////////////
//
//	Keyword definitions
//
////////////////////////////////////////////////////////////////////////////////
const Preprocessor::CKeywordMap Preprocessor::ms_mapKeywords {
	std::make_pair(std::string("STACK"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::Stack}),
	std::make_pair(std::string("Byte"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::Byte}),
	std::make_pair(std::string("BYTE"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::Byte}),
	std::make_pair(std::string("Word"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::Word}),
	std::make_pair(std::string("WORD"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::Word}),
	std::make_pair(std::string("DWord"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::DWord}),
	std::make_pair(std::string("DWORD"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::DWord}),
	std::make_pair(std::string("QWord"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::QWord}),
	std::make_pair(std::string("QWORD"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::QWord}),
	std::make_pair(std::string("String"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::String}),
	std::make_pair(std::string("STRING"), Preprocessor::SKeywordDefinition {Preprocessor::EKeyword::String}),
};
////////////////////////////////////////////////////////////////////////////////

//
//	Preprocessor implementation
//
CCodePtr Preprocessor::Run(
	std::istream& oInput, 
	t_mapSymbolTable& mapSymbols, 
	t_aVariables& aVariables,
	t_size& nStackSize)
{
	CCodePtr pCode;
	std::string sLine;
	std::vector<std::string> aProgram;
	std::vector<t_index> aSourceMapping;
	int nLineNumber = 0;
	
	// Indicates default
	nStackSize = 0;

	while (!oInput.eof())
	{
		std::getline(oInput, sLine);
		aSourceMapping.push_back((t_index) aProgram.size());
		++nLineNumber;

		// Find comments and eliminate
		base::CParser oParser(sLine);
		oParser.SkipWhiteSpaces();
		if (oParser.IsFinished() || oParser.PeekChar() == s_cchComment)
			continue; // Skip empty lines & comments
		
		std::string sToken = std::move(oParser.ParseToken());
		if (oParser.PeekChar() == s_cchLabel)
		{	// Label found
			oParser.Skip(1);
			// Define symbol
			auto it = mapSymbols.insert(std::make_pair(sToken, aProgram.size()));
			if (!it.second)
				// Label already declared
				throw CException("Code label already declared.", nLineNumber, sToken);

			sToken = std::move(oParser.ParseToken());
		}

		if (sToken.empty())
			continue; // Skip 

		auto it = ms_mapKeywords.find(sToken);
		if (it != ms_mapKeywords.end())
		{	
			EKeyword eKeyword = it->second.eType;
			if (eKeyword == EKeyword::Stack)
			{
				if (nStackSize > 0 || !aProgram.empty())
					throw CException("Stack could be defined once before code.", nLineNumber, sToken);

				nStackSize = oParser.ParseNumber<t_size>();
				if (nStackSize < t_size(CMemory::MinStackSize) ||
					nStackSize > t_size(CMemory::MaxStackSize))
					throw CException("Invalid stack size, stack should be in range [256b - 1MB].", nLineNumber, sToken);

				// Round specified size
				t_size nReminder = nStackSize % t_size(CMemory::MinStackSize);
				if (nReminder != 0)
					nStackSize += nReminder;

				continue;
			}

			// Variable declaration
			CValue oValue;

			// Parse variable name
			sToken = std::move(oParser.ParseToken());
			if (sToken.empty())
				throw CException("Expected variale name.", nLineNumber, sToken);

			t_size nSize = 1;
			char chAfterName = oParser.GetChar(true);
			if (chAfterName == '[')
			{	// Array specifier
				if (eKeyword == EKeyword::String)
					throw CException("Array specifier is not applicable to string.", nLineNumber, sToken);

				nSize = oParser.ParseNumber<t_size>(']');
				if (nSize > 1024)
					throw CException("Invalid array size specified.", nLineNumber, sToken);

				char ch = oParser.GetChar(true);
				if (ch == '=')
				{	// Parse initializer
					switch (eKeyword)
					{
					case EKeyword::Byte:
					{
						t_byte_array aValues(nSize);
						for (t_index i = 0; i < nSize; ++i)
						{
							t_byte n = oParser.ParseCharacter(',');
							aValues[i] = n;
							if (oParser.IsFinished())
								break;
						}

						oValue = std::move(CValue(aValues));
						break;
					}
					case EKeyword::Word:
					{
						t_word_array aValues(nSize);
						for (t_index i = 0; i < nSize; ++i)
						{
							t_word n = ParseWord(oParser, ',');
							aValues[i] = n;
							if (oParser.IsFinished())
								break;
						}

						oValue = std::move(CValue(aValues));
						break;
					}
					case EKeyword::DWord:
					{
						t_dword_array aValues(nSize);
						for (t_index i = 0; i < nSize; ++i)
						{
							t_dword n = ParseDWord(oParser, ',');
							aValues[i] = n;
							if (oParser.IsFinished())
								break;
						}

						oValue = std::move(CValue(aValues));
						break;
					}
					case EKeyword::QWord:
					{
						t_qword_array aValues(nSize);
						for (t_index i = 0; i < nSize; ++i)
						{
							t_qword n = ParseQWord(oParser, ',');
							aValues[i] = n;
							if (oParser.IsFinished())
								break;
						}

						oValue = std::move(CValue(aValues));
						break;
					}
					default:
						throw CException("Invalid variable type.", nLineNumber, sToken);
					}
				} //fi (ch == '=')
				else if (ch != 0)
					throw CException("Invalid symbols after variable declaration.", nLineNumber, sToken);
			} // fi (']')
			else if (chAfterName == '=')
			{	// Single value intializer
				switch (eKeyword)
				{
				case EKeyword::Byte:
				{
					t_byte n = oParser.ParseCharacter();
					oValue = std::move(CValue(n));
					break;
				}
				case EKeyword::Word:
				{
					t_word n = ParseWord(oParser);
					oValue = std::move(CValue(n));
					break;
				}
				case EKeyword::DWord:
				{
					t_dword n = ParseDWord(oParser);
					oValue = std::move(CValue(n));
					break;
				}
				case EKeyword::QWord:
				{
					t_qword n = ParseQWord(oParser);
					oValue = std::move(CValue(n));
					break;
				}
				case EKeyword::String:
				{
					std::string sLiteral = std::move(oParser.ParseString());
					oValue = std::move(CValue(sLiteral));
					break;
				}
				default:
					throw CException("Invalid variable type.", nLineNumber, sToken);
				}
			}
			else if (chAfterName == 0)
			{
				switch (eKeyword)
				{
				case EKeyword::Byte:
					oValue = std::move(CValue(EType::Byte, 1));
					break;
				case EKeyword::Word:
					oValue = std::move(CValue(EType::Word, 1));
					break;
				case EKeyword::DWord:
					oValue = std::move(CValue(EType::DWord, 1));
					break;
				case EKeyword::QWord:
					oValue = std::move(CValue(EType::QWord, 1));
					break;
				case EKeyword::String:
					throw CException("Expecting string initializer.", nLineNumber, sToken);
					break;
				default:
					throw CException("Invalid variable type.", nLineNumber, sToken);
				}

			}
			else
				throw CException("Invalid symbols after variable declaration.", nLineNumber, sToken);

			// Define variable
			aVariables.push_back(std::move(std::make_pair(std::move(sToken), std::move(oValue))));
		}	//fi Keyword
		else
		{	// Rest is the code itself
			oParser.RevertPreviousParse();
			std::string sCodeLine = std::move(oParser.ParseTillEnd(s_cchComment, true));
			if (!sCodeLine.empty())
			{	// Save code line
				aProgram.push_back(std::move(sCodeLine));
			}
		}
	}

	// Create code
	if (!aProgram.empty())
	{
		pCode = CCodePtr(new CCode(std::move(aProgram), std::move(aSourceMapping)));
	}
	return pCode;
}

t_word Preprocessor::ParseWord(base::CParser& oParser, char const chDelimiter)
{
	t_word val = 0;
	oParser.SkipWhiteSpaces();
	if (oParser.PeekChar() == '-')
		val = (t_word) oParser.ParseNumber<int16>(chDelimiter);
	else
		val = oParser.ParseNumber<uint16>(chDelimiter);
	return val;
}

t_dword Preprocessor::ParseDWord(base::CParser& oParser, char const chDelimiter)
{
	t_dword val = 0;
	oParser.SkipWhiteSpaces();
	if (oParser.PeekChar() == '-')
		val = (t_dword) oParser.ParseNumber<int32>(chDelimiter);
	else
		val = oParser.ParseNumber<uint32>(chDelimiter);
	return val;
}

t_qword Preprocessor::ParseQWord(base::CParser& oParser, char const chDelimiter)
{
	t_qword val = 0;
	oParser.SkipWhiteSpaces();
	if (oParser.PeekChar() == '-')
		val = (t_qword)oParser.ParseNumber<int64>(chDelimiter);
	else
		val = oParser.ParseNumber<uint64>(chDelimiter);
	return val;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

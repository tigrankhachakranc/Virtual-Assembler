#ifndef VM_PREPROCESSOR_H
#define VM_PREPROCESSOR_H

//
// Includes
//
#include "vm_definitions.h"
#include "vm_code.h"
#include "vm_exception.h"

// STL
#include <vector>
#include <string>
#include <istream>
#include <map>
#include <unordered_map>

// Forward declarations
namespace base { class CParser; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using t_mapSymbolTable = std::unordered_map<std::string, t_index>;
using t_aVariables = std::vector< std::pair<std::string, CValue> >;

//
//	Preprocessor definition
//	Passes through initial code,
//	handles variable declarations and collects symbols
//
class Preprocessor
{
public:
	static CCodePtr Run(std::istream& oInput,
						t_mapSymbolTable& mapSymbols,
						t_aVariables& aVariables,
						t_size& nStackSize);

private:
	// Keyword definitions
	enum class EKeyword : uint8
	{
		Void = 0,
		// Stack definition
		Stack, 
		// Variable declarations
		Byte,
		Word,
		DWord,
		QWord,
		String,
	};

	struct SKeywordDefinition
	{
		EKeyword eType;

		inline SKeywordDefinition(EKeyword keyType = EKeyword::Void);
	};

	// Helpers
	static t_word  ParseWord(base::CParser&, char const chDelimiter = 0);
	static t_dword ParseDWord(base::CParser&, char const chDelimiter = 0);
	static t_qword ParseQWord(base::CParser&, char const chDelimiter = 0);

private:
	//
	// Static constants
	//
	static const char s_cchComment; // '#'
	static const char s_cchLabel;	// ':'

	// Preprocessor keywords
	using CKeywordMap = std::map<std::string, SKeywordDefinition>;
	static const CKeywordMap ms_mapKeywords;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
Preprocessor::SKeywordDefinition::SKeywordDefinition(EKeyword keyType)
	: eType(keyType)
{
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_PREPROCESSOR_H
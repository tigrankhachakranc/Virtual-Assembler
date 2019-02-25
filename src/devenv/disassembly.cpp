//
//	Includes
//
#include "disassembly.h"
#include <base_exception.h>
#include <base_utility.h>
#include <core_decoder.h>
#include <vm_machine.h>

// STL
#include <iomanip>
#include <sstream>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace dev {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CDisassembler Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-tors
//
CDisassembler::CDisassembler()
{
	m_pCmdLib = vm::CMachine::CreateAndInitCommandLibrary();
}

CDisassembler::~CDisassembler() = default;

//
//	Methods
//
void CDisassembler::Disassemble(std::istream& oInput, std::ostream& oOutput)
{
	if (!oInput.good())
		VASM_THROW_ERROR(t_csz("Disassembler: Bad input stream"));

	// Load input into memory
	m_pMemory = CMemoryPtr(new core::CMemory);
	vm::CLoaderUPtr pLoader(new vm::CLoader);
	pLoader->Load(oInput, m_pMemory, m_tPackage);

	// Initialize symbols tables to recover them form memory addresses
	InitAddressRecovery();

	// First put .stack section
	if (m_tPackage.nStackSize > 0)
		oOutput << ".stack = " << m_tPackage.nStackSize << std::endl;

	// put .data section
	ProcessVariables(oOutput);

	// Put .code section
	ProcessFunctions(oOutput);

	// put .main section
	if (m_tPackage.nProgramStart > 0)
	{
		t_string sMain = m_pAddressRecovery->ToSymbol(m_tPackage.nProgramStart);
		oOutput << ".main = " << sMain << std::endl;
	}
}

//
//	Internal routines
//
void CDisassembler::InitAddressRecovery()
{
	// Also collect symbols to intialize adress recovery
	t_AddrssToSymbolMap mapSymbols;

	// Init variables
	t_size nCount = m_tPackage.aVariableTable.size();
	for (t_index idx = 0; idx < nCount; ++idx)
	{
		vm::SVariableInfo const& tVarInfo = m_tPackage.aVariableTable.at(idx);
		mapSymbols.insert(t_AddrssToSymbolMap::value_type(tVarInfo.nAddress, &tVarInfo.sName));
	}

	// Init functions
	nCount = m_tPackage.aFunctionTable.size();
	for (t_index idx = 0; idx < nCount; ++idx)
	{
		vm::SFunctionInfo const& tFuncInfo = m_tPackage.aFunctionTable.at(idx);
		mapSymbols.insert({tFuncInfo.nAddress, &tFuncInfo.sName});

		for (auto const& tItem : tFuncInfo.aLabels)
			mapSymbols.insert({tItem.nAddress, &tItem.sName});
	}

	// Niti address recover
	m_pAddressRecovery = CAddressRecoveryPtr(new core::CAddressRecovery(std::move(mapSymbols)));
	m_pCmdLib->SetAddressRecovery(m_pAddressRecovery);
}

void CDisassembler::ProcessVariables(std::ostream& oOutput)
{
	if (!m_tPackage.aVariableTable.empty())
	{
		oOutput << std::endl << ".data" << std::endl << std::endl;

		for (auto const& tItem : m_tPackage.aVariableTable)
		{
			switch (tItem.eType)
			{
			case EValueType::Byte:
				oOutput << "BYTE  ";
				break;
			case EValueType::Word:
				oOutput << "WORD  ";
				break;
			case EValueType::DWord:
				oOutput << "DWORD ";
				break;
			case EValueType::QWord:
				oOutput << "QWORD ";
				break;
			case EValueType::Char:
				oOutput << "CHAR  ";
				break;
			default:
				oOutput << std::endl << "# Error:  Unknown variable type happend!" << std::endl;
				VASM_THROW_ERROR(t_csz("Disassembler: Unknown variable type"));
				break;
			}

			// Write variable name
			oOutput << tItem.sName;
			if (tItem.sName.empty())
			{
				oOutput << std::endl << "# Error:  Unnamed variable definition happened!" << std::endl;
				VASM_THROW_ERROR(t_csz("Disassembler: Empty variable name"));
			}

			// Write variable size (if any)
			if (tItem.nSize == 0 || tItem.nSize == g_ciInvalid)
			{
				oOutput << std::endl << "# Error:  Empty variable definition happened!" << std::endl;
				VASM_THROW_ERROR(t_csz("Disassembler: Empty variable definition"));
			}

			if (tItem.nSize > 1)
				oOutput << "[" << tItem.nSize << "]";

			// Read value contentr from the memory
			CValue oValue = m_pMemory->ReadValue(tItem.nAddress, tItem.eType, tItem.nSize);
			if (oValue.IsValid() && !oValue.IsNull() && oValue.GetCount() > 0)
			{
				oOutput << "\t = ";

				switch (tItem.eType)
				{
				case EValueType::Byte:
				{
					oOutput << static_cast<t_byte>(oValue);
					for (t_index i = 1; i < tItem.nSize; ++i)
						oOutput << ", " << oValue.operator[]<t_byte>(i);
					break;
				}
				case EValueType::Word:
				{
					oOutput << static_cast<t_word>(oValue);
					for (t_index i = 1; i < tItem.nSize; ++i)
						oOutput << ", " << oValue.operator[]<t_word>(i);
					break;
				}
				case EValueType::DWord:
				{
					oOutput << static_cast<t_dword>(oValue);
					for (t_index i = 1; i < tItem.nSize; ++i)
						oOutput << ", " << oValue.operator[]<t_dword>(i);
					break;
				}
				case EValueType::QWord:
				{
					oOutput << static_cast<t_qword>(oValue);
					for (t_index i = 1; i < tItem.nSize; ++i)
						oOutput << ", " << oValue.operator[]<t_qword>(i);
					break;
				}
				case EValueType::Char:
				{
					if (oValue.IsString())
						oOutput << '"' << static_cast<t_csz>(oValue) << '"';
					else
						oOutput << static_cast<t_char>(oValue);
					break;
				}
				default:
					break;
				}
			}
			oOutput << std::endl;
		}
		oOutput << std::endl;
	}
}

void CDisassembler::ProcessFunctions(std::ostream& oOutput)
{
	if (!m_tPackage.aFunctionTable.empty())
	{
		oOutput << std::endl << ".code" << std::endl << std::endl;

		// At first put forward declarations for all functions mentioning their source units 
		for (auto const& tItem : m_tPackage.aFunctionTable)
		{
			if (!tItem.sSrcUnit.empty())
				oOutput << "# Unit: " << tItem.sSrcUnit << std::endl;
			oOutput << "Func " << tItem.sName << ';' << std::endl;

			if (tItem.sName.empty())
			{
				oOutput << std::endl << "# Error:  Unnamed function definition happend!" << std::endl;
				VASM_THROW_ERROR(t_csz("Disassembler: Empty function name"));
			}
		}
		oOutput << std::endl;

		// Put function definitions
		for (auto const& tItem : m_tPackage.aFunctionTable)
		{
			oOutput << "Func " << tItem.sName << ':' << std::endl;
			ProcessFunction(oOutput, tItem);
			oOutput << "EndF" << std::endl;
		}
		oOutput << std::endl;
	}
}

void CDisassembler::ProcessFunction(std::ostream& oOutput, vm::SFunctionInfo const& tFunc)
{
	// The decoder
	core::CDecoder oDecoder(m_pCmdLib);

	// Make temp lookup table for labels
	std::unordered_map<t_address, CStringRef> mapLabels;
	for (auto const& tLbl : tFunc.aLabels)
		mapLabels.insert({tLbl.nAddress, &tLbl.sName});

	// Go through code and disassemble commands
	t_address nCurrIP = tFunc.nAddress;
	t_address nEndIP = nCurrIP + tFunc.nSize;

	while (nCurrIP < nEndIP)
	{
		auto itLbl = mapLabels.find(nCurrIP);
		if (itLbl != mapLabels.end())
			oOutput << itLbl->second.str() << ':' << std::endl;

		core::EOpCode& eOpCode = m_pMemory->operator[]<core::EOpCode>(nCurrIP);
		core::CCommandBase::SInstructionInfo const& tCmd = m_pCmdLib->operator[](eOpCode);
		if (tCmd.tMetaInfo.eOpCode == core::EOpCode::Invalid)
		{
			oOutput << std::endl << "# Error:  Unknown instruction code happend!" << std::endl;
			VASM_THROW_ERROR(t_csz("Disassembler: Unknwon instruction"));
		}
		else if (tCmd.pDisasm == nullptr)
		{
			oOutput << std::endl << "# Error:  Can't disassemble instruction with code 0x" << base::toStr((uint) eOpCode) << '!' << std::endl;
			VASM_THROW_ERROR(t_csz("Disassembler: Invalid dissassembly entry"));
		}

		core::SCommandInfo tDecodedCmd;
		oDecoder.Decode(reinterpret_cast<uchar*>(&eOpCode), tDecodedCmd);
		oOutput << '\t' << tCmd.pDisasm->Translate(tDecodedCmd, false) << std::endl;
	}
}

//
//	Static methods
//
void CDisassembler::Disassemble(t_string const& sBinaryFile, t_string const& sOutputFile)
{
	// Open input file
	std::ifstream oBinFile(sBinaryFile, std::ios_base::in);
	if (oBinFile.fail())
		VASM_THROW_ERROR(base::toStr("Failed to open input file '%1'", sBinaryFile));

	// Open output file
	std::ofstream oOutFile(sOutputFile, std::ios_base::out);
	if (oOutFile.fail())
		VASM_THROW_ERROR(base::toStr("Failed to open output file '%1'", sOutputFile));

	try
	{
		CDisassembler oDisasm;
		oDisasm.Disassemble(oBinFile, oOutFile);
	}
	catch (std::exception const&)
	{
		oOutFile << "Processing aborted!" << std::endl;
		throw;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace dev
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


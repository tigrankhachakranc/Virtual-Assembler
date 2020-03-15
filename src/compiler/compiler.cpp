//
//	Component
//
#define __COMPONENT__ "Compiler"

//
// Includes
//
#include "compiler.h"
#include "input.h"
#include "asm_parser.h"
#include "code_generator.h"
#include "encoder.h"
#include "linker.h"
#include "output_generator.h"
#include <vm_machine.h>

// STL
#include <fstream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Error
//
class CError : public base::CError
{
	using Base = base::CError;
public:
	CError(t_csz const pcszErrMsg);
	CError(t_string const& sErrMsg);
	CError(t_string const& sUnitName, CAsmParser::CError const& err);
	CError(t_string const& sUnitName, CEncoder::CError const& err);
	CError(CLinker::CError const& err);
	~CError() = default;

	CError(CError const&) = default;
	CError(CError&&) = default;

	void operator=(CError const&) = delete;
	void operator=(CError&&) = delete;

protected:
	t_string FormatErrorMsg(bool bFinal) const override;

	static t_csz const s_CompilerError;
	static t_csz const s_BuilderError;
	static t_csz const s_LinkerError;
};

t_csz const CError::s_CompilerError = t_csz("Compiler error");
t_csz const CError::s_BuilderError = t_csz("Builder error");
t_csz const CError::s_LinkerError = t_csz("Linker error");

CError::CError(t_csz pcszErrMsg) :
	Base(pcszErrMsg, Reserved, VASM_ERR_INFO(s_CompilerError))
{
	std::stringstream oBuff(std::ios_base::out);
	oBuff << Info().cszFixedErrorMsg << ":";
	oBuff << pcszErrMsg << ".";
	m_sErrorMsg = oBuff.str();
}

CError::CError(t_string const& sErrMsg) :
	Base(t_string(), Reserved, VASM_ERR_INFO(s_CompilerError))
{
	std::stringstream oBuff(std::ios_base::out);
	oBuff << Info().cszFixedErrorMsg << ":";
	oBuff << sErrMsg << ".";
	m_sErrorMsg = oBuff.str();
}

CError::CError(t_string const& sUnitName, CAsmParser::CError const& err) :
	Base(t_string(), Reserved, VASM_ERR_INFO(s_CompilerError))
{
	std::stringstream oBuff(std::ios_base::out);

	oBuff << Info().cszFixedErrorMsg << ":";
	oBuff << err.ErrorMsg() << "." << std::endl;

	if (err.LineNumber() != g_ciInvalid)
		oBuff << "File: '" << sUnitName << "'  Line: " << err.LineNumber() 
		<< "  Pos: " << err.Position() << "  Token '" << err.Token() << "'.";

	m_sErrorMsg = oBuff.str();
}

CError::CError(t_string const& sUnitName, CEncoder::CError const& err) :
	Base(t_string(), Reserved, VASM_ERR_INFO(s_BuilderError))
{
	t_string sError;
	std::stringstream oBuff(std::ios_base::out);

	oBuff << Info().cszFixedErrorMsg << ":";
	oBuff << err.ErrorMsg() << "." << std::endl;

	if (err.LineNumber() != g_ciInvalid)
		oBuff << "File: '" << sUnitName << "'  Line: " << err.LineNumber()
		<< "  Command: '" << err.CommandName() << "'.";

	m_sErrorMsg = oBuff.str();
}

CError::CError(CLinker::CError const& err) :
	Base(t_string(), Reserved, VASM_ERR_INFO(s_LinkerError))
{
	std::stringstream oBuff(std::ios_base::out);

	oBuff << Info().cszFixedErrorMsg << ":";
	oBuff << err.ErrorMsg() << ".";
	oBuff << "  Making package: '" << err.Package() << "'.";

	m_sErrorMsg = oBuff.str();
}

t_string CError::FormatErrorMsg(bool) const
{
	return m_sErrorMsg;
}

//
//	C-Tors
//
CCompiler::CCompiler() = default;
CCompiler::~CCompiler() = default;


//
//	Methods
//
void CCompiler::Compile(t_string sInputUnitName, std::istream& oIStrm)
{
	SUnit tUnit;
	CInput oInput(std::move(sInputUnitName), oIStrm);

	try
	{
		CAsmParser oParser(oInput, tUnit);
		oParser.Parse();
	}
	catch (CAsmParser::CError const& err)
	{
		throw CError(oInput.UnitName(), err);
	}

	SPackage tPackage;
	if (m_pCmdLib == nullptr)
		m_pCmdLib = vm::CMachine::CreateAndInitCommandLibrary();

	try
	{
		CCodeGenerator oCodeGen(m_pCmdLib);
		oCodeGen.Make(tUnit, tPackage);
	}
	catch (CEncoder::CError const& err)
	{
		throw CError(oInput.UnitName(), err);
	}

	m_lstPackages.push_back(std::move(tPackage));
}

void CCompiler::Link(t_string sOutpuPackageName, std::ostream& oOutput)
{
	if (m_lstPackages.empty())
		throw CError(t_csz("Linker error: Nothing to Link"));

	SPackage tFinalPackage;
	try
	{
		CLinker oLinker(m_pCmdLib);
		tFinalPackage = std::move(oLinker.Build(m_lstPackages, sOutpuPackageName));
	}
	catch (CLinker::CError const& err)
	{
		throw CError(err);
	}

	COutputGenerator oOutGen;
	oOutGen.ProduceExecutable(tFinalPackage, oOutput);

	m_lstPackages.clear();
}

void CCompiler::Build(
	std::vector<t_string> const& aSrcFiles, t_string const& sOutputFile)
{
	// Open output file
	std::ofstream oOutFile(sOutputFile, std::ios::out | std::ios::binary | std::ios::trunc);
	if (oOutFile.fail())
		throw CError(base::toStr("Failed to open output file '%1'", sOutputFile));

	Build(aSrcFiles, sOutputFile, oOutFile);
}

void CCompiler::Build(
	std::vector<t_string> const& aSrcFiles, t_string sOutputName, std::ostream& oOutput)
{
	// Lambda function to find out pure name
	auto fnPureName = [](t_string const& sPath) -> t_string
	{
		t_size nStartPos = (t_size) sPath.find_last_of(t_csz("/\\"), t_string::npos, 2);
		if (nStartPos == t_string::npos)
			nStartPos = 0;
		t_size nLength = t_size(sPath.size() - nStartPos);
		if (base::EndsWith(sPath, t_csz(".vasm.bin")))
			nLength -= 9;
		else if (base::EndsWith(sPath, t_csz(".vasm")))
			nLength -= 5;
		else if (base::EndsWith(sPath, t_csz(".bin")))
			nLength -= 4;
		return std::move(sPath.substr(nStartPos, nLength));
	};

	// Find output name
	if (sOutputName.empty())
		sOutputName = t_csz("output");
	else
		sOutputName = std::move(fnPureName(sOutputName));

	// Compile sources
	cl::CCompiler oCompiler;
	for (t_string const& sFilePath : aSrcFiles)
	{
		std::ifstream oSrcFile(sFilePath, std::ios::in);
		if (oSrcFile.fail())
			throw CError(base::toStr("Failed to open source file '%1'", sFilePath));

		// Find out unit name
		std::string sUnitName = std::move(fnPureName(sFilePath));

		// Compile the unit
		oCompiler.Compile(sUnitName, oSrcFile);
	}

	// Compilation passed, now Link
	oCompiler.Link(sOutputName, oOutput);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

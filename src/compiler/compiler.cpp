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
class CError : public base::CException
{
	typedef base::CException Base;
public:
	CError(t_csz psErrMsg);
	CError(t_string const& sUnitName, CAsmParser::CError const& err);
	CError(t_string const& sUnitName, CEncoder::CError const& err);
	CError(CLinker::CError const& err);
	inline ~CError() = default;

	inline CError(CError const&) = default;
	inline CError(CError&&) = default;

	t_string GetErrorMsg(bool = false) const override;
};

CError::CError(t_csz psErrMsg) :
	Base("Compiler error", VASM_SRC_LINE)
{
	std::stringstream oBuff(std::ios_base::out);
	oBuff << "Compiler error: ";
	oBuff << psErrMsg << ".";
	m_sErrorMsg = oBuff.str();
}

CError::CError(t_string const& sUnitName, CAsmParser::CError const& err) :
	Base("Compiler error", VASM_SRC_LINE)
{
	std::stringstream oBuff(std::ios_base::out);

	oBuff << "Compiler error: ";
	oBuff << err.ErrorMsg() << "." << std::endl;

	if (err.LineNumber() != g_ciInvalid)
		oBuff << sUnitName << ":  line #" << err.LineNumber() << "  pos #" << err.Position()
		<< "  token '" << err.Token() << "'." << std::endl;

	m_sErrorMsg = oBuff.str();
}

CError::CError(t_string const& sUnitName, CEncoder::CError const& err) :
	Base("Compiler error", VASM_SRC_LINE)
{
	t_string sError;
	std::stringstream oBuff(std::ios_base::out);

	oBuff << "Builder error: ";
	oBuff << err.ErrorMsg() << "." << std::endl;

	if (err.LineNumber() != g_ciInvalid)
		oBuff << sUnitName << ":  line #" << err.LineNumber()
		<< "  command '" << err.CommandName() << "'." << std::endl;

	m_sErrorMsg = oBuff.str();
}

CError::CError(CLinker::CError const& err) :
	Base("Compiler error", VASM_SRC_LINE)
{
	std::stringstream oBuff(std::ios_base::out);

	oBuff << "Linker error: ";
	oBuff << err.ErrorMsg() << ".";
	oBuff << "  Making" << err.Package() << std::endl;

	m_sErrorMsg = oBuff.str();
}

t_string CError::GetErrorMsg(bool) const
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
		VASM_THROW_ERROR(base::toStr("Failed to open output file '%1'", sOutputFile));

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
		t_size nLength = sPath.size() - nStartPos;
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
			VASM_THROW_ERROR(base::toStr("Failed to open source file '%1'", sFilePath));

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

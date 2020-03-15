
//
//	Includes
//
#include <dev_commander.h>
#include <vm_machine.h>
#include <cl_compiler.h>
#include <dev_disassembly.h>

// STL
#include <string>
#include <iostream>
#include <fstream>

using namespace vasm;
using CError = vasm::base::CError;

CStringComparator<EComparisonType::Equal_CI> compare;

//
//	Main
//
int main(int argc, char* argv[])
{
	std::cout << "VASM Debugger version 2.0" << std::endl;
	std::cout << "Author and developer: Tigran Khachatryan" << std::endl;
	std::cout << "Is licensed under the 'GNU General Public License v3.0'" << std::endl << std::endl;
	std::cout << "Usage: VASM compile <program_source>.vasm [...] <binary_program_name>.vasm.bin" << std::endl;
	std::cout << "Usage: VASM disasm  <binary_program_name>.vasm.bin <output_program_name>.vasm" << std::endl;
	std::cout << "Usage: VASM [<program_source>.vasm [...]] | [<binary_program_name>.vasm.bin]" << std::endl;
	std::cout << "Type ? for the detailed help" << std::endl;

	int nRetCode = 0; char ch;
	try
	{
		t_string sExtension(".vasm");
		t_string sBinExtension(".vasm.bin");

		if (argc > 1)
		{
			t_string sFirstArg(argv[1]);

			// Compilation case, parse input, compile output and exit, no debugging
			if (compare(sFirstArg, t_csz("compile")))
			{
				if (argc < 4)
					throw CError(t_csz("Invalid usage: Incomplete compilation arguments"), CError::BadArguments);

				// Get output file
				t_string sExecutableName(argv[argc-1]);
				if (!base::EndsWith(sExecutableName, sBinExtension))
				{
					if (base::EndsWith(sExecutableName, sExtension))
						throw CError(t_csz("Invalid usage: Output name is missing"), CError::BadArguments);
					sExecutableName += sBinExtension;
				}

				// Get input files
				std::vector<t_string> aSrcNames;
				for (int nArg = 2; nArg < argc - 1; ++nArg)
				{
					t_string sSrcName(argv[nArg]);
					if (!base::EndsWith(sSrcName, sExtension))
						sSrcName += sExtension;
					aSrcNames.push_back(std::move(sSrcName));
				}

				// Compile and generate output
				cl::CCompiler::Build(aSrcNames, sExecutableName);
				
				// Finished!
				std::cout << "Compilattion success!" << std::endl;
				std::cin.get(ch);
				return 0;
			}
			else if (compare(sFirstArg, t_csz("disasm")))
			{
				if (argc != 4)
					throw CError(t_csz("Invalid usage: Incomplete disassembly arguments"), CError::BadArguments);

				// Get input file
				t_string sExecutableName(argv[2]);
				if (!base::EndsWith(sExecutableName, sBinExtension))
				{
					if (base::EndsWith(sExecutableName, sExtension))
						throw CError(t_csz("Invalid usage: Invalid input file name"), CError::BadArguments);
					sExecutableName += sBinExtension;
				}

				// Get output file
				t_string sOutputName(argv[3]);
				if (!base::EndsWith(sOutputName, sExtension))
					sOutputName += sExtension;

				dev::CDisassembler::Disassemble(sExecutableName, sOutputName);

				// Finished!
				std::cout << "Disassembly success!" << std::endl;
				std::cin.get(ch);
				return 0;
			}
			else if (compare(sFirstArg, t_csz("test")))
			{
				if (argc != 4)
					throw CError(t_csz("Invalid usage: Incomplete test arguments"), CError::BadArguments);

				// Get input file
				t_string sTxt(".txt");
				t_string sTestName(argv[2]);
				t_string sOutputName(argv[3]);

				if (!base::EndsWith(sTestName, sTxt))
					sTestName += sTxt;
				if (!base::EndsWith(sOutputName, sTxt))
					sOutputName += sTxt;

				std::ifstream oTestInput(sTestName, std::ios_base::in);
				std::ofstream oTestOutput(sTestName, std::ios_base::out);

				if (oTestInput.fail())
					throw CError(base::toStr("Failed to open the test '%1'", sTestName), CError::FileOpen);
				if (oTestOutput.fail())
					throw CError(base::toStr("Failed to open test output '%1'", sOutputName), CError::FileOpen);

				// Instantiate command handler
				dev::CCommanderUPtr pCommander(new dev::CCommander(oTestInput, oTestOutput));
				pCommander->Init(std::make_shared<vm::CMachine>(oTestInput, oTestOutput));
				pCommander->Start();

				// Finished!
				std::cout << "Test completed!" << std::endl;
				std::cin.get(ch);
				return 0;
			}
		}

		// Instantiate command handler
		dev::CCommanderUPtr pCommander(new dev::CCommander(std::cin, std::cout));
		pCommander->Init(std::make_shared<vm::CMachine>(std::cin, std::cout));

		// Handle command line arguments
		if (argc == 2)
		{
			t_string sFileName(argv[1]);
			if (base::EndsWith(sFileName, sBinExtension))
			{
				pCommander->Load(std::move(sFileName));
			}
			else
			{
				if (!base::EndsWith(sFileName, sExtension))
					sFileName += sExtension;
				pCommander->Open(std::move(sFileName));
			}
		}
		else if (argc > 2)
		{
			std::vector<t_string> aSrcNames;
			for (int nArg = 1; nArg < argc; ++nArg)
			{
				t_string sSrcName(argv[nArg]);
				if (!base::EndsWith(sSrcName, sExtension))
					sSrcName += sExtension;
				aSrcNames.push_back(std::move(sSrcName));
			}
			pCommander->Open(std::move(aSrcNames));
		}

		pCommander->Start();
	}
	catch (base::CException& e)
	{
		nRetCode = -1;
		std::cout << std::endl << e.GetErrorMsg() << std::endl;
		if (e.Component() != nullptr)
			std::cout << base::toStr("Program exited due to %1 error!", e.Component()) << std::endl;
		else
			std::cout << t_csz("Program exited due to error!") << std::endl;
	}
	catch (std::exception& e)
	{
		nRetCode = -2;
		std::cout << std::endl << e.what() << std::endl;
		std::cout << t_csz("Program exited due to std error!") << std::endl;
	}
	catch (...)
	{
		nRetCode = -3;
		std::cout << std::endl << t_csz("Program exited due to unknown error!") << std::endl;
	}

	std::cin.get(ch);
	return nRetCode;
}

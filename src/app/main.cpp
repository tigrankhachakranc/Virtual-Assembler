
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

	int nRetCode = 0;
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
					VASM_THROW_ERROR(t_csz("Invalid usage: Incomplete compilation arguments"));

				// Get output file
				t_string sExecutableName(argv[argc-1]);
				if (!base::EndsWith(sExecutableName, sBinExtension))
				{
					if (base::EndsWith(sExecutableName, sExtension))
						VASM_THROW_ERROR(t_csz("Invalid usage: Output name is missing"));
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
				return 0;
			}
			else if (compare(sFirstArg, t_csz("disasm")))
			{
				if (argc != 4)
					VASM_THROW_ERROR(t_csz("Invalid usage: Incomplete disassembly arguments"));

				// Get input file
				t_string sExecutableName(argv[2]);
				if (!base::EndsWith(sExecutableName, sBinExtension))
				{
					if (base::EndsWith(sExecutableName, sExtension))
						VASM_THROW_ERROR(t_csz("Invalid usage: Invalid input file name"));
					sExecutableName += sBinExtension;
				}

				// Get output file
				t_string sOutputName(argv[3]);
				if (!base::EndsWith(sOutputName, sExtension))
					sOutputName += sExtension;

				dev::CDisassembler::Disassemble(sExecutableName, sOutputName);
			}
		}

		// Instantiate command handler
		dev::CCommanderUPtr pCommander(new dev::CCommander(std::cin, std::cout));
		pCommander->Init(vm::CMachinePtr(new vm::CMachine(std::cin, std::cout)));

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
		std::cout << std::endl << t_csz("Program exited due to error!") << std::endl;
		std::cout << std::endl << e.GetErrorMsg(true) << std::endl;
	}
	catch (std::exception& e)
	{
		nRetCode = -2;
		std::cout << std::endl << t_csz("Program exited due to std error!") << std::endl;
		std::cout << std::endl << e.what() << std::endl;
	}
	catch (...)
	{
		nRetCode = -3;
		std::cout << std::endl << t_csz("Program exited due to unknown error!") << std::endl;
	}

	std::cin.get();
	return nRetCode;
}

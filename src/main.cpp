
//
//	Includes
//
#include "base_notification.h"
#include "vm_interpreter.h"
#include "vm_debugger.h"

// STL
#include <string>
#include <vector>
#include <istream>
#include <iostream>
#include <fstream>
#include <algorithm>

//
// Case Insensitive Implementation of endsWith()
// It checks if the string 'mainStr' ends with given string 'toMatch'
//
bool EndsWith(std::string const& sSource, std::string const& sTarget)
{
	bool bResult = false;
	if (sSource.size() >= sTarget.size())
	{
		auto itTarget = sTarget.begin();
		auto itSource = std::next(sSource.begin(), sSource.size() - sTarget.size());
		bResult = std::all_of(itSource, sSource.end(), [&itTarget](char const& c)
		{
			return ::tolower(c) == ::tolower(*(itTarget++));
		});
	}
	return bResult;
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 2)
		{
			std::ifstream oInput;
			std::string sExecutableName(argv[1]);
			std::string sExtension(".vasm");
			if (!EndsWith(sExecutableName, sExtension))
				sExecutableName += sExtension;

			oInput.open(sExecutableName);
			if (!oInput.is_open())
				throw std::exception("Failed to open input file.");

			vm::CInterpreterPtr pMachine = vm::CInterpreterPtr(new vm::CInterpreter);
			vm::IDebuggerPtr pDebugger = vm::CDebuggerPtr(new vm::CDebugger(pMachine));
			
			pMachine->Init(oInput);
			pMachine->SetDebugger(pDebugger);
			pMachine->Run(false);

			std::cout << std::endl << "Succeed" << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}

	//CSender1 oSender;
	//CListener1 oListener1;
	//CListener2 oListener2;

	//oSender.evnSomethingChanged.Add<CListener1>(oListener1, &CListener1::onSomethingChanged);
	//oSender.evnSomethingChanged.Add<CListener2>(oListener2, &CListener2::onSomethingChangedA);
	//oSender.evnSomethingChanged.Add<CListener2>(oListener2, &CListener2::onSomethingChangedB);

	//oSender.DoSomething();

	std::cin.get();
	return 0;
}

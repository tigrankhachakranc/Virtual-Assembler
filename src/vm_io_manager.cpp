//
//	Includes
//
#include "vm_io_manager.h"

// STL
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	CPorts Implementation
//

CIOManager::CPorts::CPorts(CIOManager* pMaster)
	: m_pMaster(pMaster), m_nLastOutPos(0)
{
}

CIOManager::CPorts::~CPorts()
{
	m_pMaster = nullptr;
}

void CIOManager::CPorts::In(t_index nPort, CValue& oValue)
{
	if (nPort == 0)
	{
		char ch;
		std::cout << "(VM) >> ";
		std::cin.get(ch);
		oValue = CValue((t_byte) ch);
	}
	else if (nPort == 1)
	{
		std::cout << "(VM) >> ";
		switch (oValue.GetType())
		{
		case EType::Byte:
		{
			t_byte val;
			std::cin >> val;
			break;
		}
		case EType::Word:
		{
			t_word val;
			std::cin >> val;
			break;
		}
		case EType::DWord:
		{
			t_dword val;
			std::cin >> val;
			break;
		}
		case EType::QWord:
		{
			t_qword val;
			std::cin >> val;
			break;
		}
		}
	}
}

void CIOManager::CPorts::Out(t_index nPort, CValue const& oValue)
{
	if (nPort == 0)
	{
		if (m_nLastOutPos != std::cout.tellp())
			std::cout << "(VM) << ";
		std::cout.put((char) static_cast<t_byte>(oValue));
		m_nLastOutPos = (t_size) std::cout.tellp();
	}
	else if (nPort == 1)
	{
		if (m_nLastOutPos != std::cout.tellp())
			std::cout << "(VM) << ";
		switch (oValue.GetType())
		{
		case EType::Byte:
			std::cout << static_cast<t_byte>(oValue) << ' ';
			break;
		case EType::Word:
			std::cout << static_cast<t_word>(oValue) << ' ';
			break;
		case EType::DWord:
			std::cout << static_cast<t_dword>(oValue) << ' ';
			break;
		case EType::QWord:
			std::cout << static_cast<t_qword>(oValue) << ' ';
			break;
		}
	}
}


//
//	CIOManager Implementation
//
CIOManager::CIOManager()
{
	m_pPorts = CPortsPtr(new CPorts(this));
}

CIOManager::~CIOManager()
{
	m_pPorts->Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VM_IO_MANAGER_H
#define VM_IO_MANAGER_H

//
// Includes
//
#include "vm_processor.h"

// STL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	IO Manager definition
//	Controls IO operations
//
class CIOManager
{
public:
	CIOManager();
	~CIOManager();

	// Neither copiable, nor movable
	CIOManager(CIOManager const&) = delete;
	CIOManager(CIOManager&&) = delete;
	void operator=(CIOManager const&) = delete;
	void operator=(CIOManager&&) = delete;

public:
	//
	//	Methods
	//
	inline IPortsPtr GetPorts();

protected:
	//
	//	CPorts definition
	//
	class CPorts : public IPorts
	{
	public:
		CPorts(CIOManager* pMaster);
		~CPorts() override;

		void In(t_index nPort, CValue& oValue) override;
		void Out(t_index nPort, CValue const& oValue) override;

		inline void Reset();

	private:
		t_size m_nLastOutPos;
		CIOManager* m_pMaster;
	};
	using CPortsPtr = std::shared_ptr<CPorts>;

private:
	//
	//	Content
	//
	CPortsPtr	m_pPorts;
};

using CIOManagerPtr = std::shared_ptr<CIOManager>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline IPortsPtr CIOManager::GetPorts()
{
	return m_pPorts;
}

inline void CIOManager::CPorts::Reset()
{
	m_pMaster = nullptr;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_IO_MANAGER_H
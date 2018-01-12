#ifndef VM_CODE_H
#define VM_CODE_H

//
// Includes
//
#include "vm_definitions.h"

// STL
#include <vector>
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vm { // vm stands for Virtual Machine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Code definition
//	Holds commands for execution
//
class CCode final
{
public:
	inline CCode(std::vector<std::string>&& aProgram);
	~CCode() = default;

	// Neither copiable, nor movable
	CCode(CCode const&) = delete;
	CCode(CCode&&) = delete;
	void operator=(CCode const&) = delete;
	void operator=(CCode&&) = delete;

	// Accessing methods
	inline t_size GetSize() const;
	inline std::string const& GetAt(t_index) const;
	inline std::string& ChangeAt(t_index);

private:
	std::vector<std::string>	m_aProgram;
};

using CCodePtr = std::shared_ptr<CCode>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CCode::CCode(std::vector<std::string>&& aProgram)
	: m_aProgram(std::move(aProgram))
{
}

inline t_size CCode::GetSize() const
{
	return m_aProgram.size();
}

inline std::string const& CCode::GetAt(t_index n) const
{
	return m_aProgram.at(n);
}

inline std::string& CCode::ChangeAt(t_index n)
{
	return m_aProgram[n];
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_CODE_H
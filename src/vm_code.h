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
#include <climits>

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
	inline CCode(std::vector<std::string>&& aProgram,
				 std::vector<t_index>&& aSourceToCodeMapping);
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

	inline t_index GetCodeLine(t_index nSourceLine) const;

private:
	// Program clean code
	std::vector<std::string>	m_aProgram;
	// Source line to code line mapping
	std::vector<t_index>		m_aMapping;
};

using CCodePtr = std::shared_ptr<CCode>;

////////////////////////////////////////////////////////////////////////////////
//
//	Inline Implementations
//
////////////////////////////////////////////////////////////////////////////////
inline CCode::CCode(
	std::vector<std::string>&& aProgram,
	std::vector<t_index>&& aSourceToCodeMapping)
	: m_aProgram(std::move(aProgram)), m_aMapping(std::move(aSourceToCodeMapping))
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

inline t_index CCode::GetCodeLine(t_index nSourceLine) const
{
	t_index nCodeLine = std::numeric_limits<t_index>::max();
	if (nSourceLine > 0 && nSourceLine <= m_aMapping.size())
		nCodeLine = m_aMapping.at(nSourceLine - 1);
	return nCodeLine;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //VM_CODE_H
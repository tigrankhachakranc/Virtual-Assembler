//
//	Component
//
#define __COMPONENT__ "Compiler"

//
// Includes
//
#include "code_generator.h"
#include "encoder.h"
#include <base_utility.h>

// STL
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	C-Tors
//
CCodeGenerator::CCodeGenerator(CCommandLibraryPtr pCmdLib) :
	m_pCmdLibrary(pCmdLib)
{
	VASM_CHECK_PTR(m_pCmdLibrary);
}

CCodeGenerator::~CCodeGenerator() = default;


//
//	Methods
//

void CCodeGenerator::Make(SUnit const& tUnit, SPackage& tPackage)
{
	// Reset
	tPackage.tInfo = SProgramInfo();
	tPackage.aCode.clear();
	tPackage.aData.clear();
	tPackage.oSymbolTbl.aEntries.clear();
	tPackage.oDebugInfo.aEntries.clear();
	tPackage.oRelocTbl.aAddressLocations.clear();

	// Unit name
	tPackage.tInfo.sName = tUnit.sName;

	// Save stack size
	tPackage.tInfo.nStackSize = tUnit.nStackSize;

	// Make Data
	MakeData(tUnit, tPackage);

	// Make Code
	MakeCode(tUnit, tPackage, true);
}

void CCodeGenerator::MakeData(SUnit const& tUnit, SPackage& tPackage)
{
	t_uoffset nDataMarker = 0; // Tracks offset within the Data section
	m_aVarToSymblIdxMapping.reserve(tUnit.aVariables.size());
	for (SVariable const& tVar : tUnit.aVariables)
	{
		VASM_CHECK(m_mapSymbolTable.count(&tVar.sName) == 0);
		
		// Define symbol, make entry for the variable in the package level symbol table
		tPackage.oSymbolTbl.aEntries.push_back({});
		SSymbolTable::SEntry& tEntry = tPackage.oSymbolTbl.aEntries.back();
		tEntry.sName = tVar.sName;
		tEntry.isFunc = false;
		tEntry.eType = tVar.tValue.GetType();
		tEntry.nSize = 0;
		tEntry.nBase = core::cnInvalidAddress; 
		
		// Keep symbol index
		m_mapSymbolTable.insert({&tEntry.sName, tPackage.oSymbolTbl.aEntries.size() - 1});
		m_aVarToSymblIdxMapping.push_back(tPackage.oSymbolTbl.aEntries.size() - 1);
		
		if (tVar.tValue.IsNull())
		{	// Variable declared without definition (forward declaration)
			continue;
		}

		// Align Data marker to Element boundaries
		t_size nReminder = nDataMarker % tVar.tValue.GetElementSize();
		if (nReminder != 0)
			nDataMarker += tVar.tValue.GetElementSize() - nReminder;

		// Write into Data 
		t_size nVarSize = tVar.tValue.GetSize();
		tPackage.aData.resize(nDataMarker + nVarSize);
		std::memcpy(&tPackage.aData[nDataMarker], tVar.tValue, nVarSize);

		// Save variable base & size
		tEntry.nSize = tVar.tValue.GetCount();
		tEntry.nBase = nDataMarker; // Offset in the Data section
		
		// Increase Data marker
		nDataMarker += nVarSize;
	}
}

void CCodeGenerator::MakeCode(SUnit const& tUnit, SPackage& tPackage, bool bIncludeDebugInfo)
{
	t_uoffset nCodeMarker = 0; // Tracks offset within the Code section
	m_aFuncToSymblIdxMapping.resize(tUnit.aFunctions.size());
	for (t_index nFunc = 0; nFunc < tUnit.aFunctions.size(); ++nFunc)
	{
		SFunction const& tFunc = tUnit.aFunctions.at(nFunc);
		VASM_CHECK(m_mapSymbolTable.count(&tFunc.sName) == 0);

		// Define symbol,
		// make entry for the variable in the package level symbol table
		t_index nSymbolIdx = tPackage.oSymbolTbl.aEntries.size();
		tPackage.oSymbolTbl.aEntries.push_back({});
		SSymbolTable::SEntry& tEntry = tPackage.oSymbolTbl.aEntries.back();
		tEntry.sName = tFunc.sName;
		tEntry.isFunc = true;
		tEntry.eType = EValueType::Unknown;
		tEntry.nSize = 0;
		tEntry.nBase = core::cnInvalidAddress;
		// Keep symbol index
		m_mapSymbolTable.insert({&tEntry.sName, nSymbolIdx});
		m_aFuncToSymblIdxMapping[nFunc] = nSymbolIdx;

		// Entry point (main)
		if (nFunc == tUnit.nMainFuncIdx)
			tPackage.tInfo.nEntryPoint = nSymbolIdx;

		if (tFunc.nSizeLine == 0)
		{	// Function declared without definition (forward declaration)
			continue;
		}

		// Function definition is available, save function base
		tEntry.nBase = nCodeMarker; // Offset in the Code section

		SDebugInfo::SEntry* pDbgEntry = nullptr;
		if (bIncludeDebugInfo)
		{
			tPackage.oDebugInfo.aEntries.push_back({});
			pDbgEntry = &tPackage.oDebugInfo.aEntries.back();

			pDbgEntry->nSymbolIndex = tPackage.oSymbolTbl.aEntries.size() - 1;
			pDbgEntry->sSrcUnitName = tUnit.sName;
			pDbgEntry->nBaseLine = tFunc.nBaseLine;
			pDbgEntry->nSizeLine = tFunc.nSizeLine;
		}

		// Generate function body
		MakeFunc(tFunc, tPackage, nCodeMarker, pDbgEntry);

		// Keep function size
		tEntry.nSize = nCodeMarker - tEntry.nBase;
	}

	// Resize code it its exact final size
	tPackage.aCode.resize(nCodeMarker);
}

void CCodeGenerator::MakeFunc(
	SFunction const& tFunc, SPackage& tPackage,
	t_uoffset& nCodeMarker, SDebugInfo::SEntry* pDbgInfo)
{
	// Function local relocation table for Labels
	std::vector<t_uoffset>	aLabelRelocationTbl;

	// Instantiate the Encoder
	CEncoder oEncoder(tPackage.aCode,
					  tPackage.oRelocTbl.aAddressLocations,
					  aLabelRelocationTbl,
					  m_aVarToSymblIdxMapping,
					  m_aFuncToSymblIdxMapping,
					  m_pCmdLibrary);
	
	// Keep Functions starting offset
	t_uoffset nCodeMarkerBase = nCodeMarker;

	// Preserve space for the code
	tPackage.aCode.resize(nCodeMarker + tFunc.aCommands.size() * core::cnCmdAvrgLength + core::cnCmdMaxLength);

	// Also collect label code locations to adjust local labels later in this function
	std::vector<t_uoffset> aLblLocations(tFunc.aLabels.size());
	t_index nCurrLbl = 0;

	t_size nDbgCodeTblIdx = 0;
	if (pDbgInfo != nullptr)
		pDbgInfo->aCodeTbl.resize(tFunc.nSizeLine);

	for (t_index nCmd = 0; nCmd < tFunc.aCommands.size(); ++nCmd)
	{
		SCommand const& tCmd = tFunc.aCommands.at(nCmd);

		// Is the current command labeld?
		if (nCurrLbl < tFunc.aLabels.size() && tFunc.aLabels.at(nCurrLbl).nIndex == nCmd)
			// Keep function relative label offset
			aLblLocations[nCurrLbl++] = (nCodeMarker - nCodeMarkerBase);

		// Check code buffer available space
		if (tPackage.aCode.size() < nCodeMarker + core::cnCmdMaxLength)
			tPackage.aCode.resize(nCodeMarker + (tFunc.aCommands.size() - nCmd) * core::cnCmdAvrgLength + core::cnCmdMaxLength);

		// Collect function relative line number to Function relative command offset mapping
		if (pDbgInfo != nullptr)
		{
			t_size nRelativeLineIdx = tCmd.nLineNumber - tFunc.nBaseLine;
			while (nDbgCodeTblIdx <= nRelativeLineIdx)
				pDbgInfo->aCodeTbl[nDbgCodeTblIdx++] = (nCodeMarker - nCodeMarkerBase);
		}

		nCodeMarker += oEncoder.Encode(tCmd, nCodeMarker);
	}

	// Once function body is encoded, it is the right time to adjust function labels with relative offsets
	for (t_uoffset nCodeOffset : aLabelRelocationTbl)
	{
		t_index nLblIndx = reinterpret_cast<uint16&>(tPackage.aCode[nCodeOffset]);
		VASM_CHECK_IDX(nLblIndx, aLblLocations.size());
		t_uoffset nLblOffset = aLblLocations[nLblIndx];
		t_soffset nRelativeOffset = nLblOffset - (nCodeOffset - nCodeMarkerBase + sizeof(uint16)); // Asjust offset to command boundary
		if (nRelativeOffset < INT16_MIN || nRelativeOffset > INT16_MAX)
			throw CEncoder::CError(base::toStr("Encoder: Label relative offset exceeds allowed range of Int-16 '%1'", 
									base::toStr("%1.%2", tFunc.sName, tFunc.aLabels[nLblIndx].sName)),
									tFunc.aCommands[tFunc.aLabels[nLblIndx].nIndex].nLineNumber, nullptr);
		reinterpret_cast<int16&>(tPackage.aCode[nCodeOffset]) = static_cast<int16>(nRelativeOffset);
	}

	// Also fill in Debug info for labels
	if (pDbgInfo != nullptr)
	{
		for (t_index nLbl = 0; nLbl < tFunc.aLabels.size(); ++nLbl)
		{
			pDbgInfo->aLabelEntries.push_back({});
			SDebugInfo::SLabelEntry& tLblEntry = pDbgInfo->aLabelEntries.back();
			tLblEntry.nOffset = aLblLocations[nLbl];
			tLblEntry.sName = tFunc.aLabels[nLbl].sName;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Includes
//
#include "linker.h"
#include "encoder.h"
#include "intermediate.h"

// STL
#include <algorithm>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace vasm {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cl {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//
//	C-Tors
//
CLinker::CLinker(CCommandLibraryPtr pCmdLib) :
	m_pCmdLib(pCmdLib)
{
}

CLinker::~CLinker() = default;


//
//	Methods
//
SPackage CLinker::Build(t_lstPackages const& lstPackages, t_string sProgramName)
{
	SPackage tFinalPkg;
	tFinalPkg.tInfo.sName = std::move(sProgramName);

	// Write program header (which will call main and exits)
	WriteHeader(tFinalPkg);
	// Merge packages into single package
	Merge(tFinalPkg, lstPackages);

	// Fill program entry point
	if (tFinalPkg.tInfo.nEntryPoint == g_ciInvalid)
		throw CError(t_csz("Linker: Program entry point not defined"), tFinalPkg.tInfo.sName);
	t_uoffset nMainJumpOffset = tFinalPkg.oRelocTbl.aCodeAddressLocations.front();
	std::memcpy(&tFinalPkg.aCode[nMainJumpOffset], &nMainJumpOffset, sizeof(t_address));

	// Resolve symbol indexes to final offsets
	ResolveOffsets(tFinalPkg);

	return std::move(tFinalPkg);
}

void CLinker::WriteHeader(SPackage& tFinalPkg)
{
	tFinalPkg.aCode.resize(s_cnGranul, 0);

	t_aRelocTable aDummy;
	CEncoder oEncoder(tFinalPkg.aCode,
					  tFinalPkg.oRelocTbl.aDataAddressLocations,
					  tFinalPkg.oRelocTbl.aCodeAddressLocations,
					  aDummy, m_pCmdLib);

	SCommand tCmd;
	int nCodeOffset = 0;

	tCmd.eOpCode = EOpCode::ASSIGNA4;
	tCmd.eImvType = EImvType::Num32;
	tCmd.nArgCount = 2;
	tCmd.aArguments[EOprIdx::First].eType = EArgType::AR;
	tCmd.aArguments[EOprIdx::First].nIdx = core::SCPUStateBase::eARBaseIndex;
	tCmd.aArguments[EOprIdx::Second].eType = EArgType::FUNC;
	tCmd.aArguments[EOprIdx::Second].nIdx = g_ciInvalid;
	nCodeOffset += oEncoder.Encode(tCmd, nCodeOffset);

	tCmd.eOpCode = EOpCode::CALL;
	tCmd.eImvType = EImvType::None;
	tCmd.nArgCount = 1;
	tCmd.aArguments[EOprIdx::First].eType = EArgType::AR;
	tCmd.aArguments[EOprIdx::First].nIdx = core::SCPUStateBase::eARBaseIndex;
	nCodeOffset += oEncoder.Encode(tCmd, nCodeOffset);

	tCmd.eOpCode = EOpCode::EXIT;
	tCmd.nArgCount = 0;
	nCodeOffset += oEncoder.Encode(tCmd, nCodeOffset);
}

void CLinker::Merge(SPackage& tFinalPkg, t_lstPackages const& lstPackages)
{
	for (SPackage const& tPkg : lstPackages)
	{
		// Take final stack size the maximum of all sizes
		tFinalPkg.tInfo.nStackSize = std::max(tFinalPkg.tInfo.nStackSize, tPkg.tInfo.nStackSize);

		// Merge Data sections
		t_uoffset nLastDataSize = tFinalPkg.aData.size();
		t_uoffset nCurrSize = tPkg.aData.size();
		t_uoffset nReminder = nCurrSize % s_cnGranul;
		t_uoffset nFinalSize = nLastDataSize + nCurrSize + (nReminder == 0 ? 0 : s_cnGranul - nReminder);
		tFinalPkg.aData.resize(nFinalSize);
		std::memcpy(&tFinalPkg.aData[nLastDataSize], &tPkg.aData[0], nCurrSize);
		if (nReminder > 0)
			std::memset(&tFinalPkg.aData.back() - nReminder, 0, nReminder);

		// Merge Code sections
		t_uoffset nLastCodeSize = tFinalPkg.aCode.size();
		nCurrSize = tPkg.aCode.size();
		nReminder = nCurrSize % s_cnGranul;
		nFinalSize = nLastCodeSize + nCurrSize + (nReminder == 0 ? 0 : s_cnGranul - nReminder);
		tFinalPkg.aCode.resize(nFinalSize);
		std::memcpy(&tFinalPkg.aCode[nLastCodeSize], &tPkg.aCode[0], nCurrSize);
		if (nReminder > 0)
			std::memset(&tFinalPkg.aCode.back() - nReminder, 0, nReminder);

		std::vector<t_index> aSymbolIndexRemapping(tPkg.oSymbolTbl.aEntries.size());

		// Merge symbol tables
		for (t_index nSblIdx = 0; nSblIdx < aSymbolIndexRemapping.size(); ++nSblIdx)
		{
			SSymbolTable::SEntry const& tEntry = tPkg.oSymbolTbl.aEntries.at(nSblIdx);
			t_uoffset nSizeShift = tEntry.isFunc ? nLastCodeSize : nLastDataSize;

			// Lookup in final symbol table
			auto it = m_mapSymbols.find({&tEntry.sName});
			if (it == m_mapSymbols.end())
			{
				SSymbolTable::SEntry tFinalEntry = tEntry;
				tFinalEntry.nBase += nSizeShift;
				aSymbolIndexRemapping[nSblIdx] = tFinalPkg.oSymbolTbl.aEntries.size();
				tFinalPkg.oSymbolTbl.aEntries.push_back(std::move(tFinalEntry));
				m_mapSymbols.insert({&tFinalPkg.oSymbolTbl.aEntries.back().sName, aSymbolIndexRemapping[nSblIdx]});
			}
			else
			{
				SSymbolTable::SEntry& tFinalEntry = tFinalPkg.oSymbolTbl.aEntries[it->second];
				if (tFinalEntry.isFunc = tEntry.isFunc || tFinalEntry.eType != tEntry.eType)
					throw CError(base::toStr("Linker: Symbol types mismatch '%1'", tEntry.sName), tPkg.tInfo.sName);
				if (tFinalEntry.nBase != core::cnInvalidAddress && tEntry.nBase != core::cnInvalidAddress)
					throw CError(base::toStr("Linker: Symbol redefinition '%1'", tEntry.sName), tPkg.tInfo.sName);
				if (tEntry.nBase != core::cnInvalidAddress)
				{
					tFinalEntry.nBase = tEntry.nBase + nSizeShift;
					tFinalEntry.nSize = tEntry.nSize;
				}
				aSymbolIndexRemapping[nSblIdx] = it->second;
			}
		}

		// Lambda function for the relocation merging
		auto fnRelocMerge = [nLastCodeSize, &tFinalPkg, &aSymbolIndexRemapping](
			std::vector<t_uoffset>& aFinalTbl, std::vector<t_uoffset> const& aTbl)
		{
			// Merge relocation tables
			size_t nRelocBase = aFinalTbl.size();
			aFinalTbl.reserve(nRelocBase + aTbl.size());
			aFinalTbl.insert(aFinalTbl.end(), aTbl.begin(), aTbl.end());

			// Adjust offsets within the relocation table and
			// Adjust Symbol indexes within the code section
			for (size_t i = nRelocBase; i < aFinalTbl.size(); ++i)
			{
				aFinalTbl[i] += nLastCodeSize;
				//
				t_uoffset uRelativeAddress = aFinalTbl[i];
				// WARNING! Machine level alignment check risk
				// TODO: do it with memory copy 
				t_address nSymbolIndex = reinterpret_cast<t_address&>(tFinalPkg.aCode[uRelativeAddress]);
				nSymbolIndex = aSymbolIndexRemapping[nSymbolIndex];
			}
		};

		// Adjust relocation for Data
		fnRelocMerge(tFinalPkg.oRelocTbl.aDataAddressLocations, tPkg.oRelocTbl.aDataAddressLocations);
		// Adjust relocation for Code
		fnRelocMerge(tFinalPkg.oRelocTbl.aCodeAddressLocations, tPkg.oRelocTbl.aCodeAddressLocations);

		// Merge debug info
		for (auto const& tEntry : tPkg.oDebugInfo.aEntries)
		{
			tFinalPkg.oDebugInfo.aEntries.push_back(tEntry);
			SDebugInfo::SEntry& tFinalEntry = tFinalPkg.oDebugInfo.aEntries.back();
			// Adjust symbol index
			tFinalEntry.nSymbolIndex = aSymbolIndexRemapping[tFinalEntry.nSymbolIndex];
			// Adjust Code Table addresses (PDB)
			for (t_uoffset& nOffset : tFinalEntry.aCodeTbl)
				nOffset += nLastCodeSize;
		}

		// Determine program entry point (main)
		if (tPkg.tInfo.nEntryPoint != g_ciInvalid)
		{
			t_index nEntryPoint = aSymbolIndexRemapping[tPkg.tInfo.nEntryPoint];
			// Only one program entry point is allowed
			if (tFinalPkg.tInfo.nEntryPoint != g_ciInvalid && tFinalPkg.tInfo.nEntryPoint != nEntryPoint)
				throw CError(t_csz("Linker: Redefinition of program entry point"), tPkg.tInfo.sName);
			tFinalPkg.tInfo.nEntryPoint = nEntryPoint;
		}
	}
}

void CLinker::ResolveOffsets(SPackage& tPkg)
{
	// Replace symbol indexes within the code with actual (resolved addresses
	auto fnResolver = [&tPkg](std::vector<t_uoffset> const& aRelocTbl)
	{
		for (t_uoffset nOffset : aRelocTbl)
		{
			t_address* nAddresPlace = reinterpret_cast<t_address*>(&tPkg.aCode[nOffset]);
			t_address nSymbolIndex;
			std::memcpy(&nSymbolIndex, nAddresPlace, sizeof(t_address));
			VASM_CHECK_IDX(nSymbolIndex, tPkg.oSymbolTbl.aEntries.size());
			SSymbolTable::SEntry const& tSymbol = tPkg.oSymbolTbl.aEntries.at(nSymbolIndex);
			if (tSymbol.nBase == core::cnInvalidAddress)
				throw CError(base::toStr("Linker: Undefined referene to '%1'", tSymbol.sName), tPkg.tInfo.sName);
			t_address nSymbolAddress = tSymbol.nBase;
			if (!tSymbol.isFunc)
				nSymbolAddress += tPkg.aCode.size(); // Data starts right after the code
			std::memcpy(nAddresPlace, &nSymbolAddress, sizeof(t_address));
		}
	};

	fnResolver(tPkg.oRelocTbl.aDataAddressLocations);
	fnResolver(tPkg.oRelocTbl.aCodeAddressLocations);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace cl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace vasm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
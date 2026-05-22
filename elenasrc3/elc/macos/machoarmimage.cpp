//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive MachO Image class implementation
//       supported platform: ARM64
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "machoarmimage.h"
#include "armrelocation.h"

using namespace elena_lang;

// --- MachOARM64ImageFormatter ---

void MachOARM64ImageFormatter :: fixSection(MemoryBase* section, AddressSpace& map)
{
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, arm64relocate);
}

void MachOARM64ImageFormatter :: fixImportSection(MemoryBase*, AddressSpace&)
{
   // Mach-O ARM64 import binding is implemented by the linker, not by the
   // first local-relocation pass.
}

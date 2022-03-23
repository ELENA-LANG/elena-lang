//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image loader class implementations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "codescope.h"

using namespace elena_lang;

// --- ReferenceMapper ---

addr_t ReferenceMapper :: resolveExternal(ustr_t referenceName)
{
   addr_t address = _exportReferences.get(referenceName);
   if (address == INVALID_ADDR) {
      address = (_exportReferences.count() + 1) | mskImportRef;

      _exportReferences.add(referenceName, address);
   }
   return address;
}

addr_t ReferenceMapper :: resolveReference(ustr_t referenceName, ref_t sectionMask)
{
   switch (sectionMask) {
      case mskSymbolRef:
         return _symbolReferences.get(referenceName);
      case mskMetaArrayRef:
         return _constReferences.get(referenceName);
      case mskExternalRef:
         return resolveExternal(referenceName);
      case mskVMTRef:
         return _dataReferences.get(referenceName);
      default:
         return INVALID_ADDR;
   }
}

void ReferenceMapper :: mapReference(ustr_t referenceName, addr_t address, ref_t sectionMask)
{
   switch (sectionMask) {
      case mskSymbolRef:
         _symbolReferences.add(referenceName, address);
         break;
      case mskMetaArrayRef:
         _constReferences.add(referenceName, address);
         break;
      case mskVMTRef:
         _dataReferences.add(referenceName, address);
         break;
      default:
         break;
   }
}

addr_t ReferenceMapper :: resolveReference(ReferenceInfo referenceInfo, ref_t mask)
{
   if (referenceInfo.isRelative()) {
      IdentifierString fullName(referenceInfo.module->name(), referenceInfo.referenceName);

      return resolveReference(*fullName, mask);
   }
   else return resolveReference(referenceInfo.referenceName, mask);
}

void ReferenceMapper :: mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t mask)
{
   if (referenceInfo.isRelative()) {
      IdentifierString fullName(referenceInfo.module->name(), referenceInfo.referenceName);

      return mapReference(*fullName, address, mask);
   }
   else return mapReference(referenceInfo.referenceName, address, mask);
}

void ReferenceMapper :: mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef)
{
   ref_t nextNameId = _actionNames.count() + 1;
   ref_t nameId = mapKey(_actionNames, actionName, nextNameId);

   ref_t refId = mapKey(_actions, encodeAction64(nameId, signRef), actionRef);
}

ref_t ReferenceMapper :: resolveAction(ustr_t actionName, ref_t signRef)
{
   ref_t actionNameId = _actionNames.get(actionName);

   return _actions.get(encodeAction64(actionNameId, signRef));
}

void ReferenceMapper :: addLazyReference(LazyReferenceInfo info)
{
   _lazyReferences.add(info);
}

// --- ImageProvider ---

Section* ImageProvider :: getTextSection()
{
   return &_text;
}

Section* ImageProvider :: getMDataSection()
{
   return &_mdata;
}

Section* ImageProvider :: getRDataSection()
{
   return &_rdata;
}

Section* ImageProvider :: getImportSection()
{
   return &_import;
}

Section* ImageProvider :: getDataSection()
{
   return &_data;
}

Section* ImageProvider :: getMBDataSection()
{
   return &_mbdata;
}

Section* ImageProvider :: getTargetDebugSection()
{
   return &_debug;
}

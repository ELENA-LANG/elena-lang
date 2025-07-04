//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image loader class implementations
//                                             (C)2021-2024, by Aleksey Rakov
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
      if (_externalMapper) {
         address = _externalMapper->resolveExternal(referenceName);

         assert(address != INVALID_ADDR);
      }
      else address = (_exportReferences.count() + 1) | mskImportRef;

      _exportReferences.add(referenceName, address);
   }
   return address;
}

addr_t ReferenceMapper :: resolveReference(ustr_t referenceName, ref_t sectionMask)
{
   switch (sectionMask) {
      case mskSymbolRef:
      case mskProcedureRef:
         return _symbolReferences.get(referenceName);
      case mskIntLiteralRef:
         return _numberReferences.get(referenceName);
      case mskLongLiteralRef:
         return _longNumberReferences.get(referenceName);
      case mskRealLiteralRef:
         return _realNumberReferences.get(referenceName);
      case mskLiteralRef:
         return _literalReferences.get(referenceName);
      case mskWideLiteralRef:
         return _wideReferences.get(referenceName);
      case mskCharacterRef:
         return _characterReferences.get(referenceName);
      case mskTypeListRef:
      case mskConstArray:
      case mskConstant:
      case mskDistrTypeListRef:
         return _constReferences.get(referenceName);
      case mskExternalRef:
         return resolveExternal(referenceName);
      case mskVMTRef:
         return _dataReferences.get(referenceName);
      case mskStaticRef:
      case mskStaticVariable:
         return _statReferences.get(referenceName);
      case mskMssgNameLiteralRef:
         return _subjReferences.get(referenceName);
      case mskPropNameLiteralRef:
         return _propSubjReferences.get(referenceName);
      case mskMssgLiteralRef:
      case mskExtMssgLiteralRef:
         return _mssgReferences.get(referenceName);
      case mskTLSVariable:
         return _tlsReferences.get(referenceName);
      default:
         return INVALID_ADDR;
   }
}

void ReferenceMapper :: mapReference(ustr_t referenceName, addr_t address, ref_t sectionMask)
{
   switch (sectionMask) {
      case mskSymbolRef:
      case mskProcedureRef:
         _symbolReferences.add(referenceName, address);
         break;
      case mskLiteralRef:
         _literalReferences.add(referenceName, address);
         break;
      case mskWideLiteralRef:
         _wideReferences.add(referenceName, address);
         break;
      case mskIntLiteralRef:
         _numberReferences.add(referenceName, address);
         break;
      case mskLongLiteralRef:
         _longNumberReferences.add(referenceName, address);
         break;
      case mskRealLiteralRef:
         _realNumberReferences.add(referenceName, address);
         break;
      case mskCharacterRef:
         _characterReferences.add(referenceName, address);
         break;
      case mskTypeListRef:
      case mskConstArray:
      case mskConstant:
      case mskDistrTypeListRef:
         _constReferences.add(referenceName, address);
         break;
      case mskMssgLiteralRef:
      case mskExtMssgLiteralRef:
         _mssgReferences.add(referenceName, address);
         break;
      case mskMssgNameLiteralRef:
         _subjReferences.add(referenceName, address);
         break;
      case mskPropNameLiteralRef:
         _propSubjReferences.add(referenceName, address);
         break;
      case mskVMTRef:
         _dataReferences.add(referenceName, address);
         break;
      case mskStaticRef:
      case mskStaticVariable:
         _statReferences.add(referenceName, address);
         break;
      case mskTLSVariable:
         _tlsReferences.add(referenceName, address);
         break;
      default:
         break;
   }
}

ustr_t ReferenceMapper :: retrieveReference(addr_t address, ref_t sectionMask)
{
   switch (sectionMask) {
      case mskVMTRef:
         return _dataReferences.retrieve<addr_t>(nullptr, address, [](addr_t reference, ustr_t key, addr_t current)
            {
               return current == reference;
            });
      case mskSymbolRef:
      case mskProcedureRef:
         return _symbolReferences.retrieve<addr_t>(nullptr, address, [](addr_t reference, ustr_t key, addr_t current)
            {
               return current == reference;
            });
      default:
         return nullptr;
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

   mapKey(_actions, encodeAction64(nameId, signRef), actionRef);
}

ref_t ReferenceMapper :: resolveAction(ustr_t actionName, ref_t signRef)
{
   ref_t actionNameId = _actionNames.get(actionName);

   return _actions.get(encodeAction64(actionNameId, signRef));
}

ustr_t ReferenceMapper :: retrieveAction(ref_t actionRef, ref_t& signRef)
{
   auto actionKey = _actions.retrieve<ref_t>(0, actionRef, [](ref_t actionRef, ref64_t key, ref_t current)
      {
         return current == actionRef;
      });

   ref_t actionName = 0;
   if (actionKey != 0) {
      decodeAction64(actionKey, actionName, signRef);
   }
   else actionName = actionRef;

   return _actionNames.retrieve<ref_t>(nullptr, actionName, [](ref_t reference, ustr_t, ref_t current)
   {
      return current == reference;
   });
}

void ReferenceMapper :: addLazyReference(LazyReferenceInfo info)
{
   _lazyReferences.add(info);
}

// --- ImageProvider ---

MemoryBase* ImageProvider :: getTextSection()
{
   return &_text;
}

MemoryBase* ImageProvider :: getADataSection()
{
   return &_adata;
}

MemoryBase* ImageProvider :: getMDataSection()
{
   return &_mdata;
}

MemoryBase* ImageProvider :: getRDataSection()
{
   return &_rdata;
}

MemoryBase* ImageProvider :: getImportSection()
{
   return &_import;
}

MemoryBase* ImageProvider :: getDataSection()
{
   return &_data;
}

MemoryBase* ImageProvider::getStatSection()
{
   return &_stat;
}

MemoryBase* ImageProvider :: getMBDataSection()
{
   return &_mbdata;
}

MemoryBase* ImageProvider :: getTargetDebugSection()
{
   return &_debug;
}

MemoryBase* ImageProvider :: getTLSSection()
{
   return &_tls;
}

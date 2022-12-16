//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image Loader class declarations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CODESCOPE_H
#define CODESCOPE_H

#include "elena.h"

namespace elena_lang
{
   // --- ReferenceMapper ---
   class ReferenceMapper : public ReferenceMapperBase
   {
   protected:
      typedef List<LazyReferenceInfo> LazyReferences;

      AddressMap     _symbolReferences;
      AddressMap     _exportReferences;
      AddressMap     _constReferences, _numberReferences, _literalReferences, _characterReferences;
      AddressMap     _longNumberReferences, _realNumberReferences;
      AddressMap     _wideReferences;
      AddressMap     _mssgReferences;
      AddressMap     _dataReferences;
      AddressMap     _statReferences;

      ReferenceMap   _actionNames;
      ActionMap      _actions;

      LazyReferences _lazyReferences;

      addr_t resolveExternal(ustr_t referenceName);
      addr_t resolveReference(ustr_t referenceName, ref_t sectionMask);
      void mapReference(ustr_t referenceName, addr_t address, ref_t sectionMask);

      List<LazyReferenceInfo>::Iterator lazyReferences() override
      {
         return _lazyReferences.start();
      }

   public:
      void mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t mask) override;
      addr_t resolveReference(ReferenceInfo referenceInfo, ref_t mask) override;
      ustr_t retrieveReference(addr_t address, ref_t sectionMask) override;

      void mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef) override;
      ref_t resolveAction(ustr_t actionName, ref_t signRef) override;
      ustr_t retrieveAction(ref_t actionRef, ref_t& signRef) override;

      void addLazyReference(LazyReferenceInfo info) override;

      ReferenceMapper() : 
         _symbolReferences(INVALID_ADDR), 
         _exportReferences(INVALID_ADDR), 
         _constReferences(INVALID_ADDR),
         _numberReferences(INVALID_ADDR),
         _literalReferences(INVALID_ADDR),
         _characterReferences(INVALID_ADDR),
         _longNumberReferences(INVALID_ADDR),
         _realNumberReferences(INVALID_ADDR),
         _wideReferences(INVALID_ADDR),
         _mssgReferences(INVALID_ADDR),
         _dataReferences(INVALID_ADDR),
         _statReferences(INVALID_ADDR),
         _actionNames(0),
         _actions(0),
         _lazyReferences({})
      {
         
      }
   };

   // --- ImageProvider ---
   class ImageProvider : public ImageProviderBase
   {
   protected:
      Section _text;
      Section _mdata;
      Section _mbdata;
      Section _rdata;
      Section _import;
      Section _data;
      Section _stat;
      Section _debug;

   public:
      MemoryBase* getTextSection() override;
      MemoryBase* getRDataSection() override;
      MemoryBase* getImportSection() override;
      MemoryBase* getDataSection() override;
      MemoryBase* getStatSection() override;
      MemoryBase* getMDataSection() override;
      MemoryBase* getMBDataSection() override;

      MemoryBase* getTargetDebugSection() override;

      ImageProvider() :
         _text(), _mdata(), _mbdata(), _rdata(),
         _import(), _data(), _stat(), _debug()
      {
      }
   };
}

#endif

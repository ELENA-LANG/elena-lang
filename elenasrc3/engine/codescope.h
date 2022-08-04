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
      AddressMap     _constReferences;
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
      Section* getTextSection() override;
      Section* getRDataSection() override;
      Section* getImportSection() override;
      Section* getDataSection() override;
      Section* getStatSection() override;
      Section* getMDataSection() override;
      Section* getMBDataSection() override;

      Section* getTargetDebugSection() override;

      Section* getTargetSection(ref_t targetMask) override
      {
         switch (targetMask) {
            case mskCodeRef:
               return getTextSection();
            case mskRDataRef:
               return getRDataSection();
            case mskDataRef:
               return getDataSection();
            case mskStatDataRef:
               return getStatSection();
            default:
               return nullptr;
         }
      }

      ImageProvider() :
         _text(), _mdata(), _mbdata(), _rdata(),
         _import(), _data(), _stat(), _debug()
      {
      }
   };
}

#endif
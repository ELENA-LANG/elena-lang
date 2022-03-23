//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef JITLINKER_H
#define JITLINKER_H

#include "elena.h"

namespace elena_lang
{
   // --- JITLinkerSettings ---
   struct JITLinkerSettings
   {
      pos_t       alignment;
      JITSettings jitSettings;
      bool        virtualMode;
      bool        autoLoadMode;
   };

   // --- JITLinker ---
   class JITLinker
   {
      struct VAddressInfo
      {
         ref_t       reference;
         ModuleBase* module;
         ref_t       addressMask;
         pos_t       disp;

         VAddressInfo()
         {
            this->reference = 0;
            this->module = nullptr;
            this->addressMask = 0;
            this->disp = 0;

         }
         VAddressInfo(ref_t reference, ModuleBase* module, ref_t addressMask, pos_t disp)
         {
            this->reference = reference;
            this->module = module;
            this->addressMask = addressMask;
            this->disp = disp;
         }
      };

      typedef Map<pos_t, VAddressInfo> VAddressMap;

      class JITLinkerReferenceHelper : public ReferenceHelperBase
      {
         JITLinker*   _owner;
         ModuleBase*  _module;
         MemoryBase*  _debug;
         VAddressMap* _references;

      public:
         void addBreakpoint(MemoryWriter& writer) override;

         void writeReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp,
            ref_t addressMask, ModuleBase* module) override;

         void writeVAddress32(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeRelAddress32(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeVAddress64(MemoryBase& target, pos_t position, addr_t vaddress, pos64_t disp,
            ref_t addressMask) override;
         void writeVAddress32Hi(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeVAddress32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeDisp32Hi(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeDisp32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;

         mssg_t importMessage(mssg_t message, ModuleBase* module = nullptr) override;

         addr_t calculateVAddress(MemoryWriter& writer, ref_t addressMask) override
         {
            return _owner->calculateVAddress(writer, addressMask);
         }

         JITLinkerReferenceHelper(JITLinker* owner, ModuleBase* module, VAddressMap* references);
      };

      ReferenceMapperBase*  _mapper;
      LibraryLoaderBase*    _loader;
      ForwardResolverBase*  _forwardResolver;
      ImageProviderBase*    _imageProvider;
      JITCompilerBase*      _compiler;

      pos_t                 _alignment;
      JITSettings           _jitSettings;
      bool                  _virtualMode;
      bool                  _classSymbolAutoLoadMode;
         
      addr_t calculateVAddress(MemoryWriter& writer, ref_t targetMask);

      addr_t getVMTAddress(ModuleBase* module, ref_t reference, VAddressMap& references);
      void* getVMTPtr(addr_t address);

      void fixReferences(VAddressMap& relocations, MemoryBase* image);

      addr_t loadMethod(ReferenceHelperBase& refHelper, MemoryReader& reader, MemoryWriter& writer);

      addr_t createVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo, 
         VAddressMap& references);

      ref_t resolveWeakAction(ustr_t actionName);

      ref_t createAction(ustr_t actionName, ref_t weakAction, ref_t signature);
      ref_t createSignature(ModuleBase* module, ref_t signature, VAddressMap& references);
      mssg_t createMessage(ModuleBase* module, mssg_t message, VAddressMap& references);

      addr_t resolveVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo);
      addr_t resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo);
      addr_t resolveMetaSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo);

   public:
      addr_t resolve(ustr_t referenceName, ref_t sectionMask, bool silentMode);
      addr_t resolve(ReferenceInfo refrenceInfo, ref_t sectionMask, bool silentMode);

      void prepare(JITCompilerBase* compiler);
      void complete(JITCompilerBase* compiler);

      JITLinker(ReferenceMapperBase* mapper, 
         LibraryLoaderBase* loader, ForwardResolverBase* forwardResolver,
         ImageProviderBase* provider,
         JITLinkerSettings* settings)
      {
         _mapper = mapper;
         _loader = loader;
         _forwardResolver = forwardResolver;
         _imageProvider = provider;
         _compiler = nullptr;

         _alignment = settings->alignment;
         _jitSettings = settings->jitSettings;
         _virtualMode = settings->virtualMode;
         _classSymbolAutoLoadMode = settings->autoLoadMode;
      }
   };

}

#endif
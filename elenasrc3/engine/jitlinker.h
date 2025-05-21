//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef JITLINKER_H
#define JITLINKER_H

#include "elena.h"

namespace elena_lang
{
   // --- MethodAddress ---
   typedef Pair<addr_t, mssg_t> MethodAddress;

   inline pos_t Map_StoreMethodAddress(MemoryDump* dump, MethodAddress addr)
   {
      pos_t position = dump->length();

      dump->write(position, &addr, sizeof(addr));

      return position;
   }

   inline MethodAddress Map_GetMethodAddress(MemoryDump* dump, pos_t position)
   {
      MethodAddress addr;
      dump->read(position, &addr, sizeof(addr));

      return addr;
   }

   typedef MemoryMap<
      MethodAddress, addr_t,
      Map_StoreMethodAddress,
      Map_GetMethodAddress
   > MethodAddressMap;

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
   public:
      struct ConstantSettings
      {
         ustr_t intLiteralClass;
         ustr_t longLiteralClass;
         ustr_t realLiteralClass;
         ustr_t literalClass;
         ustr_t wideLiteralClass;
         ustr_t characterClass;
         ustr_t messageClass;
         ustr_t extMessageClass;
         ustr_t messageNameClass;
         ustr_t propertyNameClass;
      };

      struct VAddressInfo
      {
         ref_t       reference;
         ModuleBase* module;
         ref_t       addressMask;
         pos_t       disp;
         mssg_t      message;

         VAddressInfo()
         {
            this->reference = 0;
            this->module = nullptr;
            this->addressMask = 0;
            this->disp = 0;
            this->message = 0;

         }
         VAddressInfo(ref_t reference, ModuleBase* module, ref_t addressMask, pos_t disp)
         {
            this->reference = reference;
            this->module = module;
            this->addressMask = addressMask;
            this->disp = disp;
            this->message = 0;
         }
         VAddressInfo(ref_t reference, ModuleBase* module, ref_t addressMask, pos_t disp, mssg_t message)
         {
            this->reference = reference;
            this->module = module;
            this->addressMask = addressMask;
            this->disp = disp;
            this->message = message;
         }
      };

      typedef Map<pos_t, VAddressInfo>          VAddressMap;

      class JITLinkerReferenceHelper : public ReferenceHelperBase
      {
         JITLinker*   _owner;
         ModuleBase*  _module;
         MemoryBase*  _debug;
         VAddressMap* _references;

      public:
         void addBreakpoint(MemoryWriter& writer) override;

         void writeSectionReference(MemoryBase* image, pos_t imageOffset, ref_t reference, 
            SectionInfo* sectionInfo, pos_t sectionOffset, ref_t addressMask) override;

         void writeReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp,
            ref_t addressMask, ModuleBase* module) override;
         void writeVMTMethodReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp, mssg_t message,
            ref_t addressMask, ModuleBase* module) override;

         void writeVAddress32(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeMDataRef32(MemoryBase& target, pos_t position,
            pos_t disp, ref_t addressMask) override;
         void writeStatRef32(MemoryBase& target, pos_t position,
            pos_t disp, ref_t addressMask) override;
         void writeRelAddress32(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
            ref_t addressMask) override;
         void writeVAddress64(MemoryBase& target, pos_t position, addr_t vaddress, pos64_t disp,
            ref_t addressMask) override;
         void writeMDataRef64(MemoryBase& target, pos_t position, pos64_t disp,
            ref_t addressMask) override;
         void writeStatRef64(MemoryBase& target, pos_t position, pos64_t disp,
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

         addr_t resolveMDataVAddress() override;

         addr_t calculateVAddress(MemoryWriter& writer, ref_t addressMask) override
         {
            return _owner->calculateVAddress(writer, addressMask);
         }

         void resolveLabel(MemoryWriter& writer, ref_t mask, pos_t position) override;

         JITLinkerReferenceHelper(JITLinker* owner, ModuleBase* module, VAddressMap* references);
      };

   private:
      ReferenceMapperBase*  _mapper;
      LibraryLoaderBase*    _loader;
      ForwardResolverBase*  _forwardResolver;
      ImageProviderBase*    _imageProvider;
      JITCompilerBase*      _compiler;
      MethodAddressMap      _staticMethods;
      AddressMapperBase*    _addressMapper;

      pos_t                 _alignment;      
      ConstantSettings      _constantSettings;
      bool                  _virtualMode;
      bool                  _classSymbolAutoLoadMode;
      bool                  _withOutputList;
      bool                  _withDebugInfo;
         
      addr_t calculateVAddress(MemoryWriter& writer, ref_t targetMask);
      addr_t calculateVOffset(MemoryWriter& writer, ref_t targetMask);

      addr_t getVMTAddress(ModuleBase* module, ref_t reference, VAddressMap& references);
      void* getVMTPtr(addr_t address);

      addr_t getVMTMethodAddress(addr_t vmtAddress, mssg_t message);

      void fixOffset(pos_t position, ref_t offsetMask, int offset, MemoryBase* image);
      void fixReferences(VAddressMap& relocations, MemoryBase* image);

      addr_t resolveVMTMethodAddress(ModuleBase* module, ref_t reference, mssg_t message);
      pos_t resolveVMTMethodOffset(ModuleBase* module, ref_t reference, mssg_t message);
      pos_t resolveHiddenMTMethodOffset(ModuleBase* module, ref_t reference, mssg_t message);

      addr_t loadMethod(ReferenceHelperBase& refHelper, MemoryReader& reader, MemoryWriter& writer);

      void fillMethodTable(addr_t vaddress, pos_t position, MemoryReader& vmtReader, ClassSectionInfo& sectionInfo,
         MemoryBase* vmtImage, MemoryBase* codeImage, pos_t& size, pos_t& count, pos_t& indexCount,
         VAddressMap& references, CachedOutputTypeList& outputTypeList, bool withOutputList);

      addr_t createVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo, 
         VAddressMap& references);

      ref_t resolveWeakAction(ustr_t actionName);

      ref_t createAction(ustr_t actionName, ref_t weakAction, ref_t signature);
      ref_t createSignature(ModuleBase* module, ref_t signature, bool variadicOne, 
         VAddressMap& references);
      mssg_t createMessage(ModuleBase* module, mssg_t message, VAddressMap& references);

      mssg_t parseMessageLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references);
      mssg_t parseMessageNameLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references);
      Pair<mssg_t, addr_t> parseExtMessageLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references);

      void generateOverloadListMetaAttribute(ModuleBase* module, mssg_t message, ref_t listRef);

      addr_t resolveConstantDump(ReferenceInfo referenceInfo, SectionInfo sectionInfo, ref_t sectionMask);

      addr_t resolveVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo);
      addr_t resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo);
      addr_t resolveMetaSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo);
      // NOTE : the list contains already resolved message constants, so only type references must be resolved
      addr_t resolveOutputTypeList(ReferenceInfo referenceInfo, CachedOutputTypeList& outputTypeList);
      addr_t resolveConstant(ReferenceInfo referenceInfo, ref_t sectionMask);
      addr_t resolveConstantArray(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode);
      addr_t resolveConstantDump(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode);
      addr_t resolveStaticVariable(ReferenceInfo referenceInfo, ref_t sectionMask);
      addr_t resolveThreadVariable(ReferenceInfo referenceInfo, ref_t sectionMask);
      addr_t resolveName(ReferenceInfo referenceInfo, bool onlyPath);
      addr_t resolvePackage(ReferenceInfo referenceInfo);
      addr_t resolveRawConstant(ReferenceInfo referenceInfo);
      addr_t resolveDistributeCategory(ReferenceInfo referenceInfo, ref_t sectionMask);

      void resolveStaticFields(ReferenceInfo& referenceInfo, MemoryReader& vmtReader, FieldAddressMap& staticValues);

      void resolveSymbolAttributes(ReferenceInfo referenceInfo, addr_t vaddress, SectionInfo sectionInfo);
      void resolveClassGlobalAttributes(ReferenceInfo referenceInfo, MemoryReader& vmtReader, addr_t vaddress);

      pos_t createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, addr_t vaddress);
      pos_t createNativeClassDebugInfo(ReferenceInfo referenceInfo, addr_t vaddress);
      void endNativeDebugInfo(pos_t position);

      ReferenceInfo retrieveConstantVMT(SectionInfo info);

      void createGlobalAttribute(int category, ustr_t value, addr_t address);

      void copyDistributedSymbolList(ModuleInfo info, MemoryBase* target, ModuleBase* module);

      addr_t resolve(ReferenceInfo refrenceInfo, ref_t sectionMask, bool silentMode);

   public:
      addr_t resolveTape(ustr_t referenceName, MemoryBase* tape);
      addr_t resolveTemporalByteCode(MemoryDump& tapeSymbol, ModuleBase* module);
      addr_t resolveTLSSection(JITCompilerBase* compiler);

      addr_t resolve(ustr_t referenceName, ref_t sectionMask, bool silentMode);

      ustr_t retrieveResolvedAction(ref_t reference);

      ref_t resolveAction(ustr_t actionName);

      void loadPreloaded(ustr_t preloadedSection, bool ignoreAutoLoadExtensions);
      void prepare(JITSettings jitSettings);
      void setCompiler(JITCompilerBase* compiler)
      {
         _compiler = compiler;

         _withDebugInfo = _compiler->isWithDebugInfo();
      }

      void resolveDistributed();

      void complete(JITCompilerBase* compiler, ustr_t superClass);

      void copyPreloadedMetaList(ModuleInfo info, ModuleInfoList& output, bool ignoreAutoLoadExtensions);

      JITLinker(ReferenceMapperBase* mapper,
         LibraryLoaderBase* loader, ForwardResolverBase* forwardResolver,
         ImageProviderBase* provider,
         JITLinkerSettings* settings,
         AddressMapperBase* addressMapper
      );
   };

}

#endif
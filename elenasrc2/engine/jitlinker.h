//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jitlinkerH
#define jitlinkerH 1

#include "jitcompiler.h"

namespace _ELENA_
{

// -- Loader basic constant --
#define LOADER_NOTLOADED (vaddr_t)-1

// --- JITUnresolvedException ---
struct JITUnresolvedException
{
   ReferenceInfo referenceInfo;

   JITUnresolvedException(ReferenceInfo referenceInfo)
   {
      this->referenceInfo = referenceInfo;
   }
};

struct JITConstantExpectedException
{
   ReferenceInfo referenceInfo;

   JITConstantExpectedException(ReferenceInfo referenceInfo)
   {
      this->referenceInfo = referenceInfo;
   }
};

// --- JITLinker class ---
class JITLinker : _JITLoaderListener
{
   struct RefInfo
   {
      ref_t    reference;
      _Module* module;

      RefInfo()
      {
         reference = 0;
         module = nullptr;
      }
      RefInfo(ref_t reference, _Module* module)
      {
         this->reference = reference;
         this->module = module;
      }
   };

   typedef CachedMemoryMap<ref_t, RefInfo, 10> References;

   // --- ReferenceHelper ---
   class ReferenceHelper : public _ReferenceHelper
   {
      References* _references;

      JITLinker*  _owner;
      _Memory*    _debug;
      _Module*    _module;

   public:
      virtual ref_t getLinkerConstant(ref_t constant)
      {
         return _owner->_loader->getLinkerConstant(constant);
      }

      virtual SectionInfo getSection(ref_t reference, _Module* module);
      virtual SectionInfo getCoreSection(ref_t reference);

      virtual vaddr_t getVAddress(MemoryWriter& writer, ref_t mask)
      {
         return _owner->calculateVAddress(&writer, mask);
      }

      virtual mssg_t resolveMessage(mssg_t reference, _Module* module = nullptr);

      virtual void addBreakpoint(pos_t position);

      virtual void writeVAddress(MemoryWriter& writer, vaddr_t vaddress, pos_t disp);
      virtual void writeRelVAddress(MemoryWriter& writer, vaddr_t vaddress, ref_t mask, pos_t disp);

      virtual void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp, _Module* module);
      virtual void writeMTReference(MemoryWriter& writer)
      {
         if (_owner->_virtualMode) {
            writer.writeRef(mskMessageTableRef, 0);
         }
         else {
            _Memory* section = _owner->_loader->getTargetSection(mskMessageTableRef);

            writer.writePtr((uintptr_t)section->get(0));
         }
      }

   //   virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module);

      ReferenceHelper(JITLinker* owner, _Module* module, References* references)
      {
         _references = references;
         _owner = owner;
         _module = module;
         _debug = nullptr;
      }
   };

   typedef Pair<vaddr_t, mssg_t>                 MethodInfo;
   typedef MemoryMap<MethodInfo, vaddr_t, false> MethodAddressMap;
//   typedef Memory32HashTable<ident_t, void*, mapReferenceKey, 29> StrongTypeMap;

   typedef Pair<_Module*, ref_t>            ModuleReference;
   typedef List<ModuleReference>             ModuleReferences;

   _JITLoader*       _loader;
   _JITCompiler*     _compiler; 
   bool              _virtualMode;
   bool              _withDebugInfo;
   bool              _classSymbolAutoLoadMode;
   bool              _withExtDispatchers;
   vaddr_t           _codeBase;
   pos_t             _statLength;
   MethodAddressMap  _staticMethods;
   ModuleReferences  _initializers;

//   int            _uniqueID;           // used for dynamic subject

   void createNativeDebugInfo(ident_t reference, void* param, pos_t& sizePtr);
   void createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, vaddr_t address, pos_t& sizePtr);
   void createNativeClassDebugInfo(ReferenceInfo referenceInfo, vaddr_t vaddress, pos_t& sizePtr);
   void endNativeDebugInfo(size_t sizePtr);

   //vaddr_t getVMTAddress(ref_t address);
   vaddr_t getVMTAddress(_Module* module, ref_t reference, References& references);
   void* getVMTReference(_Module* module, ref_t reference, References& references);
   vaddr_t resolveVMTMethodAddress(_Module* module, ref_t reference, mssg_t messageID);
   vaddr_t getVMTMethodAddress(vaddr_t vaddress, mssg_t messageID);
   pos_t getVMTMethodIndex(vaddr_t address, mssg_t messageID);
   ref_t getVMTFlags(vaddr_t vaddress);

   void generateMetaAttribute(int category, ident_t fullName, vaddr_t address);
   void generateMetaAttribute(int category, ReferenceInfo& referenceInfo, ref_t mask);
   void generateOverloadListMetaAttribute(_Module* module, mssg_t message, ref_t listRef);

   void fixReferences(References& relocations, _Memory* image);
   void fixSectionReferences(SectionInfo& sectionInfo, _Memory* image, pos_t position, vaddr_t &vmtVAddress, 
      bool constArrayMode, References* messageReferences);

   uintptr_t loadMethod(ReferenceHelper& refHelper, MemoryReader& reader, MemoryWriter& writer);

   ref_t mapAction(SectionInfo& messageTable, ident_t action, ref_t weakActionRef, ref_t signature);
   ref_t resolveWeakAction(SectionInfo& messageTable, ident_t action);
   mssg_t resolveMessage(_Module* module, mssg_t reference, References* references);
   ref_t resolveSignature(_Module* module, ref_t signature, bool variadicOne, References* references);

   void createAttributes(ReferenceInfo& referenceInfo, ClassInfo::CategoryInfoMap& attributes);

   vaddr_t resolveNativeVariable(ReferenceInfo referenceInfo, ref_t mask);
////   void* resolveConstVariable(ident_t  reference, int mask);
   vaddr_t resolveNativeSection(ReferenceInfo referenceInfo, ref_t mask, SectionInfo sectionInfo);
   vaddr_t resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t mask, SectionInfo sectionInfo);
   vaddr_t createBytecodeVMTSection(ReferenceInfo referenceInfo, ref_t mask, ClassSectionInfo sectionInfo, References& references);
   vaddr_t resolveBytecodeVMTSection(ReferenceInfo referenceInfo, ref_t mask, ClassSectionInfo sectionInfo);
   vaddr_t resolveConstant(ReferenceInfo referenceInfo, ref_t mask, bool silentMode);
   vaddr_t resolveStaticVariable(ReferenceInfo referenceInfo, ref_t mask);
//   void* resolveAnonymousStaticVariable();
   vaddr_t resolveMessageTable(ReferenceInfo referenceInfo, ref_t mask);
   vaddr_t resolveMetaAttributeTable(ReferenceInfo referenceInfo, ref_t mask);
   vaddr_t resolveMessage(ReferenceInfo referenceInfo, ident_t vmt);
   vaddr_t resolveAction(ReferenceInfo referenceInfo, ident_t vmt);
   vaddr_t resolveExtensionMessage(ReferenceInfo referenceInfo, ident_t vmt);
//////   void* resolveThreadSafeVariable(const TCHAR*  reference, int mask);

   vaddr_t resolveClassName(ReferenceInfo referenceInfo);
   vaddr_t resolvePackage(_Module* module);

   void resolveStaticValues(ReferenceInfo referenceInfo, MemoryReader& vmtReader, MemoryReader& attrReader, 
      _Memory* vmtImage, pos_t position);

public:
   void prepareCompiler();
   void fixImage(ident_t superClass);

   vaddr_t resolve(ReferenceInfo referenceInfo, ref_t mask, bool silentMode);
   vaddr_t resolve(ident_t reference, ref_t mask, bool silentMode);

   vaddr_t resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, ident_t reference, void* param);

   vaddr_t resolveEntry(vaddr_t programEntry);

//   //void loadModuleInfo(_Module* module);

   bool withNewInitializers() const
   {
      return _initializers.Count() > 0;
   }

   void generateInitTape(MemoryDump& tape);

//   bool getDebugMode() const { return _withDebugInfo; }

   pos_t getStaticCount() const 
   { 
      return _loader->getTargetSection((ref_t)mskStatRef)->Length() >> 2;
   }

   vaddr_t calculateVAddress(MemoryWriter* writer, ref_t mask, int alignment);
   vaddr_t calculateVAddress(MemoryWriter* writer, ref_t mask);

   mssg_t parseMessage(ident_t reference);
   ref_t parseAction(ident_t reference);
   ident_t retrieveResolvedAction(ref_t reference);

   virtual void onModuleLoad(_Module* module);

   JITLinker(_JITLoader* loader, _JITCompiler* compiler, bool virtualMode, vaddr_t codeBase, bool withExtDispatchers, bool autoLoadMode = false)
      : _staticMethods(INVALID_PTR)
   {
      _loader = loader;
      _compiler = compiler;
      _virtualMode = virtualMode;
      _withDebugInfo = compiler->isWithDebugInfo();
      _codeBase = codeBase;
      _statLength = 0;
      _classSymbolAutoLoadMode = autoLoadMode;
      _withExtDispatchers = withExtDispatchers;

      loader->addListener(this);
//      _uniqueID = 0;
   }
};

} // _ELENA_

#endif // jitlinkerH

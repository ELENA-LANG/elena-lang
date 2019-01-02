//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jitlinkerH
#define jitlinkerH 1

#include "jitcompiler.h"

namespace _ELENA_
{

// -- Loader basic constant --
#define LOADER_NOTLOADED (void*)-1

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

      virtual void* getVAddress(MemoryWriter& writer, int mask)
      {
         return _owner->calculateVAddress(&writer, mask);
      }

      virtual ref_t resolveMessage(ref_t reference, _Module* module = NULL);

      virtual void addBreakpoint(size_t position);

      virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module);
      virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp);
      virtual void writeMTReference(MemoryWriter& writer)
      {
         if (_owner->_virtualMode) {
            writer.writeRef(mskMessageTableRef, 0);
         }
         else {
            _Memory* section = _owner->_loader->getTargetSection(mskMessageTableRef);

            writer.writeDWord((ref_t)section->get(0));
         }
      }

      //virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module);

      ReferenceHelper(JITLinker* owner, _Module* module, References* references)
      {
         _references = references;
         _owner = owner;
         _module = module;
         _debug = NULL;
      }
   };

   typedef Pair<void*, int>                                       MethodInfo;
   typedef MemoryMap<MethodInfo, int, false>                      MethodMap;
//   typedef Memory32HashTable<ident_t, void*, mapReferenceKey, 29> StrongTypeMap;

   _JITLoader*    _loader;
   _JITCompiler*  _compiler; 
   bool           _virtualMode;
   bool           _withDebugInfo;
   bool           _classSymbolAutoLoadMode;
   void*          _codeBase;
   int            _statLength;
   MethodMap      _staticMethods;
   ModuleList     _loadedModules;

//   int            _uniqueID;           // used for dynamic subject

   void createNativeDebugInfo(ident_t reference, void* param, size_t& sizePtr);
   void createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, void* address, size_t& sizePtr);
   void createNativeClassDebugInfo(ReferenceInfo referenceInfo, void* vaddress, size_t& sizePtr);
   void endNativeDebugInfo(size_t sizePtr);

   void* getVMTAddress(_Module* module, ref_t reference, References& references);
   void* getVMTReference(_Module* module, ref_t reference, References& references);
   int resolveVMTMethodAddress(_Module* module, ref_t reference, int messageID);
   int getVMTMethodAddress(void* vaddress, int messageID);   
   int getVMTMethodIndex(void* vaddress, int messageID);
   size_t getVMTFlags(void* vaddress);

   void fixReferences(References& relocations, _Memory* image);
   void fixSectionReferences(SectionInfo& sectionInfo, _Memory* image, size_t position, void* &vmtVAddress);

   size_t loadMethod(ReferenceHelper& refHelper, MemoryReader& reader, MemoryWriter& writer);

   ref_t mapAction(SectionInfo& messageTable, ident_t action, ref_t weakActionRef, ref_t signature);
   ref_t resolveWeakAction(SectionInfo& messageTable, ident_t action);
   ref_t resolveMessage(_Module* module, ref_t reference);
   ref_t resolveSignature(_Module* module, ref_t signature, bool variadicOne);

   void* resolveNativeVariable(ReferenceInfo referenceInfo, int mask);
//   void* resolveConstVariable(ident_t  reference, int mask);
   void* resolveNativeSection(ReferenceInfo referenceInfo, int mask, SectionInfo sectionInfo);
   void* resolveBytecodeSection(ReferenceInfo referenceInfo, int mask, SectionInfo sectionInfo);
   void* createBytecodeVMTSection(ReferenceInfo referenceInfo, int mask, ClassSectionInfo sectionInfo, References& references);
   void* resolveBytecodeVMTSection(ReferenceInfo referenceInfo, int mask, ClassSectionInfo sectionInfo);
   void* resolveConstant(ReferenceInfo referenceInfo, int mask);
   void* resolveStaticVariable(ReferenceInfo referenceInfo, int mask);
   void* resolveAnonymousStaticVariable();
   void* resolveMessageTable(ReferenceInfo referenceInfo, int mask);
   void* resolveMessage(ReferenceInfo referenceInfo, ident_t vmt, bool actionOnlyMode);
   void* resolveExtensionMessage(ReferenceInfo referenceInfo, ident_t vmt);
////   void* resolveThreadSafeVariable(const TCHAR*  reference, int mask);

public:
   void prepareCompiler();

   void* resolve(ReferenceInfo referenceInfo, int mask, bool silentMode);
   void* resolve(ident_t reference, int mask, bool silentMode);

   void* resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, ident_t reference, void* param);

   void* resolveEntry(void* programEntry);

//   //void loadModuleInfo(_Module* module);

   void generateInitTape(MemoryDump& tape);

   bool getDebugMode() const { return _withDebugInfo; }

   size_t getStaticCount() const 
   { 
      return _loader->getTargetSection((size_t)mskStatRef)->Length() >> 2;
   }

   void* calculateVAddress(MemoryWriter* writer, int mask, int alignment);
   void* calculateVAddress(MemoryWriter* writer, int mask);

   ref_t parseMessage(ident_t reference, bool actionOnlyMode);

   virtual void onModuleLoad(_Module* module);

   JITLinker(_JITLoader* loader, _JITCompiler* compiler, bool virtualMode, void* codeBase, bool autoLoadMode = false)
      : _staticMethods(-1)
   {
      _loader = loader;
      _compiler = compiler;
      _virtualMode = virtualMode;
      _withDebugInfo = compiler->isWithDebugInfo();
      _codeBase = codeBase;
      _statLength = 0;
      _classSymbolAutoLoadMode = autoLoadMode;

      loader->addListener(this);
//      _uniqueID = 0;
   }
};

} // _ELENA_

#endif // jitlinkerH

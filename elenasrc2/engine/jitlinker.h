//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                              (C)2005-2017, by Alexei Rakov
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
   ident_t reference;

   JITUnresolvedException(ident_t reference)
   {
      this->reference = reference;
   }
};

struct JITConstantExpectedException
{
   ident_t reference;

   JITConstantExpectedException(ident_t reference)
   {
      this->reference = reference;
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

//      virtual void writeMethodReference(SectionWriter& writer, size_t tapeDisp);
      virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module);
      virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp);

      virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module);

      ReferenceHelper(JITLinker* owner, _Module* module, References* references)
      {
         _references = references;
         _owner = owner;
         _module = module;
         _debug = NULL;
      }
   };

   typedef Pair<void*, int>                  MethodInfo;
   typedef MemoryMap<MethodInfo, int, false> MethodMap;

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
   void createNativeSymbolDebugInfo(ident_t reference, void* address, size_t& sizePtr);
   void createNativeClassDebugInfo(ident_t reference, void* vaddress, size_t& sizePtr);
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

   ref_t resolveMessage(_Module* module, ref_t reference);

   void* resolveNativeVariable(ident_t  reference, int mask);
   void* resolveNativeSection(ident_t reference, int mask, SectionInfo sectionInfo);
   void* resolveBytecodeSection(ident_t reference, int mask, SectionInfo sectionInfo);
   void* createBytecodeVMTSection(ident_t reference, int mask, ClassSectionInfo sectionInfo, References& references);
   void* resolveBytecodeVMTSection(ident_t reference, int mask, ClassSectionInfo sectionInfo);
   void* resolveConstant(ident_t reference, int mask);
   void* resolveStaticVariable(ident_t reference, int mask);
//   void* resolveDump(const wchar16_t*  reference, int size, int mask);
   void* resolveMessage(ident_t reference, ident_t vmt);
   void* resolveExtensionMessage(ident_t reference, ident_t vmt);
//   void* resolveThreadSafeVariable(const TCHAR*  reference, int mask);

public:
   void prepareCompiler();

   void* resolve(ident_t reference, int mask, bool silentMode);

   void* resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, ident_t reference, void* param);

   void* resolveEntry(void* programEntry);

   void generateInitTape(MemoryDump& tape);

   bool getDebugMode() const { return _withDebugInfo; }

   size_t getStaticCount() const 
   { 
      return _loader->getTargetSection((size_t)mskStatRef)->Length() >> 2;
   }

   void* calculateVAddress(MemoryWriter* writer, int mask, int alignment);
   void* calculateVAddress(MemoryWriter* writer, int mask);

   ref_t parseMessage(ident_t reference);

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

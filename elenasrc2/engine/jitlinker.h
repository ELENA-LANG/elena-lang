//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class.
//
//                                              (C)2005-2013, by Alexei Rakov
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
   const wchar16_t* reference;

   JITUnresolvedException(const wchar16_t* reference)
   {
      this->reference = reference;
   }
};

struct JITConstantExpectedException
{
   const wchar16_t* reference;

   JITConstantExpectedException(const wchar16_t* reference)
   {
      this->reference = reference;
   }
};

// --- JITLinker class ---
class JITLinker
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
      virtual SectionInfo getPredefinedSection(ref_t reference)
      {
         return SectionInfo(_module, _module->mapSection(reference, true));
      }

      virtual void* getVAddress(MemoryWriter& writer, int mask)
      {
         return _owner->calculateVAddress(&writer, mask);
      }

      virtual ref_t resolveMessage(ref_t reference, _Module* module);

      virtual void addBreakpoint(size_t position);

//      virtual void writeMethodReference(SectionWriter& writer, size_t tapeDisp);
      virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module);
      virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp);

      ReferenceHelper(JITLinker* owner, _Module* module, References* references)
      {
         _references = references;
         _owner = owner;
         _module = module;
         _debug = NULL;
      }
   };

   _JITLoader*    _loader;
   _JITCompiler*  _compiler; 
   bool           _virtualMode;
   bool           _withDebugInfo;
   void*          _codeBase;
//   int            _uniqueID;           // used for dynamic subject

   void createNativeDebugInfo(const wchar16_t* reference, void* param, size_t& sizePtr);
   void createNativeSymbolDebugInfo(const wchar16_t* reference, size_t& sizePtr);
   void createNativeClassDebugInfo(const wchar16_t* reference, void* vaddress, size_t& sizePtr);
   void endNativeDebugInfo(size_t sizePtr);

   void* getVMTAddress(_Module* module, ref_t reference, References& references);
   void* getVMTReference(_Module* module, ref_t reference, References& references);
   int getVMTMethodAddress(void* vaddress, int messageID);
   size_t getVMTFlags(void* vaddress);

   void fixReferences(References& relocations, _Memory* image);

   size_t loadMethod(ReferenceHelper& refHelper, MemoryReader& reader, MemoryWriter& writer);

   ref_t resolveMessage(_Module* module, ref_t reference);

//
//   void* resolveNativeVariable(const wchar16_t*  reference);
   void* resolveNativeSection(const wchar16_t*  reference, int mask, SectionInfo sectionInfo);
   void* resolveBytecodeSection(const wchar16_t*  reference, int mask, SectionInfo sectionInfo);
   void* createBytecodeVMTSection(const wchar16_t*  reference, int mask, ClassSectionInfo sectionInfo, References& references);
   void* resolveBytecodeVMTSection(const wchar16_t*  reference, int mask, ClassSectionInfo sectionInfo);
   void* resolveConstant(const wchar16_t*  reference, int mask);
   void* resolveStaticVariable(const wchar16_t* reference, int mask);
//   void* resolveDump(const wchar16_t*  reference, int size, int mask);
   void* resolveMessage(const wchar16_t*  reference, const wchar16_t* vmt);
   void* resolveLoader(const wchar16_t*  reference);
////   void* resolveThreadSafeVariable(const TCHAR*  reference, int mask);

////   void* resolvePseudoVMT(const wchar_t* reference, void* codeRef, int flags);

public:
   void prepareCompiler(_Module* core, _Module* commands);

   void* resolve(const wchar16_t* reference, int mask, bool silentMode);

   void* resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, const wchar16_t* reference, void* param);

   bool getDebugMode() const { return _withDebugInfo; }

   int getStaticCount() const 
   { 
      return _loader->getTargetSection(mskStatRef)->Length() >> 2;
   }

   void* calculateVAddress(MemoryWriter* writer, int mask);

   ref_t parseMessage(const wchar16_t*  reference);

   JITLinker(_JITLoader* loader, _JITCompiler* compiler, bool virtualMode, void* codeBase, bool withDebugInfo)
   {
      _loader = loader;
      _compiler = compiler;
      _virtualMode = virtualMode;
      _withDebugInfo = withDebugInfo;
      _codeBase = codeBase;

//      _uniqueID = 0;
   }
};

} // _ELENA_

#endif // jitlinkerH

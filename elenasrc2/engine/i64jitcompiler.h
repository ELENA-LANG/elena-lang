//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: I64
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef i64jitcompilerH
#define i64jitcompilerH 1

#include "jitcompiler.h"

namespace _ELENA_
{

class I64JITCompiler;

// --- I64JITScope ---

struct I64JITScope
{
   //_Module*          module;
   //I64JITCompiler*   compiler;
   //MemoryWriter*     code;
   //_ReferenceHelper* helper;
   //MemoryReader*     tape;
   //x86LabelHelper    lh;

   //bool              withDebugInfo;
   //int               objectSize;

   // byte code command argument
   int            argument;

   //void writeReference(MemoryWriter& writer, ref_t reference, size_t disp);

   //ref_t resolveMessage(ref_t reference)
   //{
   //   return helper->resolveMessage(reference, module);
   //}

   //SectionInfo getSection(ref_t reference)
   //{
   //   return helper->getSection(reference, module);
   //}

   I64JITScope(/*MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler*/);
};

// --- I64JITCompiler ---

class I64JITCompiler : public JITCompiler64
{
protected:
   // commands
   friend void compileNop(int opcode, I64JITScope& scope);

public:
   virtual bool isWithDebugInfo() const;
   virtual size_t getObjectHeaderSize() const;

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code);

   virtual void* getPreloadedReference(ref_t reference);

   virtual void prepareCore(_ReferenceHelper& helper, _JITLoader* loader);

   virtual int allocateTLSVariable(_JITLoader* loader);
   virtual void allocateThreadTable(_JITLoader* loader, int length);

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);

   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section);

   virtual void setStaticRootCounter(_JITLoader* loader, size_t counter, bool virtualMode);

   virtual void generateProgramStart(MemoryDump& tape);
   virtual void generateSymbolCall(MemoryDump& tape, void* address);
   virtual void generateProgramEnd(MemoryDump& tape);

   I64JITCompiler(bool debugMode);
};

// --- compiler friend functions---
void compileNop(int opcode, I64JITScope& scope);

} // _ELENA_

#endif // i64jitcompilerH

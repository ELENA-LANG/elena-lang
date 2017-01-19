//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: I64
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef amd64jitcompilerH
#define amd64jitcompilerH 1

#include "jitcompiler.h"

namespace _ELENA_
{

class AMD64JITCompiler;

// --- AMD64JITScope ---

struct AMD64JITScope
{
   _Module*          module;
   AMD64JITCompiler* compiler;
   MemoryWriter*     code;
   _ReferenceHelper* helper;
   MemoryReader*     tape;
   //x86LabelHelper    lh;

   bool              withDebugInfo;
   //int               objectSize;

   // byte code command argument
   int            argument;

   void writeReference(MemoryWriter& writer, ref_t reference, size_t disp);

   //ref_t resolveMessage(ref_t reference)
   //{
   //   return helper->resolveMessage(reference, module);
   //}

   //SectionInfo getSection(ref_t reference)
   //{
   //   return helper->getSection(reference, module);
   //}

   AMD64JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, AMD64JITCompiler* compiler);
};

// --- AMD64JITCompiler ---

class AMD64JITCompiler : public JITCompiler64
{
protected:
   bool _debugMode;

   // commands
   friend void writeCoreReference(AMD64JITScope& scope, ref_t reference, int position, int offset, char* code);
   friend void loadCoreOp(AMD64JITScope& scope, char* code);

   friend void compileNop(int opcode, AMD64JITScope& scope);
   friend void compileACopyR(int opcode, AMD64JITScope& scope);
   friend void loadFunction(int opcode, AMD64JITScope& scope);
   friend void compileCallR(int opcode, AMD64JITScope& scope);

   // preloaded references
   IntFixedMap<void*> _preloaded;

   void writeCoreReference(AMD64JITScope& scope, ref_t reference, int position, int offset, char* code);

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

   AMD64JITCompiler(bool debugMode);
};

// --- compiler friend functions---
void loadCoreOp(AMD64JITScope& scope, char* code);

void compileNop(int opcode, AMD64JITScope& scope);
void compileACopyR(int opcode, AMD64JITScope& scope);
void loadFunction(int opcode, AMD64JITScope& scope);
void compileCallR(int opcode, AMD64JITScope& scope);

} // _ELENA_

#endif // amd64jitcompilerH

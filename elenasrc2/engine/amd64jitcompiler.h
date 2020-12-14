//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: I64
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef amd64jitcompilerH
#define amd64jitcompilerH 1

#include "jitcompiler.h"
#include "amd64helper.h"

namespace _ELENA_
{

class I64JITCompiler;

// --- I64JITScope ---

struct I64JITScope
{
   _Module*          module;
   I64JITCompiler*   compiler;
   MemoryWriter*     code;
   _ReferenceHelper* helper;
   MemoryReader*     tape;
   AMD64LabelHelper  lh;

   bool              bigAddressMode;
   bool              withDebugInfo;
//   //int               objectSize;
   int               frameOffset;

   // byte code command argument
   int            argument;

   void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp);
   //void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp);

//   ref64_t resolveMessage(ref_t reference)
//   {
//      ref_t message32 = helper->resolveMessage(reference, module);
//
//      return toMessage64(message32);
//   }
//
//   //SectionInfo getSection(ref_t reference)
//   //{
//   //   return helper->getSection(reference, module);
//   //}

   I64JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, I64JITCompiler* compiler, bool bigAddressMode);
};

// --- I64JITCompiler ---

class I64JITCompiler : public JITCompiler64
{
protected:
   bool _debugMode;
   bool _bigAddressMode;

   // commands
   friend void writeCoreReference(I64JITScope& scope, ref_t reference, int position, int offset, char* code);
   friend void loadCoreOp(I64JITScope& scope, char* code);
   friend void loadOneByteOp(int opcode, I64JITScope& scope);
   friend void loadOneByteLOp(int opcode, I64JITScope& scope);
   friend void loadIndexOp(int opcode, I64JITScope& scope);
   friend void loadNOp(int opcode, I64JITScope& scope);
   friend void loadFPOp(int opcode, I64JITScope& scope);
   friend void loadSPOp(int opcode, I64JITScope& scope);
   friend void loadFOp(int opcode, I64JITScope& scope);
   friend void loadFNOp(int opcode, I64JITScope& scope);

   friend void compileNop(int opcode, I64JITScope& scope);
//   friend void compileACopyR(int opcode, AMD64JITScope& scope);
   friend void loadFunction(int opcode, I64JITScope& scope);
   friend void compileCallR(int opcode, I64JITScope& scope);
   friend void compilePushA(int opcode, I64JITScope& scope);
   friend void compilePush(int opcode, I64JITScope& scope);
   //   friend void compileMCopy(int opcode, AMD64JITScope& scope);
   friend void compileInvokeVMT(int opcode, I64JITScope& scope);
   friend void compileOpen(int opcode, I64JITScope& scope);
   friend void compileQuit(int opcode, I64JITScope& scope);
   friend void compileQuitN(int opcode, I64JITScope& scope);
//   friend void compileBCopyF(int opcode, AMD64JITScope& scope);
//   friend void compileDCopy(int opcode, AMD64JITScope& scope);
   friend void compileACopyS(int opcode, I64JITScope& scope);
   friend void compileACopyF(int opcode, I64JITScope& scope);
//   friend void compileDAddN(int opcode, AMD64JITScope& scope);
   friend void compileBreakpoint(int opcode, I64JITScope& scope);
//   friend void compileInvokeVMTOffset(int opcode, AMD64JITScope& scope);
   friend void compilePopN(int opcode, I64JITScope& scope);
   friend void compileAllocI(int opcode, I64JITScope& scope);
   friend void compileReserve(int op, I64JITScope& scope);
   friend void compileRestore(int op, I64JITScope& scope);
   friend void compileDCopyCount(int, I64JITScope& scope);
   friend void compilePopA(int opcode, I64JITScope& scope);
   friend void compilePopD(int opcode, I64JITScope& scope);
   friend void compileNot(int opcode, I64JITScope& scope);
   friend void compilePushD(int, I64JITScope& scope);
   friend void compileDec(int, I64JITScope& scope);
   friend void compilePushF(int, I64JITScope& scope);
   friend void compilePushFI(int, I64JITScope& scope);
   friend void compilePushSI(int, I64JITScope& scope);

   // preloaded command set
   void* _inlines[0x100];
   IntFixedMap<void*> _inlineExs;

   // preloaded references
   IntFixedMap<vaddr_t> _preloaded;

   void writeCoreReference(I64JITScope& scope, ref_t reference, pos_t position, int offset, char* code);

public:
   virtual bool isWithDebugInfo() const;
   virtual size_t getObjectHeaderSize() const;

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code);

   virtual vaddr_t getPreloadedReference(ref_t reference);

   virtual void prepareCore(_ReferenceHelper& helper, _JITLoader* loader);

   virtual int allocateTLSVariable(_JITLoader* loader);
   virtual void allocateThreadTable(_JITLoader* loader, int length);
   virtual int allocateVMTape(_JITLoader* loader, void* tape, pos_t length);

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);

//   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section);

   virtual void setStaticRootCounter(_JITLoader* loader, pos_t counter, bool virtualMode);
   virtual void setTLSKey(vaddr_t ptr);
   virtual void setThreadTable(vaddr_t ptr);
   virtual void setEHTable(vaddr_t ptr);
   virtual void setGCTable(vaddr_t ptr);
   virtual void setVoidParent(_JITLoader* loader, vaddr_t ptr, bool virtualMode);

   virtual vaddr_t getInvoker();

   virtual void generateSymbolCall(MemoryDump& tape, vaddr_t address);
   virtual void generateProgramEnd(MemoryDump& tape);

   I64JITCompiler(bool debugMode, bool bigAddressMode);
};

// --- compiler friend functions---
void loadCoreOp(I64JITScope& scope, char* code);
void loadOneByteLOp(int opcode, I64JITScope& scope);
void loadOneByteLOp(int opcode, I64JITScope& scope);
void loadIndexOp(int opcode, I64JITScope& scope);
void loadFPOp(int opcode, I64JITScope& scope); 
void loadSPOp(int opcode, I64JITScope& scope); 
void loadFOp(int opcode, I64JITScope& scope);
void loadNOp(int opcode, I64JITScope& scope);
void loadFNOp(int opcode, I64JITScope& scope);

void compileNop(int opcode, I64JITScope& scope);
//void compileACopyR(int opcode, AMD64JITScope& scope);
void loadFunction(int opcode, I64JITScope& scope);
void compileCallR(int opcode, I64JITScope& scope);
void compilePushA(int opcode, I64JITScope& scope);
void compilePush(int opcode, I64JITScope& scope);
//void compileMCopy(int opcode, AMD64JITScope& scope);
void compileInvokeVMT(int opcode, I64JITScope& scope);
//void compileInvokeVMTOffset(int opcode, AMD64JITScope& scope);
void compileOpen(int opcode, I64JITScope& scope);
void compileQuit(int opcode, I64JITScope& scope);
void compileQuitN(int opcode, I64JITScope& scope);
//void compileBCopyF(int opcode, AMD64JITScope& scope);
//void compileDCopy(int opcode, AMD64JITScope& scope);
void compileACopyS(int opcode, I64JITScope& scope);
void compileACopyF(int opcode, I64JITScope& scope);
//void compileDAddN(int opcode, AMD64JITScope& scope);
void compileBreakpoint(int opcode, I64JITScope& scope);
void compilePopN(int opcode, I64JITScope& scope);
void compileAllocI(int opcode, I64JITScope& scope);
void compileRestore(int op, I64JITScope& scope);
void compileReserve(int op, I64JITScope& scope);
void compileDCopyCount(int, I64JITScope& scope);
void compilePopA(int opcode, I64JITScope& scope);
void compilePopD(int opcode, I64JITScope& scope);
void compileNot(int opcode, I64JITScope& scope);
void compilePushD(int, I64JITScope& scope);
void compileDec(int, I64JITScope& scope);
void compilePushF(int, I64JITScope& scope);
void compilePushFI(int, I64JITScope& scope);
void compilePushSI(int, I64JITScope& scope);

} // _ELENA_

#endif // amd64jitcompilerH

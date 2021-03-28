//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: I64
//                                              (C)2005-2021, by Alexei Rakov
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
   int               objectSize;
   int               frameOffset;

   // byte code command argument
   int            argument;
   int            extra_arg;
   int            extra_arg2;

   void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp);
   //void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp);

   mssg_t resolveMessage(ref_t reference)
   {
      return helper->resolveMessage(reference, module);
   }

//   ref64_t resolveMessage64(ref_t reference)
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
   friend void loadVMTIndexOp(int opcode, I64JITScope& scope);
   friend void loadIndexOp(int opcode, I64JITScope& scope);
   friend void loadIndexNOp(int opcode, I64JITScope& scope);
   friend void loadIndexN4OpX(int opcode, I64JITScope& scope);
   friend void loadIndexN4OpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadNOp(int opcode, I64JITScope& scope);
   friend void loadNOpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadNOpX(int opcode, I64JITScope& scope);
   friend void loadN4OpX(int opcode, I64JITScope& scope);
   friend void loadNNOp(int opcode, I64JITScope& scope);
   friend void loadNNOpX(int opcode, I64JITScope& scope);
   friend void loadNNOpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadFPOp(int opcode, I64JITScope& scope);
   friend void loadSPOp(int opcode, I64JITScope& scope);
   friend void loadFOp(int opcode, I64JITScope& scope);
   friend void loadFN4OpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadFN4OpX(int opcode, I64JITScope& scope);
   friend void loadFPN4Op(int opcode, I64JITScope& scope);
   friend void loadFPN4OpX(int opcode, I64JITScope& scope);
   friend void loadFPN4OpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadFNOp(int opcode, I64JITScope& scope, int arg2);
   friend void loadFNOp(int opcode, I64JITScope& scope);
   friend void loadROp(int opcode, I64JITScope& scope);
   friend void loadMTOp(int opcode, I64JITScope& scope);
   friend void loadMTOpX(int opcode, I64JITScope& scope, int prefix);
   friend void loadFPIndexOp(int opcode, I64JITScope& scope);
   friend void compileALoadR(int opcode, I64JITScope& scope);
   friend void compileASaveR(int opcode, I64JITScope& scope);

   friend void compileNop(int opcode, I64JITScope& scope);
   friend void loadFunction(int opcode, I64JITScope& scope);
   friend void compileCallR(int opcode, I64JITScope& scope);
   friend void compilePushA(int opcode, I64JITScope& scope);
   friend void compilePush(int opcode, I64JITScope& scope);
   friend void compileSelectR(int opcode, I64JITScope& scope);
   friend void compileSetR(int opcode, I64JITScope& scope);
   friend void compileInvokeVMT(int opcode, I64JITScope& scope);
   friend void compileOpen(int opcode, I64JITScope& scope);
   friend void compileSetFrame(int opcode, I64JITScope& scope);
   friend void compileQuit(int opcode, I64JITScope& scope);
   friend void compileQuitN(int opcode, I64JITScope& scope);
   friend void compileACopyS(int opcode, I64JITScope& scope);
   friend void compileACopyF(int opcode, I64JITScope& scope);
   friend void compileDAndN(int opcode, I64JITScope& scope);
   friend void compileBreakpoint(int opcode, I64JITScope& scope);
   friend void compileInvokeVMTOffset(int opcode, I64JITScope& scope);
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
   friend void compileMCopy(int opcode, I64JITScope& scope);
   friend void compileCreate(int opcode, I64JITScope& scope);
   friend void compileMTRedirect(int op, I64JITScope& scope);
   friend void compileLessN(int opcode, I64JITScope& scope);
   friend void compileGreaterN(int opcode, I64JITScope& scope);
   friend void compileIfN(int opcode, I64JITScope& scope);
   friend void compileElseN(int opcode, I64JITScope& scope);
   friend void compileNotGreaterE(int opcode, I64JITScope& scope);
   friend void compileNotLessE(int opcode, I64JITScope& scope);
   friend void compileElseD(int opcode, I64JITScope& scope);
   friend void compileJump(int opcode, I64JITScope& scope);
   friend void compileIfR(int opcode, I64JITScope& scope);
   friend void compileElseR(int opcode, I64JITScope& scope);
   friend void compileHook(int opcode, I64JITScope& scope);
   friend void compileCreateN(int opcode, I64JITScope& scope);
   friend void compileDynamicCreateN(int opcode, I64JITScope& scope);
   friend void compileFill(int opcode, I64JITScope& scope);
   friend void compileIfHeap(int opcode, I64JITScope& scope);
   friend void compileXRedirect(int op, I64JITScope& scope);
   friend void compileJumpN(int opcode, I64JITScope& scope);
   friend void compileMovV(int opcode, I64JITScope& scope);
   friend void compileDShiftN(int opcode, I64JITScope& scope);
   friend void compileIfE(int opcode, I64JITScope& scope);
   friend void compileIfCount(int opcode, I64JITScope& scope);
   friend void compileElseE(int opcode, I64JITScope& scope);
   friend void compileDOrN(int opcode, I64JITScope& scope);
   friend void compileSaveLen(int op, I64JITScope& scope);
   friend void compilePop(int op, I64JITScope& scope);

   // preloaded command set
   void* _inlines[0x100];
   IntFixedMap<void*> _inlineExs;

   // preloaded references
   IntFixedMap<lvaddr_t> _preloaded;

   void writeCoreReference(I64JITScope& scope, ref_t reference, pos_t position, int offset, char* code);

public:
   virtual bool isWithDebugInfo() const;
   virtual pos_t getObjectHeaderSize() const;

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code);

   virtual lvaddr_t getPreloadedReference(ref_t reference);

   virtual void prepareCore(_ReferenceHelper& helper, _JITLoader* loader);

   virtual int allocateTLSVariable(_JITLoader* loader);
   virtual void allocateThreadTable(_JITLoader* loader, int length);
   virtual int allocateVMTape(_JITLoader* loader, void* tape, pos_t length);

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);

//   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section);

   virtual void setStaticRootCounter(_JITLoader* loader, pos_t counter, bool virtualMode);
   virtual void setTLSKey(lvaddr_t ptr);
   virtual void setThreadTable(lvaddr_t ptr);
   virtual void setEHTable(lvaddr_t ptr);
   virtual void setGCTable(lvaddr_t ptr);
   virtual void setVoidParent(_JITLoader* loader, lvaddr_t ptr, bool virtualMode);

   virtual lvaddr_t getInvoker();

   virtual void generateSymbolCall(MemoryDump& tape, lvaddr_t address);
   virtual void generateProgramEnd(MemoryDump& tape);

   I64JITCompiler(bool debugMode, bool bigAddressMode);
};

// --- compiler friend functions---
void loadCoreOp(I64JITScope& scope, char* code);
void loadOneByteLOp(int opcode, I64JITScope& scope);
void loadOneByteLOp(int opcode, I64JITScope& scope);
void loadIndexOp(int opcode, I64JITScope& scope);
void loadIndexNOp(int opcode, I64JITScope& scope);
void loadIndexN4OpX(int opcode, I64JITScope& scope);
void loadIndexN4OpX(int opcode, I64JITScope& scope, int prefix);
void loadVMTIndexOp(int opcode, I64JITScope& scope);
void loadFPOp(int opcode, I64JITScope& scope); 
void loadFN4OpX(int opcode, I64JITScope& scope, int prefix);
void loadFN4OpX(int opcode, I64JITScope& scope);
void loadSPOp(int opcode, I64JITScope& scope);
void loadROp(int opcode, I64JITScope& scope);
void loadFOp(int opcode, I64JITScope& scope);
void loadNOpX(int opcode, I64JITScope& scope); 
void loadNOp(int opcode, I64JITScope& scope);
void loadNNOp(int opcode, I64JITScope& scope); 
void loadNNOpX(int opcode, I64JITScope& scope);
void loadNNOpX(int opcode, I64JITScope& scope, int prefix);
void loadFNOp(int opcode, I64JITScope& scope, int arg2);
void loadFNOp(int opcode, I64JITScope& scope);
void loadFPIndexOp(int opcode, I64JITScope& scope);
void loadFPN4Op(int opcode, I64JITScope& scope);
void loadFPN4OpX(int opcode, I64JITScope& scope);
void loadFPN4OpX(int opcode, I64JITScope& scope, int prefix);
void loadNOpX(int opcode, I64JITScope& scope, int prefix);
void loadN4OpX(int opcode, I64JITScope& scope);

void compileALoadR(int opcode, I64JITScope& scope);
void compileASaveR(int opcode, I64JITScope& scope);
void compileNop(int opcode, I64JITScope& scope);
void loadFunction(int opcode, I64JITScope& scope);
void compileCallR(int opcode, I64JITScope& scope);
void compilePushA(int opcode, I64JITScope& scope);
void compilePush(int opcode, I64JITScope& scope);
void compileSelectR(int opcode, I64JITScope& scope);
void compileSetR(int opcode, I64JITScope& scope);
void compileInvokeVMT(int opcode, I64JITScope& scope);
void compileInvokeVMTOffset(int opcode, I64JITScope& scope);
void compileOpen(int opcode, I64JITScope& scope); 
void compileSetFrame(int opcode, I64JITScope& scope);
void compileQuit(int opcode, I64JITScope& scope);
void compileQuitN(int opcode, I64JITScope& scope);
void compileACopyS(int opcode, I64JITScope& scope);
void compileACopyF(int opcode, I64JITScope& scope);
void compileDAndN(int opcode, I64JITScope& scope); 
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
void compileMCopy(int opcode, I64JITScope& scope);
void compileCreate(int opcode, I64JITScope& scope);
void compileMTRedirect(int op, I64JITScope& scope);
void compileLessN(int opcode, I64JITScope& scope);
void compileGreaterN(int opcode, I64JITScope& scope);
void compileIfN(int opcode, I64JITScope& scope);
void compileElseN(int opcode, I64JITScope& scope);
void compileNotLessE(int opcode, I64JITScope& scope);
void compileNotGreaterE(int opcode, I64JITScope& scope);
void compileElseD(int opcode, I64JITScope& scope);
void compileJump(int opcode, I64JITScope& scope);
void compileIfR(int opcode, I64JITScope& scope);
void compileElseR(int opcode, I64JITScope& scope);
void compileHook(int opcode, I64JITScope& scope);;
void compileCreateN(int opcode, I64JITScope& scope);
void compileDynamicCreateN(int opcode, I64JITScope& scope);
void compileFill(int opcode, I64JITScope& scope);
void compileIfHeap(int opcode, I64JITScope& scope);
void compileXRedirect(int op, I64JITScope& scope);
void compileJumpN(int opcode, I64JITScope& scope);
void compileMovV(int opcode, I64JITScope& scope);
void compileDShiftN(int opcode, I64JITScope& scope);
void compileIfE(int opcode, I64JITScope& scope);
void compileIfCount(int opcode, I64JITScope& scope);
void compileElseE(int opcode, I64JITScope& scope);
void compileDOrN(int opcode, I64JITScope& scope);
void compileSaveLen(int op, I64JITScope& scope);
void compilePop(int op, I64JITScope& scope);

} // _ELENA_

#endif // amd64jitcompilerH

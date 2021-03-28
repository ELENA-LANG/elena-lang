//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86jitcompilerH
#define x86jitcompilerH 1

#include "jitcompiler.h"
#include "x86helper.h"

namespace _ELENA_
{

//#define JIT_TYPE_X 1

class x86JITCompiler;

// --- x86JITScope ---

struct x86JITScope
{
   _Module*          module;
   x86JITCompiler*   compiler;
   MemoryWriter*     code;
   _ReferenceHelper* helper;
   MemoryReader*     tape;
   x86LabelHelper    lh;

   bool              withDebugInfo;
   int               objectSize;
   int               frameOffset;

   // byte code command argument
   int            argument;
   int            extra_arg;
   int            extra_arg2;

   void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp);

   mssg_t resolveMessage(mssg_t reference)
   {
      return helper->resolveMessage(reference, module);
   }

   x86JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler);
};

// --- x86JITCompiler ---

class x86JITCompiler : public JITCompiler32
{
protected:
   bool _debugMode;

   friend void writeCoreReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);
   friend void loadCoreOp(x86JITScope& scope, char* code);

   friend void loadFunction(int opcode, x86JITScope& scope);
//   ////friend void loadExtensions(int opcode, x86JITScope& scope);
//   //friend void loadCode(int opcode, x86JITScope& scope);
   friend void loadOneByteLOp(int opcode, x86JITScope& scope);
   friend void loadOneByteOp(int opcode, x86JITScope& scope);
   friend void loadIndexOp(int opcode, x86JITScope& scope);
   friend void loadVMTIndexOp(int opcode, x86JITScope& scope);
//   //friend void loadVMTMIndexOp(int opcode, x86JITScope& scope);
   friend void loadNOp(int opcode, x86JITScope& scope);
   friend void loadNOpX(int opcode, x86JITScope& scope);
   friend void loadN4OpX(int opcode, x86JITScope& scope);
   friend void loadFNOp(int opcode, x86JITScope& scope);
   friend void loadFNOp(int opcode, x86JITScope& scope, int arg2);
   friend void loadFN4OpX(int opcode, x86JITScope& scope);
   friend void loadFN4OpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadFPN4OpX(int opcode, x86JITScope& scope);
   friend void loadFPN4OpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadFPN4Op(int opcode, x86JITScope& scope);
   friend void loadFPIndexOp(int opcode, x86JITScope& scope);
   friend void loadFPIndexOpX(int opcode, x86JITScope& scope);
   friend void loadFPIndexOpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadIndexNOp(int opcode, x86JITScope& scope);
   friend void loadIndexN4OpX(int opcode, x86JITScope& scope);
   friend void loadIndexN4OpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadNNOp(int opcode, x86JITScope& scope);
   friend void loadNNOpX(int opcode, x86JITScope& scope);
   friend void loadNNOpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadSPOp(int opcode, x86JITScope& scope);
   friend void loadFPOp(int opcode, x86JITScope& scope);
   friend void loadFOp(int opcode, x86JITScope& scope);
   friend void loadROp(int opcode, x86JITScope& scope);
   friend void loadMTOp(int opcode, x86JITScope& scope);
   friend void loadMTOpX(int opcode, x86JITScope& scope, int prefix);
   friend void loadNOpX(int opcode, x86JITScope& scope, int prefix);

   // commands
   friend void compileNop(int opcode, x86JITScope& scope);
   friend void compileBreakpoint(int opcode, x86JITScope& scope);
   friend void compilePopA(int opcode, x86JITScope& scope);
//   friend void compilePopE(int opcode, x86JITScope& scope);
   friend void compilePopD(int opcode, x86JITScope& scope);
   friend void compilePopN(int opcode, x86JITScope& scope);
   friend void compileAllocI(int opcode, x86JITScope& scope);
   friend void compileOpen(int opcode, x86JITScope& scope);
   friend void compileSetFrame(int opcode, x86JITScope& scope);
   friend void compilePush(int opcode, x86JITScope& scope);
   friend void compilePushFI(int opcode, x86JITScope& scope);
   friend void compilePushF(int opcode, x86JITScope& scope);
   friend void compilePushSI(int opcode, x86JITScope& scope);
   friend void compilePushD(int opcode, x86JITScope& scope);
   friend void compileCallR(int opcode, x86JITScope& scope);
   friend void compileJumpN(int opcode, x86JITScope& scope);
   friend void compileMCopy(int opcode, x86JITScope& scope);
   friend void compileASaveR(int opcode, x86JITScope& scope);
   friend void compileJump(int opcode, x86JITScope& scope);
   friend void compileHook(int opcode, x86JITScope& scope);
   friend void compileCreate(int opcode, x86JITScope& scope);
   friend void compileFill(int opcode, x86JITScope& scope);
   friend void compileCreateN(int opcode, x86JITScope& scope);
   friend void compileDynamicCreateN(int opcode, x86JITScope& scope);
//   friend void compileCreateI(int opcode, x86JITScope& scope);
   friend void compileSelectR(int opcode, x86JITScope& scope);
   friend void compileSetR(int opcode, x86JITScope& scope);
//   friend void compileBCopyR(int opcode, x86JITScope& scope);
   friend void compileALoadR(int opcode, x86JITScope& scope);
//   friend void compileBLoadR(int opcode, x86JITScope& scope);
   friend void compilePushA(int opcode, x86JITScope& scope);
//   friend void compileACopyB(int opcode, x86JITScope& scope);
   friend void compileInvokeVMT(int opcode, x86JITScope& scope);
   friend void compileInvokeVMTOffset(int opcode, x86JITScope& scope);
//   friend void compileALoadAI(int opcode, x86JITScope& scope);
   friend void compileACopyS(int opcode, x86JITScope& scope);
//   friend void compileNext(int opcode, x86JITScope& scope);
   friend void compileIfE(int opcode, x86JITScope& scope);
   friend void compileIfCount(int opcode, x86JITScope& scope);
   friend void compileElseE(int opcode, x86JITScope& scope);
//   friend void compileLessE(int opcode, x86JITScope& scope);
   friend void compileNotLessE(int opcode, x86JITScope& scope);
   friend void compileNotGreaterE(int opcode, x86JITScope& scope);
   //   friend void compileIfB(int opcode, x86JITScope& scope);
   friend void compileElseD(int opcode, x86JITScope& scope);
   friend void compileIfR(int opcode, x86JITScope& scope);
   friend void compileElseR(int opcode, x86JITScope& scope);
   friend void compileIfN(int opcode, x86JITScope& scope);
   friend void compileElseN(int opcode, x86JITScope& scope);
   friend void compileLessN(int opcode, x86JITScope& scope);
   friend void compileGreaterN(int opcode, x86JITScope& scope);
   friend void compileIfHeap(int opcode, x86JITScope& scope);
   friend void compileQuit(int opcode, x86JITScope& scope);
   friend void compileQuitN(int opcode, x86JITScope& scope);
   friend void compileMovV(int opcode, x86JITScope& scope);
   friend void compileACopyF(int opcode, x86JITScope& scope);
   friend void compileDAndN(int opcode, x86JITScope& scope);
   friend void compileDOrN(int opcode, x86JITScope& scope);
   friend void compileDec(int opcode, x86JITScope& scope);
   friend void compileDCopyCount(int opcode, x86JITScope& scope);
   friend void compileDShiftN(int opcode, x86JITScope& scope);
   friend void compileNot(int opcode, x86JITScope& scope);
   friend void compileMTRedirect(int op, x86JITScope& scope);
   friend void compileXRedirect(int op, x86JITScope& scope);
   friend void compileReserve(int op, x86JITScope& scope);
   friend void compileRestore(int op, x86JITScope& scope);
   friend void compileSaveLen(int op, x86JITScope& scope);
   friend void compilePop(int op, x86JITScope& scope);

   // preloaded command set
   void* _inlines[0x100];
   IntFixedMap<void*> _inlineExs;

   // preloaded references
   IntFixedMap<lvaddr_t> _preloaded;

   void writeCoreReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);

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
//   virtual void generateArg(MemoryDump& tape, void* address);
//   virtual void generateExternalCall(MemoryDump& tape, ref_t functionReference);

   x86JITCompiler(bool debugMode);
};

// --- compiler friend functions---
inline void writeCoreReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);
void loadCoreOp(x86JITScope& scope, char* code);
void loadFunction(int opcode, x86JITScope& scope);
void loadOneByteLOp(int opcode, x86JITScope& scope);
void loadOneByteOp(int opcode, x86JITScope& scope);
void loadIndexOp(int opcode, x86JITScope& scope);
void loadVMTIndexOp(int opcode, x86JITScope& scope);
void loadNOp(int opcode, x86JITScope& scope);
void loadNOpX(int opcode, x86JITScope& scope);
void loadN4OpX(int opcode, x86JITScope& scope);
void loadIndexNOp(int opcode, x86JITScope& scope);
void loadIndexN4OpX(int opcode, x86JITScope& scope);
void loadIndexN4OpX(int opcode, x86JITScope& scope, int prefix);
void loadNNOp(int opcode, x86JITScope& scope);
void loadNNOpX(int opcode, x86JITScope& scope);
void loadNNOpX(int opcode, x86JITScope& scope, int prefix);
void loadFNOp(int opcode, x86JITScope& scope);
void loadFNOp(int opcode, x86JITScope& scope, int arg2);
void loadFN4OpX(int opcode, x86JITScope& scope);
void loadFN4OpX(int opcode, x86JITScope& scope, int prefix);
void loadFPN4OpX(int opcode, x86JITScope& scope);
void loadFPN4OpX(int opcode, x86JITScope& scope, int prefix);
void loadFPN4Op(int opcode, x86JITScope& scope);
void loadFPIndexOp(int opcode, x86JITScope& scope);
void loadFPIndexOpX(int opcode, x86JITScope& scope);
void loadFPIndexOpX(int opcode, x86JITScope& scope, int prefix);
void loadSPOp(int opcode, x86JITScope& scope);
void loadFPOp(int opcode, x86JITScope& scope);
void loadFOp(int opcode, x86JITScope& scope);
void loadROp(int opcode, x86JITScope& scope);
void loadMTOp(int opcode, x86JITScope& scope);
void loadMTOpX(int opcode, x86JITScope& scope, int prefix);
void loadNOpX(int opcode, x86JITScope& scope, int prefix);
void compileNop(int opcode, x86JITScope& scope);
void compileBreakpoint(int opcode, x86JITScope& scope);
//void compilePop(int opcode, x86JITScope& scope);
void compilePopA(int opcode, x86JITScope& scope);
void compilePopN(int opcode, x86JITScope& scope);
void compileAllocI(int opcode, x86JITScope& scope);
void compileOpen(int opcode, x86JITScope& scope);
void compileSetFrame(int opcode, x86JITScope& scope);
void compilePush(int opcode, x86JITScope& scope);
void compilePushF(int opcode, x86JITScope& scope);
void compilePushFI(int opcode, x86JITScope& scope);
void compilePushSI(int opcode, x86JITScope& scope);
//void compilePushB(int opcode, x86JITScope& scope);
//void compilePushE(int opcode, x86JITScope& scope);
void compilePushD(int opcode, x86JITScope& scope);
void compileCallR(int opcode, x86JITScope& scope);
void compileJumpN(int opcode, x86JITScope& scope);
void compileMCopy(int opcode, x86JITScope& scope);
void compileASaveR(int opcode, x86JITScope& scope);
//void compileSCopyF(int opcode, x86JITScope& scope);
void compileJump(int opcode, x86JITScope& scope);
void compileHook(int opcode, x86JITScope& scope);
void compileCreate(int opcode, x86JITScope& scope);
void compileFill(int opcode, x86JITScope& scope);
//void compileCreateI(int opcode, x86JITScope& scope);
void compileCreateN(int opcode, x86JITScope& scope);
void compileDynamicCreateN(int opcode, x86JITScope& scope);
void compileSelectR(int opcode, x86JITScope& scope);
void compileSetR(int opcode, x86JITScope& scope);
//void compileBCopyR(int opcode, x86JITScope& scope);
void compileALoadR(int opcode, x86JITScope& scope);
//void compileBLoadR(int opcode, x86JITScope& scope);
void compilePushA(int opcode, x86JITScope& scope);
//void compileACopyB(int opcode, x86JITScope& scope);
void compileInvokeVMT(int opcode, x86JITScope& scope);
void compileInvokeVMTOffset(int opcode, x86JITScope& scope);
//void compileIfM(int opcode, x86JITScope& scope);
//void compileElseM(int opcode, x86JITScope& scope);
//void compileALoadAI(int opcode, x86JITScope& scope);
void compileACopyS(int opcode, x86JITScope& scope);
//void compileNext(int opcode, x86JITScope& scope);
void compileIfE(int opcode, x86JITScope& scope);
void compileIfCount(int opcode, x86JITScope& scope);
void compileElseE(int opcode, x86JITScope& scope);
//void compileLessE(int opcode, x86JITScope& scope);
void compileNotLessE(int opcode, x86JITScope& scope);
void compileNotGreaterE(int opcode, x86JITScope& scope);
//void compileIfB(int opcode, x86JITScope& scope);
void compileElseD(int opcode, x86JITScope& scope);
void compileIfR(int opcode, x86JITScope& scope);
void compileElseR(int opcode, x86JITScope& scope);
void compileIfN(int opcode, x86JITScope& scope);
void compileElseN(int opcode, x86JITScope& scope);
void compileLessN(int opcode, x86JITScope& scope);
void compileGreaterN(int opcode, x86JITScope& scope);
void compileIfHeap(int opcode, x86JITScope& scope);
void compileQuit(int opcode, x86JITScope& scope);
void compileQuitN(int opcode, x86JITScope& scope);
//void compilePopE(int opcode, x86JITScope& scope);
void compilePopD(int opcode, x86JITScope& scope);
void compileMovV(int opcode, x86JITScope& scope);
//void compileSetSubj(int opcode, x86JITScope& scope);
//void compilePopB(int opcode, x86JITScope& scope);
//void compileECopyD(int opcode, x86JITScope& scope);
//void compileDCopyE(int opcode, x86JITScope& scope);
//void compileBCopyF(int opcode, x86JITScope& scope);
//void compileBCopyS(int opcode, x86JITScope& scope);
void compileACopyF(int opcode, x86JITScope& scope);
//void compileDCopy(int opcode, x86JITScope& scope);
//void compileECopy(int opcode, x86JITScope& scope);
void compileDAndN(int opcode, x86JITScope& scope);
void compileDOrN(int opcode, x86JITScope& scope);
//void compileDAddN(int opcode, x86JITScope& scope);
//void compileDMulN(int opcode, x86JITScope& scope);
void compileDec(int opcode, x86JITScope& scope);
void compileDCopyCount(int opcode, x86JITScope& scope);
//void compileIndexInc(int opcode, x86JITScope& scope);
//void compileIndexDec(int opcode, x86JITScope& scope);
void compileDShiftN(int opcode, x86JITScope& scope);
//void compileDAdd(int opcode, x86JITScope& scope);
//void compileDSub(int opcode, x86JITScope& scope);
//void compileOr(int opcode, x86JITScope& scope);
////void compileLoad(int opcode, x86JITScope& scope);
void compileNot(int opcode, x86JITScope& scope);
//void compileInit(int opcode, x86JITScope& scope);
void compileMTRedirect(int op, x86JITScope& scope);
void compileXRedirect(int op, x86JITScope& scope);
void compileReserve(int op, x86JITScope& scope);
void compileRestore(int op, x86JITScope& scope);
void compileSaveLen(int op, x86JITScope& scope);
void compilePop(int op, x86JITScope& scope);

} // _ELENA_

#endif // x86jitcompilerH

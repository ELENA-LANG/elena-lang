//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86jitcompilerH
#define x86jitcompilerH 1

#include "jitcompiler.h"
#include "x86helper.h"

namespace _ELENA_
{

#define JIT_TYPE_X 1

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

   bool              embeddedSymbols;
   bool              debugMode;
   int               objectSize;

   // byte code command argument
   int            argument;

   // offset to the previous stack frame
   size_t         prevFSPOffs;

   void writeReference(MemoryWriter& writer, ref_t reference, size_t disp)
   {
      helper->writeReference(writer, reference, disp, module);
   }

   ref_t resolveMessage(ref_t reference)
   {
      return helper->resolveMessage(reference, module);
   }

   SectionInfo getSection(ref_t reference)
   {
      return helper->getSection(reference, module);
   }

   x86JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler, bool embeddedSymbols);
};

// --- x86JITCompiler ---

class x86JITCompiler : public JITCompiler32
{
protected:
   bool _debugMode;
   bool _embeddedSymbolMode;

   friend void writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);
   friend void loadCoreOp(x86JITScope& scope, char* code);

   friend void loadFunction(int opcode, x86JITScope& scope);
   friend void loadExtensions(int opcode, x86JITScope& scope);
   friend void loadCode(int opcode, x86JITScope& scope);
   friend void loadOneByteLOp(int opcode, x86JITScope& scope);
   friend void loadOneByteOp(int opcode, x86JITScope& scope);
   friend void loadIndexOp(int opcode, x86JITScope& scope);
   friend void loadVMTIndexOp(int opcode, x86JITScope& scope);
   friend void loadNOp(int opcode, x86JITScope& scope);
   friend void loadFPOp(int opcode, x86JITScope& scope);
   friend void loadROp(int opcode, x86JITScope& scope);

   // commands
   friend void compileNop(int opcode, x86JITScope& scope);
   friend void compileBreakpoint(int opcode, x86JITScope& scope);
   friend void compileAElse(int opcode, x86JITScope& scope);
   friend void compileAThen(int opcode, x86JITScope& scope);
   friend void compileTest(int opcode, x86JITScope& scope);
   friend void compileDElse(int opcode, x86JITScope& scope);
   friend void compileDThen(int opcode, x86JITScope& scope);
   friend void compileMElse(int opcode, x86JITScope& scope);
   friend void compileMThen(int opcode, x86JITScope& scope);
   //friend void compileMElseSI(int opcode, x86JITScope& scope);
   //friend void compileMThenSI(int opcode, x86JITScope& scope);
   friend void compileMElseAccI(int opcode, x86JITScope& scope);
   friend void compileMThenAccI(int opcode, x86JITScope& scope);
   friend void compilePop(int opcode, x86JITScope& scope);
   friend void compilePopAcc(int opcode, x86JITScope& scope);
   friend void compileMccPop(int opcode, x86JITScope& scope);
   friend void compilePopN(int opcode, x86JITScope& scope);
   friend void compileOpen(int opcode, x86JITScope& scope);
   friend void compilePush(int opcode, x86JITScope& scope);
   friend void compileMccPush(int opcode, x86JITScope& scope);
   friend void compilePushF(int opcode, x86JITScope& scope);
   friend void compileXPushFPI(int opcode, x86JITScope& scope);
   friend void compilePushFPI(int opcode, x86JITScope& scope);
   friend void compilePushS(int opcode, x86JITScope& scope);
   friend void compilePushSelf(int opcode, x86JITScope& scope);
   friend void compileLoadField(int opcode, x86JITScope& scope);
   friend void compileCallR(int opcode, x86JITScope& scope);
   //friend void compileSendVMTR(int opcode, x86JITScope& scope);
   friend void compileMSet(int opcode, x86JITScope& scope);
   //friend void compileXMSet(int opcode, x86JITScope& scope);
   friend void compileMAdd(int opcode, x86JITScope& scope);
   friend void compileAAdd(int opcode, x86JITScope& scope);
   friend void compileAMul(int opcode, x86JITScope& scope);
   friend void compileAccSaveR(int opcode, x86JITScope& scope);
   friend void compilePopFI(int opcode, x86JITScope& scope);
   friend void compilePopSI(int opcode, x86JITScope& scope);
   friend void compileSPSetF(int opcode, x86JITScope& scope);
   friend void compileJump(int opcode, x86JITScope& scope);
   friend void compileHook(int opcode, x86JITScope& scope);
   friend void compileCreate(int opcode, x86JITScope& scope);
   friend void compileCreateN(int opcode, x86JITScope& scope);
   friend void compileBoxN(int opcode, x86JITScope& scope);
   //friend void compileAccCreate(int opcode, x86JITScope& scope);
   friend void compileAccSetR(int opcode, x86JITScope& scope);
   friend void compileAccSetN(int opcode, x86JITScope& scope);
   friend void compileAccLoadR(int opcode, x86JITScope& scope);
   friend void compilePushAcc(int opcode, x86JITScope& scope);
   friend void compileAccLoadSelf(int opcode, x86JITScope& scope);
   friend void compileInvokeVMT(int opcode, x86JITScope& scope);
   //friend void compileCallVMT(int opcode, x86JITScope& scope);
   friend void compileIAccCopyR(int opcode, x86JITScope& scope);
   //friend void compileIAccCopyN(int opcode, x86JITScope& scope);
   //friend void compileIAccFillR(int opcode, x86JITScope& scope);
   friend void compileAccCopyAccI(int opcode, x86JITScope& scope);
   friend void compileAccCopySPI(int opcode, x86JITScope& scope);
   //friend void compileRedirect(int opcode, x86JITScope& scope);
   //friend void compileJumpR(int opcode, x86JITScope& scope);
   friend void compileElseR(int opcode, x86JITScope& scope);
   friend void compileNext(int opcode, x86JITScope& scope);
   friend void compileThenR(int opcode, x86JITScope& scope);
   friend void compileReserve(int opcode, x86JITScope& scope);
   friend void compileElseN(int opcode, x86JITScope& scope);
   friend void compileThenN(int opcode, x86JITScope& scope);
   friend void compileElseSI(int opcode, x86JITScope& scope);
   friend void compileThenSI(int opcode, x86JITScope& scope);
   friend void compileElseFlag(int opcode, x86JITScope& scope);
   friend void compileThenFlag(int opcode, x86JITScope& scope);
   //friend void compileMccElseAcc(int opcode, x86JITScope& scope);
   //friend void compileMccThenAcc(int opcode, x86JITScope& scope);
   //friend void compileAccInc(int opcode, x86JITScope& scope);
   friend void compileIndexInc(int opcode, x86JITScope& scope);
   friend void compileIndexDec(int opcode, x86JITScope& scope);
   //friend void compileAccCopyM(int opcode, x86JITScope& scope);
   friend void compileQuit(int opcode, x86JITScope& scope);
   friend void compileQuitN(int opcode, x86JITScope& scope);
   friend void compileMccCopyVerb(int opcode, x86JITScope& scope);
   friend void compileMccCopySubj(int opcode, x86JITScope& scope);
   //friend void compileElseLocal(int opcode, x86JITScope& scope);
   //friend void compileMccCopyAcc(int opcode, x86JITScope& scope);
   friend void compilePopSelf(int opcode, x86JITScope& scope);
   //friend void compileNWrite(int opcode, x86JITScope& scope);
   //friend void compileWriteAcc(int opcode, x86JITScope& scope);
   friend void compileCallSI(int opcode, x86JITScope& scope);
   //friend void compileGetLen(int opcode, x86JITScope& scope);
   //friend void compileXAccSaveFI(int opcode, x86JITScope& scope);
   friend void compileAccCopyFPI(int opcode, x86JITScope& scope);
   friend void compileXAccCopyFPI(int opcode, x86JITScope& scope);
   friend void compileDCopyI(int opcode, x86JITScope& scope);
   friend void compileDCopyAI(int opcode, x86JITScope& scope);
   friend void compileDAddAI(int opcode, x86JITScope& scope);
   friend void compileDSubAI(int opcode, x86JITScope& scope);
   friend void compileDSaveAI(int opcode, x86JITScope& scope);
   friend void compileALoadD(int opcode, x86JITScope& scope);

   // preloaded command set
   void* _inlines[0x100];
   void* _extensions[0x8][0x800];

   // preloaded references
   IntFixedMap<void*> _preloaded;

   void writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);

public:
   bool isDebugMode() const { return _debugMode; }

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code);

   virtual void* getPreloadedReference(ref_t reference);

   virtual void prepareCoreData(_ReferenceHelper& helper, _Memory* data, _Memory* rdata, _Memory* sdata);
   virtual void prepareCore(_ReferenceHelper& helper, _Memory* code);
   virtual void prepareCommandSet(_ReferenceHelper& helper, _Memory* code);
   virtual void prepareVMData(_ReferenceHelper& helper, _Memory* data);

//   virtual void compileTLS(_JITLoader* loader);

   virtual void embedSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter, _Module* module);
   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);

   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section);

   virtual void setStaticRootCounter(_JITLoader* loader, int counter, bool virtualMode);

   x86JITCompiler(bool debugMode, bool embeddedSymbolMode);
};

// --- compiler friend functions---
inline void writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code);
void loadCoreOp(x86JITScope& scope, char* code);
void loadFunction(int opcode, x86JITScope& scope);
void loadExtensions(int opcode, x86JITScope& scope);
void loadCode(int opcode, x86JITScope& scope);
void loadOneByteLOp(int opcode, x86JITScope& scope);
void loadOneByteOp(int opcode, x86JITScope& scope);
void loadIndexOp(int opcode, x86JITScope& scope);
void loadVMTIndexOp(int opcode, x86JITScope& scope);
void loadNOp(int opcode, x86JITScope& scope);
void loadFPOp(int opcode, x86JITScope& scope);
void loadROp(int opcode, x86JITScope& scope);
void compileNop(int opcode, x86JITScope& scope);
void compileBreakpoint(int opcode, x86JITScope& scope);
void compileAElse(int opcode, x86JITScope& scope);
void compileAThen(int opcode, x86JITScope& scope);
void compileTest(int opcode, x86JITScope& scope);
void compileDElse(int opcode, x86JITScope& scope);
void compileDThen(int opcode, x86JITScope& scope);
void compilePop(int opcode, x86JITScope& scope);
void compilePopAcc(int opcode, x86JITScope& scope);
void compilePopN(int opcode, x86JITScope& scope);
void compileOpen(int opcode, x86JITScope& scope);
void compilePush(int opcode, x86JITScope& scope);
void compilePushF(int opcode, x86JITScope& scope);
void compileXPushFPI(int opcode, x86JITScope& scope);
void compilePushFPI(int opcode, x86JITScope& scope);
void compilePushS(int opcode, x86JITScope& scope);
void compilePushSelf(int opcode, x86JITScope& scope);
void compileLoadField(int opcode, x86JITScope& scope);
void compileCallR(int opcode, x86JITScope& scope);
//void compileSendVMTR(int opcode, x86JITScope& scope);
void compileMSet(int opcode, x86JITScope& scope);
//void compileXMSet(int opcode, x86JITScope& scope);
void compileAccSaveR(int opcode, x86JITScope& scope);
void compilePopFI(int opcode, x86JITScope& scope);
void compilePopSI(int opcode, x86JITScope& scope);
void compileSPSetF(int opcode, x86JITScope& scope);
void compileJump(int opcode, x86JITScope& scope);
void compileHook(int opcode, x86JITScope& scope);
////void compileAccLoadField(int opcode, x86JITScope& scope);
void compileCreate(int opcode, x86JITScope& scope);
void compileCreateN(int opcode, x86JITScope& scope);
void compileBoxN(int opcode, x86JITScope& scope);
//void compileAccCreate(int opcode, x86JITScope& scope);
void compileAccSetR(int opcode, x86JITScope& scope);
void compileAccSetN(int opcode, x86JITScope& scope);
////void compileAccSet(int opcode, x86JITScope& scope);
void compileAccLoadR(int opcode, x86JITScope& scope);
void compilePushAcc(int opcode, x86JITScope& scope);
void compileAccLoadSelf(int opcode, x86JITScope& scope);
void compileInvokeVMT(int opcode, x86JITScope& scope);
//void compileCallVMT(int opcode, x86JITScope& scope);
void compileMAdd(int opcode, x86JITScope& scope);
void compileAAdd(int opcode, x86JITScope& scope);
void compileAMul(int opcode, x86JITScope& scope);
void compileMElse(int opcode, x86JITScope& scope);
void compileMThen(int opcode, x86JITScope& scope);
//void compileMElseSI(int opcode, x86JITScope& scope);
//void compileMThenSI(int opcode, x86JITScope& scope);
//void compileMVerbElseSI(int opcode, x86JITScope& scope);
//void compileMVerbThenSI(int opcode, x86JITScope& scope);
void compileIAccCopyR(int opcode, x86JITScope& scope);
//void compileIAccCopyN(int opcode, x86JITScope& scope);
//void compileIAccFillR(int opcode, x86JITScope& scope);
void compileAccCopyAccI(int opcode, x86JITScope& scope);
void compileAccCopySPI(int opcode, x86JITScope& scope);
//void compileRedirect(int opcode, x86JITScope& scope);
//void compileJumpR(int opcode, x86JITScope& scope);
void compileElseR(int opcode, x86JITScope& scope);
void compileNext(int opcode, x86JITScope& scope);
void compileThenR(int opcode, x86JITScope& scope);
void compileElseN(int opcode, x86JITScope& scope);
void compileThenN(int opcode, x86JITScope& scope);
void compileReserve(int opcode, x86JITScope& scope);
//void compileMElseSubj(int opcode, x86JITScope& scope);
//void compileMThenSubj(int opcode, x86JITScope& scope);
//void compileAccInc(int opcode, x86JITScope& scope);
void compileIndexInc(int opcode, x86JITScope& scope);
void compileIndexDec(int opcode, x86JITScope& scope);
//void compileAccCopyM(int opcode, x86JITScope& scope);
void compileElseSI(int opcode, x86JITScope& scope);
void compileThenSI(int opcode, x86JITScope& scope);
//void compileMccElseAcc(int opcode, x86JITScope& scope);
//void compileMccThenAcc(int opcode, x86JITScope& scope);
void compileElseFlag(int opcode, x86JITScope& scope);
void compileThenFlag(int opcode, x86JITScope& scope);
void compileQuit(int opcode, x86JITScope& scope);
void compileQuitN(int opcode, x86JITScope& scope);
void compileMccPush(int opcode, x86JITScope& scope);
void compileMccPop(int opcode, x86JITScope& scope);
void compileMccCopyVerb(int opcode, x86JITScope& scope);
void compileMccCopySubj(int opcode, x86JITScope& scope);
//void compileElseLocal(int opcode, x86JITScope& scope);
//void compileMccCopyAcc(int opcode, x86JITScope& scope);
void compilePopSelf(int opcode, x86JITScope& scope);
//void compileNWrite(int opcode, x86JITScope& scope);
//void compileWriteAcc(int opcode, x86JITScope& scope);
void compileCallSI(int opcode, x86JITScope& scope);
//void compileGetLen(int opcode, x86JITScope& scope);
//void compileXAccSaveFI(int opcode, x86JITScope& scope);
void compileXAccCopyFPI(int opcode, x86JITScope& scope);
void compileAccCopyFPI(int opcode, x86JITScope& scope);
void compileMElseAccI(int opcode, x86JITScope& scope);
void compileMThenAccI(int opcode, x86JITScope& scope);
void compileDCopyI(int opcode, x86JITScope& scope);
void compileDCopyAI(int opcode, x86JITScope& scope);
void compileDAddAI(int opcode, x86JITScope& scope);
void compileDSubAI(int opcode, x86JITScope& scope);
void compileDSaveAI(int opcode, x86JITScope& scope);
void compileALoadD(int opcode, x86JITScope& scope);
void loadFunction(int opcode, x86JITScope& scope);

} // _ELENA_

#endif // x86jitcompilerH

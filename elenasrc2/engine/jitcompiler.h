//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class.
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jitcompilerH
#define jitcompilerH 1

namespace _ELENA_
{

// --- ReferenceHelper ---

class _ReferenceHelper
{
public:
   virtual ref_t getLinkerConstant(ref_t constant) = 0;
   virtual SectionInfo getCoreSection(ref_t reference) = 0;
   virtual SectionInfo getSection(ref_t reference, _Module* module = NULL) = 0;

   virtual void* getVAddress(MemoryWriter& writer, int mask) = 0;

   virtual ref_t resolveMessage(ref_t reference, _Module* module = NULL) = 0;

   virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module = NULL) = 0;
   virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp) = 0;

   // used for 64bit programming, currently only for mskVMTXMethodAddress and mskVMTXEntryOffset
   virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module = NULL) = 0;

   virtual void addBreakpoint(size_t position) = 0;
};

// --- _BinaryHelper ---

class _BinaryHelper
{
public:
   virtual void writeReference(MemoryWriter& writer, ident_t reference, int mask) = 0;
};

// --- JITCompiler class ---
class _JITCompiler
{
public:
   virtual size_t getObjectHeaderSize() const = 0;

   virtual bool isWithDebugInfo() const = 0;

   virtual void prepareCore(_ReferenceHelper& helper, _JITLoader* loader) = 0;

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code) = 0;

   virtual void compileInt32(MemoryWriter* writer, int integer) = 0;
   virtual void compileInt64(MemoryWriter* writer, long long integer) = 0;
   virtual void compileInt64(MemoryWriter* writer, int low, ref_t ref, int refOffset) = 0;
   virtual void compileInt64(MemoryWriter* writer, int low, int high) = 0;
   virtual void compileReal64(MemoryWriter* writer, double number) = 0;
   virtual void compileLiteral(MemoryWriter* writer, const char* value) = 0;
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value) = 0;
   virtual void compileChar32(MemoryWriter* writer, const char* value) = 0;
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary) = 0;
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary) = 0;

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter) = 0;

   virtual void allocateVariable(MemoryWriter& writer) = 0;
   virtual void allocateArray(MemoryWriter& writer, size_t count) = 0;

   virtual int allocateTLSVariable(_JITLoader* loader) = 0;
   virtual void allocateThreadTable(_JITLoader* loader, int length) = 0;

   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset) = 0;
   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength) = 0;

   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries) = 0;

   virtual pos_t findMethodAddress(void* refVMT, ref_t messageID, size_t vmtLength) = 0;
   virtual int findMethodIndex(void* refVMT, ref_t messageID, size_t vmtLength) = 0;
   virtual size_t findFlags(void* refVMT) = 0;
   virtual size_t findLength(void* refVMT) = 0;

   virtual void addVMTEntry(ref_t message, size_t codePosition, VMTEntry* entries, size_t& count) = 0;

   virtual void fixVMT(MemoryWriter& vmtWriter, pos_t classClassVAddress, pos_t packageVAddress, int count, bool virtualMode) = 0;

   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section) = 0;

   virtual void* getPreloadedReference(ref_t reference) = 0;

   virtual void setStaticRootCounter(_JITLoader* loader, size_t counter, bool virtualMode) = 0;

   virtual void generateProgramStart(MemoryDump& tape) = 0;
   virtual void generateSymbolCall(MemoryDump& tape, void* address) = 0;
   virtual void generateProgramEnd(MemoryDump& tape) = 0;
};

// --- JITCompiler32 class ---
class JITCompiler32 : public _JITCompiler
{
public:
   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileInt64(MemoryWriter* writer, int low, ref_t ref, int refOffset);
   virtual void compileInt64(MemoryWriter* writer, int low, int high);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileLiteral(MemoryWriter* writer, const char* value);
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value);
   virtual void compileChar32(MemoryWriter* writer, const char* value);
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary);
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual size_t findFlags(void* refVMT);
   virtual size_t findLength(void* refVMT);
   virtual pos_t findMethodAddress(void* refVMT, ref_t messageID, size_t vmtLength);
   virtual int findMethodIndex(void* refVMT, ref_t messageID, size_t vmtLength);

   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength);
   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual void addVMTEntry(ref_t message, size_t codePosition, VMTEntry* entries, size_t& count);
   virtual void fixVMT(MemoryWriter& vmtWriter, pos_t classClassVAddress, pos_t packageVAddress, int count, bool virtualMode);

   virtual void generateProgramStart(MemoryDump& tape);
   virtual void generateProgramEnd(MemoryDump& tape);
};

// --- JITCompiler64 class ---
class JITCompiler64 : public _JITCompiler
{
public:
   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileInt64(MemoryWriter* writer, int low, ref_t ref, int refOffset);
   virtual void compileInt64(MemoryWriter* writer, int low, int high);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileLiteral(MemoryWriter* writer, const char* value);
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value);
   virtual void compileChar32(MemoryWriter* writer, const char* value);
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary);
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual size_t findFlags(void* refVMT);
   virtual size_t findLength(void* refVMT);
   virtual pos_t findMethodAddress(void* refVMT, ref_t messageID, size_t vmtLength);
   virtual int findMethodIndex(void* refVMT, ref_t messageID, size_t vmtLength);

   virtual ref64_t findMethodAddressX(void* refVMT, ref64_t messageID, size_t vmtLength);
   virtual int findMethodIndexX(void* refVMT, ref64_t messageID, size_t vmtLength);

   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength);
   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual int copyParentVMTX(void* parentVMT, VMTXEntry* entries);
   virtual void addVMTEntry(ref_t message, size_t codePosition, VMTEntry* entries, size_t& count);
   virtual void addVMTXEntry(ref64_t message, size_t codePosition, VMTXEntry* entries, size_t& entryCount);
   virtual void fixVMT(MemoryWriter& vmtWriter, pos_t classClassVAddress, pos_t packageVAddress, int count, bool virtualMode);

   virtual void generateProgramStart(MemoryDump& tape);
   virtual void generateProgramEnd(MemoryDump& tape);
};

} // _ELENA_

#endif // jitcompilerH

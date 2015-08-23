//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class.
//
//                                              (C)2005-2015, by Alexei Rakov
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
   virtual void compileReal64(MemoryWriter* writer, double number) = 0;
   virtual void compileLiteral(MemoryWriter* writer, const char* value) = 0;
   virtual void compileChar32(MemoryWriter* writer, const char* value) = 0;
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary) = 0;

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter) = 0;

   virtual void allocateVariable(MemoryWriter& writer) = 0;
   virtual void allocateArray(MemoryWriter& writer, size_t count) = 0;

   virtual int allocateTLSVariable(_JITLoader* loader) = 0;
   virtual void allocateThreadTable(_JITLoader* loader, int length) = 0;

   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset) = 0;
   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength) = 0;

   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries) = 0;

   virtual int findMethodAddress(void* refVMT, ref_t messageID, size_t vmtLength) = 0;
   virtual int findMethodIndex(void* refVMT, ref_t messageID, size_t vmtLength) = 0;
   virtual size_t findFlags(void* refVMT) = 0;
   virtual size_t findLength(void* refVMT) = 0;

   virtual void addVMTEntry(_ReferenceHelper& helper, ref_t message, size_t codePosition, VMTEntry* entries, size_t& count) = 0;

   virtual void fixVMT(MemoryWriter& vmtWriter, void* classClassVAddress, int count, bool virtualMode) = 0;

   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section) = 0;

   virtual void* getPreloadedReference(ref_t reference) = 0;

   virtual void setStaticRootCounter(_JITLoader* loader, size_t counter, bool virtualMode) = 0;
};

// --- JITCompiler32 class ---
class JITCompiler32 : public _JITCompiler
{
public:
   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileLiteral(MemoryWriter* writer, const char* value);
   virtual void compileChar32(MemoryWriter* writer, const char* value);
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual size_t findFlags(void* refVMT);
   virtual size_t findLength(void* refVMT);
   virtual int findMethodAddress(void* refVMT, ref_t messageID, size_t vmtLength);
   virtual int findMethodIndex(void* refVMT, ref_t messageID, size_t vmtLength);

   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength);
   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual void addVMTEntry(_ReferenceHelper& helper, ref_t message, size_t codePosition, VMTEntry* entries, size_t& count);
   virtual void fixVMT(MemoryWriter& vmtWriter, void* classClassVAddress, int count, bool virtualMode);
};

} // _ELENA_

#endif // jitcompilerH

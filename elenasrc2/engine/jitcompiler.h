//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class.
//
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jitcompilerH
#define jitcompilerH 1

namespace _ELENA_
{

//#define DUPLICATE_ENTRY (size_t)-2

// --- ReferenceHelper ---

class _ReferenceHelper
{
public:
   virtual ref_t getLinkerConstant(ref_t constant) = 0;
   virtual SectionInfo getPredefinedSection(const wchar16_t* package, ref_t reference) = 0;
   virtual SectionInfo getSection(ref_t reference, _Module* module = NULL) = 0;

   virtual void* getVAddress(MemoryWriter& writer, int mask) = 0;

   virtual ref_t resolveMessage(ref_t reference, _Module* module = NULL) = 0;

   virtual void writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module = NULL) = 0;
   virtual void writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp) = 0;
//   //virtual void writeMethodReference(SectionWriter& writer, size_t tapeDisp) = 0;

   virtual void addBreakpoint(size_t position) = 0;
};

// --- _BinaryHelper ---

class _BinaryHelper
{
public:
   virtual void writeReference(MemoryWriter& writer, const wchar16_t* reference, int mask) = 0;
};

// --- JITCompiler class ---
class _JITCompiler
{
public:
   virtual void prepareCoreData(_ReferenceHelper& helper, _Memory* data, _Memory* rdata, _Memory* sdata) = 0;
   virtual void prepareCommandSet(_ReferenceHelper& helper, _Memory* code) = 0;

   // should be called only for VM
   virtual void prepareVMData(_ReferenceHelper& helper, _Memory* data) = 0;

//   virtual int writeInteger(MemoryWriter& writer, int value);
//   virtual int writeString(MemoryWriter& writer, const TCHAR* string);
//   virtual int writeAnsiString(MemoryWriter& writer, const TCHAR* string);

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code) = 0;

   virtual void compileInt32(MemoryWriter* writer, int integer) = 0;
   virtual void compileInt64(MemoryWriter* writer, long long integer) = 0;
   virtual void compileReal64(MemoryWriter* writer, double number) = 0;
   virtual void compileWideLiteral(MemoryWriter* writer, const wchar_t* value) = 0;

   virtual void compileTLS(_JITLoader* loader) = 0;
   virtual void compileThreadTable(_JITLoader* loader, int maxThreadNumber) = 0;

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter) = 0;

   // return VMT field position
   virtual void allocateVariable(MemoryWriter& writer) = 0;
   virtual void allocateArray(MemoryWriter& writer, size_t count) = 0;

   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset) = 0;
   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength) = 0;

   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries) = 0;

   virtual int findMethodAddress(void* refVMT, int messageID, size_t vmtLength) = 0;
   virtual size_t findFlags(void* refVMT) = 0;
   virtual size_t findLength(void* refVMT) = 0;

   virtual void addVMTEntry(_ReferenceHelper& helper, ref_t message, size_t codePosition, VMTEntry* entries, size_t& count) = 0;

   virtual void compileVMT(void* vaddress, MemoryWriter& vmtWriter, ClassHeader& header, int count, void* vaddressClass, bool virtualMode) = 0;
//   virtual void compilePseudoVMT(MemoryWriter& vmtWriter, void* address, int flags, bool virtualMode) = 0;
////   virtual void compileActionVMT(MemoryWriter& vmtWriter, void* address, int dispatchRef, int flags, bool virtualMode);

   virtual void loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section) = 0;

   virtual void* getPreloadedReference(ref_t reference) = 0;

   virtual void setStaticRootCounter(_JITLoader* loader, int counter, bool virtualMode) = 0;
};

// --- JITCompiler32 class ---
class JITCompiler32 : public _JITCompiler
{
public:
   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileWideLiteral(MemoryWriter* writer, const wchar16_t* value);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual int allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual size_t findFlags(void* refVMT);
   virtual size_t findLength(void* refVMT);
   virtual int findMethodAddress(void* refVMT, int messageID, size_t vmtLength);

   virtual void allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength);
   virtual int copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual void addVMTEntry(_ReferenceHelper& helper, ref_t message, size_t codePosition, VMTEntry* entries, size_t& count);
   virtual void compileVMT(void* vaddress, MemoryWriter& vmtWriter, ClassHeader& header, int count, void* vaddressClass, bool virtualMode);
//   virtual void compilePseudoVMT(MemoryWriter& vmtWriter, void* address, int flags, bool virtualMode);
};

} // _ELENA_

#endif // jitcompilerH

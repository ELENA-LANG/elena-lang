//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class.
//
//                                              (C)2005-2021, by Alexei Rakov
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
   virtual SectionInfo getSection(ref_t reference, _Module* module = nullptr) = 0;

   virtual lvaddr_t getVAddress(MemoryWriter& writer, ref_t mask) = 0;

   virtual mssg_t resolveMessage(mssg_t reference, _Module* module = nullptr) = 0;

   virtual void writeVAddress(MemoryWriter& writer, lvaddr_t vaddress, pos_t disp) = 0;
   virtual void writeRelVAddress(MemoryWriter& writer, lvaddr_t vaddress, ref_t mask, pos_t disp) = 0;

   virtual void writeReference(MemoryWriter& writer, ref_t reference, pos_t disp, _Module* module = NULL) = 0;
   virtual void writeMTReference(MemoryWriter& writer) = 0;

   //// used for 64bit programming, currently only for mskVMTXMethodAddress and mskVMTXEntryOffset
   //virtual void writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module = NULL) = 0;

   virtual void addBreakpoint(pos_t position) = 0;
};

//// --- _BinaryHelper ---
//
//class _BinaryHelper
//{
//public:
//   virtual void writeReference(MemoryWriter& writer, ident_t reference, int mask) = 0;
//};

// --- JITCompiler class ---
class _JITCompiler
{
public:
   virtual void allocateMetaInfo(_Module* messages) = 0;

   virtual pos_t getObjectHeaderSize() const = 0;

   virtual bool isWithDebugInfo() const = 0;

   virtual void prepareCore(_ReferenceHelper& helper, _JITLoader* loader) = 0;

   virtual void alignCode(MemoryWriter* writer, int alignment, bool code) = 0;

   virtual void compileInt32(MemoryWriter* writer, int integer) = 0;
   virtual void compileInt64(MemoryWriter* writer, long long integer) = 0;
   virtual void compileReal64(MemoryWriter* writer, double number) = 0;
   virtual void compileLiteral(MemoryWriter* writer, const char* value) = 0;
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value) = 0;
   virtual void compileChar32(MemoryWriter* writer, const char* value) = 0;
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary) = 0;
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary) = 0;

   virtual void compileMAttribute(MemoryWriter& writer, int category, ident_t fullName, lvaddr_t address, bool virtualMode) = 0;

   virtual ref_t allocateActionEntry(MemoryWriter& mdataWriter, MemoryWriter& bodyWriter, ident_t actionName,
      ref_t weakActionRef, ref_t signature) = 0;
   virtual void allocateSignatureEntry(MemoryWriter& writer, ref_t typeRef) = 0;

   virtual void compileMessage(MemoryWriter* writer, mssg_t mssg) = 0;
   virtual void compileAction(MemoryWriter* writer, ref_t actionRef) = 0;
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, ref_t ref, int refOffset) = 0;
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, uintptr_t addr) = 0;

   virtual void compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter);
   virtual void compileProcedure(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter) = 0;

   virtual void allocateVariable(MemoryWriter& writer) = 0;
//   virtual void allocateArray(MemoryWriter& writer, size_t count) = 0;

   virtual int allocateTLSVariable(_JITLoader* loader) = 0;
   virtual void allocateThreadTable(_JITLoader* loader, int length) = 0;
   virtual int allocateVMTape(_JITLoader* loader, void* tape, pos_t length) = 0;

   virtual pos_t allocateConstant(MemoryWriter& writer, size_t objectOffset) = 0;
   virtual void allocateVMT(MemoryWriter& vmtWriter, ref_t flags, pos_t vmtLength, pos_t staticSize) = 0;

   virtual pos_t copyParentVMT(void* parentVMT, VMTEntry* entries) = 0;

   virtual lvaddr_t findMethodAddress(void* refVMT, mssg_t messageID, size_t vmtLength) = 0;
   virtual pos_t findMethodIndex(void* refVMT, mssg_t messageID, size_t vmtLength) = 0;
   virtual pos_t findMemberPosition(int index) = 0;

   virtual ref_t findFlags(void* refVMT) = 0;

   virtual size_t findLength(void* refVMT) = 0;
   virtual lvaddr_t findClassPtr(void* refVMT) = 0;

   virtual void addVMTEntry(mssg_t message, lvaddr_t codePosition, VMTEntry* entries, pos_t& count) = 0;

   virtual void fixVMT(MemoryWriter& vmtWriter, lvaddr_t classClassVAddress, lvaddr_t parentVAddress,
      pos_t count, bool virtualMode, bool abstractMode) = 0;

   virtual lvaddr_t getPreloadedReference(ref_t reference) = 0;

   virtual void setStaticRootCounter(_JITLoader* loader, pos_t counter, bool virtualMode) = 0;
   virtual void setTLSKey(lvaddr_t ptr) = 0;
   virtual void setThreadTable(lvaddr_t ptr) = 0;
   virtual void setEHTable(lvaddr_t ptr) = 0;
   virtual void setGCTable(lvaddr_t ptr) = 0;
   virtual void setVoidParent(_JITLoader* loader, lvaddr_t ptr, bool virtualMode) = 0;

   virtual lvaddr_t getInvoker() = 0;

   virtual void generateProgramStart(MemoryDump& tape) = 0;
   virtual void generateSymbolCall(MemoryDump& tape, lvaddr_t address) = 0;
   virtual void generateProgramEnd(MemoryDump& tape) = 0;
};

// --- JITCompiler32 class ---
class JITCompiler32 : public _JITCompiler
{
public:
   virtual void allocateMetaInfo(_Module* messages);

   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileLiteral(MemoryWriter* writer, const char* value);
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value);
   virtual void compileChar32(MemoryWriter* writer, const char* value);
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary);
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary);

   virtual void compileMessage(MemoryWriter* writer, mssg_t mssg);
   virtual void compileAction(MemoryWriter* writer, ref_t actionRef);
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, ref_t ref, int refOffset);
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, uintptr_t addr);

   virtual void compileMAttribute(MemoryWriter& writer, int category, ident_t fullName, lvaddr_t address, bool virtualMode);

   virtual ref_t allocateActionEntry(MemoryWriter& mdataWriter, MemoryWriter& bodyWriter, ident_t actionName,
      ref_t weakActionRef, ref_t signature);
   virtual void allocateSignatureEntry(MemoryWriter& writer, ref_t typeRef);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual pos_t allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual ref_t findFlags(void* refVMT);

   virtual size_t findLength(void* refVMT);
   virtual lvaddr_t findMethodAddress(void* refVMT, mssg_t messageID, size_t vmtLength);
   virtual pos_t findMethodIndex(void* refVMT, mssg_t messageID, size_t vmtLength);
   virtual lvaddr_t findClassPtr(void* refVMT);

   virtual void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticSize);
   virtual pos_t copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual void addVMTEntry(mssg_t message, lvaddr_t codePosition, VMTEntry* entries, pos_t& count);
   virtual void fixVMT(MemoryWriter& vmtWriter, lvaddr_t classClassVAddress, lvaddr_t parentVAddress, 
      pos_t count, bool virtualMode, bool abstractMode);

   virtual pos_t findMemberPosition(int index)
   {
      return index << 2;
   }

   virtual void generateProgramStart(MemoryDump& tape);
   virtual void generateProgramEnd(MemoryDump& tape);
};

// --- JITCompiler64 class ---
class JITCompiler64 : public _JITCompiler
{
public:
   virtual void allocateMetaInfo(_Module* messages);

   virtual void compileInt32(MemoryWriter* writer, int integer);
   virtual void compileInt64(MemoryWriter* writer, long long integer);
   virtual void compileReal64(MemoryWriter* writer, double number);
   virtual void compileLiteral(MemoryWriter* writer, const char* value);
   virtual void compileWideLiteral(MemoryWriter* writer, const wide_c* value);
   virtual void compileChar32(MemoryWriter* writer, const char* value);
   virtual void compileBinary(MemoryWriter* writer, _Memory* binary);
   virtual void compileCollection(MemoryWriter* writer, _Memory* binary);

   virtual void compileMessage(MemoryWriter* writer, mssg_t mssg);
   virtual void compileAction(MemoryWriter* writer, ref_t actionRef);
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, ref_t ref, int refOffset);
   virtual void compileMssgExtension(MemoryWriter* writer, mssg_t low, uintptr_t addr);

   virtual void compileMAttribute(MemoryWriter& writer, int category, ident_t fullName, lvaddr_t address, bool virtualMode);

   virtual ref_t allocateActionEntry(MemoryWriter& mdataWriter, MemoryWriter& bodyWriter, ident_t actionName,
      ref_t weakActionRef, ref_t signature);
   virtual void allocateSignatureEntry(MemoryWriter& writer, ref_t typeRef);

   virtual void allocateVariable(MemoryWriter& writer);
   virtual void allocateArray(MemoryWriter& writer, size_t count);

   // return VMT field position
   virtual pos_t allocateConstant(MemoryWriter& writer, size_t objectOffset);

   virtual ref_t findFlags(void* refVMT);
   virtual size_t findLength(void* refVMT);
   virtual lvaddr_t findMethodAddress(void* refVMT, mssg_t messageID, size_t vmtLength);
   virtual lvaddr_t findMethodAddressX(void* refVMT, mssg64_t messageID, size_t vmtLength);
   virtual pos_t findMethodIndex(void* refVMT, mssg_t messageID, size_t vmtLength);
   virtual pos_t findMethodIndexX(void* refVMT, mssg64_t messageID, size_t vmtLength);
   virtual lvaddr_t findClassPtr(void* refVMT);

   virtual void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticSize);
   virtual pos_t copyParentVMT(void* parentVMT, VMTEntry* entries);
   virtual pos_t copyParentVMTX(void* parentVMT, VMTXEntry* entries);
   virtual void addVMTEntry(mssg_t message, lvaddr_t codePosition, VMTEntry* entries, pos_t& count);
   virtual void addVMTXEntry(mssg64_t message, lvaddr_t codePosition, VMTXEntry* entries, pos_t& entryCount);
   virtual void fixVMT(MemoryWriter& vmtWriter, lvaddr_t classClassVAddress, lvaddr_t packageParentVAddress, pos_t count, 
      bool virtualMode, bool abstractMode);

   virtual pos_t findMemberPosition(int index)
   {
      return index << 3;
   }

   virtual void generateProgramStart(MemoryDump& tape);
   virtual void generateProgramEnd(MemoryDump& tape);
};

} // _ELENA_

#endif // jitcompilerH

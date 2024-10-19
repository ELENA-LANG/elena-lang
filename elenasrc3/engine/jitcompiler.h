//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef JITCOMPILER_H
#define JITCOMPILER_H

#include "elena.h"
#include "bytecode.h"

namespace elena_lang
{
   constexpr auto NumberOfInlines = 12;

   // --- JITCompilerScope ---
   class JITCompiler;

   // --- JITConstants ---
   struct JITConstants
   {
      // an offset to raw stack data
      int          dataOffset;
      int          dataHeader;
      int          indexPower;
      ref_t        inlineMask;
      int          alignmentVA;
      int          structMask;
      int          unframedOffset;
      int          vmtSize;

      // used for RISC CPUs to deal with "big" arguments
      int          mediumForm;
      int          extendedForm;
      // for ARM negative offsets should have a special treatment
      bool         noNegative;
   };

   struct JITCompilerScope
   {
      ReferenceHelperBase* helper;
      JITCompiler*         compiler;
      LabelHelperBase*     lh;
      MemoryWriter*        codeWriter;
      MemoryReader*        tapeReader;
      ByteCommand          command;
      JITConstants*        constants;

      bool                 inlineMode;
      bool                 withDebugInfo;
      ref_t                frameOffset;
      ref_t                stackOffset;
      bool                 altMode;

      bool getAltMode()
      {
         bool oriValue = altMode;
         altMode = false;

         return oriValue;
      }

      unsigned char code() const
      {
         return (unsigned char)command.code;
      }

      JITCompilerScope(ReferenceHelperBase* helper, JITCompiler* compiler, LabelHelperBase* lh, MemoryWriter* writer,
         MemoryReader* tapeReader, JITConstants* constants);
   };

   typedef void(*CodeGenerator)(JITCompilerScope*);

   // --- JITCompiler ---
   class JITCompiler : public JITCompilerBase
   {
   public:
      typedef FixedMemoryMap<ref_t, void*> PreloadedMap;

   protected:
      void*        _inlines[NumberOfInlines][0x100];
      PreloadedMap _preloaded;

      JITConstants _constants;

      CodeGenerator* codeGenerators();

      virtual int calcFrameOffset(int argument, bool extMode) = 0;
      virtual int calcTotalSize(int numberOfFields) = 0;
      virtual int calcTotalStructSize(int size) = 0;

      void writeArgAddress(JITCompilerScope* scope, ref_t arg, pos_t offset, ref_t addressMask);
      void writeVMTMethodArg(JITCompilerScope* scope, ref_t arg, pos_t offset, mssg_t message, ref_t addressMask);

      virtual void compileTape(ReferenceHelperBase* helper, MemoryReader& bcReader, pos_t endPos, 
         MemoryWriter& codeWriter, LabelHelperBase* lh);

      friend void writeCoreReference(JITCompilerScope* scope, ref_t reference, 
         pos_t disp, void* code, ModuleBase* module);
      friend void writeMDataReference(JITCompilerScope* scope, ref_t reference,
         pos_t disp, void* code, ModuleBase* module);
      friend void allocateCode(JITCompilerScope* scope, void* code);
      friend void loadCode(JITCompilerScope* scope, void* code, ModuleBase* module);

      friend void* retrieveCode(JITCompilerScope* scope);
      friend void* retrieveIndexRCode(JITCompilerScope* scope);
      friend void* retrieveFrameIndexRCode(JITCompilerScope* scope);
      friend void* retrieveCodeWithNegative(JITCompilerScope* scope);
      friend void* retrieveICode(JITCompilerScope* scope, int arg);
      friend void* retrieveRCode(JITCompilerScope* scope, int arg);
      friend void* retrieveIRCode(JITCompilerScope* scope, int arg1, int arg2);

      friend void loadOp(JITCompilerScope* scope);
      friend void loadSysOp(JITCompilerScope* scope);
      friend void loadLOp(JITCompilerScope* scope);
      friend void loadIndexOp(JITCompilerScope* scope);
      friend void loadNOp(JITCompilerScope* scope);
      friend void loadTLSOp(JITCompilerScope* scope);
      friend void loadFieldIndexOp(JITCompilerScope* scope);
      friend void loadStackIndexOp(JITCompilerScope* scope);
      friend void loadArgIndexOp(JITCompilerScope* scope);
      friend void loadVMTIndexOp(JITCompilerScope* scope);
      friend void loadFrameIndexOp(JITCompilerScope* scope);
      friend void loadFrameDispOp(JITCompilerScope* scope);
      friend void loadNOp(JITCompilerScope* scope);
      friend void loadLenOp(JITCompilerScope* scope);
      friend void loadROp(JITCompilerScope* scope);
      friend void loadRROp(JITCompilerScope* scope);
      friend void loadONOp(JITCompilerScope* scope);
      friend void loadMOp(JITCompilerScope* scope);
      friend void loadCallOp(JITCompilerScope* scope);
      friend void loadCallROp(JITCompilerScope* scope);
      friend void loadStackIndexROp(JITCompilerScope* scope);
      friend void loadFrameIndexROp(JITCompilerScope* scope);
      friend void loadIndexNOp(JITCompilerScope* scope);
      friend void loadStackIndexFrameIndexOp(JITCompilerScope* scope);
      friend void loadStackIndexIndexOp(JITCompilerScope* scope);
      friend void loadNewOp(JITCompilerScope* scope);
      friend void loadNewNOp(JITCompilerScope* scope);
      friend void loadCreateNOp(JITCompilerScope* scope);
      friend void loadMROp(JITCompilerScope* scope);
      friend void loadVMTROp(JITCompilerScope* scope);
      friend void loadDPNOp(JITCompilerScope* scope);
      friend void loadDPNOp2(JITCompilerScope* scope);
      friend void loadDispNOp(JITCompilerScope* scope);
      friend void loadDPROp(JITCompilerScope* scope);
      friend void loadDPLabelOp(JITCompilerScope* scope);
      friend void loadIOp(JITCompilerScope* scope);
      friend void loadIROp(JITCompilerScope* scope);

      friend void compileAlloc(JITCompilerScope* scope);
      friend void compileFree(JITCompilerScope* scope);
      friend void compileBreakpoint(JITCompilerScope* scope);
      friend void compileAltMode(JITCompilerScope* scope);
      friend void compileClose(JITCompilerScope* scope);
      friend void compileOpen(JITCompilerScope* scope);
      friend void compileExtOpen(JITCompilerScope* scope);
      friend void compileXOpen(JITCompilerScope* scope);
      friend void compileJump(JITCompilerScope* scope);
      friend void compileJeq(JITCompilerScope* scope);
      friend void compileJne(JITCompilerScope* scope);
      friend void compileJlt(JITCompilerScope* scope);
      friend void compileJge(JITCompilerScope* scope);
      friend void compileJle(JITCompilerScope* scope);
      friend void compileJgr(JITCompilerScope* scope);
      friend void compileDispatchMR(JITCompilerScope* scope);
      friend void compileHookDPR(JITCompilerScope* scope);

      void loadCoreRoutines(
         LibraryLoaderBase* loader,
         ImageProviderBase* imageProvider,
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings,
         Map<ref_t, pos_t>& positions, bool declareMode, bool virtualMode);

   public:
      void allocateBody(MemoryWriter& writer, int size) override;

      bool isWithDebugInfo() override
      {
         // in the current implementation, debug info (i.e. debug section)
         // is always generated (to be used by RTManager)
         return true;
      }

      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings,
         bool virtualMode) override;

      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter, 
         LabelHelperBase* lh) override;

      void writeImm16(MemoryWriter* writer, int value, int type) override
      {
         switch (type) {
            case INV_ARG:
               writer->writeWord(-value);
               break;
            default:
               writer->writeWord(value);
               break;
         }         
      }

      void writeImm16Hi(MemoryWriter* writer, int value, int type) override
      {
         writeImm16(writer, value >> 16, type);
      }

      void writeImm32(MemoryWriter* writer, int value) override
      {
         writer->writeDWord(value);
      }

      void writeDump(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) override;

      void resolveLabelAddress(MemoryWriter* writer, ref_t mask, pos_t position, bool virtualMode) override;

      void populatePreloaded(
         uintptr_t eh_table, uintptr_t th_single_content) override;

      addr_t allocateTLSIndex(ReferenceHelperBase* helper, MemoryWriter& writer) override;

      void allocateThreadContent(MemoryWriter* tlsWriter) override;
      pos_t getTLSSize(MemoryBase* tlsSection) override;

      void* getSystemEnv() override;

      JITCompiler()
         : _inlines{}, _preloaded(nullptr)
      {
         _constants.indexPower = 0;
         _constants.dataHeader = _constants.dataOffset = 0;
         _constants.inlineMask = 0;
         _constants.alignmentVA = 8;
         _constants.unframedOffset = 0;
         _constants.mediumForm = _constants.extendedForm = INT32_MAX;
         _constants.noNegative = false;
      }
   };

   // --- JITCompiler32 ---
   class JITCompiler32 : public JITCompiler
   {
   protected:
      int calcFrameOffset(int argument, bool extMode) override
      {
         return (extMode ? 36  : 4) + (argument > 0 ? align(argument + 8, 4) : 0);
      }

      int calcTotalSize(int numberOfFields) override;
      int calcTotalStructSize(int size) override;

   public:
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings,
         bool virtualMode) override;

      int getExtMessageSize() override
      {
         return 8;
      }

      void compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length) override;

      void compileOutputTypeList(ReferenceHelperBase* helper, MemoryWriter& writer, CachedOutputTypeList& outputTypeList) override;

      pos_t getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed) override;

      pos_t getVMTLength(void* targetVMT) override;
      addr_t findMethodAddress(void* entries, mssg_t message) override;
      pos_t findMethodOffset(void* entries, mssg_t message) override;
      pos_t findHiddenMethodOffset(void* entries, mssg_t message) override;

      void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, 
         pos_t indexTableLength, pos_t staticLength, bool withOutputList) override;
      void addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount) override;
      void addIndexEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t indexOffset, pos_t& indexCount) override;
      void updateVMTHeader(MemoryWriter& vmtWriter, VMTFixInfo& fixInfo, FieldAddressMap& staticValues, bool virtualMode) override;
      Pair<pos_t, pos_t> copyParentVMT(void* parentVMT, void* targetVMT, pos_t indexTableOffset) override;

      void allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length, 
         bool structMode, bool virtualMode) override;

      pos_t addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, ustr_t actionName, 
         ref_t weakActionRef, ref_t signature, bool virtualMode) override;
      pos_t addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode) override;
      void addActionEntryStopper(MemoryWriter& messageWriter) override;
      void addSignatureStopper(MemoryWriter& messageWriter) override;

      void addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode) override;
      void addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode) override;

      void writeInt32(MemoryWriter& writer, unsigned value) override;
      void writeInt64(MemoryWriter& writer, unsigned long long value) override;
      void writeFloat64(MemoryWriter& writer, double value) override;
      void writeLiteral(MemoryWriter& writer, ustr_t value) override;
      void writeWideLiteral(MemoryWriter& writer, wstr_t value) override;
      void writeChar32(MemoryWriter& writer, ustr_t value) override;
      void writeMessage(MemoryWriter& writer, mssg_t value) override;
      void writeExtMessage(MemoryWriter& writer, Pair<mssg_t, addr_t> extensionInfo, bool virtualMode) override;
      void writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) override;
      void writeVariable(MemoryWriter& writer) override;

      void writeAttribute(MemoryWriter& writer, int category, ustr_t value, addr_t address, bool virtualMode) override;

      void updateEnvironment(MemoryBase* rdata, pos_t staticCounter, pos_t tlsSize, bool virtualMode) override;
      void updateVoidObject(MemoryBase* rdata, addr_t superAddress, bool virtualMode) override;

      void allocateVariable(MemoryWriter& writer) override;

      JITCompiler32()
         : JITCompiler()
      {
      }
   };

   // --- JITCompiler64 ---
   class JITCompiler64 : public JITCompiler
   {
   protected:
      int calcFrameOffset(int argument, bool extMode) override
      {
         return (extMode ? 40 : 8) + (argument > 0 ? align(argument + 16, 16) : 0);
      }

      int calcTotalSize(int numberOfFields) override;
      int calcTotalStructSize(int size) override;

   public:
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings,
         bool virtualMode) override;

      void compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length) override;

      void compileOutputTypeList(ReferenceHelperBase* helper, MemoryWriter& writer, CachedOutputTypeList& outputTypeList) override;

      pos_t getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed) override;

      int getExtMessageSize() override
      {
         return 16;
      }

      pos_t getVMTLength(void* targetVMT) override;
      addr_t findMethodAddress(void* entries, mssg_t message) override;
      pos_t findMethodOffset(void* entries, mssg_t message) override;
      pos_t findHiddenMethodOffset(void* entries, mssg_t message) override;

      void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, 
         pos_t indexTableLength, pos_t staticLength, bool withOutputList) override;
      Pair<pos_t, pos_t> copyParentVMT(void* parentVMT, void* targetVMT, pos_t indexTableOffset) override;
      void addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount) override;
      void addIndexEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t indexOffset, pos_t& indexCount) override;
      void updateVMTHeader(MemoryWriter& vmtWriter, VMTFixInfo& fixInfo, FieldAddressMap& staticValues, bool virtualMode) override;

      void allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length, 
         bool structMode, bool virtualMode) override;

      pos_t addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, ustr_t actionName, ref_t weakActionRef, 
         ref_t signature, bool virtualMode) override;
      pos_t addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode) override;
      void addActionEntryStopper(MemoryWriter& messageWriter) override;
      void addSignatureStopper(MemoryWriter& messageWriter) override;

      void addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode) override;
      void addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode) override;

      void writeInt32(MemoryWriter& writer, unsigned value) override;
      void writeInt64(MemoryWriter& writer, unsigned long long value) override;
      void writeFloat64(MemoryWriter& writer, double value) override;
      void writeLiteral(MemoryWriter& writer, ustr_t value) override;
      void writeWideLiteral(MemoryWriter& writer, wstr_t value) override;
      void writeChar32(MemoryWriter& writer, ustr_t value) override;
      void writeMessage(MemoryWriter& writer, mssg_t value) override;
      void writeExtMessage(MemoryWriter& writer, Pair<mssg_t, addr_t> extensionInfo, bool virtualMode) override;
      void writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) override;
      void writeVariable(MemoryWriter& writer) override;
      void writeDump(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) override;

      void writeAttribute(MemoryWriter& writer, int category, ustr_t value, addr_t address, bool virtualMode) override;

      void updateEnvironment(MemoryBase* rdata, pos_t staticCounter, pos_t tlsSize, bool virtualMode) override;
      void updateVoidObject(MemoryBase* rdata, addr_t superAddress, bool virtualMode) override;

      void allocateVariable(MemoryWriter& writer) override;

      JITCompiler64()
         : JITCompiler()
      {
      }
   };

   void writeCoreReference(JITCompilerScope* scope, ref_t reference, pos_t disp, void* code, ModuleBase* module = nullptr);
   void writeMDataReference(JITCompilerScope* scope, ref_t reference, pos_t disp, void* code, ModuleBase* module = nullptr);
   void loadCode(JITCompilerScope* scope, void* code, ModuleBase* module);
   void allocateCode(JITCompilerScope* scope, void* code);

   inline void* retrieveCode(JITCompilerScope* scope);
   inline void* retrieveIndexRCode(JITCompilerScope* scope);
   inline void* retrieveFrameIndexRCode(JITCompilerScope* scope);
   inline void* retrieveCodeWithNegative(JITCompilerScope* scope);
   inline void* retrieveICode(JITCompilerScope* scope, int arg);
   inline void* retrieveRCode(JITCompilerScope* scope, int arg);
   inline void* retrieveIRCode(JITCompilerScope* scope, int arg1, int arg2);

   void loadNop(JITCompilerScope*);
   void loadXNop(JITCompilerScope*);
   void loadOp(JITCompilerScope* scope);
   void loadSysOp(JITCompilerScope* scope);
   void loadLOp(JITCompilerScope* scope);
   void loadIndexOp(JITCompilerScope* scope);
   void loadNOp(JITCompilerScope* scope);
   void loadTLSOp(JITCompilerScope* scope);
   void loadFieldIndexOp(JITCompilerScope* scope);
   void loadVMTIndexOp(JITCompilerScope* scope);
   void loadFrameIndexOp(JITCompilerScope* scope);
   void loadStackIndexOp(JITCompilerScope* scope);
   void loadArgIndexOp(JITCompilerScope* scope);
   void loadFrameDispOp(JITCompilerScope* scope);
   void loadNOp(JITCompilerScope* scope);
   void loadLenOp(JITCompilerScope* scope);
   void loadROp(JITCompilerScope* scope);
   void loadRROp(JITCompilerScope* scope);
   void loadONOp(JITCompilerScope* scope);
   void loadMOp(JITCompilerScope* scope);
   void loadCallOp(JITCompilerScope* scope);
   void loadCallROp(JITCompilerScope* scope);
   void loadStackIndexROp(JITCompilerScope* scope);
   void loadFrameIndexROp(JITCompilerScope* scope);
   void loadIndexNOp(JITCompilerScope* scope);
   void loadStackIndexFrameIndexOp(JITCompilerScope* scope);
   void loadStackIndexIndexOp(JITCompilerScope* scope);
   void loadNewOp(JITCompilerScope* scope);
   void loadNewNOp(JITCompilerScope* scope);
   void loadCreateNOp(JITCompilerScope* scope);
   void loadMROp(JITCompilerScope* scope);
   void loadVMTROp(JITCompilerScope* scope);
   void loadDPNOp(JITCompilerScope* scope);
   void loadDPNOp2(JITCompilerScope* scope);
   void loadDispNOp(JITCompilerScope* scope);
   void loadDPROp(JITCompilerScope* scope);
   void loadDPLabelOp(JITCompilerScope* scope);
   void loadIOp(JITCompilerScope* scope);
   void loadIROp(JITCompilerScope* scope);

   void compileAlloc(JITCompilerScope* scope);
   void compileFree(JITCompilerScope* scope);
   void compileClose(JITCompilerScope* scope);
   void compileOpen(JITCompilerScope* scope);
   void compileExtOpen(JITCompilerScope* scope);
   void compileXOpen(JITCompilerScope* scope);
   void compileBreakpoint(JITCompilerScope* scope);
   void compileAltMode(JITCompilerScope* scope);
   void compileJump(JITCompilerScope* scope);
   void compileJeq(JITCompilerScope* scope);
   void compileJne(JITCompilerScope* scope);
   void compileJlt(JITCompilerScope* scope);
   void compileJge(JITCompilerScope* scope);
   void compileJle(JITCompilerScope* scope);
   void compileJgr(JITCompilerScope* scope);
   void compileDispatchMR(JITCompilerScope* scope);
   void compileHookDPR(JITCompilerScope* scope);
}

#endif
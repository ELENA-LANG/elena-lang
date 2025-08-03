//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENA_H
#define ELENA_H

#include "elenacommon.h"
#include "section.h"

namespace elena_lang
{
   // --- miscellaneous routines ---
   inline bool isWeakReference(ustr_t referenceName)
   {
      return (!referenceName.empty() && referenceName[0] == '\'');
   }

   inline bool isForwardReference(ustr_t referenceName)
   {
      return referenceName.startsWith(FORWARD_PREFIX_NS);
   }

   inline bool isTemplateWeakReference(ustr_t referenceName)
   {
      return referenceName[0] == '\'' && referenceName.startsWith(TEMPLATE_PREFIX_NS);
   }

   inline bool isPrimitiveRef(ref_t reference)
   {
      return (int)reference < 0;
   }

   inline mssg_t encodeMessage(ref_t actionRef, pos_t argCount, ref_t flags)
   {
      return flags | ((actionRef << ACTION_ORDER) + argCount);
   }

   inline void decodeMessage(mssg_t message, ref_t& actionRef, pos_t& argCount, ref_t& flags)
   {
      actionRef = (message >> ACTION_ORDER);

      argCount = message & ARG_MASK;

      flags = message & MESSAGE_FLAG_MASK;
   }

   inline ref_t getAction(mssg_t message)
   {
      ref_t actionRef, flags;
      pos_t argCount;
      decodeMessage(message, actionRef, argCount, flags);

      return actionRef;
   }

   inline ref_t getFlags(mssg_t message)
   {
      ref_t actionRef, flags;
      pos_t argCount;
      decodeMessage(message, actionRef, argCount, flags);

      return flags;
   }

   inline pos_t getArgCount(mssg_t message)
   {
      ref_t actionRef, flags;
      pos_t argCount;
      decodeMessage(message, actionRef, argCount, flags);

      return argCount;
   }

   inline ref64_t encodeAction64(ref_t actionNameRef, ref_t signatureRef)
   {
      ref64_t r = signatureRef;

      r = (r << 32) + actionNameRef;

      return r;
   }

   inline void decodeAction64(ref64_t r, ref_t& actionName, ref_t& signatureRef)
   {
      actionName = r & 0xFFFFFF;

      signatureRef = (r >> 32);
   }

   inline mssg_t overwriteAction(mssg_t message, ref_t newAction)
   {
      pos_t argCount;
      ref_t actionRef, flags;
      decodeMessage(message, actionRef, argCount, flags);

      return encodeMessage(newAction, argCount, flags);
   }

   inline mssg_t overwriteArgCount(mssg_t message, pos_t newArgCount)
   {
      pos_t argCount;
      ref_t actionRef, flags;
      decodeMessage(message, actionRef, argCount, flags);

      return encodeMessage(actionRef, newArgCount, flags);
   }

   inline bool isOpenArg(mssg_t message)
   {
      return (message & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE;
   }

   // --- Misc types ---
   typedef unsigned int          parse_key_t;
   typedef Pair<ref_t, mssg_t>   ExtensionInfo;

   // --- Maps ---
   typedef Map<ustr_t, ref_t, allocUStr, freeUStr>                         ReferenceMap;
   typedef Map<ref64_t, ref_t>                                             ActionMap;
   typedef Map<ustr_t, addr_t, allocUStr, freeUStr>                        AddressMap;
   typedef Map<mssg_t, ExtensionInfo>                                      ExtensionMap;
   typedef Map<mssg_t, ustr_t, nullptr, nullptr, freeUStr>                 ExtensionTemplateMap;
   typedef Map<ref_t, ref_t>                                               ResolvedMap;
   typedef Map<int, addr_t>                                                FieldAddressMap;
   typedef MemoryMap<ustr_t, ustr_t, Map_StoreUStr, Map_GetUStr, freeUStr> Forwards;
   typedef Map<ustr_t, bool, allocUStr, freeUStr>                          Variables;

   // --- Lists ---
   typedef List<ustr_t, freeUStr>                           IdentifierList;

   // --- Tuples ---

   // --- ImageProviderBase ---
   class ImageProviderBase
   {
   public:
      virtual AddressMap::Iterator externals() = 0;

      virtual MemoryBase* getTextSection() = 0;
      virtual MemoryBase* getRDataSection() = 0;
      virtual MemoryBase* getADataSection() = 0;   // NOTE : adata should contains at least 4 bytes (zero value)
      virtual MemoryBase* getMDataSection() = 0;
      virtual MemoryBase* getMBDataSection() = 0;
      virtual MemoryBase* getImportSection() = 0;
      virtual MemoryBase* getDataSection() = 0;
      virtual MemoryBase* getStatSection() = 0;
      virtual MemoryBase* getTLSSection() = 0;

      virtual MemoryBase* getTargetSection(ref_t targetMask)
      {
         switch (targetMask) {
            case mskCodeRef:
               return getTextSection();
            case mskRDataRef:
               return getRDataSection();
            case mskDataRef:
               return getDataSection();
            case mskStatDataRef:
               return getStatSection();
            case mskTLSRef:
               return getTLSSection();
            default:
               return nullptr;
         }
      }

      virtual MemoryBase* getTargetDebugSection() = 0;

      virtual addr_t getEntryPoint() = 0;
      virtual addr_t getDebugEntryPoint() = 0;
      virtual addr_t getTLSVariable() = 0;

      virtual pos_t getStackReserved() = 0;

      virtual ~ImageProviderBase() = default;
   };

   // --- ReferenceInfo ---
   struct ReferenceInfo
   {
      ModuleBase* module;
      ustr_t      referenceName; // when module is not null - referenceName is weak one

      bool isRelative() const
      {
         return module != nullptr && isWeakReference(referenceName);
      }

      ReferenceInfo()
      {
         module = nullptr;
      }
      ReferenceInfo(ustr_t name)
         : referenceName(name)
      {
         this->module = nullptr;
      }
      ReferenceInfo(ModuleBase* module, ustr_t name)
         : referenceName(name)
      {
         this->module = module;
      }
   };

   // --- LazyReferenceInfo ---
   struct LazyReferenceInfo
   {
      ref_t       mask;
      pos_t       position;
      ModuleBase* module;
      ref_t       reference;
      ref_t       addressMask;
   };

   // --- ReferenceMapperBase ---
   class ReferenceMapperBase
   {
   protected:
      virtual List<LazyReferenceInfo>::Iterator lazyReferences() = 0;

   public:
      virtual addr_t resolveReference(ReferenceInfo referenceInfo, ref_t sectionMask) = 0;
      virtual void mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t sectionMask) = 0;
      virtual ustr_t retrieveReference(addr_t address, ref_t sectionMask) = 0;

      virtual ref_t resolveAction(ustr_t actionName, ref_t signRef) = 0;
      virtual void mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef) = 0;
      virtual ustr_t retrieveAction(ref_t actionRef, ref_t& signRef) = 0;

      virtual void addLazyReference(LazyReferenceInfo info) = 0;

      template<class ArgT> void forEachLazyReference(ArgT arg, void(*lambda)(ArgT arg, LazyReferenceInfo item))
      {
         for (auto it = lazyReferences(); !it.eof(); ++it) {
            lambda(arg, *it);
         }
      }

      virtual ~ReferenceMapperBase() = default;
   };

   // --- ForwardResolverBase ---
   class ForwardResolverBase
   {
   public:
      virtual void addForward(ustr_t forward, ustr_t referenceName) = 0;

      virtual ustr_t resolveForward(ustr_t forward) = 0;
      virtual ustr_t resolveExternal(ustr_t forward) = 0;
      virtual ustr_t resolveWinApi(ustr_t forward) = 0;

      virtual void forEachForward(void* arg, void(*feedback)(void* arg, ustr_t key, ustr_t value)) = 0;
   };

   // --- VariableResolverBase ---
   class VariableResolverBase
   {
   public:
      virtual bool checkVariable(ustr_t name) = 0;
   };

   // --- ModuleLoaderBase ---
   struct SectionInfo
   {
      ModuleBase* module;
      MemoryBase* section;
      MemoryBase* metaSection;
      ref_t       reference;

      SectionInfo()
      {
         module = nullptr;
         section = metaSection = nullptr;
         reference = 0;
      }
      SectionInfo(MemoryBase* section)
      {
         this->module = nullptr;
         this->section = section;
         this->metaSection = nullptr;
         this->reference = 0;
      }
      SectionInfo(MemoryBase* section, MemoryBase* metaSection)
      {
         this->module = nullptr;
         this->section = section;
         this->metaSection = metaSection;
         this->reference = 0;
      }
   };

   struct ClassSectionInfo
   {
      ModuleBase* module;
      MemoryBase* vmtSection;
      MemoryBase* codeSection;
      ref_t       reference;

      ClassSectionInfo()
      {
         module = nullptr;
         vmtSection = nullptr;
         codeSection = nullptr;
         reference = 0;
      }
   };

   struct ModuleInfo
   {
      ModuleBase* module;
      ref_t       reference;

      bool unassigned() const
      {
         return module == nullptr || reference == 0;
      }

      ModuleInfo()
      {
         module = nullptr;
         reference = 0;
      }
      ModuleInfo(ModuleBase* module, ref_t reference)
      {
         this->module = module;
         this->reference = reference;
      }
   };

   typedef List<ModuleInfo> ModuleInfoList;

   class LibraryLoaderListenerBase
   {
   public:
      virtual void onLoad(ModuleBase*) = 0;
   };

   class LibraryLoaderBase
   {
   public:
      virtual ustr_t Namespace() = 0;

      virtual path_t OutputPath() = 0;

      virtual ReferenceInfo retrieveReferenceInfo(ModuleBase* module, ref_t reference, ref_t mask,
         ForwardResolverBase* forwardResolver) = 0;
      virtual ReferenceInfo retrieveReferenceInfo(ustr_t referenceName,
         ForwardResolverBase* forwardResolver) = 0;

      virtual SectionInfo getCoreSection(ref_t reference, bool silentMode) = 0;
      virtual SectionInfo getSection(ReferenceInfo referenceInfo, ref_t mask, ref_t metaMask, bool silentMode) = 0;
      virtual ClassSectionInfo getClassSections(ReferenceInfo referenceInfo, ref_t vmtMask, ref_t codeMask,
         bool silentMode) = 0;

      virtual ModuleInfo getModule(ReferenceInfo referenceInfo, bool silentMode) = 0;
      virtual ModuleInfo getWeakModule(ustr_t weakReferenceName, bool silentMode) = 0;
      virtual ModuleInfo getDebugModule(ReferenceInfo referenceInfo, bool silentMode) = 0;

      virtual void resolvePath(ustr_t ns, PathString& path) = 0;

      virtual void loadDistributedSymbols(ustr_t virtualSymbolName, ModuleInfoList& list) = 0;
   };

   class LibraryProviderBase
   {
   public:
      virtual void addCorePath(path_t path) = 0;
      virtual void addPrimitivePath(ustr_t alias, path_t path) = 0;
      virtual void addPackage(ustr_t ns, path_t path) = 0;

      virtual void setRootPath(path_t path) = 0;
   };

   // --- SectionScopeBase ---
   class SectionScopeBase
   {
   public:
      ModuleBase* module;
      ModuleBase* debugModule;

      virtual ustr_t resolveFullName(ref_t reference) = 0;

      virtual MemoryBase* mapSection(ref_t reference, bool existing) = 0;

      SectionScopeBase()
      {
         module = nullptr;
         debugModule = nullptr;
      }
   };

   // --- ErrorProcessorBase ---
   class ErrorProcessorBase
   {
   public:
      virtual void info(int code, ustr_t arg) = 0;
      virtual void info(int code, ustr_t arg, ustr_t arg2) = 0;

      virtual void raiseInternalError(int code) = 0;
      virtual void raiseError(int code) = 0;
      virtual void raiseError(int code, ustr_t arg) = 0;
      virtual void raisePathError(int code, path_t pathArg) = 0;
      virtual void raisePathWarning(int code, path_t pathArg) = 0;
   };

   // --- ReferenceHelperBase ---
   class ReferenceHelperBase
   {
   public:
      virtual void addBreakpoint(MemoryWriter& writer) = 0;

      virtual addr_t calculateVAddress(MemoryWriter& writer, ref_t addressMask) = 0;

      virtual void writeSectionReference(MemoryBase* image, pos_t imageOffset, ref_t reference,
         SectionInfo* sectionInfo, pos_t sectionOffset, ref_t addressMask) = 0;

      virtual void writeReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp,
         ref_t addressMask, ModuleBase* module = nullptr) = 0;
      virtual void writeVMTMethodReference(/*MemoryBase& target, */pos_t position, ref_t reference, pos_t disp, mssg_t message,
         ref_t addressMask, ModuleBase* module = nullptr) = 0;

      virtual void writeRelAddress32(MemoryBase& target, pos_t position, addr_t vaddress,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeVAddress32(MemoryBase& target, pos_t position, addr_t vaddress,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeVAddress64(MemoryBase& target, pos_t position, addr_t vaddress,
         pos64_t disp, ref_t addressMask) = 0;
      virtual void writeVAddress32Hi(MemoryBase& target, pos_t position, addr_t vaddress,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeVAddress32Lo(MemoryBase& target, pos_t position, addr_t vaddress,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeDisp32Hi(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
         ref_t addressMask) = 0;
      virtual void writeDisp32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
         ref_t addressMask) = 0;

      virtual void writeMDataRef32(MemoryBase& target, pos_t position,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeMDataRef64(MemoryBase& target, pos_t position,
         pos64_t disp, ref_t addressMask) = 0;

      virtual void writeStatRef32(MemoryBase& target, pos_t position,
         pos_t disp, ref_t addressMask) = 0;
      virtual void writeStatRef64(MemoryBase& target, pos_t position,
         pos64_t disp, ref_t addressMask) = 0;

      virtual mssg_t importMessage(mssg_t message, ModuleBase* module = nullptr) = 0;

      virtual addr_t resolveMDataVAddress() = 0;

      virtual void resolveLabel(MemoryWriter& writer, ref_t mask, pos_t position) = 0;
   };

   // --- ProcessSettings ---
   struct ProcessSettings
   {
      pos_t    mgSize;
      pos_t    ygSize;
      pos_t    stackReserved;
      pos_t    threadCounter;
      bool     classSymbolAutoLoad;
      bool     withAlignedJump;

      ProcessSettings() = default;
      ProcessSettings(pos_t mgSize, pos_t ygSize, pos_t stackReserved,
         pos_t threadCounter, bool classSymbolAutoLoad, bool withAlignedJump)
         : mgSize(mgSize), ygSize(ygSize), stackReserved(stackReserved), threadCounter(threadCounter), 
           classSymbolAutoLoad(classSymbolAutoLoad), withAlignedJump(withAlignedJump)
      {

      }
      ProcessSettings(pos_t mgSize, pos_t ygSize, pos_t stackReserved)
         : mgSize(mgSize), ygSize(ygSize), stackReserved(stackReserved), threadCounter(1),
         classSymbolAutoLoad(false), withAlignedJump(false)
      {

      }
   };

   // --- LabelHelperBase ---
   struct LabelHelperBase
   {
      virtual bool checkLabel(pos_t label) = 0;

      virtual bool setLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) = 0;

      virtual bool fixLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) = 0;

      virtual void writeJumpBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJeqBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJeqForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJneBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJneForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJltBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJltForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJgeBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJgeForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJleBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJleForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeJgrBack(pos_t label, MemoryWriter& writer) = 0;
      virtual void writeJgrForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) = 0;

      virtual void writeLabelAddress(pos_t label, MemoryWriter& writer, ref_t mask) = 0;
   };

   typedef CachedList<Pair<mssg_t, ref_t>, 10> CachedOutputTypeList;

   // --- JITCompilerSettings ---
   struct JITCompilerSettings
   {
      int minimalStackLength;

      int stackAlignment;

      int rawStackAlignment;

      int ehTableEntrySize;
   };

   // --- JITCompilerBase ---
   class JITCompilerBase
   {
   public:
      struct VMTFixInfo
      {
         addr_t parentAddress;
         addr_t classClassAddress;
         addr_t outputListAddress;
         ref_t  flags;
         pos_t  count;
         pos_t  indexCount;
      };

      virtual void prepare(
         LibraryLoaderBase* loader,
         ImageProviderBase* imageProvider,
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         ProcessSettings& settings,
         bool virtualMode) = 0;

      virtual bool isWithDebugInfo() = 0;

      virtual int getExtMessageSize() = 0;

      virtual void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) = 0;
      virtual void alignJumpAddress(MemoryWriter& writer) = 0;

      virtual void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader,
         MemoryWriter& codeWriter, LabelHelperBase* lh) = 0;
      virtual void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader,
         MemoryWriter& codeWriter, LabelHelperBase* lh) = 0;

      virtual void compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length) = 0;

      virtual void compileOutputTypeList(ReferenceHelperBase* helper, MemoryWriter& writer, CachedOutputTypeList& outputTypeList) = 0;

      virtual pos_t getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed = false) = 0;
      virtual pos_t getTLSSize(MemoryBase* tlsSection) = 0;

      virtual pos_t getVMTLength(void* targetVMT) = 0;
      virtual addr_t findMethodAddress(void* entries, mssg_t message) = 0;
      virtual pos_t findMethodOffset(void* entries, mssg_t message) = 0;
      virtual pos_t findHiddenMethodOffset(void* entries, mssg_t message) = 0;

      virtual void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength,
         pos_t indexTableLength, pos_t staticLength, bool withOutputList) = 0;
      virtual void addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount) = 0;
      virtual void addIndexEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t indexOffset, pos_t& indexCount) = 0;

      virtual void updateVMTHeader(MemoryWriter& vmtWriter, VMTFixInfo& fixInfo, FieldAddressMap& staticValues, bool virtualMode) = 0;
      virtual Pair<pos_t, pos_t> copyParentVMT(void* parentVMT, void* targetVMT, pos_t indexTableOffset) = 0;

      virtual void allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length,
         bool structMode, bool virtualMode) = 0;
      virtual void allocateBody(MemoryWriter& writer, int size) = 0;
      virtual void writeInt32(MemoryWriter& writer, unsigned int value) = 0;
      virtual void writeInt64(MemoryWriter& writer, unsigned long long value) = 0;
      virtual void writeFloat64(MemoryWriter& writer, double value) = 0;
      virtual void writeLiteral(MemoryWriter& writer, ustr_t value) = 0;
      virtual void writeWideLiteral(MemoryWriter& writer, wstr_t value) = 0;
      virtual void writeChar32(MemoryWriter& writer, ustr_t value) = 0;
      virtual void writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) = 0;
      virtual void writeDump(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) = 0;
      virtual void writeVariable(MemoryWriter& writer) = 0;
      virtual void writeMessage(MemoryWriter& writer, mssg_t message) = 0;
      virtual void writeExtMessage(MemoryWriter& writer, Pair<mssg_t, addr_t> extensionInfo, bool virtualMode) = 0;

      virtual void addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode) = 0;
      virtual void addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode) = 0;

      virtual pos_t addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode) = 0;
      virtual pos_t addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter,
         ustr_t actionName, ref_t weakActionRef, ref_t signature, bool virtualMode) = 0;
      virtual void addActionEntryStopper(MemoryWriter& messageWriter) = 0;
      virtual void addSignatureStopper(MemoryWriter& messageWriter) = 0;

      virtual void writeImm9(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm12(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm16(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm16Hi(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm32(MemoryWriter* writer, int value) = 0;

      virtual void writeAttribute(MemoryWriter& writer, int category, ustr_t value, addr_t address, bool virtualMode) = 0;

      virtual void resolveLabelAddress(MemoryWriter* writer, ref_t mask, pos_t position, bool virtualMode) = 0;

      virtual void populatePreloaded(
         uintptr_t eh_table, uintptr_t th_single_content) = 0;

      virtual void* getSystemEnv() = 0;

      virtual void updateEnvironment(MemoryBase* rdata, pos_t staticCounter, pos_t tlsSize, bool virtualMode) = 0;
      virtual void updateVoidObject(MemoryBase* rdata, addr_t superAddress, bool virtualMode) = 0;

      virtual void allocateVariable(MemoryWriter& writer) = 0;

      virtual addr_t allocateTLSIndex(ReferenceHelperBase* helper, MemoryWriter& writer) = 0;

      virtual void allocateThreadContent(MemoryWriter* tlsWriter) = 0;

      virtual ~JITCompilerBase() = default;
   };

   // --- AddressMapperBase ---
   class AddressMapperBase
   {
   public:
      virtual void addMethod(addr_t vaddress, mssg_t message, addr_t methodPosition) = 0;
      virtual void addSymbol(addr_t vaddress, addr_t position) = 0;

      virtual ~AddressMapperBase() = default;
   };

   // --- PresenterBase ----
   class PresenterBase
   {
   public:
      virtual void readLine(char* buffer, size_t length) = 0;

      virtual ustr_t getMessage(int code) = 0;

      virtual void print(ustr_t msg) = 0;
      virtual void print(ustr_t msg, ustr_t arg) = 0;
      virtual void print(ustr_t msg, ustr_t arg1, ustr_t arg2) = 0;
      virtual void print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) = 0;
      virtual void print(ustr_t msg, int arg1) = 0;
      virtual void print(ustr_t msg, int arg1, int arg2) = 0;
      virtual void print(ustr_t msg, int arg1, int arg2, int arg3) = 0;
      virtual void print(ustr_t msg, ustr_t arg1, int arg2, int arg3, ustr_t arg4) = 0;
      virtual void printPath(ustr_t msg, path_t arg) = 0;
      virtual void printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) = 0;

      virtual void printLine(ustr_t msg) = 0;
      virtual void printLine(ustr_t msg, ustr_t arg) = 0;
      virtual void printLine(ustr_t msg, ustr_t arg1, ustr_t arg2) = 0;
      virtual void printLine(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) = 0;
      virtual void printLine(ustr_t msg, int arg1) = 0;
      virtual void printLine(ustr_t msg, int arg1, int arg2) = 0;
      virtual void printLine(ustr_t msg, int arg1, int arg2, int arg3) = 0;
      virtual void printLine(ustr_t msg, int arg1, int arg2, int arg3, ustr_t arg4) = 0; //version support print
      virtual void printLine(ustr_t msg, ustr_t arg1, int arg2, int arg3, ustr_t arg4) = 0;
      virtual void printPathLine(ustr_t msg, path_t arg) = 0;
      virtual void printPathLine(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) = 0;

      /**
       * indicates the continues process
       */
      virtual void showProgress() = 0;
      /**
       * indicates the end of the process
       */
      virtual void stopProgress() = 0;

      virtual ~PresenterBase() = default;
   };

   // --- WideMessage ---
   class WideMessage : public String<wide_c, MESSAGE_LEN>
   {
   public:
      wstr_t operator*() const { return wstr_t(_string); }

      void appendUstr(const char* s)
      {
         size_t len = length();

         size_t subLen = MESSAGE_LEN - length();
         StrConvertor::copy(_string + len, s, getlength(s), subLen);
         _string[len + subLen] = 0;
      }
      void appendUstr(const char* s, size_t str_len)
      {
         size_t len = length();

         size_t subLen = MESSAGE_LEN - length();
         StrConvertor::copy(_string + len, s, str_len, subLen);
         _string[len + subLen] = 0;
      }

      WideMessage()
      {
         _string[0] = 0;
      }
      WideMessage(const char* s)
      {
         size_t len = MESSAGE_LEN;
         StrConvertor::copy(_string, s, getlength(s), len);
         _string[len] = 0;
      }
      WideMessage(const char* s1, const char* s2)
      {
         size_t len = MESSAGE_LEN;
         size_t len2 = MESSAGE_LEN;
         StrConvertor::copy(_string, s1, getlength(s1), len);
         StrConvertor::copy(_string + len, s2, getlength(s2), len2);

         _string[len + len2] = 0;
      }
      WideMessage(const char* s1, const char* s2, const char* s3)
      {
         size_t len = MESSAGE_LEN;
         size_t len2 = MESSAGE_LEN;
         size_t len3 = MESSAGE_LEN;
         StrConvertor::copy(_string, s1, getlength(s1), len);
         StrConvertor::copy(_string + len, s2, getlength(s2), len2);
         StrConvertor::copy(_string + len + len2, s3, getlength(s3), len3);

         _string[len + len2 + len3] = 0;
      }
      WideMessage(const char* s1, const char* s2, const char* s3, const char* s4)
      {
         size_t len = MESSAGE_LEN;
         size_t len2 = MESSAGE_LEN;
         size_t len3 = MESSAGE_LEN;
         size_t len4 = MESSAGE_LEN;
         StrConvertor::copy(_string, s1, getlength(s1), len);
         StrConvertor::copy(_string + len, s2, getlength(s2), len2);
         StrConvertor::copy(_string + len + len2, s3, getlength(s3), len3);
         StrConvertor::copy(_string + len + len2 + len3, s4, getlength(s4), len4);

         _string[len + len2 + len3 + len4] = 0;
      }
      WideMessage(const wide_c* s)
      {
         copy(s);
      }
      WideMessage(const wide_c* s1, const wide_c* s2)
      {
         copy(s1);
         append(s2);
      }
   };


   // --- QuoteString ---
   class QuoteString : public String<char, LINE_LEN>
   {
   public:
      QuoteString(ustr_t s, pos_t len)
      {
         int mode = 0; // 1 - normal, 2 - character code
         size_t index = 0;
         for (size_t i = 0; i <= len; i++) {
            switch (mode) {
               case 0:
                  if (s[i] == '"') {
                     mode = 1;
                  }
                  else if (s[i] == '$') {
                     mode = 2;
                     index = i + 1;
                  }
                  break;
               case 1:
                  if (s[i]==0) {
                     mode = 0;
                  }
                  else if (s[i] == '"') {
                     if (s[i + 1] == '"')
                        append(s[i]);

                     mode = 0;
                  }
                  else append(s[i]);
                  break;
               case 2:
                  if ((s[i] < '0' || s[i] > '9') && (s[i] < 'A' || s[i] > 'F')  && (s[i] < 'a' || s[i] > 'f'))
                  {
                     String<char, 12> number(s + index, i - index);
                     unic_c ch = StrConvertor::toInt(number.str(), (s[i] == 'h') ? 16 : 10);

                     char temp[5];
                     size_t temp_len = 4;
                     StrConvertor::copy(temp, &ch, 1, temp_len);
                     append(temp, temp_len);

                     if (s[i] == '"') {
                        mode = 1;
                     }
                     else if (s[i] == '$') {
                        index = i + 1;
                        mode = 2;
                     }
                     else mode = 0;
                  }
                  break;
               default:
                  // to make compiler happy
                  break;
            }
         }
      }
   };

   // --- ReferenceProperName ---
   class ReferenceProperName : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      void copyName(ustr_t referenceName)
      {
         size_t pos = referenceName.findLast('\'', 0);
         copy(referenceName.str() + pos + 1);
      }

      ReferenceProperName(ustr_t referenceName)
      {
         copyName(referenceName);
      }
      ReferenceProperName()
      {
      }
   };

   typedef Pair<ref_t, ClassAttribute, 0, ClassAttribute::None> ClassAttributeKey;
   typedef MemoryMap<ClassAttributeKey, ref_t, Map_StoreKey<ClassAttributeKey>, Map_GetKey<ClassAttributeKey>> ClassAttributes;

#pragma pack(push, 1)
   // --- TypeInfo ---
   struct TypeInfo
   {
      ref_t typeRef;
      bool nillable;
      ref_t elementRef;
      bool nillableElement;

      bool isPrimitive() const
      {
         return isPrimitiveRef(typeRef);
      }

      bool operator ==(TypeInfo& val) const
      {
         return this->typeRef == val.typeRef && this->elementRef == val.elementRef;
      }

      bool operator !=(TypeInfo& val) const
      {
         return this->typeRef != val.typeRef || this->elementRef != val.elementRef;
      }

      TypeInfo()
      {
         typeRef = elementRef = 0;
         nillableElement = nillable = false;
      }
      TypeInfo(ref_t typeRef)
      {
         this->typeRef = typeRef;
         this->elementRef = 0;
         this->nillable = false;
         this->nillableElement = false;
      }
      TypeInfo(ref_t typeRef, ref_t elemantRef)
      {
         this->typeRef = typeRef;
         this->elementRef = elemantRef;
         this->nillable = false;
         this->nillableElement = false;
      }
      TypeInfo(ref_t typeRef, ref_t elemantRef, bool nillable)
      {
         this->typeRef = typeRef;
         this->elementRef = elemantRef;
         this->nillable = nillable;
         this->nillableElement = false;
      }
      TypeInfo(ref_t typeRef, ref_t elemantRef, bool nillable, bool nillableElement)
      {
         this->typeRef = typeRef;
         this->elementRef = elemantRef;
         this->nillable = nillable;
         this->nillableElement = nillableElement;
      }
   };

   // --- StaticFieldInfo ---
   struct StaticFieldInfo
   {
      int      offset   = 0;
      TypeInfo typeInfo = {};
      ref_t    valueRef = 0;
   };

   // --- ClassHeader ---
   struct ClassHeader
   {
      pos_t  staticSize;      // static table size
      ref_t  classRef;        // class class reference
      pos_t  count;
      pos_t  indexCount;
      ref_t  flags;
      ref_t  parentRef;

      ClassHeader() = default;
   };

   // --- MethodEntry ---
   struct MethodEntry
   {
      mssg_t message;
      pos_t  codeOffset;
      ref_t  outputRef;

      MethodEntry() = default;

      MethodEntry(mssg_t message, pos_t codeOffset)
         : message(message), codeOffset(codeOffset), outputRef(0)
      {
      }
      MethodEntry(mssg_t message, pos_t codeOffset, ref_t outputRef)
         : message(message), codeOffset(codeOffset), outputRef(outputRef)
      {
      }
   };
#pragma pack(pop)

   // --- InternalStrError ---
   struct InternalStrError : ExceptionBase
   {
      int              messageCode;
      IdentifierString arg;

      InternalStrError(int messageCode)
      {
         this->messageCode = messageCode;
      }

      InternalStrError(int messageCode, ustr_t arg)
      {
         this->messageCode = messageCode;
         this->arg.copy(arg);
      }
   };

   // --- JITUnresolvedException ---
   struct JITUnresolvedException : ExceptionBase
   {
      ReferenceInfo referenceInfo;

      JITUnresolvedException(ReferenceInfo referenceInfo)
      {
         this->referenceInfo = referenceInfo;
      }
   };

   // --- IdentifierTextReader ---
   typedef StringTextReader<char> IdentifierStringReader;

   // --- IdentifierReader ---
   typedef TextReader<char> UStrReader;

   // --- ReferenceHelper ---

   class MapHelper
   {
   public:
      template<class Map, class T> static void readStringMap(StreamReader* reader, Map& map)
      {
         pos_t counter = reader->getPos();
         IdentifierString temp;
         while (counter > 0) {
            reader->readString(temp);

            T value = map.DefaultValue();
            reader->read(&value, sizeof(T));

            map.add(*temp, value);

            counter--;
         }
      }
      template<class Map, class T> static void readReferenceMap(StreamReader* reader, Map& map)
      {
         pos_t counter = reader->getPos();
         IdentifierString temp;
         while (counter > 0) {
            ref_t reference = reader->getRef();

            T value = map.DefaultValue();
            reader->read(&value, sizeof(T));

            map.add(reference, value);

            counter--;
         }
      }

      template<class Map, class T> static void readReferenceMap64(StreamReader* reader, Map& map)
      {
         pos_t counter = reader->getPos();
         IdentifierString temp;
         while (counter > 0) {
            ref64_t reference = reader->getRef64();

            T value = map.DefaultValue();
            reader->read(&value, sizeof(T));

            map.add(reference, value);

            counter--;
         }
      }

      template<class Map, class T> static void writeRelocationMap(StreamWriter* writer, Map& map)
      {
         writer->writePos(map.count());
         for (auto it = map.start(); !it.eof(); ++it) {
            ustr_t key = it.key();
            if (key.empty()) {
               writer->writeChar(0);
            }
            else writer->writeString(key);

            writer->write(&(*it), sizeof(T));
         }
      }

      template<class Map, class T> static void writeReferenceMap(StreamWriter* writer, Map& map)
      {
         writer->writePos(map.count());
         for (auto it = map.start(); !it.eof(); ++it) {
            writer->writeRef(it.key());
            writer->write(&(*it), sizeof(T));
         }
      }

      template<class Map, class T> static void writeReferenceMap64(StreamWriter* writer, Map& map)
      {
         writer->writePos(map.count());
         for (auto it = map.start(); !it.eof(); ++it) {
            writer->writeRef64(it.key());
            writer->write(&(*it), sizeof(T));
         }
      }
   };

} // _ELENA_

#endif // ELENA_H

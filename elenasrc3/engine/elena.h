//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENA_H
#define ELENA_H

#include "common.h"
#include "elenaconst.h"
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
   typedef Map<ustr_t, ref_t, allocUStr, freeUStr>    ReferenceMap;
   typedef Map<ref64_t, ref_t>                        ActionMap;
   typedef Map<ustr_t, addr_t, allocUStr, freeUStr>   AddressMap;
   typedef Map<mssg_t, ExtensionInfo>                 ExtensionMap;
   typedef Map<ref_t, ref_t>                          ResolvedMap;
   typedef Map<int, addr_t>                           FieldAddressMap;

   // --- Maps ---
   typedef List<ustr_t, freeUStr>                     IdentifierList;

   // --- Tuples ---

   // --- ModuleBase ---
   class ModuleBase
   {
   public:
      virtual ustr_t name() const = 0;

      virtual ustr_t resolveReference(ref_t reference) = 0;
      virtual size_t resolveSignature(ref_t signature, ref_t* references) = 0;
      virtual ustr_t resolveAction(ref_t reference, ref_t& signature) = 0;
      virtual ustr_t resolveConstant(ref_t reference) = 0;

      virtual ref_t mapReference(ustr_t referenceName) = 0;
      virtual ref_t mapReference(ustr_t referenceName, bool existing) = 0;

      virtual void mapPredefinedReference(ustr_t referenceName, ref_t reference) = 0;

      virtual ref_t mapSignature(ref_t* references, size_t length, bool existing) = 0;
      virtual ref_t mapAction(ustr_t actionName, ref_t signature, bool existing) = 0;
      virtual ref_t mapConstant(ustr_t reference) = 0;

      virtual MemoryBase* mapSection(ref_t reference, bool existing) = 0;

      virtual void forEachReference(void* arg, void(*lambda)(ModuleBase*, ref_t, void*)) = 0;

      virtual ~ModuleBase() = default;
   };

   typedef Map<ustr_t, ModuleBase*, allocUStr, freeUStr, freeobj> ModuleMap;

   // --- ImageProviderBase ---
   class ImageProviderBase
   {
   public:
      virtual AddressMap::Iterator externals() = 0;

      virtual MemoryBase* getTextSection() = 0;
      virtual MemoryBase* getRDataSection() = 0;
      virtual MemoryBase* getMDataSection() = 0;
      virtual MemoryBase* getMBDataSection() = 0;
      virtual MemoryBase* getImportSection() = 0;
      virtual MemoryBase* getDataSection() = 0;
      virtual MemoryBase* getStatSection() = 0;

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
            default:
               return nullptr;
         }
      }

      virtual MemoryBase* getTargetDebugSection() = 0;

      virtual addr_t getEntryPoint() = 0;
      virtual addr_t getDebugEntryPoint() = 0;

      virtual ~ImageProviderBase() = default;
   };

   // --- ReferenceInfo ---
   struct ReferenceInfo
   {
      ModuleBase* module;
      ustr_t      referenceName; // when module is not null - referenceName is weak one

      bool isRelative()
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

   // --- ModuleLoaderBase ---
   struct SectionInfo
   {
      ModuleBase* module;
      MemoryBase* section;
      ref_t       reference;

      SectionInfo()
      {
         module = nullptr;
         section = nullptr;
         reference = 0;
      }
      SectionInfo(MemoryBase* section)
      {
         this->module = nullptr;
         this->section = section;
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
   };

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
      virtual SectionInfo getSection(ReferenceInfo referenceInfo, ref_t mask, bool silentMode) = 0;
      virtual ClassSectionInfo getClassSections(ReferenceInfo referenceInfo, ref_t vmtMask, ref_t codeMask, 
         bool silentMode) = 0;

      virtual ModuleInfo getModule(ReferenceInfo referenceInfo, bool silentMode) = 0;
      virtual ModuleInfo getWeakModule(ustr_t weakReferenceName, bool silentMode) = 0;

      virtual void resolvePath(ustr_t ns, PathString& path) = 0;
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

      virtual ref_t importSignature(ModuleBase* referenceModule, ref_t signRef) = 0;
      virtual ref_t importMessage(ModuleBase* referenceModule, mssg_t message) = 0;
      virtual ref_t importReference(ModuleBase* referenceModule, ustr_t referenceName) = 0;
      virtual ref_t importReference(ModuleBase* referenceModule, ref_t reference) = 0;
      virtual ref_t importConstant(ModuleBase* referenceModule, ref_t reference) = 0;
      virtual ref_t importExternal(ModuleBase* referenceModule, ref_t reference) = 0;
      virtual ref_t importMessageConstant(ModuleBase* referenceModule, ref_t reference) = 0;

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

      virtual void raiseInternalError(int code) = 0;
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
      virtual void writeVMTMethodReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp, mssg_t message,
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

      virtual mssg_t importMessage(mssg_t message, ModuleBase* module = nullptr) = 0;

      virtual addr_t resolveMDataVAddress() = 0;

      virtual void resolveLabel(MemoryWriter& writer, ref_t mask, pos_t position) = 0;
   };

   // --- JITSettings ---
   struct JITSettings
   {
      pos_t    mgSize;
      pos_t    ygSize;
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

      virtual void writeLabelAddress(pos_t label, MemoryWriter& writer, ref_t mask) = 0;
   };

   // --- JITCompilerBase ---
   class JITCompilerBase
   {
   public:
      virtual void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings) = 0;

      virtual bool isWithDebugInfo() = 0;

      virtual void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) = 0;

      virtual void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) = 0;
      virtual void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) = 0;

      virtual void compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length) = 0;

      virtual pos_t getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed = false) = 0;

      virtual pos_t getVMTLength(void* targetVMT) = 0;
      virtual addr_t findMethodAddress(void* entries, mssg_t message) = 0;
      virtual pos_t findMethodOffset(void* entries, mssg_t message) = 0;

      virtual void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticLength) = 0;
      virtual void addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount) = 0;
      virtual void updateVMTHeader(MemoryWriter& vmtWriter, addr_t parentAddress, addr_t classClassAddress, 
         ref_t flags, pos_t count, FieldAddressMap& staticValues, bool virtualMode) = 0;
      virtual pos_t copyParentVMT(void* parentVMT, void* targetVMT) = 0;

      virtual void allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length, 
         bool structMode, bool virtualMode) = 0;
      virtual void writeInt32(MemoryWriter& writer, unsigned int value) = 0;
      virtual void writeInt64(MemoryWriter& writer, unsigned long long value) = 0;
      virtual void writeFloat64(MemoryWriter& writer, double value) = 0;
      virtual void writeLiteral(MemoryWriter& writer, ustr_t value) = 0;
      virtual void writeWideLiteral(MemoryWriter& writer, wstr_t value) = 0;
      virtual void writeChar32(MemoryWriter& writer, ustr_t value) = 0;
      virtual void writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo) = 0;
      virtual void writeDump(MemoryWriter& writer, SectionInfo* sectionInfo) = 0;
      virtual void writeVariable(MemoryWriter& writer) = 0;
      virtual void writeMessage(MemoryWriter& writer, mssg_t message) = 0;

      virtual void addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode) = 0;
      virtual void addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode) = 0;

      virtual pos_t addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode) = 0;
      virtual pos_t addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, 
         ustr_t actionName, ref_t weakActionRef, ref_t signature, bool virtualMode) = 0;

      virtual void writeImm9(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm12(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm16(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm16Hi(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm32(MemoryWriter* writer, int value) = 0;

      virtual void resolveLabelAddress(MemoryWriter* writer, ref_t mask, pos_t position, bool virtualMode) = 0;

      virtual void populatePreloaded(uintptr_t env, uintptr_t eh_table, uintptr_t gc_table) = 0;

      virtual void updateEnvironment(MemoryBase* rdata, pos_t staticCounter, bool virtualMode) = 0;

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


   // --- IdentifierString ---
   class IdentifierString : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      bool compare(ustr_t s)
      {
         return s.compare(_string);
      }

      ref_t toRef(int radix = 10) const
      {
         return StrConvertor::toUInt(_string, radix);
      }

      IdentifierString() = default;

      IdentifierString(ustr_t s)
         : String(s)
      {

      }
      IdentifierString(ustr_t s, size_t length)
         : String(s, length)
      {
      }
      IdentifierString(ustr_t s1, ustr_t s2)
         : String(s1)
      {
         append(s2);
      }

      IdentifierString(ustr_t s1, ustr_t s2, ustr_t s3)
         : String(s1)
      {
         append(s2);
         append(s3);
      }

      IdentifierString(wstr_t s)
      {
         size_t len = IDENTIFIER_LEN;
         StrConvertor::copy(_string, s, getlength(s), len);
         _string[len] = 0;
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

   // --- NamespaceString ---
   class NamespaceString : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      static bool isIncluded(ustr_t packageNs, ustr_t ns)
      {
         size_t length = packageNs.length();
         if (ns.length() <= length) {
            return packageNs.compare(ns);
         }
         else if (ns[length] == '\'') {
            return packageNs.compare(ns, length);
         }
         else return false;
      }

      static bool compareNs(ustr_t referenceName, ustr_t ns)
      {
         size_t pos = referenceName.findLast('\'', 0);
         if (pos == 0 && ns.length() == 0)
            return true;
         else if (ns.length() == pos) {
            return referenceName.compare(ns, pos);
         }
         else return false;
      }

      void trimLastSubNs()
      {
         size_t index = (**this).findLast('\'', 0);
         _string[index] = 0;
      }

      NamespaceString(ustr_t rootNs, ustr_t referenceName)
      {
         copy(rootNs);
         if (referenceName[0] != '\'')
            append('\'');

         size_t pos = referenceName.findLast('\'', 0);
         append(referenceName, pos);
      }
      NamespaceString(ustr_t referenceName)
      {
         size_t pos = referenceName.findLast('\'', 0);
         copy(referenceName, pos);
      }
   };

   // --- ReferenceProperName ---
   class ReferenceProperName : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      ReferenceProperName(ustr_t referenceName)
      {
         size_t pos = referenceName.findLast('\'', 0);
         copy(referenceName.str() + pos + 1);
      }
   };

   // --- ReferenceName ---
   class ReferenceName : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      static void copyProperName(ReferenceName& target, ustr_t referenceName)
      {
         size_t pos = referenceName.findLast('\'') + 1;

         target.copy(referenceName + pos);
      }

      static void nameToPath(PathString& path, ustr_t name)
      {
         PathString subPath;

         bool stopped = false;
         bool rootNs = true;
         while (!stopped) {
            size_t pos = name.find('\'');
            if (pos == NOTFOUND_POS) {
               pos = name.length();
               stopped = true;
            }
            subPath.copy(name, pos);

            if (rootNs) {
               path.combine(*subPath);
               rootNs = false;
            }
            else path.appendSubName(*subPath, subPath.length());

            name += pos + 1;
         }
      }

      void pathToName(path_t path)
      {
         char buf[IDENTIFIER_LEN];
         size_t bufLen = IDENTIFIER_LEN;

         while (!path.empty()) {
            if (!empty())
               append('\'');

            size_t pos = path.find(PATH_SEPARATOR);
            if (pos != NOTFOUND_POS) {
               bufLen = IDENTIFIER_LEN;
               path.copyTo(buf, pos, bufLen);

               append(buf, bufLen);
               path += pos + 1u;
            }
            else {
               pos = path.findLast('.');
               if (pos == NOTFOUND_POS)
                  pos = path.length();

               bufLen = IDENTIFIER_LEN;
               path.copyTo(buf, pos, bufLen);

               // replace dots with apostrophes
               for (size_t i = 0; i < bufLen; i++) {
                  if (buf[i] == '.')
                     buf[i] = '\'';
               }

               append(buf, bufLen);

               break;
            }
          }
      }

      bool combine(ustr_t name)
      {
         if (!name.empty()) {
            append('\'');
            return append(name);
         }
         else return true;
      }

      ReferenceName()
         : String<char, IDENTIFIER_LEN>()
      {

      }
      ReferenceName(ustr_t name)
      {
         copy(name);
      }
      ReferenceName(ustr_t rootNs, ustr_t name)
      {
         copy(rootNs);
         combine(name);
      }
   };

   typedef Pair<ref_t, ClassAttribute, 0, ClassAttribute::None> ClassAttributeKey;
   typedef MemoryMap<ClassAttributeKey, ref_t, Map_StoreKey<ClassAttributeKey>, Map_GetKey<ClassAttributeKey>> ClassAttributes;

#pragma pack(push, 1)
   // --- TypeInfo ---
   struct TypeInfo
   {
      ref_t typeRef;
      ref_t elementRef;

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
   };

   // --- FieldInfo ---
   struct FieldInfo
   {
      int      offset;
      TypeInfo typeInfo;
   };

   // --- StaticFieldInfo ---
   struct StaticFieldInfo
   {
      int      offset;
      TypeInfo typeInfo;
      ref_t    valueRef;
   };

   // --- MethodInfo ---
   struct MethodInfo
   {
      bool   inherited;
      ref_t  hints;
      ref_t  outputRef;
      mssg_t multiMethod;
      mssg_t byRefHandler;

      MethodInfo()
      {
         inherited = false;
         hints = 0;
         outputRef = 0;
         multiMethod = 0;
         byRefHandler = 0;
      }
      MethodInfo(bool inherited, ref_t hints, ref_t outputRef, mssg_t multiMethod, mssg_t byRefHandler) :
         inherited(inherited),
         hints(hints),
         outputRef(outputRef),
         multiMethod(multiMethod),
         byRefHandler(byRefHandler)
      {
      }
   };

   // --- ClassHeader ---
   struct ClassHeader
   {
      pos_t  staticSize;      // static table size
      ref_t  classRef;        // class class reference
      pos_t  count;
      ref_t  flags;
      ref_t  parentRef;
   };

   // --- MethodEntry ---
   struct MethodEntry
   {
      mssg_t message;
      pos_t  codeOffset;
   };

   // --- DebugLineInfo ---

   struct DebugLineInfo
   {
      DebugSymbol symbol;
      int         col, row/*, length*/;
      union
      {
         struct Source { addr_t nameRef; } source;
         struct Module { addr_t nameRef; int flags; } classSource;
         struct Step { addr_t address; } step;
         struct Local { addr_t nameRef; int offset; } local;
         struct Field { addr_t nameRef; int offset; } field;
         struct Offset { pos_t disp; } offset;
      } addresses;

      DebugLineInfo()
      {
         symbol = DebugSymbol::None;
         col = row = /*length = */0;

         this->addresses.classSource.nameRef = 0;
         this->addresses.classSource.flags = 0;
      }
      DebugLineInfo(DebugSymbol symbol)
      {
         this->symbol = symbol;
         this->col = 0;
         this->row = 0;
         //this->length = length;

         this->addresses.classSource.nameRef = 0;
         this->addresses.classSource.flags = 0;
      }
      DebugLineInfo(DebugSymbol symbol, int col, int row/*, int length*/)
      {
         this->symbol = symbol;
         this->col = col;
         this->row = row;
         //this->length = length;

         this->addresses.classSource.nameRef = 0;
         this->addresses.classSource.flags = 0;
      }
   };
#pragma pack(pop)

   // --- SymbolInfo ---
   enum class SymbolType : int
   {
      Symbol = 0,
      Singleton,
      Constant,
   };

   struct SymbolInfo
   {
      SymbolType symbolType;
      ref_t      valueRef;
      ref_t      typeRef;

      void load(StreamReader* reader)
      {
         symbolType = (SymbolType)reader->getDWord();
         valueRef = reader->getRef();
         typeRef = reader->getRef();
      }

      void save(StreamWriter* writer)
      {
         writer->writeDWord((unsigned int)symbolType);
         writer->writeRef(valueRef);
         writer->writeRef(typeRef);
      }
   };

   // --- ClassInfo ---
   struct ClassInfo
   {
      typedef MemoryMap<mssg_t, MethodInfo, Map_StoreUInt, Map_GetUInt> MethodMap;
      typedef MemoryMap<ustr_t, FieldInfo, Map_StoreUStr, Map_GetUStr> FieldMap;
      typedef MemoryMap<ustr_t, StaticFieldInfo, Map_StoreUStr, Map_GetUStr> StaticFieldMap;

      ClassHeader     header;
      pos_t           size;           // Object size
      MethodMap       methods;
      FieldMap        fields;
      StaticFieldMap  statics;
      ClassAttributes attributes;

      static void loadStaticFields(StreamReader* reader, StaticFieldMap& statics)
      {
         pos_t statCount = reader->getPos();
         for (pos_t i = 0; i < statCount; i++) {
            IdentifierString fieldName;
            reader->readString(fieldName);
            StaticFieldInfo fieldInfo;
            reader->read(&fieldInfo, sizeof(fieldInfo));

            statics.add(*fieldName, fieldInfo);
         }
      }

      static void saveStaticFields(StreamWriter* writer, StaticFieldMap& statics)
      {
         writer->writePos(statics.count());
         statics.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t name, StaticFieldInfo info)
            {
               writer->writeString(name);
               writer->write(&info, sizeof(info));
            });
      }

      void save(StreamWriter* writer, bool headerAndSizeOnly = false)
      {
         writer->write(&header, sizeof(ClassHeader));
         writer->writeDWord(size);
         if (!headerAndSizeOnly) {
            writer->writePos(fields.count());
            fields.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t name, FieldInfo info)
               {
                  writer->writeString(name);
                  writer->write(&info, sizeof(info));
               });

            writer->writePos(methods.count());
            methods.forEach<StreamWriter*>(writer, [](StreamWriter* writer, mssg_t message, MethodInfo info)
               {
                  writer->writeDWord(message);
                  writer->write(&info, sizeof(info));
               });

            writer->writePos(attributes.count());
            attributes.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ClassAttributeKey key, ref_t reference)
               {
                  writer->write(&key, sizeof(key));
                  writer->writeRef(reference);
               });

            saveStaticFields(writer, statics);
         }
      }

      void load(StreamReader* reader, bool headerAndSizeOnly = false, bool fieldsOnly = false)
      {
         reader->read(&header, sizeof(ClassHeader));
         size = reader->getDWord();
         if (!headerAndSizeOnly) {
            pos_t fieldCount = reader->getPos();
            for (pos_t i = 0; i < fieldCount; i++) {
               IdentifierString fieldName;
               reader->readString(fieldName);
               FieldInfo fieldInfo;
               reader->read(&fieldInfo, sizeof(fieldInfo));

               fields.add(*fieldName, fieldInfo);
            }

            if (!fieldsOnly) {
               pos_t methodsCount = reader->getPos();
               for (pos_t i = 0; i < methodsCount; i++) {
                  mssg_t message = reader->getDWord();
                  MethodInfo methodInfo;
                  reader->read(&methodInfo, sizeof(MethodInfo));

                  methods.add(message, methodInfo);
               }
               pos_t attrCount = reader->getPos();
               for (pos_t i = 0; i < attrCount; i++) {
                  ClassAttributeKey key;
                  reader->read(&key, sizeof(key));

                  ref_t reference = reader->getRef();

                  attributes.add(key, reference);
               }

               loadStaticFields(reader, statics);
            }
         }
      }

      ClassInfo() :
         header({}),
         size(0),
         methods({}),
         fields({ -1 }),
         statics({ -1 }),
         attributes(0)
      {
         //header.staticSize = 0;
         //header.parentRef = header.classRef = 0;
         //header.flags = 0;
         //header.count = size = 0;
      }
   };

   // --- ExceptionBase ---
   class ExceptionBase {};

   class AbortError : ExceptionBase {};

   // --- InternalError ---
   struct InternalError : ExceptionBase
   {
      int messageCode;
      int arg;

      InternalError(int messageCode)
      {
         this->messageCode = messageCode;
         this->arg = 0;
      }

      InternalError(int messageCode, int arg)
      {
         this->messageCode = messageCode;
         this->arg = arg;
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

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                             (C)2021-2022, by Aleksey Rakov
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

   // --- Misc types ---
   typedef unsigned int parse_key_t;

   // --- Maps ---
   typedef Map<ustr_t, ref_t, allocUStr, freeUStr>    ReferenceMap;
   typedef Map<ref64_t, ref_t>                        ActionMap;
   typedef Map<ustr_t, addr_t, allocUStr, freeUStr>   AddressMap;

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

      virtual Section* getTextSection() = 0;
      virtual Section* getRDataSection() = 0;
      virtual Section* getMDataSection() = 0;
      virtual Section* getMBDataSection() = 0;
      virtual Section* getImportSection() = 0;
      virtual Section* getDataSection() = 0;

      virtual Section* getTargetSection(ref_t targetMask) = 0;
      virtual Section* getTargetDebugSection() = 0;

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
   };

   // --- ReferenceMapperBase ---
   class ReferenceMapperBase
   {
   protected:
      virtual List<LazyReferenceInfo>::Iterator lazyReferences() = 0;

   public:
      virtual addr_t resolveReference(ReferenceInfo referenceInfo, ref_t sectionMask) = 0;

      virtual void mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t sectionMask) = 0;

      virtual ref_t resolveAction(ustr_t actionName, ref_t signRef) = 0;
      virtual void mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef) = 0;

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

      virtual void writeReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp,
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
   };

   // --- JITSettings ---
   struct JITSettings
   {
      pos_t    mgSize;
      pos_t    ygSize;
   };

   // --- JITCompilerBase ---
   class JITCompilerBase
   {
   public:
      virtual void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         JITSettings settings) = 0;

      virtual bool isWithDebugInfo() = 0;

      virtual void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) = 0;

      virtual void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter) = 0;
      virtual void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter) = 0;

      virtual void compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length) = 0;

      virtual void allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength) = 0;
      virtual void addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount) = 0;
      virtual void updateVMTHeader(MemoryWriter& vmtWriter, addr_t parentAddress, addr_t classClassAddress, 
         ref_t flags, pos_t count, bool virtualMode) = 0;
      virtual pos_t copyParentVMT(void* parentVMT, void* targetVMT) = 0;

      virtual void allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length, 
         bool structMode, bool virtualMode) = 0;
      virtual void writeInt32(MemoryWriter& writer, unsigned int value) = 0;

      virtual void addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode) = 0;
      virtual void addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode) = 0;

      virtual pos_t addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, bool virtualMode) = 0;
      virtual pos_t addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, 
         ustr_t actionName, ref_t weakActionRef, ref_t signature) = 0;

      virtual void writeImm9(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm12(MemoryWriter* writer, int value, int type) = 0;
      virtual void writeImm16(MemoryWriter* writer, int value, int type) = 0;

      virtual ~JITCompilerBase() = default;
   };

   // --- WideMessage ---
   class WideMessage : public String<wide_c, MESSAGE_LEN>
   {
   public:
      WideMessage(const char* s)
      {
         size_t len = MESSAGE_LEN;
         StrConvertor::copy(_string, s, getlength(s), len);
         _string[len] = 0;
      }
   };

   // --- IdentifierString ---
   class IdentifierString : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

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
         for (size_t i = 0; i <= len; i++) {
            switch (mode) {
               case 0:
                  if (s[i] == '"') {
                     mode = 1;
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

#pragma pack(push, 1)
   // --- FieldInfo ---
   struct FieldInfo
   {
      int   offset;
      ref_t typeRef;
      ref_t elementRef;
   };

   // --- MethodInfo ---
   struct MethodInfo
   {
      bool  inherited;
      ref_t hints;

      MethodInfo()
      {
         inherited = false;
         hints = 0;
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
         struct Source { pos_t nameRef; } source;
         struct Module { pos_t nameRef; int flags; } classSource;
         struct Step { addr_t address; } step;
      //   struct Local { pos_t nameRef; int level; } local;
      //   struct Field { pos_t nameRef; int size; } field;
      //   struct Offset { pos_t disp; } offset;
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

   // --- ClassInfo ---
   struct ClassInfo
   {
      typedef MemoryMap<mssg_t, MethodInfo, Map_StoreUInt, Map_GetUInt> MethodMap;
      typedef MemoryMap<ustr_t, FieldInfo, Map_StoreUStr, Map_GetUStr> FieldMap;

      ClassHeader     header;
      pos_t           size;           // Object size
      MethodMap       methods;
      FieldMap        fields;

      void save(StreamWriter* writer, bool headerAndSizeOnly = false)
      {
         writer->write(&header, sizeof(ClassHeader));
         writer->writeDWord(size);
         if (!headerAndSizeOnly) {
            writer->writePos(methods.count());
            methods.forEach<StreamWriter*>(writer, [](StreamWriter* writer, mssg_t message, MethodInfo info)
               {
                  writer->writeDWord(message);
                  writer->write(&info, sizeof(info));
               });

            writer->writePos(fields.count());
            fields.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t name, FieldInfo info)
               {
                  writer->writeString(name);
                  writer->write(&info, sizeof(info));
               });
         }
      }

      void load(StreamReader* reader, bool headerAndSizeOnly = false)
      {
         reader->read(&header, sizeof(ClassHeader));
         size = reader->getDWord();
         if (!headerAndSizeOnly) {
            pos_t methodsCount = reader->getPos();
            for (pos_t i = 0; i < methodsCount; i++) {
               mssg_t message = reader->getDWord();
               MethodInfo methodInfo;
               reader->read(&methodInfo, sizeof(MethodInfo));

               methods.add(message, methodInfo);
            }
            pos_t fieldCount = reader->getPos();
            for (pos_t i = 0; i < fieldCount; i++) {
               IdentifierString fieldName;
               reader->readString(fieldName);
               FieldInfo fieldInfo;
               reader->read(&fieldInfo, sizeof(fieldInfo));

               fields.add(*fieldName, fieldInfo);
            }
         }
      }

      ClassInfo()
         : methods({}), fields({})
      {
         header.staticSize = 0;
         header.parentRef = header.classRef = 0;
         header.flags = 0;
         header.count = size = 0;
      }
   };

   // --- ExceptionBase ---
   class ExceptionBase {};

   class AbortError : ExceptionBase {};

   // --- InternalError ---
   struct InternalError : ExceptionBase
   {
      int messageCode;

      InternalError(int messageCode)
      {
         this->messageCode = messageCode;
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
            writer->writeString(it.key());
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

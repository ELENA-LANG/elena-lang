//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenaH
#define elenaH 1

#include "common.h"
#include "elenaconst.h"
#include "section.h"

namespace _ELENA_
{

// --- _Module interface ---

class _Module
{
public:
   virtual ident_t Name() const = 0;

   virtual ident_t resolveReference(ref_t reference) = 0;
   virtual ident_t resolveAction(ref_t reference, ref_t& signature) = 0;
   virtual size_t resolveSignature(ref_t signature, ref_t* references) = 0;
   virtual ident_t resolveConstant(ref_t reference) = 0;

   virtual ref_t mapReference(ident_t reference) = 0;
   virtual ref_t mapReference(ident_t reference, bool existing) = 0;

   virtual ref_t mapSignature(ref_t* references, size_t length, bool existing) = 0;
   virtual ref_t mapAction(ident_t actionName, ref_t signature, bool existing) = 0;
   virtual ref_t mapConstant(ident_t reference) = 0;

   virtual void mapPredefinedReference(ident_t name, ref_t reference) = 0;
   virtual void mapPredefinedAction(ident_t name, ref_t reference, ref_t signature) = 0;

   virtual _Memory* mapSection(ref_t reference, bool existing) = 0;

   virtual bool save(StreamWriter& writer) = 0;

   virtual ~_Module() {}
};

// --- _LibraryManager ---

class _LibraryManager
{
public:
   virtual _Module* resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference) = 0;
   virtual _Module* resolveDebugModule(ident_t referenceName, LoadResult& result, ref_t& reference) = 0;
};

// -- ReferenceInfo ---

struct ReferenceInfo
{
   _Module* module;
   ident_t  referenceName; // when module is not null - referenceName is weak one

   bool isRelative() const
   {
      return module != NULL && isWeakReference(referenceName);
   }

   ReferenceInfo()
   {
      this->module = NULL;
      this->referenceName = NULL;
   }
   ReferenceInfo(ident_t referenceName)
   {
      this->module = NULL;
      this->referenceName = referenceName;
   }
   ReferenceInfo(_Module* module, ident_t referenceName)
   {
      this->module = module;
      this->referenceName = referenceName;
   }
};

// --- SectionInfo ---

struct SectionInfo
{
   _Module* module;
   _Memory* section;
   _Memory* attrSection;

   SectionInfo()
   {
      module = nullptr;
      attrSection = section = nullptr;
   }
   SectionInfo(_Module* module, _Memory* section, _Memory* attrSection)
   {
      this->module = module;
      this->section = section;
      this->attrSection = attrSection;
   }
};

// --- ClassSectionInfo ---

struct ClassSectionInfo
{
   _Module* module;
   _Memory* codeSection;
   _Memory* vmtSection;
   _Memory* attrSection;

   ClassSectionInfo()
   {
      module = nullptr;
      codeSection = vmtSection = nullptr;
      attrSection = nullptr;
   }
};

// --- _LoaderListener ---

class _JITLoaderListener
{
public:
   virtual void onModuleLoad(_Module*) = 0;
};

// --- _JITLoader ---

class _JITLoader
{
public:
   virtual _Memory* getTargetSection(ref_t mask) = 0;

   virtual _Memory* getTargetDebugSection() = 0;

   virtual SectionInfo getSectionInfo(ReferenceInfo referenceInfo, ref_t mask, bool silentMode) = 0;
   virtual SectionInfo getCoreSectionInfo(ref_t reference, ref_t mask) = 0;
   virtual ClassSectionInfo getClassSectionInfo(ReferenceInfo referenceInfo, ref_t codeMask, ref_t vmtMask, bool silentMode) = 0;

   virtual pos_t getLinkerConstant(int id) = 0;

   virtual ident_t getLiteralClass() = 0;
   virtual ident_t getWideLiteralClass() = 0;
   virtual ident_t getCharacterClass() = 0;
   virtual ident_t getIntegerClass() = 0;
   virtual ident_t getRealClass() = 0;
   virtual ident_t getLongClass() = 0;
   virtual ident_t getMessageClass() = 0;
   virtual ident_t getExtMessageClass() = 0;
   virtual ident_t getMessageNameClass() = 0;
   virtual ident_t getNamespace() = 0;

   virtual ReferenceInfo retrieveReference(_Module* module, ref_t reference, ref_t mask) = 0;

   virtual lvaddr_t resolveReference(ReferenceInfo referenceInfo, ref_t mask) = 0;

//   //virtual void mapPredefinedAction(ident_t name, ref_t reference) = 0;

   virtual void mapReference(ReferenceInfo referenceInfo, lvaddr_t vaddress, ref_t mask) = 0;

   virtual void addListener(_JITLoaderListener* listener) = 0;

   virtual ~_JITLoader() {}
};

// --- IdentifierString ---

class IdentifierString : public String<char, IDENTIFIER_LEN>
{
public:
   operator ident_t() const { return _string; }

   static char* clonePath(path_t value)
   {
      char buf[IDENTIFIER_LEN];
      size_t length = IDENTIFIER_LEN;
      value.copyTo(buf, length);
      buf[length] = 0;

      return ((ident_t)buf).clone();
   }

   ident_t ident() const
   {
      return ident_t(_string);
   }

   bool compare(ident_t value)
   {
      return ((ident_t)_string).compare(value);
   }

   bool compare(ident_t value, size_t length)
   {
      return ((ident_t)_string).compare(value, length);
   }

   char* clone()
   {
      return ((ident_t)_string).clone();
   }

   char* clone(size_t index)
   {
      return ((ident_t)_string).clone(index);
   }

   void copyWideStr(const wide_c* value)
   {
      size_t length = IDENTIFIER_LEN;
      ((wide_t)value).copyTo(_string, getlength(value), length);
      _string[length] = 0;
   }

   IdentifierString()
   {
   }
   IdentifierString(const char* value)
      : String(value)
   {
   }
   IdentifierString(ident_t value, size_t length)
      : String(value, length)
   {
   }
   IdentifierString(ident_t value, size_t index, size_t length)
      : String(value, index, length)
   {
   }
   IdentifierString(ident_t value1, ident_t value2)
      : String(value1, value2)
   {
   }
   IdentifierString(ident_t value1, ident_t value2, ident_t value3)
      : String(value1, value2, value3)
   {
   }
   IdentifierString(ident_t value1, ident_t value2, ident_t value3, ident_t value4)
      : String(value1, value2, value3, value4)
   {
   }
   //   IdentifierString(const wide_c* value, size_t sourLength)
   //   {
   //      size_t length = IDENTIFIER_LEN;
   //      StringHelper::copy(_string, value, sourLength, length);
   //      _string[length] = 0;
   //   }
   IdentifierString(const wide_c* value)
   {
      size_t length = IDENTIFIER_LEN;
      ((wide_t)value).copyTo(_string, getlength(value), length);
      _string[length] = 0;
   }
};

// --- ReferenceName ---

class ReferenceName : public IdentifierString
{
public:
   ReferenceName()
   {
   }
   ReferenceName(ident_t reference)
   {
      copy(reference + reference.findLast('\'') + 1);
   }
   ReferenceName(ident_t reference, ident_t package)
   {
      size_t length = getlength(package);

      if (reference.compare(package, length) && reference[length] == '\'') {
         copy(reference + length + 1);
      }
      else copy(reference + reference.findLast('\'') + 1);
   }
};

// --- NamespaceName ---

class NamespaceName : public IdentifierString
{
public:
   static bool isIncluded(ident_t root, ident_t ns)
   {
      size_t length = getlength(root);
      if (getlength(ns) <= length) {
         return root.compare(ns);
      }
      else if (ns[length] == '\'') {
         return root.compare(ns, length);
      }
      else return false;
   }

   NamespaceName(ident_t reference)
   {
      size_t pos = reference.findLast('\'', 0);
      copy(reference, pos);
   }
   NamespaceName(ident_t root, ident_t reference)
   {
      size_t pos = reference.findLast('\'', 0);

      if (reference[0] != '\'') {
         copy(reference, pos);
      }
      else {
         copy(root);
         append(reference, pos);
      }
   }

   static bool compare(ident_t reference, ident_t ns)
   {
      size_t pos = reference.findLast('\'', 0);
      if (pos == 0 && getlength(ns) == 0)
         return true;
      else if (getlength(ns) == pos) {
         return reference.compare(ns, pos);
      }
      else return false;
   }

   bool compare(ident_t reference)
   {
      return NamespaceName::compare(reference, _string);
   }

   static bool isRelativeSubnamespace(ident_t reference)
   {
      if (reference[0] == '\'') {
         return ident_t(reference + 1).find('\'') != NOTFOUND_POS;
      }
      else return false;
   }

   void trimLastSubNs()
   {
      size_t index = ident_t(_string).findLast('\'', 0);
      _string[index] = 0;
   }
};

// --- ReferenceNs ---

class ReferenceNs : public String<char, IDENTIFIER_LEN * 2>
{
public:
   operator ident_t() const { return _string; }

   ident_t ident() { return (const char*)_string; }

   void pathToName(path_t path)
   {
      char buf[IDENTIFIER_LEN];
      size_t bufLen;

      while (!emptystr(path)) {
         if (!emptystr(_string)) {
            append('\'');
         }
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
               pos = getlength(path);

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

   void trimProperName()
   {
      size_t index = ident_t(_string).findLast('\'', 0);
      _string[index] = 0;
   }

   void combine(ident_t s)
   {
      if (!emptystr(s)) {
         append('\'');
         append(s);
      }
   }

   ReferenceNs()
   {
   }
   ReferenceNs(ident_t properName)
      : String<char, IDENTIFIER_LEN * 2>(properName)
   {
   }
   ReferenceNs(ident_t moduleName, ident_t properName)
   {
      copy(moduleName);
      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
};

// --- Quote ---

template<class S> class QuoteTemplate
{
   S _string;

public:
   size_t Length() { return _string.Length(); }

   ident_t ident() { return (const char*)_string; }

   QuoteTemplate(ident_t string)
   {
      int mode = 0; // 1 - normal, 2 - character code
      int index = 0;
      for (pos_t i = 0 ; i <= getlength(string) ; i++) {
         switch (mode) {
            case 0:
               if (string[i]=='"') {
                  mode = 1;
               }
               else if (string[i]=='$') {
                  mode = 2;
                  index = i + 1;
               }
               else return;

               break;
            case 1:
               if (string[i]==0) {
                  mode = 0;
               }
               else if (string[i]=='"') {
                  if (string[i + 1]=='"')
                     _string.append(string[i]);

                  mode = 0;
               }
               else _string.append(string[i]);
               break;
            case 2:
               if ((string[i] < '0' || string[i] > '9') && (string[i] < 'A' || string[i] > 'F') && (string[i] < 'a' || string[i] > 'f')) {
                  String<char, 12> number(string + index, i - index);
                  unic_c ch = (string[i] == 'h') ? ((ident_t)number).toLong(16) : ((ident_t)number).toInt();

                  String<char, 5> temp;
                  size_t temp_len = 4;
                  Convertor::copy(temp, &ch, 1, temp_len);
                  _string.append(temp, temp_len);

                  if(string[i] == '"') {
                     mode = 1;
                  }
                  else if(string[i] == '$') {
                     index = i + 1;
                     mode = 2;
                  }
                  else mode = 0;
               }
               break;
         }
      }
   }
};

// --- VMTEntry ---

struct VMTEntry
{
   mssg_t message;
   pos_t  address;
};

// --- VMTXEntry ---

struct VMTXEntry
{
   mssg64_t message;
   pos64_t  address;
};

// --- ClassHeader ---
/// NOTE : ClassHeader is used for meta data
struct ClassHeader
{
   pos_t  staticSize;      // static table size
   ref_t  classRef;        // class class reference
   pos_t  count;
   ref_t  flags;
   ref_t  parentRef;
};

// --- VMTHeader ---

struct VMTHeader
{
   pos_t parentRef;
   ref_t flags;
   ref_t classRef;
   pos_t count;
};

#pragma pack(push, 1)
struct VMTXHeader
{
   pos64_t parentRef;
   pos64_t flags;
   pos64_t classRef;
   pos64_t count;
};
#pragma pack(pop)

// --- ClassInfo ---

enum MethodAttribute
{
   maActionMask         = 0x100,
   maRefefernceMask     = 0x200,
   maMessageMask        = 0x400,

   maNone               = 0x000,
   maHint               = 0x001,
   maReference          = 0x202,
   maConstant           = 0x203,
   maEmbeddableRet      = 0x403,
   maEmbeddableIdle     = 0x005,
   maEmbeddableNew      = 0x409,
   maOverloadlist       = 0x20A,
   maMultimethod        = 0x40B,
   maInternal           = 0x40C,
   maPrivate            = 0x40D,
   maProtected          = 0x40E,
   maYieldContext       = 0x00F,
   maYieldLocals        = 0x010,
   maYieldContextLength = 0x011,
   maYieldLocalLength   = 0x012,
   maStaticInherited    = 0x213,
   maSingleMultiDisp    = 0x414, // indicates that the multi-method has single dispatch target
   //   maYieldPreallocated  = 0x011,
};

enum ClassAttribute : int
{
   caRefefernceMask     = 0x100,

   caNone               = 0x000,
   caInitializer        = 0x101,
   // if the class can be loaded dynamically
   caSerializable       = 0x102,
   // if the symbol can be loaded dynamically
   caSymbolSerializable = 0x103,
   // a description
   caInfo               = 0x004,
   // parameter name
   caParamName          = 0x005,
   // extension message overload list
   caExtOverloadlist    = 0x106,
};

struct ClassInfo
{
   typedef Pair<ref_t, ref_t>                  FieldInfo;       // value1 - reference ; value2 - element
   typedef Pair<mssg_t, int>                   Attribute;
   typedef MemoryMap<mssg_t, bool, false>      MethodMap;
   typedef MemoryMap<ident_t, int, true>       FieldMap;
   typedef MemoryMap<ident_t, FieldInfo, true> StaticFieldMap;   // class static fields
   typedef MemoryMap<int, FieldInfo>           FieldTypeMap;
   typedef MemoryMap<Attribute, ref_t, false>  CategoryInfoMap;
   typedef MemoryMap<int, ref_t, false>        StaticInfoMap;

   ClassHeader     header;
   int             size;           // Object size
   MethodMap       methods;        // list of methods, true means the method was declared in this instance
   FieldMap        fields;
   StaticFieldMap  statics;
   StaticInfoMap   staticValues;

   FieldTypeMap    fieldTypes;
   CategoryInfoMap methodHints;
   CategoryInfoMap mattributes;

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->write((void*)this, sizeof(ClassHeader));
      writer->writeDWord(size);
      if (!headerAndSizeOnly) {
         mattributes.write(writer);
         statics.write(writer);
         staticValues.write(writer);
         methods.write(writer);
         methodHints.write(writer);
         fields.write(writer);
         fieldTypes.write(writer);
      }
   }

   void load(StreamReader* reader, bool headerOnly = false, bool ignoreFields = false)
   {
      reader->read((void*)&header, sizeof(ClassHeader));
      size = reader->getDWord();
      if (!headerOnly) {
         mattributes.read(reader);
         statics.read(reader);
         staticValues.read(reader);
         methods.read(reader);
         methodHints.read(reader);
         if (!ignoreFields) {
            fields.read(reader);
            fieldTypes.read(reader);
         }
      }
   }

   ClassInfo()
      : fields(-1), methods(0), methodHints(0), fieldTypes(FieldInfo(0, 0)), statics(FieldInfo(0, 0))
   {
      header.flags = 0;
      header.classRef = 0;
      size = 0;
   }
};

// --- SymbolExpressionInfo ---

struct SymbolExpressionInfo
{
   enum class Type : int
   {
      Normal = 0,
      Constant = 1,
      Singleton = 2,
      ConstantSymbol = 3,
      ArrayConst = 4,
   };

   Type  type;
   ref_t exprRef;
   ref_t typeRef;

   void save(StreamWriter* writer)
   {
      writer->writeDWord((int)type);
      writer->writeDWord(exprRef);
      writer->writeDWord(typeRef);
   }

   void load(StreamReader* reader)
   {
      type = (Type)reader->getDWord();
      exprRef = reader->getDWord();
      typeRef = reader->getDWord();
   }

   SymbolExpressionInfo()
   {
      typeRef = exprRef = 0;
      type = Type::Normal;
   }
};

// --- DebugLineInfo ---

struct DebugLineInfo
{
   DebugSymbol symbol;
   int         col, row, length;
   union
   {
      struct Source { pos_t nameRef;            } source;
      struct Module { pos_t nameRef; int flags; } symbol;
      struct Step   { pos_t address;            } step;
      struct Local  { pos_t nameRef; int level; } local;
      struct Field  { pos_t nameRef; int size;  } field;
      struct Offset { pos_t disp;               } offset;
   } addresses;

   DebugLineInfo()
   {
      symbol = dsNone;
      col = row = length = 0;

      this->addresses.symbol.nameRef = 0;
      this->addresses.symbol.flags = 0;
   }
   DebugLineInfo(DebugSymbol symbol, int length, int col, int row)
   {
      this->symbol = symbol;
      this->col = col;
      this->row = row;
      this->length = length;

      this->addresses.symbol.nameRef = 0;
      this->addresses.symbol.flags = 0;
   }
};

// --- Exception base class ---

struct _Exception
{
};

// --- InternalError ---

struct InternalError : _Exception
{
   ident_t message;

   InternalError(ident_t message)
   {
      this->message = message;
   }
};

// --- EAbortException ---

class EAbortException : _Exception
{
};

// --- key mapping routines ---

inline int simpleRule(int key)
{
   return key;
}

inline ref_t syntaxRule(ref_t key)
{
   return key >> cnSyntaxPower;
}

inline ref_t tableRule(ref_t key)
{
   return key >> cnTableKeyPower;
}

inline ref_t __map64Key(ref64_t key)
{
   return key & 0x3F;
}

//inline ref_t __mapKey(ref_t key)
//{
//   return key & 0x3F;
//}

// --- mapping keys ---
inline ref_t mapReferenceKey(ident_t key)
{
   ident_t p = key + key.findLast('\'', 0) + 1;

   int position = *p - 'a';
   if (position > 26)
      position = 26;
   else if (position < 0)
      position = 0;

   return position;
}

// --- Common type definitions ---

typedef Map<ident_t, _Module*>    ModuleMap;
typedef Map<ident_t, void*, true> ClassMap;
typedef List<char*>               IdentifierList;

// --- Reference mapping types ---
typedef Memory32HashTable<ident_t, ref_t, mapReferenceKey, 29>   ReferenceMap;
typedef Memory32HashTable<ident_t, lvaddr_t, mapReferenceKey, 29> VAddressMap;
typedef Memory32HashTable<ref64_t, ref_t, __map64Key, 64>        ActionMap;

// --- Message mapping types ---
typedef Map<ident_t, ref_t> AttributeMap;
typedef Map<ident_t, mssg_t> MessageMap;

// --- Extensions ---
typedef Map<ref_t, Pair<ref_t, ref_t>> ExtensionMap;
typedef Map<ref_t, char*>              ExtensionTmplMap;

// --- ParserTable auxiliary types ---
typedef Stack<int>                                          ParserStack;
typedef MemoryMap<ident_t, int>                             SymbolMap;
typedef MemoryHashTable<ref_t, int, syntaxRule, cnHashSize> SyntaxHash;
typedef MemoryHashTable<ref_t, int, tableRule, cnHashSize>  TableHash;

// --- miscellaneous routines ---

inline bool isSealedStaticField(ref_t ref)
{
   return (int)ref >= 0;
}

inline bool isTemplateWeakReference(ident_t referenceName)
{
   return referenceName[0] == '\'' && referenceName.startsWith(TEMPLATE_PREFIX_NS);
}

inline bool isForwardReference(ident_t referenceName)
{
   return referenceName.startsWith(FORWARD_PREFIX_NS);
}

inline mssg_t encodeMessage(ref_t actionRef, pos_t argCount, ref_t flags)
{
   return flags | ((actionRef << ACTION_ORDER) + (mssg_t)argCount);
}

inline mssg64_t encodeMessage64(ref_t actionRef, pos_t argCount, ref_t flags)
{
   mssg64_t message = actionRef;
   message <<= ACTION_ORDER;

   message += argCount;

   return message | flags;
}

inline mssg_t encodeAction(ref_t actionId)
{
   return encodeMessage(actionId, 0, 0);
}

inline void decodeMessage(mssg_t message, ref_t& actionRef, pos_t& argCount, ref_t& flags)
{
   actionRef = (message >> ACTION_ORDER);

   argCount = message & ARG_MASK;

   flags = message & MESSAGE_FLAG_MASK;
}

inline mssg_t overwriteArgCount(mssg_t message, pos_t argCount)
{
   pos_t dummy;
   ref_t actionRef, flags;
   decodeMessage(message, actionRef, dummy, flags);

   return encodeMessage(actionRef, argCount, flags);
}

inline mssg_t overwriteAction(mssg_t message, ref_t newAction)
{
   pos_t argCount;
   ref_t actionRef, flags;
   decodeMessage(message, actionRef, argCount, flags);

   return encodeMessage(newAction, argCount, flags);
}

inline void decodeMessage64(mssg64_t message, ref_t& actionRef, pos_t& argCount, ref_t& flags)
{
   actionRef = (ref_t)(message >> 16);

   argCount = message & ARGX_MASK;

   flags = message & MESSAGE_FLAG_MASK;
}

inline pos_t getArgCount(mssg_t message)
{
   pos_t argCount;
   ref_t action, flags;
   decodeMessage(message, action, argCount, flags);

   return argCount;
}

inline ref_t getAction(mssg_t message)
{
   pos_t argCount;
   ref_t action, flags;
   decodeMessage(message, action, argCount, flags);

   return action;
}

inline mssg64_t toMessage64(mssg_t message)
{
   pos_t argCount;
   ref_t actionRef, flags;
   decodeMessage(message, actionRef, argCount, flags);

   return encodeMessage64(actionRef, argCount, flags);
}

inline mssg_t fromMessage64(mssg64_t message)
{
   pos_t argCount;
   ref_t actionRef, flags;
   decodeMessage64(message, actionRef, argCount, flags);

   return encodeMessage(actionRef, argCount, flags);
}

inline bool IsExprOperator(int operator_id)
{
   switch (operator_id) {
      case ADD_OPERATOR_ID:
      case SUB_OPERATOR_ID:
      case MUL_OPERATOR_ID:
      case DIV_OPERATOR_ID:
      case SHIFTR_OPERATOR_ID:
      case SHIFTL_OPERATOR_ID:
      case BAND_OPERATOR_ID:
      case BOR_OPERATOR_ID:
      case BXOR_OPERATOR_ID:
      case NEGATIVE_OPERATOR_ID:
      case BINVERTED_OPERATOR_ID:
         return true;
      default:
         return false;
   }
}

inline bool isOpenArg(mssg_t message)
{
   return (message & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE;
}

inline bool isPrimitiveRef(ref_t reference)
{
   return (int)reference < 0;
}

inline ref_t importConstant(_Module* exporter, ref_t exportRef, _Module* importer)
{
   ident_t val = exporter->resolveConstant(exportRef);

   return importer->mapConstant(val);
}

inline ref_t importReference(_Module* exporter, ref_t exportRef, _Module* importer)
{
   if (isPrimitiveRef(exportRef)) {
      return exportRef;
   }
   else if (exportRef) {
      ident_t reference = exporter->resolveReference(exportRef);
      if (isWeakReference(reference) && !isTemplateWeakReference(reference)) {
         IdentifierString fullName(exporter->Name(), reference);

         return importer->mapReference(fullName.c_str());
      }
      else return importer->mapReference(reference);
   }
   else return 0;
}

inline ref_t importSignature(_Module* exporter, ref_t exportRef, _Module* importer)
{
   if (!exportRef)
      return 0;

   ref_t dump[ARG_COUNT];
   size_t len = exporter->resolveSignature(exportRef, dump);
   for (size_t i = 0; i < len; i++) {
      dump[i] = importReference(exporter, dump[i], importer);
   }

   return importer->mapSignature(dump, len, false);
}

inline mssg_t importMessage(_Module* exporter, mssg_t exportRef, _Module* importer)
{
   if (!exportRef)
      return exportRef;

   pos_t paramCount = 0;
   ref_t actionRef, flags;
   decodeMessage(exportRef, actionRef, paramCount, flags);

   // signature and custom verb should be imported
   ref_t signature = 0;
   ident_t actionName = exporter->resolveAction(actionRef, signature);

   actionRef = importer->mapAction(actionName, importSignature(exporter, signature, importer), false);

   return encodeMessage(actionRef, paramCount, flags);
}

} // _ELENA_

#endif // elenaH

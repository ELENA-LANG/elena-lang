//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                              (C)2005-2019, by Alexei Rakov
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
   //_Memory* attrSection;

   SectionInfo()
   {
      module = nullptr;
      /*attrSection = */section = nullptr;
   }
   SectionInfo(_Module* module, _Memory* section/*, _Memory* attrSection*/)
   {
      this->module = module;
      this->section = section;
      //this->attrSection = attrSection;
   }
};

// --- ClassSectionInfo ---

struct ClassSectionInfo
{
   _Module* module;
   _Memory* codeSection;
   _Memory* vmtSection;
   //_Memory* attrSection;

   ClassSectionInfo()
   {
      module = nullptr;
      codeSection = vmtSection = nullptr;
      //attrSection = nullptr;
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

   virtual size_t getLinkerConstant(int id) = 0;

   //virtual ident_t getLiteralClass() = 0;
   //virtual ident_t getWideLiteralClass() = 0;
   //virtual ident_t getCharacterClass() = 0;
   //virtual ident_t getIntegerClass() = 0;
   //virtual ident_t getRealClass() = 0;
   //virtual ident_t getLongClass() = 0;
   //virtual ident_t getMessageClass() = 0;
   //virtual ident_t getExtMessageClass() = 0;
   //virtual ident_t getMessageNameClass() = 0;
   virtual ident_t getNamespace() = 0;

   virtual ReferenceInfo retrieveReference(_Module* module, ref_t reference, ref_t mask) = 0;

   virtual void* resolveReference(ReferenceInfo referenceInfo, ref_t mask) = 0;

   //virtual void mapPredefinedAction(ident_t name, ref_t reference) = 0;

   virtual void mapReference(ReferenceInfo referenceInfo, void* vaddress, ref_t mask) = 0;

   virtual void addListener(_JITLoaderListener* listener) = 0;

   virtual ~_JITLoader() {}
};

// --- IdentifierString ---

class IdentifierString : public String <char, IDENTIFIER_LEN>
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
            if (pos == -1)
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
      for (size_t i = 0 ; i <= getlength(string) ; i++) {
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
   ref_t message;
   pos_t address;
};

//// --- VMTXEntry ---
//
//struct VMTXEntry
//{
//   ref64_t message;
//   ref64_t address;
//};

// --- ClassHeader ---

struct ClassHeader
{
//   ref_t  staticSize;      // static table size
   ref_t  classRef;        // class class reference
   size_t count;
   size_t flags;
   ref_t  parentRef;
};

// --- ClassInfo ---

enum MethodAttribute
{
   maActionMask         = 0x100,
   maRefefernceMask     = 0x200,
   maMessageMask        = 0x400,

   maNone               = 0x000,
   maHint               = 0x001,
//   maReference          = 0x202,
//   maEmbeddableRet      = 0x403,
//   maEmbeddableIdle     = 0x005,
//   maEmbeddableNew      = 0x409,
//   maOverloadlist       = 0x20A,
//   maMultimethod        = 0x40B,
//   maYieldContext       = 0x00C,
//   maYieldLocals        = 0x00D,
//   maYieldPreallocated  = 0x00E,
};

//enum ClassAttribute
//{
//   caNone               = 0x000,
//   caInitializer        = 0x001,
//   // if the class can be loaded dynamically
//   caSerializable       = 0x002,
//   // if the symbol can be loaded dynamically
//   caSymbolSerializable = 0x003,
//   caInfo               = 0x004
//};

struct ClassInfo
{
//   typedef Pair<ref_t, ref_t>                  FieldInfo;       // value1 - reference ; value2 - element
   typedef Pair<ref_t, int>                    Attribute;
   typedef MemoryMap<ref_t, bool, false>       MethodMap;
//   typedef MemoryMap<ident_t, int, true>       FieldMap;
//   typedef MemoryMap<ident_t, FieldInfo, true> StaticFieldMap;   // class static fields
//   typedef MemoryMap<int, FieldInfo>           FieldTypeMap;
   typedef MemoryMap<Attribute, ref_t, false>  CategoryInfoMap;
//   typedef MemoryMap<int, ref_t, false>        StaticInfoMap;

   ClassHeader     header;
//   int             size;           // Object size
   MethodMap       methods;        // list of methods, true means the method was declared in this instance
//   FieldMap        fields;
//   StaticFieldMap  statics;
//   StaticInfoMap   staticValues;
//
//   FieldTypeMap    fieldTypes;
   CategoryInfoMap methodHints;
//   CategoryInfoMap mattributes;   

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->write((void*)this, sizeof(ClassHeader));
//      writer->writeDWord(size);
      if (!headerAndSizeOnly) {
//         mattributes.write(writer);
//         statics.write(writer);
//         staticValues.write(writer);
         methods.write(writer);
         methodHints.write(writer);
//         fields.write(writer);
//         fieldTypes.write(writer);
      }
   }

   void load(StreamReader* reader, bool headerOnly = false, bool ignoreFields = false)
   {
      reader->read((void*)&header, sizeof(ClassHeader));
//      size = reader->getDWord();
      if (!headerOnly) {
//         mattributes.read(reader);
//         statics.read(reader);
//         staticValues.read(reader);
         methods.read(reader);
         methodHints.read(reader);
//         if (!ignoreFields) {
//            fields.read(reader);
//            fieldTypes.read(reader);
//         }
      }
   }

   ClassInfo()
      : /*fields(-1), */methods(0), methodHints(0)//, fieldTypes(FieldInfo(0, 0)), statics(FieldInfo(0, 0))
   {
      header.flags = 0;
      header.classRef = 0;
//      size = 0;
   }
};

// --- SymbolExpressionInfo ---

struct SymbolExpressionInfo
{
//   ref_t expressionClassRef;
//   ref_t listRef;
//   bool  constant;

   void save(StreamWriter* writer)
   {
      //writer->writeDWord(listRef);
      //writer->writeDWord(constant ? -1: 0);
      //writer->writeDWord(expressionClassRef);
   }

   void load(StreamReader* reader)
   {
      //listRef = reader->getDWord();
      //constant = (reader->getDWord() != 0);
      //expressionClassRef = reader->getDWord();
   }

   SymbolExpressionInfo()
   {
//      expressionClassRef = 0;
//      listRef = 0;
//      constant = false;
   }
};

// --- DebugLineInfo ---

struct DebugLineInfo
{
   DebugSymbol symbol;
   int         col, row, length;
   union
   {
      struct Source { pos_t nameRef; } source;
      struct Module { pos_t nameRef; int flags; } symbol;
      struct Step   { pos_t address;         } step;
      struct Local  { pos_t nameRef; int level; } local;
      struct Field  { pos_t nameRef; int size;  } field;
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

inline ref_t __mapKey(ref_t key)
{
   return key & 0x3F;
}

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

typedef Map<ident_t, _Module*> ModuleMap;
typedef List<_Module*>         ModuleList;
typedef List<char*>            IdentifierList;

// --- Reference mapping types ---
////typedef Memory32HashTable<ident_t, ref_t, mapIdentifierKey, 29> TypeMap;
typedef Memory32HashTable<ident_t, ref_t, mapReferenceKey, 29>  ReferenceMap;
//typedef Memory32HashTable<ref_t, ref_t, __mapKey, 64>           AddressMap;
typedef Map<ref_t, ref_t>                                       SubjectMap;
//typedef List<ref_t>                                             SubjectList;
typedef Memory32HashTable<ref64_t, ref_t, __map64Key, 64>       ActionMap;

// --- Message mapping types ---
typedef Map<ident_t, ref_t> MessageMap;

// --- Extensions ---
typedef Map<ref_t, Pair<ref_t, ref_t>> ExtensionMap;
typedef Map<ref_t, char*>              ExtensionTmplMap;

// --- ParserTable auxiliary types ---
typedef Stack<int>                                          ParserStack;
typedef MemoryMap<ident_t, int>                             SymbolMap;
typedef MemoryHashTable<ref_t, int, syntaxRule, cnHashSize> SyntaxHash;
typedef MemoryHashTable<ref_t, int, tableRule, cnHashSize>  TableHash;

// --- miscellaneous routines ---

//inline bool isSealedStaticField(ref_t ref)
//{
//   return (int)ref >= 0;
//}

inline bool isTemplateWeakReference(ident_t referenceName)
{
   return referenceName[0] == '\'' && referenceName.startsWith(TEMPLATE_PREFIX_NS);
}

inline bool isForwardReference(ident_t referenceName)
{
   return referenceName.startsWith(FORWARD_PREFIX_NS);
}

inline ref_t encodeMessage(ref_t actionRef, int argCount, ref_t flags)
{
   return flags | ((actionRef << ACTION_ORDER) + argCount);
}

//inline ref64_t encodeMessage64(ref_t actionRef, int paramCount, ref_t flags)
//{
//   ref64_t message = actionRef;
//   message <<= ACTION_ORDER;
//
//   message += paramCount;
//
//   return message | flags;
//}

inline ref_t encodeAction(ref_t actionId)
{
   return encodeMessage(actionId, 0, 0);
}

inline void decodeMessage(ref_t message, ref_t& actionRef, int& argCount, ref_t& flags)
{
   actionRef = (message >> ACTION_ORDER);

   argCount = message & ARG_MASK;

   flags = message & MESSAGE_FLAG_MASK;
}

//inline ref_t overwriteParamCount(ref_t message, int paramCount)
//{
//   int dummy;
//   ref_t actionRef, flags;
//   decodeMessage(message, actionRef, dummy, flags);
//
//   return encodeMessage(actionRef, paramCount, flags);
//}
//
//inline ref_t overwriteAction(ref_t message, ref_t newAction)
//{
//   int paramCount;
//   ref_t actionRef, flags;
//   decodeMessage(message, actionRef, paramCount, flags);
//
//   return encodeMessage(newAction, paramCount, flags);
//}

//inline void decodeMessage64(ref64_t message, ref_t& actionRef, int& paramCount)
//{
//   actionRef = (ref_t)(message >> 16);
//
//   actionRef &= ACTION_MASK;
//
//   paramCount = message & PARAMX_MASK;
//}

inline int getArgCount(ref_t message)
{
   int   argCount;
   ref_t action, flags;
   decodeMessage(message, action, argCount, flags);

   return argCount;
}

inline ref_t getAction(ref_t message)
{
   int   argCount;
   ref_t action, flags;
   decodeMessage(message, action, argCount, flags);

   return action;
}

//inline ref64_t toMessage64(ref_t message)
//{
//   int   paramCount;
//   ref_t actionRef;
//   decodeMessage(message, actionRef, paramCount);
//
//   return encodeMessage64(actionRef, paramCount);
//}
//
//inline ref_t fromMessage64(ref64_t message)
//{
//   int   paramCount;
//   ref_t actionRef;
//   decodeMessage64(message, actionRef, paramCount);
//
//   return encodeMessage(actionRef, paramCount);
//}

//inline bool IsExprOperator(int operator_id)
//{
//   switch (operator_id) {
//      case ADD_OPERATOR_ID:
//      case SUB_OPERATOR_ID:
//      case MUL_OPERATOR_ID:
//      case DIV_OPERATOR_ID:
//      case AND_OPERATOR_ID:
//      case OR_OPERATOR_ID:
//      case XOR_OPERATOR_ID:
//         return true;
//      default:
//         return false;
//   }
//}
//
//inline bool IsShiftOperator(int operator_id)
//{
//   switch (operator_id) {
//      case SHIFTR_OPERATOR_ID:
//      case SHIFTL_OPERATOR_ID:
//         return true;
//      default:
//         return false;
//   }
//}
//
//////inline bool IsRealExprOperator(int operator_id)
//////{
//////   switch (operator_id) {
//////   case ADD_MESSAGE_ID:
//////   case SUB_MESSAGE_ID:
//////   case MUL_MESSAGE_ID:
//////   case DIV_MESSAGE_ID:
//////      return true;
//////   default:
//////      return false;
//////   }
//////}
//
//inline bool isOpenArg(ref_t message)
//{
//   return test(message, VARIADIC_MESSAGE);
//}

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

inline ref_t importMessage(_Module* exporter, ref_t exportRef, _Module* importer)
{
   int paramCount = 0;
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

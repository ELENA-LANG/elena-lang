//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                              (C)2005-2017, by Alexei Rakov
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
   virtual ident_t resolveSubject(ref_t reference) = 0;
   virtual ident_t resolveConstant(ref_t reference) = 0;

   virtual ref_t mapReference(ident_t reference) = 0;
   virtual ref_t mapReference(ident_t reference, bool existing) = 0;

   virtual ref_t mapSubject(ident_t reference, bool existing) = 0;
   virtual ref_t mapConstant(ident_t reference) = 0;

   virtual void mapPredefinedReference(ident_t name, ref_t reference) = 0;

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

// --- _Project ---

class _ProjectManager
{
public:
   virtual ident_t Namespace() const = 0; // !! obsolete??

   virtual int getDefaultEncoding() = 0; // !! obsolete
   virtual int getTabSize() = 0;  // !! obsolete

   virtual bool HasWarnings() const = 0;     // !! obsolete
   virtual int getWarningMask() const = 0;
   virtual bool WarnOnUnresolved() const = 0;
   virtual bool WarnOnWeakUnresolved() const = 0;

   virtual ident_t getManinfestName() = 0;
   virtual ident_t getManinfestVersion() = 0;
   virtual ident_t getManinfestAuthor() = 0;

   virtual void printInfo(const char* msg, ident_t value) = 0;

   //   virtual void raiseError(const char* msg) = 0;
   virtual void raiseError(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseError(ident_t msg, ident_t value) = 0;

   virtual void raiseErrorIf(bool throwExecption, ident_t msg, ident_t identifier) = 0;

   virtual void raiseWarning(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseWarning(ident_t msg, ident_t path) = 0;

   virtual _Module* createModule(ident_t name) = 0;      // !! obsolete
   virtual _Module* createDebugModule(ident_t name) = 0 ; // !! obsolete

   virtual _Module* loadModule(ident_t package, bool silentMode) = 0;
   virtual void saveModule(_Module* module, ident_t extension) = 0; // !! obsolete

   virtual _Module* resolveModule(ident_t referenceName, ref_t& reference, bool silentMode = false) = 0;

   virtual ident_t resolveForward(ident_t forward) = 0;

   virtual ident_t resolveExternalAlias(ident_t alias, bool& stdCall) = 0;
};

// --- SectionInfo ---

struct SectionInfo
{
   _Module* module;
   _Memory* section;

   SectionInfo()
   {
      module = NULL;
      section = NULL;
   }
   SectionInfo(_Module* module, _Memory* section)
   {
      this->module = module;
      this->section = section;
   }
};

// --- ClassSectionInfo ---

struct ClassSectionInfo
{
   _Module* module;
   _Memory* codeSection;
   _Memory* vmtSection;

   ClassSectionInfo()
   {
      module = NULL;
      codeSection = vmtSection = NULL;
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

   virtual SectionInfo getSectionInfo(ident_t reference, ref_t mask, bool silentMode) = 0;
   virtual SectionInfo getCoreSectionInfo(ref_t reference, ref_t mask) = 0;
   virtual ClassSectionInfo getClassSectionInfo(ident_t reference, ref_t codeMask, ref_t vmtMask, bool silentMode) = 0;

   virtual size_t getLinkerConstant(int id) = 0;

   virtual ident_t getLiteralClass() = 0;
   virtual ident_t getWideLiteralClass() = 0;
   virtual ident_t getCharacterClass() = 0;
   virtual ident_t getIntegerClass() = 0;
   virtual ident_t getRealClass() = 0;
   virtual ident_t getLongClass() = 0;
   virtual ident_t getMessageClass() = 0;
   virtual ident_t getExtMessageClass() = 0;
   virtual ident_t getSignatureClass() = 0;
   virtual ident_t getVerbClass() = 0;
   virtual ident_t getNamespace() = 0;

   virtual ident_t retrieveReference(_Module* module, ref_t reference, ref_t mask) = 0;

   virtual void* resolveReference(ident_t reference, ref_t mask) = 0;

   virtual void mapReference(ident_t reference, void* vaddress, ref_t mask) = 0;

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
      _string[pos] = 0;
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
            append(buf, bufLen);

            break;
         }
      }
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

// --- VMTXEntry ---

struct VMTXEntry
{
   ref64_t message;
   ref64_t address;
};

// --- ClassHeader ---

struct ClassHeader
{
   ref_t  packageRef;      // package header
   ref_t  classRef;        // class class reference
   size_t count;
   size_t flags;
   ref_t  parentRef;
};

// --- ClassInfo ---

enum MethodAttribute
{
   maSubjectMask        = 0x100,
   maRefefernceMask     = 0x200,

   maNone               = 0x000,
   maHint               = 0x001,
   maType               = 0x102,
   maReference          = 0x202,
   maEmbeddableGet      = 0x103,
   maEmbeddableEval     = 0x104,
   maEmbeddableIdle     = 0x005,
   maEmbeddableGetAt    = 0x106,
   maEmbeddableGetAt2   = 0x107,
   maEmbeddableEval2    = 0x108,
};

struct ClassInfo
{
   typedef Pair<ref_t, ref_t>                  FieldInfo;       // value1 - reference ; value2 - type
   typedef Pair<ref_t, int>                    Attribute;
   typedef MemoryMap<ref_t, bool, false>       MethodMap;
   typedef MemoryMap<ident_t, int, true>       FieldMap;
   typedef MemoryMap<ident_t, FieldInfo, true> StaticFieldMap;   // class static fields
   typedef MemoryMap<int, FieldInfo>           FieldTypeMap;
   typedef MemoryMap<Attribute, ref_t, false>  MethodInfoMap;

   ClassHeader    header;
   int            size;           // Object size
   MethodMap      methods;
   FieldMap       fields;
   StaticFieldMap statics;

   FieldTypeMap   fieldTypes;
   MethodInfoMap  methodHints;

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->write((void*)this, sizeof(ClassHeader));
      writer->writeDWord(size);
      if (!headerAndSizeOnly) {
         methods.write(writer);
         fields.write(writer);
         fieldTypes.write(writer);
         methodHints.write(writer);
         statics.write(writer);
      }
   }

   void load(StreamReader* reader, bool headerOnly = false)
   {
      reader->read((void*)&header, sizeof(ClassHeader));
      size = reader->getDWord();
      if (!headerOnly) {
         methods.read(reader);
         fields.read(reader);
         fieldTypes.read(reader);
         methodHints.read(reader);
         statics.read(reader);
      }
   }

   ClassInfo()
      : fields(-1), methods(0), methodHints(0), fieldTypes(FieldInfo(0, 0)), statics(FieldInfo(0, 0))
   {
      header.flags = 0;
      header.classRef = 0;
   }
};

// --- SymbolExpressionInfo ---

struct SymbolExpressionInfo
{
   ref_t expressionTypeRef;
   ref_t expressionClassRef;
   ref_t listRef;
   bool  constant;

   void save(StreamWriter* writer)
   {
      writer->writeDWord(expressionTypeRef);
      writer->writeDWord(listRef);
      writer->writeDWord(constant ? -1: 0);
      writer->writeDWord(expressionClassRef);
   }

   void load(StreamReader* reader)
   {
      expressionTypeRef = reader->getDWord();
      listRef = reader->getDWord();
      constant = (reader->getDWord() != 0);
      expressionClassRef = reader->getDWord();
   }

   SymbolExpressionInfo()
   {
      expressionClassRef = expressionTypeRef = 0;
      listRef = 0;
      constant = false;
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

// --- Reference mapping types ---
typedef Memory32HashTable<ident_t, ref_t, mapReferenceKey, 29> ReferenceMap;
typedef Map<ref_t, ref_t>                                      SubjectMap;

// --- Message mapping types ---
typedef Map<ident_t, ref_t> MessageMap;

// --- ParserTable auxiliary types ---
typedef Stack<int>                                          ParserStack;
typedef MemoryMap<ident_t, int>                             SymbolMap;
typedef MemoryHashTable<ref_t, int, syntaxRule, cnHashSize> SyntaxHash;
typedef MemoryHashTable<ref_t, int, tableRule, cnHashSize>  TableHash;

// --- miscellaneous routines ---

inline bool isWeakReference(ident_t referenceName)
{
   return (referenceName != NULL && referenceName[0] != 0 && referenceName[0]=='\'');
}

inline ref_t encodeMessage(ref_t signatureRef, ref_t verbId, int paramCount)
{
   return (verbId << 24) + (signatureRef << 4) + paramCount;
}

inline ref64_t encodeMessage64(ref_t signatureRef, ref_t verbId, int paramCount)
{
   ref64_t message = verbId;
   message <<= 56;

   message += (signatureRef << 16) + paramCount;

   return message;
}

inline ref_t encodeVerb(int verbId)
{
   return encodeMessage(0, verbId, 0);
}

inline ref_t overwriteSubject(ref_t message, ref_t subject)
{
   message &= ~SIGN_MASK;
   message |= (subject << 4);

   return message;
}

inline ref_t overwriteVerb(ref_t message, int verb)
{
   message &= ~VERB_MASK;
   message |= (verb << 24);

   return message;

}

inline ref_t overwriteParamCount(ref_t message, int paramCount)
{
   message &= ~PARAM_MASK;
   message |= paramCount;

   return message;

}

inline void decodeMessage(ref_t message, ref_t& signatureRef, ref_t& verbId, int& paramCount)
{
   verbId = (message & VERB_MASK) >> 24;
   signatureRef = (message & SIGN_MASK) >> 4;
   paramCount = message & PARAM_MASK;
}

inline void decodeMessage64(ref64_t message, ref_t& signatureRef, ref_t& verbId, int& paramCount)
{
   verbId = (message & VERBX_MASK) >> 56;
   signatureRef = (ref_t)((message & SIGNX_MASK) >> 16);
   paramCount = message & PARAMX_MASK;
}

inline int getParamCount(ref_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage(message, signature, verb, paramCount);

   if (paramCount == OPEN_ARG_COUNT)
      return 0;

   return paramCount;
}

inline ref_t getVerb(ref_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage(message, signature, verb, paramCount);

   return verb;
}

inline ref_t getSignature(ref_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage(message, signature, verb, paramCount);

   return signature;
}

inline ref64_t toMessage64(ref_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage(message, signature, verb, paramCount);

   return encodeMessage64(signature, verb, paramCount);
}

inline ref_t fromMessage64(ref64_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage64(message, signature, verb, paramCount);

   return encodeMessage(signature, verb, paramCount);
}

inline bool IsExprOperator(int operator_id)
{
   switch (operator_id) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsShiftOperator(int operator_id)
{
   switch (operator_id) {
      case READ_MESSAGE_ID:
      case WRITE_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsRealExprOperator(int operator_id)
{
   switch (operator_id) {
   case ADD_MESSAGE_ID:
   case SUB_MESSAGE_ID:
   case MUL_MESSAGE_ID:
   case DIV_MESSAGE_ID:
      return true;
   default:
      return false;
   }
}

inline bool isOpenArg(ref_t message)
{
   return (message & PARAM_MASK) == OPEN_ARG_COUNT;
}

} // _ELENA_

#endif // elenaH

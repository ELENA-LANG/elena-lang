//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                              (C)2005-2015, by Alexei Rakov
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

// --- _JITLoader ---

class _JITLoader
{
public:
   virtual _Memory* getTargetSection(size_t mask) = 0;

   virtual _Memory* getTargetDebugSection() = 0;

   virtual SectionInfo getSectionInfo(ident_t reference, size_t mask) = 0;
   virtual SectionInfo getCoreSectionInfo(ref_t reference, size_t mask) = 0;
   virtual ClassSectionInfo getClassSectionInfo(ident_t reference, size_t codeMask, size_t vmtMask, bool silentMode) = 0;

   virtual size_t getLinkerConstant(int id) = 0;

   virtual ident_t getLiteralClass() = 0;
   virtual ident_t getCharacterClass() = 0;
   virtual ident_t getIntegerClass() = 0;
   virtual ident_t getRealClass() = 0;
   virtual ident_t getLongClass() = 0;
   virtual ident_t getMessageClass() = 0;
   virtual ident_t getSignatureClass() = 0;
   virtual ident_t getVerbClass() = 0;
   virtual ident_t getNamespace() = 0;

   virtual ident_t retrieveReference(_Module* module, ref_t reference, ref_t mask) = 0;

   virtual void* resolveReference(ident_t reference, size_t mask) = 0;

   virtual void mapReference(ident_t reference, void* vaddress, size_t mask) = 0;

   virtual ~_JITLoader() {}
};

class IdentifierString : public String < ident_c, IDENTIFIER_LEN >
{
public:
   static ident_c* clonePath(path_t value)
   {
      ident_c buf[IDENTIFIER_LEN];
      size_t length = IDENTIFIER_LEN;
      StringHelper::copy(buf, value, getlength(value), length);
      buf[length] = 0;

      return StringHelper::clone(buf);
   }
   IdentifierString()
   {
   }
   IdentifierString(ident_t value)
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
   IdentifierString(const wide_c* value, size_t sourLength)
   {
      size_t length = IDENTIFIER_LEN;
      StringHelper::copy(_string, value, sourLength, length);
      _string[length] = 0;
   }
   IdentifierString(const wide_c* value)
   {
      size_t length = IDENTIFIER_LEN;
      StringHelper::copy(_string, value, getlength(value), length);
      _string[length] = 0;
   }
};

// --- ReferenceNs ---

class ReferenceNs : public String<ident_c, IDENTIFIER_LEN * 2>
{
public:
//   static bool compareNs(const wchar16_t* reference, const wchar16_t* ns)
//   {
//      int length = getlength(ns);
//      return StringHelper::compare(reference, ns, length) && reference[length] == '\'';
//   }

   void pathToName(path_t path)
   {
      ident_c buf[IDENTIFIER_LEN];
      size_t bufLen;

      while (!emptystr(path)) {
         if (!emptystr(_string)) {
            append('\'');
         }
         int pos = StringHelper::find(path, PATH_SEPARATOR);
         if (pos != -1) {
            bufLen = IDENTIFIER_LEN;
            StringHelper::copy(buf, path, pos, bufLen);

            append(buf, bufLen);
            path += pos + 1;
         }
         else {
            pos = StringHelper::findLast(path, '.');
            if (pos == -1)
               pos = getlength(path);

            bufLen = IDENTIFIER_LEN;
            StringHelper::copy(buf, path, pos, bufLen);
            append(buf, bufLen);

            break;
         }
      }
   }

   void combine(ident_t s)
   {
      append('\'');
      append(s);
   }

//   void appendName(const wchar16_t* reference)
//   {
//      append(reference + StringHelper::findLast(reference, '\'') + 1);
//   }

   ReferenceNs()
   {
   }
   ReferenceNs(ident_t properName)
      : String<ident_c, IDENTIFIER_LEN * 2>(properName)
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

// --- ReferenceName ---

class ReferenceName : public IdentifierString
{
public:
   ReferenceName()
   {
   }
   ReferenceName(ident_t reference)
   {
      copy(reference + StringHelper::findLast(reference, '\'') + 1);
   }
   ReferenceName(ident_t reference, ident_t package)
   {
      int length = getlength(package);

      if (StringHelper::compare(reference, package, length) && reference[length] == '\'') {
         copy(reference + length + 1);
      }
      else copy(reference + StringHelper::findLast(reference, '\'') + 1);
   }
};

// --- NamespaceName ---

class NamespaceName : public String < ident_c, IDENTIFIER_LEN >
{
public:
//   static bool hasNameSpace(const wchar16_t* reference)
//   {
//      return (StringHelper::findLast(reference, '\'', -1) != -1);
//   }

   static bool isIncluded(ident_t root, ident_t ns)
   {
      size_t length = getlength(root);
      if (getlength(ns) <= length) {
         return StringHelper::compare(root, ns);
      }
      else if (ns[length]=='\'') {
         return StringHelper::compare(root, ns, length);
      }
      else return false;
   }

   NamespaceName(ident_t reference)
   {
      int pos = StringHelper::findLast(reference, '\'', 0);
      copy(reference, pos);
      _string[pos] = 0;
   }

   bool compare(ident_t reference)
   {
      size_t pos = StringHelper::findLast(reference, '\'', 0);
      if (pos == 0 && getlength(_string)==0)
         return true;
      else if (getlength(_string)==pos) {
         return StringHelper::compare(_string, reference, pos);
      }
      else return false;
   }
};

// --- Quote ---

template<class S> class QuoteTemplate
{
   S _string;

public:
   size_t Length() { return _string.Length(); }

   operator ident_t() const { return _string; }

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
               else if (string[i]=='#') {
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
                  String<ident_c, 12> number(string + index, i - index);
                  unic_c ch = (string[i] == 'h') ? number.toLong(16) : number.toInt();

                  ident_c temp[5];
                  size_t temp_len = 4;
                  StringHelper::copy(temp, &ch, 1, temp_len);
                  _string.append(temp, temp_len);

                  if(string[i] == '"') {
                     mode = 1;
                  }
                  else if(string[i] == '#') {
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
   size_t message;
   int address;
};

// --- ClassHeader ---

struct ClassHeader
{
   size_t count;
   size_t flags;
   ref_t  parentRef;
};

// --- ClassInfo ---

enum MethodAttribute
{
   maTypeMask      = 0x100,

   maNone          = 0x000,
   maHint          = 0x001,
   maType          = 0x102,
   maEmbeddableGet = 0x103,
   maEmbeddedInit  = 0x104
};

struct ClassInfo
{
   typedef Pair<ref_t, int>                   Attribute;
   typedef MemoryMap<ref_t, bool, false>      MethodMap;
   typedef MemoryMap<ident_t, int, true>      FieldMap;
   typedef MemoryMap<int, ref_t>              FieldTypeMap;
   typedef MemoryMap<Attribute, ref_t, false> MethodInfoMap;

   ClassHeader   header;
   size_t        size;           // Object size
   ref_t         classClassRef;  // reference to class class VMT
   ref_t         extensionTypeRef;
   MethodMap     methods;
   FieldMap      fields;

   FieldTypeMap  fieldTypes;
   MethodInfoMap methodHints;

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->write((void*)this, sizeof(ClassHeader));
      writer->writeDWord(size);
      writer->writeDWord(classClassRef);
      writer->writeDWord(extensionTypeRef);
      if (!headerAndSizeOnly) {
         methods.write(writer);
         fields.write(writer);
         fieldTypes.write(writer);
         methodHints.write(writer);
      }
   }

   void load(StreamReader* reader, bool headerOnly = false)
   {
      reader->read((void*)&header, sizeof(ClassHeader));
      size = reader->getDWord();
      classClassRef = reader->getDWord();
      extensionTypeRef = reader->getDWord();
      if (!headerOnly) {
         methods.read(reader);
         fields.read(reader);
         fieldTypes.read(reader);
         methodHints.read(reader);
      }
   }

   ClassInfo()
      : fields(-1), methods(0), methodHints(0)
   {
      header.flags = 0;
      classClassRef = extensionTypeRef = 0;
   }
};

// --- SymbolExpressionInfo ---

struct SymbolExpressionInfo
{
   size_t expressionTypeRef;
   bool   constant;

   void save(StreamWriter* writer)
   {
      writer->writeDWord(expressionTypeRef);
      writer->writeDWord(constant ? -1: 0);
   }

   void load(StreamReader* reader)
   {
      expressionTypeRef = reader->getDWord();
      constant = (reader->getDWord() != 0);
   }

   SymbolExpressionInfo()
   {
      expressionTypeRef = 0;
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
      struct Source { int nameRef; } source;
      struct Module { int nameRef; int flags; } symbol;
      struct Step   { size_t address;         } step;
      struct Local  { int nameRef; int level; } local;
      struct Field  { int nameRef; int size;  } field;
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
   const char* message;

   InternalError(const char* message)
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

inline size_t syntaxRule(size_t key)
{
   return key >> cnSyntaxPower;
}

inline size_t tableRule(size_t key)
{
   return key >> cnTableKeyPower;
}

// --- mapping keys ---
inline size_t mapReferenceKey(ident_t key)
{
   ident_t p = key + StringHelper::findLast(key, '\'', 0) + 1;

   int position = *p - 'a';
   if (position > 26)
      position = 26;
   else if (position < 0)
      position = 0;

   return position;
}

// --- Common type definitions ---

typedef Map<ident_t, _Module*> ModuleMap;

// --- Reference mapping types ---
typedef MemoryHashTable<ident_t, ref_t, mapReferenceKey, 29> ReferenceMap;

// --- Message mapping types ---
typedef Map<ident_t, ref_t> MessageMap;

// --- ParserTable auxiliary types ---
typedef Stack<int>                                           ParserStack;
typedef MemoryMap<ident_t, int>                              SymbolMap;
typedef MemoryHashTable<size_t, int, syntaxRule, cnHashSize> SyntaxHash;
typedef MemoryHashTable<size_t, int, tableRule, cnHashSize>  TableHash;

// --- miscellaneous routines ---

inline bool isWeakReference(ident_t referenceName)
{
   return (referenceName != NULL && referenceName[0] != 0 && referenceName[0]=='\'');
}

inline ref_t encodeMessage(ref_t signatureRef, ref_t verbId, int paramCount)
{
   return (verbId << 24) + (signatureRef << 4) + paramCount;
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

inline void decodeMessage(ref_t message, ref_t& signatureRef, ref_t& verbId, int& paramCount)
{
   verbId = (message & VERB_MASK) >> 24;
   signatureRef = (message & SIGN_MASK) >> 4;
   paramCount = message & PARAM_MASK;
}

inline int getParamCount(ref_t message)
{
   int   paramCount;
   ref_t verb, signature;
   decodeMessage(message, signature, verb, paramCount);

   if (paramCount >= OPEN_ARG_COUNT)
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

} // _ELENA_

#endif // elenaH

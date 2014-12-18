//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                              (C)2005-2014, by Alexei Rakov
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
   virtual const wchar16_t* Name() const = 0;

   virtual const wchar16_t* resolveReference(ref_t reference) = 0;
   virtual const wchar16_t* resolveSubject(ref_t reference) = 0;
   virtual const wchar16_t* resolveConstant(ref_t reference) = 0;

   virtual ref_t mapReference(const wchar16_t* reference) = 0;
   virtual ref_t mapReference(const wchar16_t* reference, bool existing) = 0;

   virtual ref_t mapSubject(const wchar16_t* reference, bool existing) = 0;
   virtual ref_t mapConstant(const wchar16_t* reference) = 0;

   virtual void mapPredefinedReference(const wchar16_t* name, ref_t reference) = 0;

   virtual _Memory* mapSection(ref_t reference, bool existing) = 0;

   virtual bool save(StreamWriter& writer) = 0;

   virtual ~_Module() {}
};

// --- _LibraryManager ---

class _LibraryManager
{
public:
   virtual _Module* resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference) = 0;
   virtual _Module* resolveDebugModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference) = 0;
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

   virtual SectionInfo getSectionInfo(const wchar16_t* reference, size_t mask) = 0;
   virtual SectionInfo getCoreSectionInfo(ref_t reference, size_t mask) = 0;
   virtual ClassSectionInfo getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask, bool silentMode) = 0;

   virtual size_t getLinkerConstant(int id) = 0;

   virtual const wchar16_t* getLiteralClass() = 0;
   virtual const wchar16_t* getIntegerClass() = 0;
   virtual const wchar16_t* getRealClass() = 0;
   virtual const wchar16_t* getLongClass() = 0;
   virtual const wchar16_t* getMessageClass() = 0;
   virtual const wchar16_t* getSignatureClass() = 0;
   virtual const wchar16_t* getNamespace() = 0;

   virtual const wchar_t* retrieveReference(_Module* module, ref_t reference, ref_t mask) = 0;

   virtual void* resolveReference(const wchar16_t* reference, size_t mask) = 0;

   virtual void mapReference(const wchar16_t* reference, void* vaddress, size_t mask) = 0;

   virtual ~_JITLoader() {}
};

// --- IdentifierString ---

typedef String<wchar16_t, IDENTIFIER_LEN> IdentifierString;

// --- ConstantIdentifier ---

class ConstantIdentifier : public String<wchar16_t, 30>
{
public:
   // NOTE: Should be used only to compare with ANSI s2
   static bool compare(const wchar16_t* s1, const char* s2)
   {
      if (s1 && s2) {
         while(*s1 && *s2) {
            if (*s2 >= 0x80 || (short)*s1 != (short)*s2)
               return false;

            s1++;
            s2++;
         }

         return (*s1 == *s2);
      }
      return (emptystr(s1) && emptystr(s2));
   }

   // NOTE: Should be used only to compare with ANSI s2
   static bool compare(const wchar16_t* s1, const char* s2, size_t length)
   {
      if (s1 && s2) {
         while(length > 0) {
            if (*s2 >= 0x80 || (short)*s1 != (short)*s2)
               return false;

            s1++;
            s2++;

            length--;
         }

         return true;
      }
      return (emptystr(s1) && emptystr(s2));
   }

   ConstantIdentifier()
   {
      _string[0] = 0;
   }
   ConstantIdentifier(const char* s)
   {
      int length = getlength(s);
      for(int i = 0 ; i <= length ; i++) {
         _string[i] = s[i];
      }
   }
};

//typedef ConstantIdentifier ConstIdentifier;

// --- ReferenceNs ---

class ReferenceNs : public String<wchar16_t, IDENTIFIER_LEN * 2>
{
public:
   static bool compareNs(const wchar16_t* reference, const wchar16_t* ns)
   {
      int length = getlength(ns);
      return StringHelper::compare(reference, ns, length) && reference[length] == '\'';
   }

   void pathToName(const tchar_t* path)
   {
      while (!emptystr(path)) {
         if (!emptystr(_string)) {
            append('\'');
         }
         int pos = StringHelper::find(path, PATH_SEPARATOR);
         if (pos != -1) {
            append(path, pos);
            path += pos + 1;
         }
         else {
            pos = StringHelper::findLast(path, '.');
            if (pos != -1) {
               append(path, pos);
            }
            else append(path);

            break;
         }
      }
   }

   void combine(const wchar16_t* s)
   {
      append('\'');
      append(s);
   }

   void combine(const char* s)
   {
      append('\'');
      append(s);
   }

   void appendName(const wchar16_t* reference)
   {
      append(reference + StringHelper::findLast(reference, '\'') + 1);
   }

   ReferenceNs()
   {
   }
   ReferenceNs(const wchar16_t* properName)
      : String<wchar16_t, IDENTIFIER_LEN * 2>(properName)
   {
   }
   ReferenceNs(const wchar16_t* moduleName, const wchar16_t* properName)
   {
      copy(moduleName);
      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const wchar16_t* moduleName, const wchar16_t* ns, const wchar16_t* properName)
   {
      copy(moduleName);

      if (!emptystr(_string)) {
         combine(ns);
      }
      else copy(ns);

      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const wchar16_t* moduleName, const char* properName)
   {
      copy(moduleName);
      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const char* moduleName, const wchar16_t* properName)
   {
      copy(moduleName);
      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const char* moduleName, const char* properName)
   {
      copy(moduleName);
      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const char* rootName, const char* moduleName, const char* properName)
   {
      copy(rootName);
      if (!emptystr(_string)) {
         combine(moduleName);
      }
      else copy(moduleName);

      if (!emptystr(_string)) {
         combine(properName);
      }
      else copy(properName);
   }
   ReferenceNs(const char* moduleName)
   {
      copy(moduleName);
   }
};

// --- ReferenceName ---

class ReferenceName : public IdentifierString
{
public:
   ReferenceName()
   {
   }
   ReferenceName(const wchar16_t* reference)
   {
      copy(reference + StringHelper::findLast(reference, '\'') + 1);
   }
   ReferenceName(const wchar16_t* reference, const wchar16_t* package)
   {
      int length = getlength(package);

      if (StringHelper::compare(reference, package, length) && reference[length] == '\'') {
         copy(reference + length + 1);
      }
      else copy(reference + StringHelper::findLast(reference, '\'') + 1);
   }
};

// --- NamespaceName ---

class NamespaceName : public IdentifierString
{
public:
   static bool hasNameSpace(const wchar16_t* reference)
   {
      return (StringHelper::findLast(reference, '\'', -1) != -1);
   }

   NamespaceName(const wchar16_t* reference)
   {
      int pos = StringHelper::findLast(reference, '\'', 0);
      copy(reference, pos);
      _string[pos] = 0;
   }

   bool compare(const wchar16_t* reference)
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
   operator const wchar16_t*() const { return _string; }

   QuoteTemplate(const wchar16_t* string)
   {
      for (size_t i = 1 ; i < getlength(string) ; i++) {
         if (string[i]=='%') {
            i++;
            if (string[i]=='n') {
               _string.append('\n');
            }
            else if (string[i]=='r') {
               _string.append('\r');
            }
            else if (string[i]=='t') {
               _string.append('\t');
            }
            else if (string[i]=='a') {
               _string.append('\a');
            }
            else if (string[i]=='b') {
               _string.append('\b');
            }
            else if (string[i]!='%') {
               size_t j = i;
               while (string[i] >= '0' && string[i]<='9')
                  i++;

               String<wchar16_t, 12> number(string + j, i - j);
               i--;

               _string.append((wchar16_t)number.toInt());
            }
            else _string.append(string[i]);
         }
         else if (string[i]=='"') {
            if (string[i+1]!='"') {
               break;
            }
            else _string.append(string[i++]);
         }
         else _string.append(string[i]);
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
   size_t typeRef;
   size_t count;
   size_t flags;
   ref_t  parentRef;
};

// --- ClassInfo ---

struct ClassInfo
{
   struct MethodInfo
   {
      ref_t signatureRef;
      int   argNumber;
      ref_t messageId;
      bool  flag;        // true means overridden / newly implemented; false means inherited

      MethodInfo()
      {
         messageId = signatureRef = argNumber = 0;
      }
      MethodInfo(ref_t messageId, ref_t signatureRef, int argNumber, bool flag)
      {
         this->messageId = messageId;
         this->signatureRef = signatureRef;
         this->argNumber = argNumber;
         this->flag = flag;
      }
   };

   typedef MemoryMap<ref_t, bool, false>          MethodMap;
   typedef MemoryMap<const wchar16_t*, int, true> FieldMap;
   typedef MemoryMap<int, ref_t>                  FieldTypeMap;

   ClassHeader  header;
   size_t       size;           // Object size
   ref_t        classClassRef;  // reference to class class VMT
   MethodMap    methods;
   FieldMap     fields;
   FieldTypeMap fieldTypes;

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->write((void*)this, sizeof(ClassHeader));
      writer->writeDWord(size);
      if (!headerAndSizeOnly) {
         writer->writeDWord(classClassRef);
         methods.write(writer);
         fields.write(writer);
         fieldTypes.write(writer);
      }
   }

   void load(StreamReader* reader, bool headerOnly = false)
   {
      reader->read((void*)&header, sizeof(ClassHeader));
      size = reader->getDWord();
      classClassRef = reader->getDWord();
      if (!headerOnly) {
         methods.read(reader);
         fields.read(reader);
         fieldTypes.read(reader);
      }
   }

   ClassInfo()
      : fields(-1), methods(0)
   {
      header.flags = 0;
   }
};

// --- SymbolExpressionInfo ---

struct SymbolExpressionInfo
{
   size_t expressionTypeRef;

   void save(StreamWriter* writer, bool headerAndSizeOnly = false)
   {
      writer->writeDWord(expressionTypeRef);
   }

   void load(StreamReader* reader, bool headerOnly = false)
   {
      expressionTypeRef = reader->getDWord();
   }

   SymbolExpressionInfo()
   {
      expressionTypeRef = 0;
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
inline size_t mapReferenceKey(const wchar16_t* key)
{
   size_t index = StringHelper::findLast(key, '\'');
   const wchar16_t* p = key + StringHelper::findLast(key, '\'', 0) + 1;

   int position = *p - 'a';
   if (position > 26)
      position = 26;
   else if (position < 0)
      position = 0;

   return position;
}

// --- Common type definitions ---

typedef Map<const wchar16_t*, _Module*> ModuleMap;

// --- Reference mapping types ---
typedef MemoryHashTable<const wchar16_t*, ref_t, mapReferenceKey, 29> ReferenceMap;

// --- Message mapping types ---
typedef Map<const wchar16_t*, ref_t> MessageMap;

// --- ParserTable auxiliary types ---
typedef Stack<int>                                           ParserStack;
typedef MemoryMap<const wchar16_t*, int>                     SymbolMap;
typedef MemoryHashTable<size_t, int, syntaxRule, cnHashSize> SyntaxHash;
typedef MemoryHashTable<size_t, int, tableRule, cnHashSize>  TableHash;

// --- miscellaneous routines ---

inline bool isWeakReference(const wchar16_t* referenceName)
{
   return (referenceName != NULL && referenceName[0] != 0 && referenceName[0]=='\'');
}

inline ref_t encodeMessage(ref_t signatureRef, int verbId, int paramCount)
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

inline void decodeMessage(ref_t message, ref_t& signatureRef, int& verbId, int& paramCount)
{
   verbId = (message & VERB_MASK) >> 24;
   signatureRef = (message & SIGN_MASK) >> 4;
   paramCount = message & PARAM_MASK;
}

inline int getParamCount(ref_t message)
{
   int   verb, paramCount;
   ref_t signature;
   decodeMessage(message, signature, verb, paramCount);

   if (paramCount == OPEN_ARG_COUNT)
      return 0;

   return paramCount;
}

inline ref_t getVerb(ref_t message)
{
   int   verb, paramCount;
   ref_t signature;
   decodeMessage(message, signature, verb, paramCount);

   return verb;
}

inline ref_t getSignature(ref_t message)
{
   int   verb, paramCount;
   ref_t signature;
   decodeMessage(message, signature, verb, paramCount);

   return signature;
}

} // _ELENA_

#endif // elenaH

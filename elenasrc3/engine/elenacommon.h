//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENACOMMON_H
#define ELENACOMMON_H

#include "common.h"
#include "elenaconst.h"

namespace elena_lang
{
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

   // --- IdentifierString ---
   class IdentifierString : public String<char, IDENTIFIER_LEN>
   {
   public:
      ustr_t operator*() const { return ustr_t(_string); }

      bool compare(ustr_t s)
      {
         return s.compare(_string);
      }

      bool compare(ustr_t s, size_t index, size_t length)
      {
         return ustr_t(_string + index).compare(s, length);
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

      IdentifierString(ustr_t s1, ustr_t s2, ustr_t s3, ustr_t s4)
         : String(s1)
      {
         append(s2);
         append(s3);
         append(s4);
      }

      IdentifierString(wstr_t s)
      {
         size_t len = IDENTIFIER_LEN;
         StrConvertor::copy(_string, s, getlength(s), len);
         _string[len] = 0;
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

      static bool compareNs(ustr_t referenceName, ustr_t ns, size_t length)
      {
         size_t len = referenceName.length();
         if (len <= length)
            return false;

         if (referenceName.compareSub(ns, 0, length) && referenceName[length] == '\'')
         {
            return true;
         }
         else return false;
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

      void trimLastSubNs()
      {
         size_t index = (**this).findLast('\'', 0);
         _string[index] = 0;
      }

      NamespaceString() = default;
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
         struct Info { addr_t nameRef; int size; } info;
         struct Offset { pos_t disp; } offset;
         struct Inline { int index; int offset; } inlineField;
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

   // --- base collection declaration ---
   typedef Map<ustr_t, ModuleBase*, allocUStr, freeUStr, freeobj> ModuleMap;
}

#endif // ELENACOMMON_H

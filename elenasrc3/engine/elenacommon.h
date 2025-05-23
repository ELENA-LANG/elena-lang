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

   // --- base collection declaration ---
   typedef Map<ustr_t, ModuleBase*, allocUStr, freeUStr, freeobj> ModuleMap;
}

#endif // ELENACOMMON_H

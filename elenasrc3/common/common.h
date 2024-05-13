//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

namespace elena_lang
{

   typedef unsigned int       pos_t;
   typedef unsigned int       ref_t;
   typedef int                arg_t;
   typedef unsigned int       mssg_t;

   typedef unsigned long long pos64_t;
   typedef unsigned long long ref64_t;
   typedef unsigned long long mssg64_t;

   typedef unsigned int       unic_c;

#if defined _M_IX86 || __i386__

   typedef unsigned int       addr_t;
   typedef int                disp_t;

#elif defined _M_X64 || __x86_64__ || __PPC64__ || __aarch64__

   typedef unsigned long long addr_t;
   typedef long long          disp_t;

#endif

#ifdef _MSC_VER

   constexpr auto PATH_SEPARATOR = '\\';

   typedef wchar_t            wide_c;
   typedef wchar_t            path_c;

#else

   constexpr auto PATH_SEPARATOR = '/';

   typedef unsigned short     wide_c;
   typedef char               path_c;

#endif

   constexpr pos_t   INVALID_POS = -1;
   constexpr ref_t   INVALID_REF = -1;
   constexpr ref_t   MAX_OFFSET = 0x7FFFFFFF;
   constexpr addr_t  INVALID_ADDR = -1;
   constexpr size_t  INVALID_SIZE = -1;

}

// --- Common headers ---
#include "tools.h"
#include "ustring.h"
#include "streams.h"
#include "paths.h"
#include "dump.h"
#include "lists.h"
#include "files.h"

#define DEFAULT_STR (elena_lang::ustr_t)nullptr

namespace elena_lang
{
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
}

#endif // COMMON_H

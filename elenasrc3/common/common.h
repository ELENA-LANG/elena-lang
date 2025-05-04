//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
#define DISABLE_WARNING_POP            __pragma(warning( pop )) 
#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(4505)
#define DISABLE_WARNING_UNINITIALIZED_FIELD              DISABLE_WARNING(26495)
// other warnings you want to deactivate...

#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop) 
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
#define DISABLE_WARNING_UNINITIALIZED_FIELD              DISABLE_WARNING(-Wunused-function)
// other warnings you want to deactivate... 

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
// other warnings you want to deactivate... 

#endif

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

#if (defined(_WIN32) || defined(__WIN32__))

   constexpr auto PATH_SEPARATOR = '\\';

   typedef wchar_t            wide_c;
   typedef wchar_t            path_c;

#else

   constexpr auto PATH_SEPARATOR = '/';

   typedef unsigned short     wide_c;
   typedef char               path_c;

#endif

   constexpr pos_t   INVALID_POS = (pos_t)-1;
   constexpr ref_t   INVALID_REF = (ref_t)-1;
   constexpr ref_t   MAX_OFFSET = 0x7FFFFFFF;
   constexpr ref_t   MID_OFFSET = 0x3FFFFFFF;
   constexpr addr_t  INVALID_ADDR = (addr_t)-1;
   constexpr size_t  INVALID_SIZE = (size_t)-1;

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
   // --- ExceptionBase ---
   class ExceptionBase {};

   class AbortError : public ExceptionBase {};

   // --- InternalError ---
   struct InternalError : public ExceptionBase
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
}

#endif // COMMON_H

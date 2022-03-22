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
   constexpr pos_t   INVALID_POS = -1;
   constexpr ref_t   INVALID_REF = -1;
   constexpr addr_t  INVALID_ADDR = -1;

}

#endif // COMMON_H

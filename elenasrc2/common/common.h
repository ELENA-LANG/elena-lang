//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef commonH
#define commonH 1

// --- Common defintions ---

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <wchar.h>

#define PATH_SEPARATOR '\\'

namespace _ELENA_
{
   typedef const wchar_t*  path_t;
   typedef wchar_t         path_c;
   typedef wchar_t         wide_c;
   typedef unsigned int    unic_c;

   typedef const char*     ident_t;
   typedef char            ident_c;
   typedef unsigned char   uident_c;
   typedef size_t          ref_t;

   // --- FileEncoding ---
   enum FileEncoding { feAnsi = 0, feRaw = -1, feUTF8 = -2, feUTF16 = -3, feUTF32 = -4 };
}

#else

#define PATH_SEPARATOR '/'

namespace _ELENA_
{
   typedef const char*     path_t;
   typedef char            path_c;
   typedef unsigned short  wide_c;
   typedef unsigned int    unic_c;

   typedef const char*     ident_t;
   typedef char            ident_c;
   typedef unsigned char   uident_c;
   typedef size_t          ref_t;

   // --- FileEncoding ---
   enum FileEncoding { feUTF8 = 0, feRaw = -1, feUTF16 = -2, feUTF32 = -3 };
}

#endif

// --- Common headers ---
#include "tools.h"
#include "altstrings.h"
#include "streams.h"
#include "dump.h"
#include "lists.h"
#include "files.h"

#define DEFAULT_STR (_ELENA_::ident_t)NULL

namespace _ELENA_
{

// --- Common mapping type definitions ---
typedef Dictionary2D<ident_t, ident_t> ConfigSettings;
typedef _Iterator<ConfigSettings::VItem, _MapItem<ident_t, ConfigSettings::VItem>, ident_t> ConfigCategoryIterator;

// --- Base Config File ---
class _ConfigFile
{
public:
   virtual bool load(path_t path, int encoding) = 0;

   virtual ConfigCategoryIterator getCategoryIt(ident_t name) = 0;

   virtual ident_t getSetting(ident_t category, ident_t key, ident_t defaultValue = NULL) = 0;
   virtual int getIntSetting(ident_t category, ident_t key, int defaultValue = 0)
   {
      String<char, 255> value(getSetting(category, key));

      return value.toInt(defaultValue);
   }

   virtual bool getBoolSetting(ident_t category, ident_t key, bool defaultValue = false)
   {
      String<char, 255> value(getSetting(category, key));

      if (!emptystr(value)) {
         return value.compare("-1");
      }
      else return defaultValue;
   }

   virtual ~_ConfigFile() {}
};

// --- ConstantIdentifier ---

class WideString : public String <wide_c, 0x100>
{
public:
   WideString()
   {

   }
   WideString(ident_t message)
   {
      size_t length = 0x100;
      StringHelper::copy(_string, message, getlength(message), length);
      _string[length] = 0;
   }
   WideString(ident_t message, size_t length)
   {
      size_t wideLength = 0x100;
      StringHelper::copy(_string, message, length, wideLength);
      _string[wideLength] = 0;
   }
};

} // _ELENA_

#endif // commonH

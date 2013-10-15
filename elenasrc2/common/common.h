//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef commonH
#define commonH 1

// --- Common defintions ---

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32

#define PATH_SEPARATOR '\\'

#define _T(x)       L ## x

typedef wchar_t     wchar16_t;
typedef wchar_t     _path_t;
typedef wchar_t     _text_t;

#else

#define PATH_SEPARATOR '/'

#define _T(x)       x

typedef unsigned short wchar16_t;
typedef char           _path_t;
typedef char           _text_t;

#endif

//#ifdef _WIN32
//
//#include <io.h>
//
//#else
//
//#include <wctype.h>
//
//#endif // WIN32

#define ref_t       size_t

//#endif

// --- Common headers ---
#include "tools.h"
#include "altstrings.h"
#include "streams.h"
#include "dump.h"
#include "lists.h"
#include "files.h"

#define DEFAULT_STR (const wchar16_t*)NULL

namespace _ELENA_
{

#ifdef _WIN32

// --- FileEncoding ---
enum FileEncoding { feAnsi = 0, feRaw = -1, feUTF8 = -2, feUTF16 = -3, feUTF32 = -4 };

#else

// --- PrintableValue ---
typedef DynamicString<char> PrintableValue;

// --- FileEncoding ---
enum FileEncoding { feUTF8 = 0, feRaw = -1, feUTF16 = -2, feUTF32 = -3 };

#endif

// --- Param string template ---
typedef String<_text_t, 255> Param;

// --- Common mapping type definitions ---
typedef Dictionary2D<const _text_t*, const _text_t*> ConfigSettings;
typedef _Iterator<ConfigSettings::VItem, _MapItem<const _text_t*, ConfigSettings::VItem>, const _text_t*> ConfigCategoryIterator;

// --- Base Config File ---
class _ConfigFile
{
public:
   virtual bool load(const _path_t* path, int encoding) = 0;

   virtual ConfigCategoryIterator getCategoryIt(const _text_t* name) = 0;

   virtual const _text_t* getSetting(const _text_t* category, const _text_t* key, const _text_t* defaultValue = NULL) = 0;
   virtual int getIntSetting(const _text_t* category, const _text_t* key, int defaultValue = 0)
   {
      Param value(getSetting(category, key));

      return value.toInt(defaultValue);
   }

   virtual bool getBoolSetting(const _text_t* category, const _text_t* key, bool defaultValue = false)
   {
      Param value(getSetting(category, key));

      if (!emptystr(value)) {
         return value.compare(_T("-1"));
      }
      else return defaultValue;
   }

   virtual ~_ConfigFile() {}
};

} // _ELENA_

#endif // commonH

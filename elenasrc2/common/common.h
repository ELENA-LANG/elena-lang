//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                              (C)2005-2014, by Alexei Rakov
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

#define _T(x)       L ## x

typedef wchar_t  wchar16_t;
typedef wchar_t  tchar_t;

#else

#define PATH_SEPARATOR '/'

#define _T(x)       x

typedef unsigned short wchar16_t;
typedef char           tchar_t;

#endif

//#ifdef _WIN32
//
//#include <io.h>
//
//#else
//
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

// --- FileEncoding ---
enum FileEncoding { feUTF8 = 0, feRaw = -1, feUTF16 = -2, feUTF32 = -3 };

#endif

// --- Param string template ---
typedef String<char, 255> UTF8String;

// --- Common mapping type definitions ---
typedef Dictionary2D<const char*, const char*> ConfigSettings;
typedef _Iterator<ConfigSettings::VItem, _MapItem<const char*, ConfigSettings::VItem>, const char*> ConfigCategoryIterator;

// --- Base Config File ---
class _ConfigFile
{
public:
   virtual bool load(const tchar_t* path, int encoding) = 0;

   virtual ConfigCategoryIterator getCategoryIt(const char* name) = 0;

   virtual const char* getSetting(const char* category, const char* key, const char* defaultValue = NULL) = 0;
   virtual int getIntSetting(const char* category, const char* key, int defaultValue = 0)
   {
      String<char, 255> value(getSetting(category, key));

      return value.toInt(defaultValue);
   }

   virtual bool getBoolSetting(const char* category, const char* key, bool defaultValue = false)
   {
      String<char, 255> value(getSetting(category, key));

      if (!emptystr(value)) {
         return value.compare("-1");
      }
      else return defaultValue;
   }

   virtual ~_ConfigFile() {}
};

} // _ELENA_

#endif // commonH

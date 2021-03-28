//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common templates, classes,
//		structures, functions and constants
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef commonH
#define commonH 1

// --- Common definitions ---

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>

#ifdef _MSC_VER

#include <wchar.h>

#define PATH_SEPARATOR '\\'

namespace _ELENA_
{
   typedef wchar_t            path_c;
   typedef wchar_t            wide_c;
   typedef unsigned int       unic_c;

   typedef unsigned char      uident_c;
   typedef unsigned int       ref_t;
   typedef unsigned int       pos_t;
   typedef unsigned long long ref64_t;
   typedef unsigned long long pos64_t;
   typedef unsigned int       mssg_t;
   typedef unsigned long long mssg64_t;

   typedef unsigned __int64   uint64_t;

#ifdef _WIN64

   typedef unsigned __int64   lvaddr_t;

#elif _WIN32

   typedef unsigned int       lvaddr_t;

#endif

   // --- FileEncoding ---
   enum FileEncoding { feAnsi = 0, feRaw = -1, feUTF8 = -2, feUTF16 = -3, feUTF32 = -4 };
}

#define __FASTCALL __fastcall
#define __VECTORCALL __vectorcall

#elif _LINUX

#include <climits>

#define PATH_SEPARATOR '/'

namespace _ELENA_
{
   typedef char                   path_c;
   typedef unsigned short         wide_c;
   typedef unsigned int           unic_c;

   typedef unsigned char          uident_c;
   typedef unsigned int           ref_t;
   typedef unsigned int           pos_t;

#if defined(__LP64__)

   typedef unsigned long long int lvaddr_t;

#else

   typedef unsigned int           lvaddr_t;

#endif

   typedef unsigned long long int ref64_t;
   typedef unsigned long long int pos64_t;
   typedef unsigned int           mssg_t;
   typedef unsigned long long int mssg64_t;

   //typedef unsigned long long int uint64_t;

   // --- FileEncoding ---
   enum FileEncoding { feUTF8 = 0, feRaw = -1, feUTF16 = -2, feUTF32 = -3 };
}

// temporally fast call is supported only for MSC
#define __FASTCALL
#define __VECTORCALL

#endif

// --- Common headers ---
#include "tools.h"
#include "altstrings.h"
#include "streams.h"
#include "dump.h"
#include "lists.h"
#include "files.h"

#define DEFAULT_STR (_ELENA_::ident_t)nullptr

namespace _ELENA_
{

// --- Common mapping type definitions ---
typedef Dictionary2D<ident_t, ident_t> ConfigSettings;
typedef _Iterator<ConfigSettings::VItem, _MapItem<ident_t, ConfigSettings::VItem>, ident_t> ConfigCategoryIterator;

// --- Base Config File ---
class _ConfigFile
{
public:
   struct Node
   {
      _ConfigFile* owner;
      void*        reference;

      ident_t Content()
      {
         if (reference) {
            return owner->getNodeContent(reference);
         }
         else return NULL;
      }

      ident_t Attribute(ident_t name)
      {
         return owner->getNodeAttribute(reference, name);
      }

      bool select(ident_t key, Map<ident_t, Node>& list)
      {
         return owner->select(*this, key, list);
      }

      Node(_ConfigFile* owner, void* reference)
      {
         this->owner = owner;
         this->reference = reference;
      }
      Node(_ConfigFile* owner)
      {
         this->owner = owner;
         this->reference = NULL;
      }
      Node()
      {
         owner = NULL;
         reference = NULL;
      }
   };

   typedef Map<ident_t, Node> Nodes;

   virtual Node get(ident_t key) = 0;

   virtual bool select(ident_t key, Nodes& list) = 0;
   virtual bool select(Node root, ident_t subKey, Nodes& list) = 0;

   virtual ident_t getNodeContent(void* reference) = 0;

   virtual ident_t getNodeAttribute(void* reference, ident_t name) = 0;

   virtual ident_t getSetting(ident_t key)
   {
      Node node = get(key);

      return node.Content();
   }
   virtual int getIntSetting(ident_t key, int defaultValue = 0)
   {
      ident_t value = getSetting(key);

      return emptystr(value) ? defaultValue : value.toInt();
   }
   virtual int getHexSetting(ident_t key, int defaultValue = 0)
   {
      ident_t value = getSetting(key);

      return emptystr(value) ? defaultValue : value.toLong(16);
   }
   virtual int getBoolSetting(ident_t key, bool defaultValue = false)
   {
      ident_t value = getSetting(key);

      if (!emptystr(value)) {
         return value.compare("-1");
      }
      else return defaultValue;
   }

   virtual bool load(path_t path, int encoding) = 0;

   virtual ~_ConfigFile() {}
};

// --- ConstantIdentifier ---

class WideString : public String<wide_c, 0x100>
{
public:
   WideString()
   {

   }
   WideString(ident_t message)
   {
      size_t length = 0x100;
      message.copyTo(_string, length);
      _string[length] = 0;
   }
   WideString(ident_t message, size_t length)
   {
      size_t wideLength = 0x100;
      Convertor::copy(_string, message, length, wideLength);
      _string[wideLength] = 0;
   }
};

} // _ELENA_

#endif // commonH

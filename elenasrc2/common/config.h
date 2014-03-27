//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class header
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef configH
#define configH

#include "common.h"

namespace _ELENA_
{

// --- IniConfigFile ---

class IniConfigFile : public _ConfigFile
{
   ConfigSettings _settings;

public:
   virtual ConfigCategoryIterator getCategoryIt(const char* name)
   {
      return _settings.getIt(name);
   }

   virtual const char* getSetting(const char* category, const char* key, const char* defaultValue = NULL);

   void setSetting(const char* category, const char* key, const char* value);
   void setSetting(const char* category, const char* key, int value);
   void setSetting(const char* category, const char* key, size_t value);
   void setSetting(const char* category, const char* key, bool value);

   void clear(const char* category, const char* key);
   void clear(const char* category);
   void clear();

   virtual bool load(const tchar_t* path, int encoding);
   virtual bool save(const tchar_t* path, int encoding);
   virtual bool save(const tchar_t* path)
   {
      return save(path, 0);
   }

   IniConfigFile();
   IniConfigFile(bool allowDuplicates);
   virtual ~IniConfigFile() { }
};

} // _ELENA_

#endif // configH

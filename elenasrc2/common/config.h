//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class header
//                                              (C)2005-2012, by Alexei Rakov
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
   virtual ConfigCategoryIterator getCategoryIt(const _text_t* name)
   {
      return _settings.getIt(name);
   }

   virtual const _text_t* getSetting(const _text_t* category, const _text_t* key, const _text_t* defaultValue = NULL);

   void setSetting(const _text_t* category, const _text_t* key, const _text_t* value);
   void setSetting(const _text_t* category, const _text_t* key, int value);
   void setSetting(const _text_t* category, const _text_t* key, size_t value);
   void setSetting(const _text_t* category, const _text_t* key, bool value);

   void clear(const _text_t* category, const _text_t* key);
   void clear(const _text_t* category);
   void clear();

   virtual bool load(const _path_t* path, int encoding);
   virtual bool save(const _path_t* path, int encoding);
   virtual bool save(const _path_t* path)
   {
      return save(path, 0);
   }

   IniConfigFile();
   IniConfigFile(bool allowDuplicates);
   virtual ~IniConfigFile() { }
};

} // _ELENA_

#endif // configH

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class header
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef configH
#define configH

#include "common.h"
#include "xmlreader.h"

namespace _ELENA_
{

// --- IniConfigFile ---

class IniConfigFile : public _ConfigFile
{
   ConfigSettings _settings;

public:
   virtual Node get(ident_t key);

   virtual bool select(ident_t key, Map<ident_t, Node>& list);
   virtual bool select(Node, ident_t, Nodes&)
   {
      return false;
   }

   virtual ident_t getNodeContent(void* reference);

   virtual ident_t getNodeAttribute(void* reference, ident_t name);

   ConfigCategoryIterator getCategoryIt(ident_t name)
   {
      return _settings.getIt(name);
   }

   virtual ident_t getSetting(ident_t key)
   {
      return _ConfigFile::getSetting(key);
   }

   ident_t getSetting(ident_t category, ident_t key, ident_t defaultValue = NULL);

   void setSetting(ident_t key, const char* value);
   void setSetting(ident_t key, int value);
   void setSetting(ident_t key, unsigned int value);
   void setSetting(ident_t key, bool value);

   void clearSetting(ident_t key);

   void setSetting(const char* category, const char* key, const char* value); // !! obsolete
   void setSetting(const char* category, const char* key, int value);
   void setSetting(const char* category, const char* key, unsigned int value);
   void setSetting(const char* category, const char* key, bool value);

   void clear(const char* category, const char* key);    // !! obsolete
   void clear(const char* category);
   void clear();

   virtual bool load(path_t path, int encoding);
   virtual bool save(path_t path, int encoding);

   IniConfigFile();
   IniConfigFile(bool allowDuplicates);
   virtual ~IniConfigFile() { }
};

// --- XmlConfigFile ---

class XmlConfigFile : public _ConfigFile
{
   typedef Map<size_t, char*>            NodeValues;
   typedef Dictionary2D<size_t, ident_t> NodeAttributes;

   XMLTree        _tree;
   NodeValues     _values;
   NodeAttributes _attributes;

   size_t find(ident_t key);
   size_t find(XMLNode& node, ident_t key);

public:
   virtual Node get(ident_t key);

   virtual bool select(ident_t key, Map<ident_t, Node>& list);
   virtual bool select(Node root, ident_t subKey, Nodes& list);

   virtual ident_t getNodeContent(void* reference);

   virtual ident_t getNodeAttribute(void* reference, ident_t name);

   //ident_t getSetting(ident_t category, ident_t key, ident_t defaultValue = NULL);

   void appendSetting(ident_t key, ident_t attribute, const char* value);
   void appendSetting(ident_t key, const char* value);

   void removeSetting(ident_t key, const char* value);

   void setSetting(ident_t key, const char* value);
   void setSetting(ident_t key, int value);
   void setSetting(ident_t key, size_t value);
   void setSetting(ident_t key, bool value);

   void clear();

   virtual bool load(path_t path, int encoding);
   virtual bool save(path_t path, int encoding, bool withBOM);

   XmlConfigFile();
   virtual ~XmlConfigFile() { }
};


} // _ELENA_

#endif // configH

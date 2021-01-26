//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class implementation
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "config.h"

#pragma warning(disable : 4100)

using namespace _ELENA_;

// --- IniConfigFile ---

IniConfigFile :: IniConfigFile()
{
}

IniConfigFile :: IniConfigFile(bool allowDuplicates)
   : _settings(allowDuplicates)
{
}

_ConfigFile::Node IniConfigFile :: get(ident_t key)
{
   size_t index = key.find('/');
   if (index != NOTFOUND_POS) {
      String<char, 50> category(key, index);

      ident_t settingName = key + index + 1;
      index = settingName.find('/');
      if (index != NOTFOUND_POS) {
         // HOTFIX : if sub category is used
         String<char, 50> temp(settingName);
         temp[index] = ':';

         const char* value = getSetting((const char*)category, (const char*)temp);

         return _ConfigFile::Node(this, (void*)value);
      }
      else {
         const char* value = getSetting((const char*)category, settingName);

         return _ConfigFile::Node(this, (void*)value);
      }
   }
   else return _ConfigFile::Node();
}

bool IniConfigFile :: select(ident_t key, Map<ident_t, Node>& list)
{
   bool idle = true;
   if (key.endsWith("/*")) {
      String<char, 50> category(key, getlength(key) - 2);
      for (ConfigCategoryIterator it = getCategoryIt((const char*)category) ; !it.Eof() ; it++) {
         idle = false;

         ident_t value = *it;

         list.add(it.key(), _ConfigFile::Node(this, (void*)value.c_str()));
      }
   }

   return !idle;
}

ident_t IniConfigFile :: getNodeContent(void* reference)
{
   return (const char*)reference;
}

ident_t IniConfigFile :: getNodeAttribute(void* reference, ident_t name)
{
   return NULL;
}

bool IniConfigFile :: load(path_t path, int encoding)
{
   String<char, 255> line, key, subKey;
   TextFileReader reader(path, encoding, true);

   if (!reader.isOpened())
      return false;

   char buffer[BLOCK_SIZE];
   while (reader.readLine(line, buffer)) {
      line.trim('\n');
      line.trim('\r');
      line.trim(' ');

      if (emptystr(line)) continue;

      if (line[0]=='[' && line[line.Length() - 1]==']') {
         if (line.Length() < 3) {
            return false;
         }
         key.copy(line + 1, line.Length() - 2);
         key[line.Length() - 2] = 0;
      }
      else {
         if (emptystr(key)) {
            return false;
         }
         size_t pos = ((ident_t)line).find('=');
         if (pos != NOTFOUND_POS) {
            subKey.copy(line, pos);

            _settings.add((ident_t)key, (ident_t)subKey, ((ident_t)line).clone(pos + 1));
         }
         else _settings.add((ident_t)key, (ident_t)line, (char*)NULL);
      }
   }
   return true;
}

bool IniConfigFile :: save(path_t path, int encoding)
{
   TextFileWriter  writer(path, encoding, true);

   if (!writer.isOpened())
      return false;

   // goes through the section keys
   _Iterator<ConfigSettings::VItem, _MapItem<ident_t, ConfigSettings::VItem>, ident_t> it = _settings.start();
   while (!it.Eof()) {
      ConfigCategoryIterator cat_it = _settings.getIt(it.key());
      if (!cat_it.Eof()) {
         writer.writeLiteral("[");
         writer.writeLiteral(it.key());
         writer.writeLiteralNewLine("]");

         while (!cat_it.Eof()) {
            writer.writeLiteral(cat_it.key());
            ident_t value = *cat_it;
            if (!emptystr(value)) {
               writer.writeLiteral("=");
               writer.writeLiteralNewLine(value);
            }
            else writer.writeNewLine();

            cat_it++;
         }
         writer.writeNewLine();
      }
      it++;
   }
   return true;
}

void IniConfigFile :: setSetting(const char* category, const char* key, const char* value)
{
   _settings.add(category, key, ((ident_t)value).clone());
}

void IniConfigFile :: setSetting(const char* category, const char* key, int value)
{
   String<char, 15> string;
   string.appendInt(value);

   _settings.add(category, key, ((ident_t)string).clone());
}

void IniConfigFile :: setSetting(const char* category, const char* key, unsigned int value)
{
   String<char, 15> string;
   string.appendInt(value);

   _settings.add(category, key, ((ident_t)string).clone());
}

void IniConfigFile :: setSetting(const char* category, const char* key, bool value)
{
   _settings.add(category, key, value ? "-1" : "0");
}

void IniConfigFile :: setSetting(ident_t key, const char* value)
{
   size_t index = key.find('/');
   if (index != NOTFOUND_POS) {
      String<char, 50> category(key, index);

      ident_t settingName = key + index + 1;
      index = settingName.find('/');
      if (index != NOTFOUND_POS) {
         // HOTFIX : if sub category is used
         String<char, 50> temp(settingName);
         temp[index] = ':';

         _settings.add(category.c_str(), temp.c_str(), ((ident_t)value).clone());
      }
      else _settings.add(category.c_str(), settingName, ((ident_t)value).clone());
   }
}

void IniConfigFile::setSetting(ident_t key, int value)
{
   if (value != 0) {
      String<char, 15> string;
      string.appendInt(value);

      setSetting(key, string.c_str());
   }
   else clearSetting(key);
}

void IniConfigFile::setSetting(ident_t key, unsigned int value)
{
   String<char, 15> string;
   string.appendInt(value);

   setSetting(key, string.c_str());
}

void IniConfigFile::setSetting(ident_t key, bool value)
{
   setSetting(key, value ? "-1" : "0");
}

ident_t IniConfigFile :: getSetting(ident_t category, ident_t key, ident_t defaultValue)
{
   return _settings.get(category, key, defaultValue);
}

void IniConfigFile :: clearSetting(ident_t key)
{
   size_t index = key.find('/');
   if (index != NOTFOUND_POS) {
      String<char, 50> category(key, index);

      ident_t settingName = key + index + 1;
      index = settingName.find('/');
      if (index != NOTFOUND_POS) {
         // HOTFIX : if sub category is used
         String<char, 50> temp(settingName);
         temp[index] = ':';

         _settings.clear(category.c_str(), temp.c_str());
      }
      else _settings.clear(category.c_str(), settingName);
   }
}

void IniConfigFile :: clear(const char* category, const char* key)
{
	_settings.clear(category, key);
}

void IniConfigFile :: clear(const char* category)
{
   _settings.clear(category);
}

void IniConfigFile :: clear()
{
    _settings.clear();
}

// --- XmlConfigFile ---

XmlConfigFile :: XmlConfigFile()
   : _values(NULL, freestr)
{
   _tree.loadXml("<configuration></configuration>");
}

bool XmlConfigFile :: load(path_t path, int encoding)
{
   try
   {
      if (_tree.load(path, encoding)) {
         return _tree.compareTag("configuration");
      }
   }
   catch (XMLException&)
   {
   }
   return false;
}

bool XmlConfigFile :: save(path_t path, int encoding, bool withBOM)
{
   try
   {
      return _tree.save(path, encoding, withBOM, true);
   }
   catch (XMLException&)
   {
   }
   return false;
}

size_t XmlConfigFile :: find(ident_t key)
{
   size_t length = getlength(key);
   size_t end = key.find('/', length);

   XMLNodeTag tag((const char*)key, end);
   if (_tree.compareTag(tag.c_str())) {
      if (end < length) {
         return find(_tree, key + end + 1);         
      }
      else return _tree.Position();
   }
   else return (size_t)-1;
}

size_t XmlConfigFile :: find(XMLNode& node, ident_t key)
{
   if (!emptystr(key)) {
      size_t length = getlength(key);
      size_t end = key.find('/', length);

      XMLNodeTag tag((const char*)key, end);
      XMLNode foundNode = node.findNode((const char*)tag);
      if (foundNode.Position() != -1) {
         if (end < length) {
            return find(foundNode, (const char*)key + end + 1);
         }
         else return foundNode.Position();
      }

      return (size_t)-1;
   }
   else return node.Position();
}

_ConfigFile::Node XmlConfigFile :: get(ident_t key)
{
   size_t position = find(key);
   if (position != NOTFOUND_POS) {
      return _ConfigFile::Node(this, (void*)position);
   }
   else return _ConfigFile::Node();
}

bool XmlConfigFile :: select(ident_t key, Map<ident_t, _ConfigFile::Node>& list)
{
   size_t length = getlength(key);
   size_t end = key.findLast('/', length);

   size_t position = 0;
   if (length > end) {
      XMLNodeTag tag((const char*)key, end);

      position = find((const char*)tag);
   }

   if (position == NOTFOUND_POS)
      return false;

   XMLNode node(position, &_tree);
   NodeList nodeList;
   if (node.getNodeList(nodeList)) {
      XMLNodeTag tag;
      for (NodeList::Iterator it = nodeList.start(); !it.Eof(); it++) {
         position = (*it).Position();
         (*it).readTag(tag);

         list.add(tag.c_str(), _ConfigFile::Node(this, (void*)position));
      }

      return true;
   }
   else return false;
}

bool XmlConfigFile :: select(Node root, ident_t key, Map<ident_t, _ConfigFile::Node>& list)
{
   bool found = false;
   size_t position = (size_t)root.reference;

   XMLNode node(position, &_tree);
   NodeList nodeList;
   if (node.getNodeList(nodeList)) {
      XMLNodeTag tag;
      for (NodeList::Iterator it = nodeList.start(); !it.Eof(); it++) {
         position = (*it).Position();
         (*it).readTag(tag);

         if ((*it).compareTag(key)) {
            found = true;

            list.add(tag.c_str(), _ConfigFile::Node(this, (void*)position));
         }
      }
   }

   return found;
}

ident_t XmlConfigFile :: getNodeContent(void* reference)
{
   if (!_values.exist((size_t)reference)) {
      XMLNode node((size_t)reference, &_tree);

      DynamicString<char> content;
      node.readContent(content);

      char* value = content.cut();
      _values.add((size_t)reference, value);

      return value;
   }
   else return _values.get((size_t)reference);
}

ident_t XmlConfigFile :: getNodeAttribute(void* reference, ident_t name)
{
   const char* value = _attributes.get((size_t)reference, name, DEFAULT_STR).c_str();
   if (emptystr(value)) {
      XMLNode node((size_t)reference, &_tree);

      DynamicString<char> attrValue;
      if (node.readAttribute(name, attrValue)) {
         char* s = attrValue.cut();

         _attributes.add((size_t)reference, name, s);

         return s;
      }
      else return NULL;
   }
   else return value;
}

void XmlConfigFile :: setSetting(ident_t key, const char* value)
{
   // clear cached values
   _values.clear();
   _attributes.clear();

   size_t position = find(key);

   if (position != NOTFOUND_POS) {
      XMLNode node(position, &_tree);

      node.writeContent(value);
   }
   else {
      size_t length = getlength(key);
      size_t end = key.findLast('/', length);
      if (end != NOTFOUND_POS) {
         String<char, 255> subCategory(key, end);
         position = find(subCategory.c_str());
         if (position == NOTFOUND_POS) {
            setSetting(subCategory.c_str(), DEFAULT_STR);
            position = find(subCategory.c_str());
         }

         XMLNode parent(position, &_tree);
         XMLNode newNode = parent.appendNode(key + end + 1);

         newNode.writeContent(value);
      }
      else {
         XMLNode parent(0, &_tree);
         XMLNode newNode = parent.appendNode(key);

         newNode.writeContent(value);
      }
   }
}

void XmlConfigFile :: setSetting(ident_t key, int value)
{
   String<char, 15> string;
   string.appendInt(value);

   setSetting(key, (ident_t)string);
}

void XmlConfigFile :: setSetting(ident_t key, size_t value)
{
   String<char, 15> string;
   string.appendSize(value);

   setSetting(key, (ident_t)string);
}

void XmlConfigFile :: setSetting(ident_t key, bool value)
{
   setSetting(key, value ? "-1" : "0");
}

void XmlConfigFile  :: appendSetting(ident_t key, ident_t attribute, const char* value)
{
   size_t length = getlength(key);
   size_t end = key.findLast('/', length);
   if (end != NOTFOUND_POS) {
      String<char, 255> subCategory(key, end);

      size_t position = find(subCategory.c_str());
      if (position == NOTFOUND_POS) {
         setSetting(subCategory.c_str(), DEFAULT_STR);
         position = find(subCategory.c_str());
      }

      XMLNode parent(position, &_tree);
      XMLNode newNode = parent.appendNode(key + end + 1);

      newNode.writeAttribute("key", attribute);
      newNode.writeContent(value);
   }
}

void XmlConfigFile :: removeSetting(ident_t key, const char* value)
{
   DynamicString<char> content;

   size_t length = getlength(key);
   size_t end = key.findLast('/', length);
   if (end != NOTFOUND_POS) {
      String<char, 255> subCategory(key, end);

      size_t position = find(subCategory.c_str());
      if (position == NOTFOUND_POS) {
         setSetting(subCategory.c_str(), DEFAULT_STR);
         position = find(subCategory.c_str());
      }

      XMLNode node(position, &_tree);
      NodeList nodeList;
      if (node.getNodeList(nodeList)) {
         for (NodeList::Iterator it = nodeList.start(); !it.Eof(); it++) {
            position = (*it).Position();
            (*it).readContent(content);
            if (ident_t(content.str()).compare(value)) {
               (*it).remove();
               break;
            }
         }
      }

   }
}

void XmlConfigFile :: appendSetting(ident_t key, const char* value)
{
   size_t length = getlength(key);
   size_t end = key.findLast('/', length);
   if (end != NOTFOUND_POS) {
      String<char, 255> subCategory(key, end);

      size_t position = find(subCategory.c_str());
      if (position == NOTFOUND_POS) {
         setSetting(subCategory.c_str(), DEFAULT_STR);
         position = find(subCategory.c_str());
      }

      XMLNode parent(position, &_tree);
      XMLNode newNode = parent.appendNode(key + end + 1);

      newNode.writeContent(value);
   }
}

void XmlConfigFile :: clear()
{
   _values.clear();
   _attributes.clear();

   _tree.loadXml("<configuration></configuration>");
}

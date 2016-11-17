//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class implementation
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "config.h"

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
   int index = key.find('/');
   if (index != -1) {
      String<char, 50> category(key, index);

      ident_t settingName = key + index + 1;
      index = settingName.find('/');
      if (index > 0) {
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
         int pos = ((ident_t)line).find('=');
         if (pos != -1) {
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

void IniConfigFile :: setSetting(const char* category, const char* key, size_t value)
{
   String<char, 15> string;
   string.appendInt(value);

   _settings.add(category, key, ((ident_t)string).clone());
}

void IniConfigFile :: setSetting(const char* category, const char* key, bool value)
{
   _settings.add(category, key, value ? "-1" : "0");
}

ident_t IniConfigFile :: getSetting(ident_t category, ident_t key, ident_t defaultValue)
{
   return _settings.get(category, key, defaultValue);
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

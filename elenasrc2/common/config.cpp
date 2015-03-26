//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains Config File class implementation
//                                              (C)2005-2015, by Alexei Rakov
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
         if (line.find('=') != -1) {
            int pos = line.find('=');
            subKey.copy(line, pos);

            _settings.add(key, subKey, line.clone(pos + 1));
         }
         else _settings.add(key, line, (char*)NULL);
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
   _Iterator<ConfigSettings::VItem, _MapItem<const char*, ConfigSettings::VItem>, const char*> it = _settings.start();
   while (!it.Eof()) {
      ConfigCategoryIterator cat_it = _settings.getIt(it.key());
      if (!cat_it.Eof()) {
         writer.writeLiteral("[");
         writer.writeLiteral(it.key());
         writer.writeLiteralNewLine("]");

         while (!cat_it.Eof()) {
            writer.writeLiteral(cat_it.key());
            const char* value = *cat_it;
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
   _settings.add(category, key, StringHelper::clone(value));
}

void IniConfigFile :: setSetting(const char* category, const char* key, int value)
{
   String<char, 15> string;
   string.appendInt(value);

   _settings.add(category, key, string.clone());
}

void IniConfigFile :: setSetting(const char* category, const char* key, size_t value)
{
   String<char, 15> string;
   string.appendInt(value);

   _settings.add(category, key, string.clone());
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

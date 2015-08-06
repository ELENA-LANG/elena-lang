//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "instance.h"
#include "rtman.h"
#include "config.h"

#define PROJECT_CATEGORY            "project"
#define LIBRARY_PATH                "libpath"

using namespace _ELENA_;

// --- Instance::ImageSection ---

void* Instance::ImageSection :: get(size_t position) const
{
   return (unsigned char*)_section + position;
}

bool Instance::ImageSection :: read(size_t position, void* s, size_t length)
{
   if (position < _length && _length >= position + length) {
      memcpy(s, (unsigned char*)_section + position, length);

      return true;
   }
   else return false;
}

// --- Instance ---

Instance :: Instance(path_t rootPath)
   : _rootPath(rootPath)
{
}

bool Instance :: loadConfig(path_t configFile)
{
   Path configPath(_rootPath);
   Path::combinePath(configPath, configFile);

   IniConfigFile config;
   if (!config.load(configPath, feUTF8)) {
      return false;
   }

   Path path(_rootPath);
   Path::combinePath(path, config.getSetting(PROJECT_CATEGORY, LIBRARY_PATH, NULL));

   if (!emptystr(path)) {
      _loader.setRootPath(path);
   }

   return true;
}

void Instance :: init(void* debugSection, ident_t package, path_t configPath)
{
   _debugSection.init(debugSection);

   loadConfig(configPath);
   _loader.setPackage(package);
}

int Instance :: readCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   RTManager manager;

   ImageSection image;
   MemoryReader reader(&image);

   return manager.readCallStack(reader, framePosition, currentAddress, startLevel, buffer, maxLength);
}

int Instance :: loadAddressInfo(size_t retPoint, ident_c* buffer, size_t maxLength)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, 8);

   return manager.readAddressInfo(reader, retPoint, &_loader, buffer, maxLength);
}

int Instance :: loadClassName(size_t classAddress, ident_c* buffer, size_t length)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, 8);

   return manager.readClassName(reader, classAddress, buffer, length);
}

bool Instance :: initSubjectSection(ImageSection& subjectSection)
{
   void* ptr = _debugSection.get(_debugSection.Length());
   int size = *((int*)ptr);

   if (size > 0) {
      subjectSection.init(ptr, size + 8);

      return true;
   }
   else return false;
}

int Instance::loadSubjectName(size_t subjectRef, ident_c* buffer, size_t length)
{
   RTManager manager;

   // initialize image section ;
   // it directly follows debug section
   ImageSection subjectSection;
   if (initSubjectSection(subjectSection)) {
      MemoryReader reader(&subjectSection);

      ref_t verb, subject;
      int count;
      decodeMessage(subjectRef, subject, verb, count);

      return manager.readSubjectName(reader, subject, buffer, length);
   }
   else return 0;
}

void* Instance :: loadSymbol(ident_t name)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, 8);

   return manager.loadSymbol(reader, name);
}

void* Instance :: loadSubject(ident_t name)
{
   RTManager manager;

   // initialize image section ;
   // it directly follows debug section
   ImageSection subjectSection;
   if (initSubjectSection(subjectSection)) {
      void* ptr = _debugSection.get(_debugSection.Length());
      int size = *((int*)ptr);

      subjectSection.init(ptr, size + 8);

      MemoryReader reader(&subjectSection);

      return manager.loadSubject(reader, name);
   }
   else return NULL;
}

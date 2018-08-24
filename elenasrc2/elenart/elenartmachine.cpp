//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include "rtman.h"
#include "config.h"
#include "bytecode.h"

#define LIBRARY_PATH                "configuration/library/path"

using namespace _ELENA_;

// --- Instance ---

void ELENARTMachine :: start(void* programEntry)
{
   _Entry entry;
   entry.address = programEntry;

   (*entry.entry)();
}

// !! --

// --- Instance::ImageSection ---

void* ELENARTMachine :: ImageSection :: get(pos_t position) const
{
   return (unsigned char*)_section + position;
}

bool ELENARTMachine :: ImageSection :: read(pos_t position, void* s, pos_t length)
{
   if (position < _length && _length >= position + length) {
      memcpy(s, (unsigned char*)_section + position, length);

      return true;
   }
   else return false;
}

ELENARTMachine :: ELENARTMachine(path_t rootPath)
   : _rootPath(rootPath), _verbs(0)
{
   ByteCodeCompiler::loadVerbs(_verbs);
}

bool ELENARTMachine :: loadConfig(path_t configFile)
{
   Path configPath((path_t)_rootPath);
   configPath.combine(configFile);

   XmlConfigFile config;
   if (!config.load(configPath.c_str(), feUTF8)) {
      return false;
   }

   Path path(_rootPath.c_str(), config.getSetting(LIBRARY_PATH));

   if (!emptystr(path)) {
      _loader.setRootPath(path.c_str());
   }

   return true;
}

void ELENARTMachine :: init(void* debugSection, void* messageTable, path_t configPath)
{
   IdentifierString package;

   _debugOffset = _debugSection.init(debugSection, package);
   _messageSection = messageTable;

   loadConfig(configPath);
   _loader.setNamespace(package);
}

int ELENARTMachine :: readCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   RTManager manager;

   ImageSection image;
   MemoryReader reader(&image);

   return manager.readCallStack(reader, framePosition, currentAddress, startLevel, buffer, maxLength);
}

int ELENARTMachine :: loadAddressInfo(size_t retPoint, char* buffer, size_t maxLength)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, _debugOffset);

   return manager.readAddressInfo(reader, retPoint, &_loader, buffer, maxLength);
}

int ELENARTMachine :: loadClassName(size_t classAddress, char* buffer, size_t length)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, _debugOffset);

   return manager.readClassName(reader, classAddress, buffer, length);
}

bool ELENARTMachine :: initSubjectSection(ImageSection& subjectSection)
{
   void* ptr = _debugSection.get(_debugSection.Length());
   int size = *((int*)ptr);

   if (size > 0) {
      subjectSection.init(ptr, size + 8);

      return true;
   }
   else return false;
}

int ELENARTMachine :: loadSubjectName(size_t subject, char* buffer, size_t length)
{
   RTManager manager;

   // initialize image section ;
   // it directly follows debug section
   ImageSection subjectSection;
   if (initSubjectSection(subjectSection)) {
      MemoryReader reader(&subjectSection);

      return manager.readSubjectName(reader, subject, buffer, length);
   }
   else return 0;
}

int ELENARTMachine :: loadMessageName(size_t subjectRef, char* buffer, size_t length)
{
   RTManager manager;

   // initialize image section ;
   // it directly follows debug section
   ImageSection subjectSection;
   if (initSubjectSection(subjectSection)) {
      MemoryReader reader(&subjectSection);

      ref_t action;
      int count;
      decodeMessage(subjectRef, action, count);

      size_t used = 0;
      if (test(subjectRef, encodeAction(SIGNATURE_FLAG))) {
         ImageSection messageSection;
         messageSection.init(_messageSection, 0x10000); // !! dummy size

         ref_t verb = messageSection[action];
         used += manager.readSubjectName(reader, verb, buffer + used, length - used);
      }
      else used += manager.readSubjectName(reader, action, buffer + used, length - used);

      if (count > 0) {
         size_t dummy = 10;
         String<char, 10>temp;
         temp.appendInt(count);

         buffer[used++] = '[';
         Convertor::copy(buffer + used, temp, getlength(temp), dummy);
         used += dummy;
         buffer[used++] = ']';
      }
      buffer[used] = 0;

      return used;
   }
   else return 0;
}

void* ELENARTMachine :: loadSymbol(ident_t name)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, _debugOffset);

   return manager.loadSymbol(reader, name);
}

void* ELENARTMachine :: loadSubject(ident_t name)
{
   if (name.find('$') != -1) {
      //setStatus("Invalid subject");

      return 0;
   }

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

void* ELENARTMachine :: loadMessage(ident_t name)
{
   if (name.find('$') != -1) {
      //setStatus("Invalid subject");

      return 0;
   }

   RTManager manager;

   // initialize image section ;
   // it directly follows debug section
   ImageSection subjectSection;
   if (initSubjectSection(subjectSection)) {
      void* ptr = _debugSection.get(_debugSection.Length());
      int size = *((int*)ptr);

      subjectSection.init(ptr, size + 8);

      MemoryReader reader(&subjectSection);

      return manager.loadMessage(reader, name, _verbs);
   }
   else return NULL;
}

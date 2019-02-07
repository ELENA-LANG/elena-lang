//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2019, by Alexei Rakov
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

void ELENARTMachine :: startSTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry)
{
   // setting up system
   __routineProvider.Prepare();
   __routineProvider.InitSTA((SystemEnv*)env, frameHeader);

   _Entry entry;
   entry.address = programEntry;

   (*entry.entry)();

   // winding down system
   Exit(0);
}

void ELENARTMachine :: startMTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry)
{
   // setting up system
   __routineProvider.Prepare();
   __routineProvider.InitMTA((SystemEnv*)env, frameHeader);

   _Entry entry;
   entry.address = programEntry;

   (*entry.entry)();

   // winding down system
   Exit(0);
}

void ELENARTMachine :: startThread(ProgramHeader* frameHeader, SystemEnv* env, void* entryPoint, int index)
{
   __routineProvider.NewThread(env, frameHeader);

   _Entry entry;
   entry.address = entryPoint;

   (*entry.evaluate)((void*)index);

   __routineProvider.ExitThread(env, 0, false);
}

void ELENARTMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}

void ELENARTMachine :: ExitThread(SystemEnv* env, int exitCode)
{
   __routineProvider.ExitThread(env, exitCode, true);
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
//   ByteCodeCompiler::loadVerbs(_verbs);
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

int ELENARTMachine :: loadSubjectName(size_t subjectRef, char* buffer, size_t length)
{
   ImageSection messageSection;
   messageSection.init(_messageSection, 0x10000); // !! dummy size

   ref_t actionPtr = messageSection[subjectRef * 8];
   if (actionPtr == 0) {
      size_t used = length;
      pos_t namePtr = messageSection[subjectRef * 8 + 4];

      MemoryReader reader(&messageSection);
      reader.seek(namePtr);

      IdentifierString messageName;
      reader.readString(messageName);

      Convertor::copy(buffer, messageName.c_str(), messageName.Length(), used);

      return used;
   }
   else return loadSubjectName(actionPtr, buffer, length);
}

int ELENARTMachine :: loadMessageName(size_t messageRef, char* buffer, size_t length)
{
   int paramCount = 0;
   ref_t actionRef, flags;
   decodeMessage(messageRef, actionRef, paramCount, flags);

   int used = loadSubjectName(actionRef, buffer, length);
   if (used > 0) {
      size_t dummy = 10;
      String<char, 10>temp;
      temp.appendInt(paramCount);

      buffer[used++] = '[';
      Convertor::copy(buffer + used, temp, getlength(temp), dummy);
      used += dummy;
      buffer[used++] = ']';
   }

   return used;
}

void* ELENARTMachine :: loadSymbol(ident_t name)
{
   RTManager manager;
   MemoryReader reader(&_debugSection, _debugOffset);

   return manager.loadSymbol(reader, name);
}

void* ELENARTMachine :: loadSubject(ident_t name)
{
   ImageSection messageSection;
   messageSection.init(_messageSection, 0x10000); // !! dummy size
   MemoryReader reader(&messageSection);

   for (ref_t subjectRef = 1; true; subjectRef++) {
      if (messageSection[subjectRef * 8] == 0) {
         pos_t namePtr = messageSection[subjectRef * 8 + 4];
         if (!namePtr)
            break;

         reader.seek(namePtr);

         IdentifierString messageName;
         reader.readString(messageName);
         if (messageName.compare(name)) {
            return (void*)subjectRef;
         }
      }
   }

   return nullptr;
}

void* ELENARTMachine :: loadMessage(ident_t message)
{
   IdentifierString messageName;
   size_t subject = 0;
   size_t param = 0;
   int paramCount = -1;
   for (size_t i = 0; i < getlength(message); i++) {
      if (message[i] == '[') {
         if (message[getlength(message) - 1] == ']') {
            messageName.copy(message + i + 1, getlength(message) - i - 2);
            paramCount = messageName.ident().toInt();
            if (paramCount > ARG_COUNT)
               return nullptr;
         }
         else return nullptr;

         param = i;
      }
      else if (message[i] >= 65 || (message[i] >= 48 && message[i] <= 57)) {
      }
      else if (message[i] == ']' && i == (getlength(message) - 1)) {
      }
      else return nullptr;
   }

   ref_t flags = 0;

   if (param != 0) {
      messageName.copy(message + subject, param - subject);
   }
   else messageName.copy(message + subject);

   if (messageName.ident().startsWith("prop#")) {
      flags |= PROPERTY_MESSAGE;

      messageName.cut(0, getlength("prop#"));
   }

   ref_t actionRef = (ref_t)loadSubject(messageName.ident());
   if (!actionRef)
      return nullptr;

   return (void*)(encodeMessage(actionRef, paramCount, flags));
}

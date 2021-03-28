//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include "rtman.h"
#include "config.h"
#include "bytecode.h"
#include "core.h"

#define LIBRARY_PATH                "configuration/library/path"

using namespace _ELENA_;

#ifdef _WIN64

constexpr auto cnNameOffset = sizeof(VMTXHeader) + sizeof(uintptr_t);
constexpr auto cnPackageOffset = cnNameOffset + sizeof(uintptr_t);

#else

constexpr size_t cnNameOffset = sizeof(VMTHeader) + sizeof(uintptr_t);
constexpr size_t cnPackageOffset = cnNameOffset + sizeof(uintptr_t);

#endif

// --- Instance ---

void ELENARTMachine :: startSTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry)
{
   // setting up system
   __routineProvider.InitSTA((SystemEnv*)env, frameHeader);

   _Entry entry;
   entry.address = env->Invoker;

   // executing the program
   int retVal = entry.evaluate3(0, programEntry, nullptr);

   // winding down system
   Exit(0);
}

void ELENARTMachine :: startMTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry)
{
   // setting up system
   __routineProvider.InitMTA((SystemEnv*)env, frameHeader);

   _Entry entry;
   entry.address = env->Invoker;

   // executing the program
   int retVal = entry.evaluate3(0, programEntry, nullptr);

   // winding down system
   Exit(0);
}

void ELENARTMachine :: startThread(ProgramHeader* frameHeader, SystemEnv* env, void* threadEntry, int index)
{
   __routineProvider.NewThread(env, frameHeader);

   _Entry entry;
   entry.address = env->Invoker;

   entry.evaluate3(0, threadEntry, &index);

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

ELENARTMachine :: ELENARTMachine(path_t rootPath, path_t execFileName)
   : _rootPath(rootPath), _generated(nullptr)
{
   _messageSection = nullptr;
   _mattributesSection = nullptr;

   _debugFilePath.copy(execFileName);
   _debugFilePath.changeExtension("dn");
}

bool ELENARTMachine :: loadDebugSection()
{
   if (_debugFilePath.isEmpty())
      return false;

   FileReader reader(_debugFilePath.c_str(), feRaw, false);
   if (!reader.isOpened()) {
      // clear the path to indicate an absence of debug data
      _debugFilePath.clear();

      return false;
   }
   else {
      char header[5];
      reader.read(header, 5);
      if (!ident_t(DEBUG_MODULE_SIGNATURE).compare(header, 0, 5)) {
         // clear the path to indicate invalid debug data
         _debugFilePath.clear();

         return false;
      }

      size_t len = reader.getDWord();
      MemoryWriter writer(&_debugSection);
      writer.read(&reader, len);

      return len != 0;
   }
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

void ELENARTMachine :: init(void* messageTable, void* mattributeTable, path_t configPath)
{
   _messageSection = messageTable;
   _mattributesSection = mattributeTable;

   loadConfig(configPath);
}

int ELENARTMachine :: readCallStack(size_t framePosition, lvaddr_t currentAddress, lvaddr_t startLevel, lvaddr_t* buffer, pos_t maxLength)
{
   RTManager manager;

   ImageSection image;
   MemoryReader reader(&image);

   return manager.readCallStack(reader, framePosition, currentAddress, startLevel, buffer, maxLength);
}

lvaddr_t ELENARTMachine :: loadAddressInfo(size_t retPoint, char* buffer, size_t maxLength)
{
   // lazy load of debug data
   if (_debugSection.Length() == 0 && !loadDebugSection())
      return 0;

   RTManager manager;
   MemoryReader reader(&_debugSection);

   // skip a debugger entry pointer
   reader.getDWord();

   // set the root namespace
   _loader.setNamespace(reader.getLiteral(DEFAULT_STR));

   return manager.readAddressInfo(reader, retPoint, &_loader, buffer, maxLength);
}

size_t ELENARTMachine :: loadClassName(lvaddr_t classAddress, char* buffer, size_t length)
{
   uintptr_t packagePtr = *(uintptr_t*)(classAddress - cnPackageOffset);
   uintptr_t namePtr = *(uintptr_t*)(classAddress - cnNameOffset);

   char* name = (char*)namePtr;
   char* ns = ((char**)packagePtr)[0];

   size_t ns_len = length;
   if (!ident_t(ns).copyTo(buffer, ns_len))
      return 0;

   length -= ns_len;
   if (!ident_t(name).copyTo(buffer + ns_len, length))
      return 0;

   return length + ns_len;
}

size_t ELENARTMachine :: loadSubjectName(ref_t subjectRef, char* buffer, size_t length)
{
   ImageSection messageSection;
   messageSection.init(_messageSection, 0x1000000); // !! dummy size

   ref_t actionPtr = messageSection[subjectRef * sizeof(uintptr_t) * 2];

   if (actionPtr == 0) {
      size_t used = length;
      pos_t namePtr = messageSection[subjectRef * sizeof(uintptr_t) * 2 + sizeof(uintptr_t)];

      MemoryReader reader(&messageSection);
      reader.seek(namePtr);

      IdentifierString messageName;
      reader.readString(messageName);

      Convertor::copy(buffer, messageName.c_str(), messageName.Length(), used);

      return used;
   }
   else return loadSubjectName(actionPtr, buffer, length);
}

size_t ELENARTMachine :: loadMessageName(mssg_t messageRef, char* buffer, size_t length)
{
   size_t prefixLen = 0;
   pos_t paramCount = 0;
   ref_t actionRef, flags;
   decodeMessage(messageRef, actionRef, paramCount, flags);
   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      size_t len = 7;
      Convertor::copy(buffer, "params#", 7, len);

      buffer += len;
      length -= len;
      prefixLen += len;
   }

   size_t used = loadSubjectName(actionRef, buffer, length);
   if (used > 0) {
      size_t dummy = 10;
      String<char, 10>temp;
      temp.appendInt(paramCount);

      buffer[used++] = '[';
      Convertor::copy(buffer + used, temp, getlength(temp), dummy);
      used += dummy;
      buffer[used++] = ']';
   }

   return prefixLen + used;
}

lvaddr_t ELENARTMachine :: loadMetaAttribute(ident_t name, int category)
{
   ImageSection mattrSection;
   mattrSection.init(_mattributesSection, 0x10000); // !! dummy size
   MemoryReader reader(&mattrSection);

   pos_t len = reader.getDWord();

   RTManager manager;

   return manager.loadMetaAttribute(reader, name, category, len);
}

lvaddr_t ELENARTMachine :: loadSignatureMember(mssg_t message, int index)
{
   ImageSection messageSection;
   messageSection.init(_messageSection, 0x1000000); // !! dummy size

   return SystemRoutineProvider::GetSignatureMember(messageSection.get(0), message, index);
}

constexpr pos_t MessageEntryLen = sizeof(uintptr_t) * 2;

ref_t ELENARTMachine :: loadSubject(ident_t name)
{
   ImageSection messageSection;
   messageSection.init(_messageSection, 0x1000000); // !! dummy size
   MemoryReader reader(&messageSection);

   for (ref_t subjectRef = 1; true; subjectRef++) {
      if (messageSection[subjectRef * MessageEntryLen] == 0) {
         pos_t namePtr = messageSection[subjectRef * MessageEntryLen + sizeof(uintptr_t)];
         if (!namePtr)
            break;

         reader.seek(namePtr);

         IdentifierString messageName;
         reader.readString(messageName);

         if (messageName.compare(name)) {
            return subjectRef;
         }
      }
   }

   return 0;
}

mssg_t ELENARTMachine :: loadMessage(ident_t message)
{
   IdentifierString messageName;
   pos_t paramCount = -1;
   ref_t flags = 0;

   if (SystemRoutineProvider::parseMessageLiteral(message, messageName, paramCount, flags)) {
      ref_t actionRef = loadSubject(messageName.ident());
      if (!actionRef)
         return 0;

      return encodeMessage(actionRef, paramCount, flags);
   }
   else return 0;
}

ref_t ELENARTMachine :: loadDispatcherOverloadlist(ident_t referenceName)
{
   return (ref_t)loadMetaAttribute(referenceName, caExtOverloadlist);
}

int ELENARTMachine :: loadExtensionDispatcher(const char* moduleList, mssg_t message, void* output)
{
   // load message name
   char messageName[IDENTIFIER_LEN];
   size_t mssgLen = loadMessageName(message, messageName, IDENTIFIER_LEN);
   messageName[mssgLen] = 0;

   int len = 0;

   // search message dispatcher
   IdentifierString messageRef;
   int listLen = getlength(moduleList);
   int i = 0;
   while (moduleList[i]) {
      ident_t ns = moduleList + i;

      messageRef.copy(ns);
      messageRef.append('\'');
      messageRef.append(messageName);

      ref_t listRef = loadDispatcherOverloadlist(messageRef.c_str());
      if (listRef) {
         ((int*)output)[len] = listRef;
         len++;
      }

      i += getlength(ns) + 1;
   }

   return len;
}

uintptr_t ELENARTMachine :: createPermString(SystemEnv* env, ident_t s, uintptr_t classPtr)
{
   size_t nameLen = getlength(s) + 1;
   uintptr_t nameAddr = (uintptr_t)SystemRoutineProvider::GCRoutinePerm(env->Table, align(nameLen, gcPageSize32),
      env->GCPERMSize);

   Convertor::copy((char*)nameAddr, s.c_str(), nameLen, nameLen);

   ObjectPage32* header = (ObjectPage32*)(nameAddr - elObjectOffset32);
   header->vmtPtr = classPtr;
   header->size = nameLen;

   return nameAddr;
}

inline uintptr_t RetrievePackageVMT(uintptr_t ptr)
{
   // HOTFIX : it is hard-coded, better to redesign the routine;
   // probably to keep the reference to the string class in SystemEnv

   uintptr_t str = *(uintptr_t*)(ptr - sizeof(VMTHeader) - 4);

   return *(uintptr_t*)(str - elPageVMTOffset32);
}

lvaddr_t ELENARTMachine :: inherit(SystemEnv* env, const char* name, VMTEntry* src, VMTEntry* base, size_t srcLength, 
   size_t baseLength, pos_t* addresses, size_t length, int flags)
{
   static int nameIndex = 0;
   static uintptr_t packageAddr = 0;

   bool namedOne = !emptystr(name);
   if (namedOne) {
      void* addr = _generated.get(name);
      if (addr)
         return (lvaddr_t)addr;
   }

   // TODO : check if the source class is stateless interface (and probably without static fields?)

   uintptr_t stringVMT = RetrievePackageVMT((uintptr_t)src);

   // HOTFIX : generate package name (note it is hard-coded, better to analize class attributes and generate accordinately)
   if (!packageAddr) {
      packageAddr = createPermString(env, "$d", stringVMT);
   }

   // HOTFIX : generate class name (note it is hard-coded, better to analize class attributes and generate accordinately)
   IdentifierString dynamicName("'");
   if (emptystr(name)) {
      nameIndex++;

      dynamicName.append('$');
      dynamicName.appendHex(nameIndex);
   }
   else dynamicName.append(name);

   uintptr_t nameAddr = createPermString(env, dynamicName.c_str(), stringVMT);

   // HOTFIX : currently only two built-in static fields are supported,;
   // the correct check should be implemented; 
   // interface static and constant fields should be supported
   int staticSize = 2;

   size_t size = (srcLength * sizeof(VMTEntry)) + sizeof(VMTHeader) + elObjectOffset32 + staticSize * sizeof(uintptr_t);
   lvaddr_t ptr = (lvaddr_t)SystemRoutineProvider::GCRoutinePerm(env->Table, size,
      env->GCPERMSize);

   // HOTFIX : hard-codied copy build-in static variables
   *(uintptr_t*)ptr = packageAddr;
   *(uintptr_t*)(ptr + 4) = nameAddr;

   VMTHeader* header = (VMTHeader*)(ptr + staticSize * sizeof(uintptr_t));
   VMTEntry* entries = (VMTEntry*)(ptr + staticSize * sizeof(uintptr_t) + sizeof(VMTHeader));

   size_t i = 0;
   size_t j = 0;
   size_t addr_i = 0;

   while (i < srcLength) {
      if (base[j].message == src[i].message) {
         entries[i] = src[i];
         j++;
      }
      else if (src[i].address) {
         // if the method is not abstract
         entries[i] = src[i];
      }
      else {
         entries[i].message = src[i].message;
         entries[i].address = addresses[addr_i];
         
         addr_i += (addr_i < (length - 1) ? 1 : 0);
      }

      i++;
   }

   header->parentRef = (pos_t)src;
   header->count = srcLength;
   header->flags = flags;
   header->classRef = (pos_t)base;

   if (namedOne)
      _generated.add(name, entries);

   return (lvaddr_t)entries;
}
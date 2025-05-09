//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENARTMACHINE_H
#define ELENARTMACHINE_H

#include "rtcommon.h"
#include "libman.h"
#include "config.h"

namespace elena_lang
{
   // --- ELENARTMachine ---
   class ELENARTMachine : public ELENAMachine
   {
      PathString        _execPath;
      PathString        _debugFilePath;

      PlatformType      _platform;
      LibraryProvider   _libraryProvider;
      bool              _providerInitialized;

      MemoryDump        _debugSection;

      void*             _mdata;

      void loadConfig(path_t path);
      void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root);

      bool loadDebugSection();

      ref_t loadSubject(ustr_t actionName);

      addr_t retrieveGlobalAttribute(int attribute, ustr_t name);

      addr_t loadDispatcherOverloadlist(ustr_t referenceName);

      void Exit(int exitCode);

   public:
      void startApp(SystemEnv* env, void* entry);
      void startThread(SystemEnv* env, void* entryPoint, int index);

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);
      size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);
      size_t loadActionName(mssg_t messageRef, char* buffer, size_t length);
      size_t loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length);

      addr_t loadSymbol(ustr_t name);
      addr_t loadClassReference(ustr_t name);

      mssg_t loadMessage(ustr_t messageName);
      mssg_t loadAction(ustr_t actionName);

      size_t loadClassName(addr_t classAddress, char* buffer, size_t length);

      size_t loadClassMessages(void* classPtr, mssg_t* output, size_t skip, size_t maxLength);

      bool checkClassMessage(void* classPtr, mssg_t message);

      int loadExtensionDispatcher(const char* moduleList, mssg_t message, void* output);

      int loadSignature(mssg_t message, addr_t* output, pos_t maximalCount);

      void initRandomSeed(SeedStruct& seed)
      {
         __routineProvider.InitRandomSeed(seed, __routineProvider.GenerateSeed());
      }

      size_t allocateThreadEntry(SystemEnv* env);
      void clearThreadEntry(SystemEnv* env, size_t index);

      void* allocateThread(SystemEnv* env, void* arg, void* threadProc, int stackSize, int flags);

      unsigned int getRandomNumber(SeedStruct& seed)
      {
         return __routineProvider.GetRandomNumber(seed);
      }

      ELENARTMachine(path_t dllRootPath, path_t execPath, path_t configFile, PlatformType platform, void* mdata);

      virtual ~ELENARTMachine() = default;
   };
}

#endif
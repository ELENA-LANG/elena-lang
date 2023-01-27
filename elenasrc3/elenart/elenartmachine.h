//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                              (C)2021, by Aleksey Rakov
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

      MemoryDump        _debugSection;

      void*             _mdata;

      void loadConfig(path_t path);
      void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root);

      bool loadDebugSection();

      void Exit(int exitCode);

   public:
      void startSTA(SystemEnv* env, void* entry);

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);
      size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);
      size_t loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length);

      addr_t loadSymbol(ustr_t name);

      ELENARTMachine(path_t dllRootPath, path_t execPath, path_t configFile, PlatformType platform, void* mdata);

      virtual ~ELENARTMachine()
      {
      }
   };
}

#endif
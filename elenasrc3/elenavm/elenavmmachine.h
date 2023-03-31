//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAVMMACHINE_H
#define ELENAVMMACHINE_H

#include "libman.h"
#include "elenamachine.h"
#include "jitlinker.h" 
#include "codescope.h" 
#include "projectbase.h"

namespace elena_lang
{
   constexpr auto ELENAVM_GREETING        = "ELENA VM %d.%d.%d (C)2022 by Aleksex Rakov";
   constexpr auto ELENAVM_INITIALIZING    = "Initializing...";

   // --- ELENARTMachine ---
   class ELENAVMMachine : public ELENAMachine, public ImageProviderBase, public ExternalMapper
   {
   protected:
      bool                 _initialized;
      LibraryProvider      _libraryProvider;
      PresenterBase*       _presenter;
      ReferenceMapper      _mapper;
      JITLinkerSettings    _settings;

      path_t               _rootPath;

      ProjectBase*         _configuration;
      JITCompilerBase*     _compiler;

      virtual addr_t resolveExternal(ustr_t dll, ustr_t function) = 0;

      void addForward(ustr_t forwardLine);
      void addPackage(ustr_t packageLine);

      int interprete(SystemEnv* env, void* tape, pos_t size, const char* criricalHandlerReference);

      void onNewCode();

      void stopVM();

      bool configurateVM(MemoryReader& reader, SystemEnv* env);
      void compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker, 
         ModuleBase* dummyModule);

      void resumeVM(SystemEnv* env, void* criricalHandler);

      void init(JITLinker& jitLinker, SystemEnv* env);

      AddressMap::Iterator externals() override;

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);

   public:
      addr_t resolveExternal(ustr_t reference) override;

      void startSTA(SystemEnv* env, void* tape, const char* criricalHandlerReference);

      size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);
      size_t loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length);

      addr_t loadSymbol(ustr_t name);
      addr_t loadClassReference(ustr_t name);

      mssg_t loadMessage(ustr_t messageName);

      void Exit(int exitCode);

      ELENAVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform,
         int codeAlignment, JITSettings gcSettings,
         JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType));
      virtual ~ELENAVMMachine()
      {
         freeobj(_configuration);
         freeobj(_compiler);
      }
   };
}

#endif   
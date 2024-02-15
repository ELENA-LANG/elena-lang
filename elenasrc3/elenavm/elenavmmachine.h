//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                             (C)2022-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAVMMACHINE_H
#define ELENAVMMACHINE_H

#include "libman.h"
#include "elenamachine.h"
#include "jitlinker.h" 
#include "codescope.h" 
#include "projectbase.h"
#include "xmlprojectbase.h"

namespace elena_lang
{
   constexpr auto ELENAVM_GREETING        = "ELENA VM %d.%d.%d (C)2022-2024 by Aleksey Rakov";
   constexpr auto ELENAVM_INITIALIZING    = "Initializing...";

   // --- ELENAVMConfiguration ---

   class ELENAVMConfiguration : public XmlProjectBase
   {
   protected:
      void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root);
      bool loadConfig(path_t path);

   public:
      ustr_t resolveWinApi(ustr_t forward);

      bool loadConfigByName(path_t configPath, ustr_t name);

      void forEachForward(void* arg, void (*feedback)(void* arg, ustr_t key, ustr_t value)) override;

      ELENAVMConfiguration(PlatformType platform, path_t path)
         : XmlProjectBase(platform)
      {
         loadConfig(path);
      }
   };

   // --- ELENARTMachine ---
   class ELENAVMMachine : public ELENAMachine, public ImageProviderBase, public ExternalMapper, public LibraryLoaderListenerBase
   {
   protected:
      bool                    _initialized;
      LibraryProvider         _libraryProvider;
      PresenterBase*          _presenter;
      ReferenceMapper         _mapper;
      JITLinkerSettings       _settings;
      SystemEnv*              _env;

      bool                    _standAloneMode;
      bool                    _startUpCode;

      path_t                  _rootPath;

      ELENAVMConfiguration*   _configuration;
      JITCompilerBase*        _compiler;

      IdentifierString        _preloadedSection;
      ModuleInfoList          _preloadedList;

      addr_t retrieveGlobalAttribute(int attribute, ustr_t name);

      virtual addr_t resolveExternal(ustr_t dll, ustr_t function) = 0;

      bool loadConfig(ustr_t configName);

      void addForward(ustr_t forwardLine);
      void addPackage(ustr_t packageLine);

      addr_t interprete(SystemEnv* env, void* tape, pos_t size, 
         const char* criricalHandlerReference, bool withConfiguration);

      void onNewCode(JITLinker& jitLinker);

      virtual void stopVM();

      bool configurateVM(MemoryReader& reader, SystemEnv* env);
      bool compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker, 
         ModuleBase* dummyModule);

      virtual void resumeVM(JITLinker& jitLinker, SystemEnv* env, void* criricalHandler);

      void init(JITLinker& jitLinker, SystemEnv* env);

      AddressMap::Iterator externals() override;

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);
      ref_t loadSubject(ustr_t actionName);
      addr_t loadDispatcherOverloadlist(ustr_t referenceName);

      void fillPreloadedSymbols(JITLinker& jitLinker, MemoryWriter& writer, ModuleBase* dummyModule);

      addr_t loadReference(ustr_t name, int command);

      void resolvePreloaded();

      bool loadModule(ustr_t ns);

   public:
      bool isStandAlone() { return _standAloneMode; }

      addr_t resolveExternal(ustr_t reference) override;

      void startSTA(SystemEnv* env, void* tape, const char* criricalHandlerReference);

      addr_t evaluate(void* tape);

      size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);
      size_t loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length);

      addr_t loadSymbol(ustr_t name);
      addr_t loadClassReference(ustr_t name);

      mssg_t loadMessage(ustr_t messageName);
      mssg_t loadAction(ustr_t actionName);
      size_t loadActionName(mssg_t message, char* buffer, size_t length);

      size_t loadClassMessages(void* classPtr, mssg_t* output, size_t skip, size_t maxLength);

      bool checkClassMessage(void* classPtr, mssg_t message);

      int loadExtensionDispatcher(const char* moduleList, mssg_t message, void* output);

      void Exit(int exitCode);

      SystemEnv* getSystemEnv()
      {
         return _env;
      }

      void onLoad(ModuleBase*) override;

      void initRandomSeed(SeedStruct& seed)
      {
         __routineProvider.InitRandomSeed(seed, __routineProvider.GenerateSeed());
      }

      unsigned int getRandomNumber(SeedStruct& seed)
      {
         return __routineProvider.GetRandomNumber(seed);
      }

      pos_t getStackReserved() override
      {
         return _settings.jitSettings.stackReserved;
      }

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
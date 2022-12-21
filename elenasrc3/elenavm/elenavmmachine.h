//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                              (C)2022, by Aleksey Rakov
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
   class ELENAVMMachine : public ELENAMachine, public ImageProviderBase
   {
   public:
      class VMReferenceMapper : public ReferenceMapper
      {
      public:
         //void addLazyReference(LazyReferenceInfo info) override;

         //List<LazyReferenceInfo>::Iterator lazyReferences() override;

         //void mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef) override;

         //void mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t sectionMask) override;

         //ref_t resolveAction(ustr_t actionName, ref_t signRef) override;

         //addr_t resolveReference(ReferenceInfo referenceInfo, ref_t sectionMask) override;

         //ustr_t retrieveAction(ref_t actionRef, ref_t& signRef) override;

         //ustr_t retrieveReference(addr_t address, ref_t sectionMask) override;
      };

   private:
      bool                 _initialized;
      LibraryProvider      _libraryProvider;
      PresenterBase*       _presenter;
      ReferenceMapper      _mapper;
      JITLinkerSettings    _settings;

      ProjectBase*         _configuration;
      JITCompilerBase*     _compiler;

      int interprete(SystemEnv* env, void* tape, pos_t size, void* criricalHandler);

      void onNewCode();

      void stopVM();

      void configurateVM(MemoryReader& reader, JITLinker& jitLinker);
      void compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker);

      void resumeVM(SystemEnv* env, void* criricalHandler);

      void init(JITLinker& jitLinker);

      void loadSymbolArrayList(void* addr);

      AddressMap::Iterator externals() override;

   public:
      void startSTA(SystemEnv* env, void* tape, void* criricalHandler);

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
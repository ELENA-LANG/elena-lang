//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAVMMACHINE_H
#define ELENAVMMACHINE_H

#include "vmcommon.h"
#include "libman.h"
#include "elenamachine.h"
#include "jitlinker.h" 

namespace elena_lang
{
   constexpr auto ELENAVM_GREETING        = "ELENA VM %d.%d.%d (C)2022 by Aleksex Rakov";
   constexpr auto ELENAVM_INITIALIZING    = "Initializing...";

   // --- ELENARTMachine ---
   class ELENAVMMachine : public ELENAMachine, public ImageProviderBase
   {
   public:
      class ReferenceMapper : public ReferenceMapperBase
      {
      public:
         void addLazyReference(LazyReferenceInfo info) override;

         List<LazyReferenceInfo>::Iterator lazyReferences() override;

         void mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef) override;

         void mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t sectionMask) override;

         ref_t resolveAction(ustr_t actionName, ref_t signRef) override;

         addr_t resolveReference(ReferenceInfo referenceInfo, ref_t sectionMask) override;

         ustr_t retrieveAction(ref_t actionRef, ref_t& signRef) override;

         ustr_t retrieveReference(addr_t address, ref_t sectionMask) override;
      };

      class Configuration : public ForwardResolverBase
      {
      public:
         JITLinkerSettings settings;
         PlatformType      platform;

         void addForward(ustr_t forward, ustr_t referenceName) override;

         ustr_t resolveExternal(ustr_t forward) override;

         ustr_t resolveForward(ustr_t forward) override;

         ustr_t resolveWinApi(ustr_t forward) override;

         Configuration(PlatformType platform)
            : platform(platform)
         {
            
         }
      };

   private:
      bool              _initialized;
      LibraryProvider   _libraryProvider;
      PresenterBase*    _presenter;
      ReferenceMapper   _mapper;
      Configuration     _configuration;
      JITCompilerBase*  _compiler;

      int interprete(SystemEnv* env, void* tape, pos_t size, void* criricalHandler);

      void onNewCode();

      void stopVM();
      void* evaluateVMTape(MemoryReader& reader);

      void resumeVM(SystemEnv* env, void* criricalHandler);

      void init(JITLinker& jitLinker);

      AddressMap::Iterator externals() override;

      Section* getDataSection() override;
      Section* getImportSection() override;
      Section* getMBDataSection() override;
      Section* getMDataSection() override;
      Section* getRDataSection() override;
      Section* getStatSection() override;
      Section* getTargetDebugSection() override;
      Section* getTargetSection(ref_t targetMask) override;
      Section* getTextSection() override;

      addr_t getDebugEntryPoint() override;
      addr_t getEntryPoint() override;

   public:
      void startSTA(SystemEnv* env, void* tape, void* criricalHandler);

      void Exit(int exitCode);

      ELENAVMMachine(PresenterBase* presenter, PlatformType platform, 
         JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType));
      virtual ~ELENAVMMachine()
      {
      }
   };
}

#endif
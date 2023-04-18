//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Image class declarations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CODEIMAGE_H
#define CODEIMAGE_H

#include "codescope.h"
#include "clicommon.h"

namespace elena_lang
{
   // --- TargetImageInfo ---
   struct TargetImageInfo
   {
      PlatformType type;
      pos_t        codeAlignment;
      bool         autoClassSymbol;
      bool         withTLS;
      JITSettings  coreSettings;
      ustr_t       ns;

      TargetImageInfo()
      {
         type = PlatformType::None;
         codeAlignment = 0;
         autoClassSymbol = withTLS = false;
         coreSettings = {};
         ns = nullptr;
      }
   };

   // --- TargetImage ---
   class TargetImage : public ReferenceMapper, public ImageProvider, public TapeGeneratorBase
   {
      PlatformType       _systemTarget;

      pos_t              _entryPoint;
      pos_t              _debugEntryPoint;

      addr_t             _tlsVariable;

      void createVMTape(MemoryBase* tape, ustr_t ns, path_t nsPath, ForwardResolverBase* resolver);
      void prepareImage(ustr_t ns);

   public:
      AddressMap::Iterator externals() override;

      addr_t getTLSVariable() override
      {
         return _tlsVariable;
      }

      addr_t getEntryPoint() override
      {
         return _entryPoint & ~mskAnyRef;
      }

      addr_t getDebugEntryPoint() override
      {
         return _debugEntryPoint & ~mskAnyRef;
      }

      void generateAutoSymbol(ModuleInfoList& list, ModuleBase* module, MemoryDump& tapeSymbol) override;

      TargetImage(PlatformType systemTarget, ForwardResolverBase* resolver, LibraryLoaderBase* loader,
         JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType),
         TargetImageInfo imageInfo, AddressMapperBase* addressMapper);
   };
}

#endif // IMAGE_H

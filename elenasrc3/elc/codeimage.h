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
      JITSettings  coreSettings;

      TargetImageInfo()
      {
         codeAlignment = 0;
         type = PlatformType::None;
         autoClassSymbol = false;
      }
   };

   // --- TargetImage ---
   class TargetImage : public ReferenceMapper, public ImageProvider
   {
      pos_t _entryPoint;
      pos_t _debugEntryPoint;

   public:
      AddressMap::Iterator externals() override;

      addr_t getEntryPoint() override
      {
         return _entryPoint & ~mskAnyRef;
      }

      addr_t getDebugEntryPoint() override
      {
         return _debugEntryPoint;
      }

      TargetImage(ForwardResolverBase* resolver, LibraryLoaderBase* loader, 
         JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType),
         TargetImageInfo imageInfo);
   };
}

#endif // IMAGE_H

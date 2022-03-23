//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image class implementations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "codeimage.h"
#include "jitlinker.h"

using namespace elena_lang;

// --- CodeImage ---

AddressMap::Iterator TargetImage :: externals()
{
   return _exportReferences.start();
}

TargetImage :: TargetImage(ForwardResolverBase* resolver, LibraryLoaderBase* loader, 
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType),
   TargetImageInfo imageInfo)
{
   JITCompilerBase* compiler = jitCompilerFactory(loader, imageInfo.type);

   JITLinkerSettings settings = 
   {
      imageInfo.codeAlignment,
      imageInfo.coreSettings,
      true,
      imageInfo.autoClassSymbol
   };

   JITLinker linker(
      dynamic_cast<ReferenceMapperBase*>(this), 
      loader, resolver,
      dynamic_cast<ImageProviderBase*>(this), 
      &settings);

   linker.prepare(compiler);

   // resolve the program entry
   ustr_t entryName = resolver->resolveForward(SYSTEM_ENTRY);
   _entryPoint = entryName.empty() ? INVALID_POS : (pos_t)linker.resolve(entryName, mskSymbolRef, true);
   if (_entryPoint == INVALID_POS)
      throw JITUnresolvedException(ReferenceInfo(SYSTEM_ENTRY));

   // resolvethe debug entry
   ustr_t debugEntryName = resolver->resolveForward(PROGRAM_ENTRY);
   if (!debugEntryName)
      debugEntryName = entryName;

   _debugEntryPoint = resolveReference(debugEntryName, mskSymbolRef);

   linker.complete(compiler);

   freeobj(compiler);
}

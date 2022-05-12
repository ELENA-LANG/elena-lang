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
   TargetImageInfo imageInfo, AddressMapperBase* addressMapper)
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
      &settings,
      addressMapper);

   // add predefined values
   prepareImage(imageInfo.ns);

   linker.prepare(compiler);

   // resolve the program entry
   ustr_t entryName = resolver->resolveForward(SYSTEM_ENTRY);
   _entryPoint = entryName.empty() ? INVALID_POS : (pos_t)linker.resolve(entryName, mskSymbolRef, true);
   if (_entryPoint == INVALID_POS)
      throw JITUnresolvedException(ReferenceInfo(SYSTEM_ENTRY));

   // resolvethe debug entry
   _debugEntryPoint = (pos_t)linker.resolve(PROGRAM_ENTRY, mskSymbolRef, true);
   if (_debugEntryPoint == INVALID_ADDR) {
      _debugEntryPoint = _entryPoint;
   }

   linker.complete(compiler);

   freeobj(compiler);
}

void TargetImage :: prepareImage(ustr_t ns)
{
   MemoryWriter rdataWriter(getRDataSection());

   // put SYSTEM_ENV reference place holder
   addr_t envPtr = 0;
   rdataWriter.write(&envPtr, sizeof(addr_t));

   // put a signature
   rdataWriter.write(ELENA_SIGNITURE, strlen(ELENA_SIGNITURE));

   String<char, 4> number;
   number.appendInt(ENGINE_MAJOR_VERSION);
   rdataWriter.write(number.str(), number.length_pos());
   rdataWriter.writeChar('.');

   number.clear();
   number.appendInt(ENGINE_MINOR_VERSION);
   rdataWriter.write(number.str(), number.length_pos());

   rdataWriter.align(4, 0);

   // save root namespace
   MemoryWriter debugWriter(getTargetDebugSection());
   debugWriter.writeString(ns);

}

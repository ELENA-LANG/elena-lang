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

TargetImage :: TargetImage(PlatformType systemTarget, ForwardResolverBase* resolver, LibraryLoaderBase* loader,
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType),
   TargetImageInfo imageInfo, AddressMapperBase* addressMapper)
{
   _systemTarget = systemTarget;

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
   ustr_t entryName = resolver->resolveForward(SYSTEM_FORWARD);
   _entryPoint = entryName.empty() ? INVALID_POS : (pos_t)linker.resolve(entryName, mskSymbolRef, true);
   if (_entryPoint == INVALID_POS)
      throw JITUnresolvedException(ReferenceInfo(SYSTEM_FORWARD));

   // resolvethe debug entry
   _debugEntryPoint = (pos_t)linker.resolve(PROGRAM_ENTRY, mskSymbolRef, true);
   if (_debugEntryPoint == INVALID_ADDR) {
      _debugEntryPoint = _entryPoint;
   }

   linker.complete(compiler);

   freeobj(compiler);
}

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command, ustr_t arg)
{
   rdataWriter.writeDWord(command);
   rdataWriter.writeString(arg);
}

void TargetImage :: createVMTape(MemoryWriter& rdataWriter)
{
   addr_t tapeRef = rdataWriter.position();

   mapReference(ReferenceInfo{ VM_TAPE }, tapeRef, mskConstArray);

   addVMTapeEntry(rdataWriter, VM_LOAD, PROGRAM_ENTRY);
}

void TargetImage :: prepareImage(ustr_t ns)
{
   MemoryWriter rdataWriter(getRDataSection());

   // put SYSTEM_ENV reference place holder
   addr_t envPtr = 0;
   rdataWriter.write(&envPtr, sizeof(addr_t));

   // put a signature
   switch (_systemTarget) {
      case PlatformType::VMClient:
         rdataWriter.write(ELENA_VM_SIGNITURE, getlength_pos(ELENA_VM_SIGNITURE));
         break;
      default:
         rdataWriter.write(ELENA_SIGNITURE, getlength_pos(ELENA_SIGNITURE));
         break;
   }
   

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

   if (_systemTarget == PlatformType::VMClient) {
      createVMTape(rdataWriter);
   }
}

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image class implementations
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "codeimage.h"

#include "bytecode.h"
#include "jitlinker.h"
#include "module.h"

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
   _tlsVariable = INVALID_ADDR;
   _stackReserved = imageInfo.coreSettings.stackReserved;

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

   if (imageInfo.withTLS) {
      _tlsVariable = linker.resolveTLSSection(compiler);
   }

   // resolve the debug entry
   _debugEntryPoint = INVALID_POS;

   if (_systemTarget == PlatformType::VMClient) {
      MemoryDump tape;
      createVMTape(&tape, loader->Namespace(), loader->OutputPath(), resolver);

      linker.resolveTape(VM_TAPE, &tape);

      // resolve the system entry
      ustr_t entryName = resolver->resolveForward(SYSTEM_FORWARD);
      _entryPoint = entryName.empty() ? INVALID_POS : (pos_t)linker.resolve(entryName, mskSymbolRef, true);
      if (_entryPoint == INVALID_POS)
         throw JITUnresolvedException(ReferenceInfo(SYSTEM_FORWARD));
   }
   else {
      // resolving program entry
      _debugEntryPoint = (pos_t)linker.resolve(PROGRAM_ENTRY, mskSymbolRef, true);

      // creating start up symbol
      Module* dummyModule = new Module();
      linker.loadPreloaded(PRELOADED_FORWARD);
      addLazyReference({ mskAutoSymbolRef, INVALID_POS, 
         dummyModule, dummyModule->mapReference(resolver->resolveForward(START_FORWARD)), 0});

      MemoryDump tapeSymbol;
      generateAutoSymbol(dummyModule, tapeSymbol);
      addr_t vaddress = linker.resolveTemporalByteCode(tapeSymbol, dummyModule);
      mapReference(STARTUP_ENTRY, vaddress, mskSymbolRef);

      // resolve the system entry
      ustr_t entryName = resolver->resolveForward(SYSTEM_FORWARD);
      _entryPoint = entryName.empty() ? INVALID_POS : (pos_t)linker.resolve(entryName, mskSymbolRef, true);
      if (_entryPoint == INVALID_POS)
         throw JITUnresolvedException(ReferenceInfo(SYSTEM_FORWARD));

      ustr_t superClass = resolver->resolveForward(NILVALUE_FORWARD);
      linker.complete(compiler, superClass);

      freeobj(dummyModule);
   }

   if (_debugEntryPoint == INVALID_ADDR) {
      _debugEntryPoint = _entryPoint;
   }

   freeobj(compiler);
}

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command, ustr_t arg)
{
   rdataWriter.writeDWord(command);
   rdataWriter.writeString(arg);
}

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command, ustr_t key, ustr_t value)
{
   IdentifierString arg;
   arg.copy(key);
   arg.append('=');
   arg.append(value);

   rdataWriter.writeDWord(command);
   rdataWriter.writeString(*arg);
}

#ifdef _MSC_VER

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command, path_t arg)
{
   IdentifierString ustrArg(arg);

   rdataWriter.writeDWord(command);
   rdataWriter.writeString(*ustrArg);
}

#endif

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command)
{
   rdataWriter.writeDWord(command);
}

void TargetImage :: createVMTape(MemoryBase* tape, ustr_t ns, path_t nsPath, ForwardResolverBase* resolver)
{
   MemoryWriter tapeWriter(tape);

   addVMTapeEntry(tapeWriter, VM_SETNAMESPACE_CMD, ns);

   resolver->forEachForward(&tapeWriter, [](void* arg, ustr_t key, ustr_t value)
   {
      addVMTapeEntry(*(MemoryWriter*)arg, VM_FORWARD_CMD, key, value);
   });

   IdentifierString nsPathStr(nsPath);
   addVMTapeEntry(tapeWriter, VM_PACKAGE_CMD, ns, *nsPathStr);
   addVMTapeEntry(tapeWriter, VM_PRELOADED_CMD, PRELOADED_FORWARD);

   addVMTapeEntry(tapeWriter, VM_INIT_CMD);
   addVMTapeEntry(tapeWriter, VM_CALLSYMBOL_CMD, resolver->resolveForward(START_FORWARD));
   addVMTapeEntry(tapeWriter, VM_ENDOFTAPE_CMD);
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
}

void TargetImage :: generateAutoSymbol(ModuleBase* module, MemoryDump& tapeSymbol)
{
   // fill the list of virtual references
   ModuleInfoList symbolList({});
   forEachLazyReference<ModuleInfoList*>(&symbolList, [](ModuleInfoList* symbolList, LazyReferenceInfo info)
      {
         if (info.mask == mskAutoSymbolRef) {
            symbolList->add({ info.module, info.reference });
         }
      });

   ByteCodeUtil::generateAutoSymbol(symbolList, module, tapeSymbol);
}

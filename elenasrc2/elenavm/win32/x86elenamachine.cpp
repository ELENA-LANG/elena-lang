//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86elenamachine.h"
#include "x86jitcompiler.h"

using namespace _ELENA_;

// --- x86Instance ---

x86Instance :: x86Instance(ELENAMachine* machine)
   : Instance(machine),
     _debugProcess(0x10000, true, false),
     _codeProcess(0x100000, false, true),
     _dataProcess(0x100000, false, false),
     _bssProcess(0x100, true, false),
     _statProcess(0x10000, true, false)  // !! temporal
{
   _rootPath = machine->getRootPath();
}

void x86Instance :: resumeVM()
{
   _codeProcess.protect(true, false);
   _dataProcess.protect(true, false);
}

void x86Instance :: stopVM()
{
   _codeProcess.protect(false, true);
   _dataProcess.protect(false, false);
}

_Memory* x86Instance :: getTargetDebugSection()
{
   return &_debugProcess;
}

_Memory* x86Instance :: getTargetSection(size_t mask)
{
   switch(mask & mskImageMask)
   {
      case mskCodeRef:
      case mskRelCodeRef:
         return &_codeProcess;
      case mskRDataRef:
         return &_dataProcess;
      case mskDataRef:
         return &_bssProcess;
      case mskStatRef:
         return &_statProcess;
      default:
         return NULL;
   }
}

void x86Instance :: mapReference(ident_t reference, void* vaddress, size_t mask)
{
   Instance::mapReference(reference, vaddress, mask);

   if (mask == mskStatRef) {
      // fix up static table size
      _compiler->setStaticRootCounter(this, _linker->getStaticCount(), false);
   }
}

ref_t x86Instance :: resolveExternal(ident_t external)
{
   ref_t reference = _exportReferences.get(external);
   if (reference == (size_t)-1) {
      ident_t function = external + StringHelper::findLast(external, '.') + 1;

      Path dll;
      Path::loadPath(dll, external + getlength(DLL_NAMESPACE) + 1, function - (external + getlength(DLL_NAMESPACE)) - 2);

      // align memory
      MemoryWriter writer(&_dataProcess);
      _compiler->alignCode(&writer, VA_ALIGNMENT, false);

      reference = _dataProcess.Length();

      if(!_dataProcess.exportFunction(_rootPath, reference, dll, function))
         return (ref_t)LOADER_NOTLOADED;

      reference = (ref_t)_dataProcess.get(reference); // !! should loader do this?

      _exportReferences.add(external, reference);
   }
   return reference;
}

void x86Instance :: raiseBreakpoint()
{
#ifdef MINGW
   asm("int3");
#else
   __debugbreak();
#endif
}

void* x86Instance :: loadDebugSection()
{
   return _debugProcess.get(0);
}

bool x86Instance :: restart(bool debugMode)
{
   // !! make sure all threads are stopped

   // clear memory used by VM Instance
   _codeProcess.trim(0);
   _dataProcess.trim(0);
   _bssProcess.trim(0);
   _statProcess.trim(0);
   _debugProcess.trim(0);

   // put debug size and entry point place holders
   getTargetDebugSection()->writeBytes(0, 0, 8);

   // free compiler & linker
   freeobj(_compiler);
   freeobj(_linker);

   // create new compiler & linker (in debug mode we do not use embedded symbols)
   _compiler = new _ELENA_::x86JITCompiler(debugMode);
   _linker = new JITLinker(this, _compiler, false, _codeProcess.get(0)/*, _config.maxThread*/);

   return Instance::restart(debugMode);
}

// --- x86ELENAMachine ---

x86ELENAMachine :: x86ELENAMachine(path_t rootPath)
   : ELENAMachine(rootPath), _instances(NULL, freeobj)
{
}

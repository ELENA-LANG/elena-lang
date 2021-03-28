//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

//#define NO_BREAKPOINT 1

#include "elena.h"
// --------------------------------------------------------------------------
#include "amd64elenavmachine.h"
#include "amd64jitcompiler.h"

#include <fcntl.h>
#include <io.h>

#include <iostream>
#include <fstream>

//static const WORD MAX_CONSOLE_LINES = 500;

using namespace _ELENA_;

// --- AMD64Instance ---

AMD64Instance :: AMD64Instance(ELENAVMMachine* machine)
   : Instance(machine),
     _codeProcess(0x500000, false, true), 
     _debugProcess(0x200000, true, false),
     _dataProcess(0x500000, false, false),
     _messageProcess(0x100000, false, false),
     _mattributeProcess(0x100000, false, false),
     _bssProcess(0x1000, true, false),
     _statProcess(0x10000, true, false, 0x100)  // !! temporal
{
   _rootPath = machine->getRootPath();
}

void AMD64Instance :: resumeVM()
{
   _codeProcess.protect(true, false);
   _dataProcess.protect(true, false);
}

void AMD64Instance :: stopVM()
{
   _codeProcess.protect(false, true);
   _dataProcess.protect(false, false);
}

_Memory* AMD64Instance :: getTargetDebugSection()
{
   return &_debugProcess;
}

_Memory* AMD64Instance :: getTargetSection(pos_t mask)
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
      case mskDebugRef:
         if (mask == mskMessageTableRef) {
            return getMessageSection();
         }
         else if (mask == mskMetaAttributes) {
            return getMetaAttributeSection();
         }
      default:
         return NULL;
   }
}

_Memory* AMD64Instance :: getMessageSection()
{
   return &_messageProcess;
}

_Memory* AMD64Instance :: getMetaAttributeSection()
{
   return &_mattributeProcess;
}

lvaddr_t AMD64Instance :: resolveExternal(ident_t external)
{
   lvaddr_t reference = _exportReferences.get(external);
   if (reference == INVALID_VADDR) {
      ident_t function = external + external.findLast('.') + 1;

      Path dll;
      int len = function - (external + getlength(DLL_NAMESPACE)) - 2;
      if (ident_t(RTDLL_FORWARD).compare(external + getlength(DLL_NAMESPACE) + 1, len)) {
         dll.copy(_loader.resolvePrimitive(RTDLL_FORWARD));
      }
      else dll.copy(external + getlength(DLL_NAMESPACE) + 1, len);

      // align memory
      MemoryWriter writer(&_dataProcess);
      _compiler->alignCode(&writer, VA_ALIGNMENT, false);

      reference = _dataProcess.Length();

      if(!_dataProcess.exportFunction(_rootPath, reference, dll.c_str(), function))
         return LOADER_NOTLOADED;

      reference = (ref_t)_dataProcess.get(reference); // !! should loader do this?

      _exportReferences.add(external, reference);
   }
   return reference;
}

void AMD64Instance :: raiseBreakpoint()
{
#ifdef NO_BREAKPOINT

#elif MINGW
   asm("int3");
#else
   __debugbreak();
#endif
}

void* AMD64Instance :: loadDebugSection()
{
   return _debugProcess.get(0);
}

bool AMD64Instance :: restart(SystemEnv* env, void* sehTable, bool debugMode, bool withExtDispatchers)
{
   // !! make sure all threads are stopped

   // clear memory used by VM Instance
   _codeProcess.trim(0);
   _dataProcess.trim(0);
   _bssProcess.trim(0);
   _statProcess.trim(0);
   _debugProcess.trim(0);

   // put debug size and entry point place holders
   MemoryWriter debugWriter(&_debugProcess);
   debugWriter.writeBytes(0, 8);
   
   // save default namespace
   debugWriter.writeLiteral(_loader.getNamespace());

   // free compiler & linker
   freeobj(_compiler);
   freeobj(_linker);

   // create new compiler & linker (in debug mode we do not use embedded symbols)
   _compiler = new _ELENA_::I64JITCompiler(debugMode, true);
   _linker = new JITLinker(this, _compiler, false, (ref_t)_codeProcess.get(0)/*, _config.maxThread*/, withExtDispatchers);

   return Instance::restart(env, sehTable, debugMode, withExtDispatchers);
}

void AMD64Instance :: createConsole()
{
   // HOTFIX : create a console only for GUI application 
   if (test(_config.platform, mtGUI)) {
      //CONSOLE_SCREEN_BUFFER_INFO coninfo;
      //FILE *fp;

      // allocate a console for this app
      AllocConsole();
      AttachConsole(GetCurrentProcessId());
      freopen("CON", "w", stdout);
   }
}

// --- AMD64ELENAVMMachine ---

AMD64ELENAVMMachine :: AMD64ELENAVMMachine(path_t rootPath)
   : ELENAVMMachine(rootPath)
{
   _instance = new AMD64Instance(this);
}

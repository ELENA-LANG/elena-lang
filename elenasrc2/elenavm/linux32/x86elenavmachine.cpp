//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86elenavmachine.h"
#include "x86jitcompiler.h"

//static const WORD MAX_CONSOLE_LINES = 500;

using namespace _ELENA_;

// --- x86Instance ---

x86Instance :: x86Instance(ELENAVMMachine* machine)
   : Instance(machine),
     _debugProcess(0x200000, true, false),
     _codeProcess(0x500000, /*false*/true, true),
     _dataProcess(0x500000, /*false*/true, false),
     _messageProcess(0x100000, /*false*/true, false),
     _mattributeProcess(0x100000, /*false*/true, false),
     _bssProcess(0x1000, true, false),
     _statProcess(0x10000, true, false, 0x100)  // !! temporal
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
      case mskDebugRef:
         if (mask == mskMessageTableRef) {
            return getMessageSection();
         }
         else if (mask == mskMetaAttributes) {
            return getMetaAttributeSection();
         }
      default:
         return nullptr;
   }
}

_Memory* x86Instance :: getMessageSection()
{
   return &_messageProcess;
}

_Memory* x86Instance :: getMetaAttributeSection()
{
   return &_mattributeProcess;
}

ref_t x86Instance :: resolveExternal(ident_t external)
{
   ref_t reference = _exportReferences.get(external);
   if (reference == (size_t)-1) {
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
         return (ref_t)LOADER_NOTLOADED;

      reference = (ref_t)_dataProcess.get(reference); // !! should loader do this?

      _exportReferences.add(external, reference);
   }
   return reference;
}

void x86Instance :: raiseBreakpoint()
{
#ifdef _LINUX
   asm("int3");
#else
   __debugbreak();
#endif
}

void* x86Instance :: loadDebugSection()
{
   return _debugProcess.get(0);
}

bool x86Instance :: restart(SystemEnv* env, void* sehTable, bool debugMode, bool withExtDispatchers)
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
   _compiler = new _ELENA_::x86JITCompiler(debugMode);
   _linker = new JITLinker(this, _compiler, false, (ref_t)_codeProcess.get(0)/*, _config.maxThread*/, withExtDispatchers);

   return Instance::restart(env, sehTable, debugMode, withExtDispatchers);
}

void x86Instance :: createConsole()
{
//   // HOTFIX : create a console only for GUI application
//   if (test(_config.platform, mtGUI)) {
//      //CONSOLE_SCREEN_BUFFER_INFO coninfo;
//      //FILE *fp;
//
//      // allocate a console for this app
//      AllocConsole();
//      AttachConsole(GetCurrentProcessId());
//      freopen("CON", "w", stdout);
//   }
}

// --- x86ELENAVMMachine ---

x86ELENAVMMachine ::x86ELENAVMMachine(path_t rootPath)
   : ELENAVMMachine(rootPath)
{
   _instance = new x86Instance(this);
}

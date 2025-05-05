//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows VM Implementation
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenalnxvmachine.h"
#include "langcommon.h"
#include <dlfcn.h>

using namespace elena_lang;

constexpr auto VA_ALIGNMENT = 0x08;

ELENAUnixVMMachine :: ELENAUnixVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform,
   int codeAlignment, JITSettings gcSettings,
   JITCompilerBase*(* jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
      : ELENAVMMachine(configPath, presenter, platform, codeAlignment, gcSettings, jitCompilerFactory),
         _text(TEXT_MAX_SIZE, false, true),
         _rdata(RDATA_MAX_SIZE, false, false),
         _data(DATA_MAX_SIZE, true, false),
         _stat(STAT_MAX_SIZE, true, false),
         _adata(ADATA_MAX_SIZE, false, false),
         _mdata(MDATA_MAX_SIZE, false, false),
         _mbdata(MBDATA_MAX_SIZE, false, false),
         _debug(DEBUG_MAX_SIZE, true, false)
{
}

addr_t ELENAUnixVMMachine::getDebugEntryPoint()
{
   throw InternalError(errVMBroken);
}

addr_t ELENAUnixVMMachine::getEntryPoint()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAUnixVMMachine::getImportSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAUnixVMMachine :: getMBDataSection()
{
   return &_mbdata;
}

MemoryBase* ELENAUnixVMMachine :: getDataSection()
{
   return &_data;
}

MemoryBase* ELENAUnixVMMachine :: getADataSection()
{
   return &_adata;
}

MemoryBase* ELENAUnixVMMachine :: getMDataSection()
{
   return &_mdata;
}

MemoryBase* ELENAUnixVMMachine :: getRDataSection()
{
   return &_rdata;
}

MemoryBase* ELENAUnixVMMachine :: getStatSection()
{
   return &_stat;
}

MemoryBase* ELENAUnixVMMachine :: getTargetDebugSection()
{
   return &_debug;
}

MemoryBase* ELENAUnixVMMachine :: getTextSection()
{
   return &_text;
}

bool ELENAUnixVMMachine :: exportFunction(path_t rootPath, size_t position, path_t dllName, ustr_t funName)
{
   void* handle = dlopen(dllName, RTLD_LAZY);
   //// if dll is not found, use root path
   //if (handle == NULL) {
   //   Path dllPath(rootPath);
   //   dllPath.combine(dllName);

   //   handle = ::LoadLibrary(dllPath);
   //}

   String<char, 200> lpFunName(funName);
   ref_t address = (ref_t)dlsym(handle, funName);
   if (address == 0)
      return false;

   return _data.write(position, &address, 4);
}

addr_t ELENAUnixVMMachine :: resolveExternal(ustr_t dll, ustr_t function)
{
   // align memory
   MemoryWriter writer(&_data);
   _compiler->alignCode(writer, VA_ALIGNMENT, false);

   pos_t reference = _data.length();

   PathString dllPath(dll);
   if (!exportFunction(_rootPath, reference, *dllPath, function))
      return INVALID_ADDR;

   return (addr_t)_data.get(reference);
}

MemoryBase* ELENAUnixVMMachine :: getTLSSection()
{
   return nullptr; // !! temporal
}

addr_t ELENAUnixVMMachine :: getTLSVariable()
{
   return INVALID_ADDR;
}

void ELENAUnixVMMachine :: stopVM()
{
   ELENAVMMachine::stopVM();

   _text.protect(true, false);
   _rdata.protect(true, false);
   _adata.protect(true, false);
   _mdata.protect(true, false);
   _mbdata.protect(true, false);
}

void ELENAUnixVMMachine :: resumeVM(SystemEnv* env, void* criricalHandler)
{
   ELENAVMMachine::resumeVM(env, criricalHandler);

   _text.protect(false, true);
   _rdata.protect(false, false);
   _adata.protect(false, false);
   _mdata.protect(false, false);
   _mbdata.protect(false, false);
}

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows VM Implementation
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "windows/elenawinvmachine.h"
#include "langcommon.h"

using namespace elena_lang;

constexpr auto VA_ALIGNMENT = 0x08;

ELENAWinVMMachine :: ELENAWinVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform,
   int codeAlignment, JITSettings gcSettings,
   JITCompilerBase*(* jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
      : ELENAVMMachine(configPath, presenter, platform, codeAlignment, gcSettings, jitCompilerFactory),
         _text(TEXT_MAX_SIZE, false, true),
         _rdata(RDATA_MAX_SIZE, false, false),
         _data(DATA_MAX_SIZE, true, false),
         _stat(STAT_MAX_SIZE, true, false),
         _mdata(MDATA_MAX_SIZE, false, false),
         _mbdata(MBDATA_MAX_SIZE, false, false),
         _debug(DEBUG_MAX_SIZE, true, false)
{
}

addr_t ELENAWinVMMachine::getDebugEntryPoint()
{
   throw InternalError(errVMBroken);
}

addr_t ELENAWinVMMachine::getEntryPoint()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getImportSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine :: getMBDataSection()
{
   return &_mbdata;
}

MemoryBase* ELENAWinVMMachine :: getDataSection()
{
   return &_data;
}

MemoryBase* ELENAWinVMMachine :: getMDataSection()
{
   return &_mdata;
}

MemoryBase* ELENAWinVMMachine :: getRDataSection()
{
   return &_rdata;
}

MemoryBase* ELENAWinVMMachine :: getStatSection()
{
   return &_stat;
}

MemoryBase* ELENAWinVMMachine :: getTargetDebugSection()
{
   return &_debug;
}

MemoryBase* ELENAWinVMMachine :: getTextSection()
{
   return &_text;
}

bool ELENAWinVMMachine :: exportFunction(path_t rootPath, size_t position, path_t dllName, ustr_t funName)
{
   HMODULE handle = ::LoadLibrary(dllName);
   // if dll is not found, use root path
   if (handle == nullptr) {
      PathString dllPath(rootPath);
      dllPath.combine(dllName);

      handle = ::LoadLibrary(*dllPath);
   }

   String<char, 200> lpFunName(funName);
   addr_t address = (addr_t)::GetProcAddress(handle, lpFunName.str());
   if (address == 0)
      return false;

   return _data.write(position, &address, sizeof(address));
}

addr_t ELENAWinVMMachine :: resolveExternal(ustr_t dll, ustr_t function)
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

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

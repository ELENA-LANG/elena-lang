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

ELENAWinVMMachine :: ELENAWinVMMachine(PresenterBase* presenter, PlatformType platform, 
   JITCompilerBase*(* jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
      : ELENAVMMachine(presenter, platform, jitCompilerFactory)
{
   
}

MemoryBase* ELENAWinVMMachine::getDataSection()
{
   throw InternalError(errVMBroken);
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

MemoryBase* ELENAWinVMMachine::getMBDataSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getMDataSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getRDataSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getStatSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getTargetDebugSection()
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getTargetSection(ref_t targetMask)
{
   throw InternalError(errVMBroken);
}

MemoryBase* ELENAWinVMMachine::getTextSection()
{
   throw InternalError(errVMBroken);
}

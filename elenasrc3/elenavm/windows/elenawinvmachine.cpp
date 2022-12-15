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

Section* ELENAWinVMMachine::getDataSection()
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

Section* ELENAWinVMMachine::getImportSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getMBDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getMDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getRDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getStatSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getTargetDebugSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getTargetSection(ref_t targetMask)
{
   throw InternalError(errVMBroken);
}

Section* ELENAWinVMMachine::getTextSection()
{
   throw InternalError(errVMBroken);
}

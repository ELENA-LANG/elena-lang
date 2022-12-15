//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows VM declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAWINVMMACHINE_H
#define ELENAWINVMMACHINE_H

#include "elenavmmachine.h"
#include "windows/winsection.h"

namespace elena_lang
{

class ELENAWinVMMachine : public ELENAVMMachine
{
   WinImageSection _text, _mdata, _mbdata, _rdata, _data, _stat, _debug;

public:
   MemoryBase* getDataSection() override;
   MemoryBase* getImportSection() override;
   MemoryBase* getMBDataSection() override;
   MemoryBase* getMDataSection() override;
   MemoryBase* getRDataSection() override;
   MemoryBase* getStatSection() override;
   MemoryBase* getTargetDebugSection() override;
   MemoryBase* getTextSection() override;

   MemoryBase* getTargetSection(ref_t targetMask) override;

   addr_t getDebugEntryPoint() override;
   addr_t getEntryPoint() override;

   ELENAWinVMMachine(PresenterBase* presenter, PlatformType platform,
      JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType));
};

}

#endif

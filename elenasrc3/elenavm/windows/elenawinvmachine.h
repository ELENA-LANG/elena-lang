//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows VM declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAWINVMMACHINE_H
#define ELENAWINVMMACHINE_H

#include "elenavmmachine.h"

namespace elena_lang
{

class ELENAWinVMMachine : public ELENAVMMachine
{
public:
   Section* getDataSection() override;
   Section* getImportSection() override;
   Section* getMBDataSection() override;
   Section* getMDataSection() override;
   Section* getRDataSection() override;
   Section* getStatSection() override;
   Section* getTargetDebugSection() override;
   Section* getTargetSection(ref_t targetMask) override;
   Section* getTextSection() override;

   addr_t getDebugEntryPoint() override;
   addr_t getEntryPoint() override;

   ELENAWinVMMachine(PresenterBase* presenter, PlatformType platform,
      JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType));
};

}

#endif


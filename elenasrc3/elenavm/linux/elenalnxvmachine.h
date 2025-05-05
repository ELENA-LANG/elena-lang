//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA *nix VM declaration
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENALNXVMMACHINE_H
#define ELENALNXVMMACHINE_H

#include "elenavmmachine.h"
#include "lnxsection.h"

namespace elena_lang
{

constexpr auto TEXT_MAX_SIZE     = 0x500000;
constexpr auto RDATA_MAX_SIZE    = 0x500000;
constexpr auto DATA_MAX_SIZE     = 0x010000;
constexpr auto STAT_MAX_SIZE     = 0x010000;
constexpr auto ADATA_MAX_SIZE    = 0x100000;
constexpr auto MDATA_MAX_SIZE    = 0x100000;
constexpr auto MBDATA_MAX_SIZE   = 0x100000;
constexpr auto DEBUG_MAX_SIZE    = 0x500000;

class ELENAUnixVMMachine : public ELENAVMMachine
{
   UnixImageSection _text, _rdata, _data, _stat, _adata, _mdata, _mbdata, _debug;

   bool exportFunction(path_t rootPath, size_t position, path_t dllName, ustr_t funName);

   void stopVM() override;
   void resumeVM(SystemEnv* env, void* criricalHandler) override;

public:
   MemoryBase* getDataSection() override;
   MemoryBase* getImportSection() override;
   MemoryBase* getMBDataSection() override;
   MemoryBase* getMDataSection() override;
   MemoryBase* getADataSection() override;
   MemoryBase* getRDataSection() override;
   MemoryBase* getStatSection() override;
   MemoryBase* getTargetDebugSection() override;
   MemoryBase* getTextSection() override;
   MemoryBase* getTLSSection() override;
   addr_t getTLSVariable() override;

   addr_t getDebugEntryPoint() override;
   addr_t getEntryPoint() override;

   addr_t resolveExternal(ustr_t dll, ustr_t function) override;

   ELENAUnixVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform,
      int codeAlignment, JITSettings gcSettings,
      JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType));
};

}

#endif // ELENALNXVMMACHINE_H

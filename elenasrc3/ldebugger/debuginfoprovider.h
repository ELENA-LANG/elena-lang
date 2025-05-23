//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugInfoProviderBase class declaration
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGINFOPROVIDE_H
#define DEBUGINFOPROVIDE_H

#include "ldebugger\ldbg_common.h"

namespace elena_lang
{
   class DebugInfoProviderBase
   {
   protected:
      typedef MemoryMap<addr_t, addr_t, Map_StoreAddr, Map_GetAddr> ClassInfoMap;
      typedef MemoryMap<ustr_t, addr_t, Map_StoreUStr, Map_GetUStr> SymbolMap;

      addr_t            _entryPoint;
      pos_t             _debugInfoSize;
      addr_t            _debugInfoPtr;

      ModuleMap         _modules;
      ClassInfoMap      _classes;
      SymbolMap         _classNames;

      virtual void retrievePath(ustr_t name, PathString& path, path_t extension) = 0;

      ModuleBase* loadDebugModule(ustr_t reference);

      bool loadSymbol(ustr_t reference, StreamReader& addressReader, DebugProcessBase* process);

   public:
      bool load(StreamReader& reader, bool setEntryAddress, DebugProcessBase* process);

      void clear()
      {
         _entryPoint = 0;
         _debugInfoSize = 0;
         _debugInfoPtr = 0;

         _classes.clear();
         _modules.clear();
         _classNames.clear();

         //_tape.clear();
         //_tapeBookmarks.clear();
      }

      DebugInfoProviderBase();
   };
}

#endif
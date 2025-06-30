//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugInfoProviderBase class declaration
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGINFOPROVIDE_H
#define DEBUGINFOPROVIDE_H

#include "engine/elenacommon.h"
#include "ldebugger/ldbg_common.h"

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

      ModuleBase* getDebugModule(addr_t address);

      bool loadSymbol(ustr_t reference, StreamReader& addressReader, DebugProcessBase* process);

   public:
      static bool loadDebugInfo(path_t debuggee, DebugInfoProviderBase* provider, DebugProcessBase* process);

      static void defineModulePath(ustr_t name, PathString& path, path_t projectPath, path_t outputPath, path_t extension);

      static bool isEqualOrSubSetNs(ustr_t package, ustr_t value)
      {
         return package.compare(value) || (value.compare(package, package.length()) && (value[package.length()] == '\''));
      }

      DebugLineInfo* seekDebugLineInfo(addr_t lineInfoAddress, IdentifierString& moduleName, ustr_t& sourcePath);
      DebugLineInfo* seekDebugLineInfo(size_t lineInfoAddress)
      {
         return (DebugLineInfo*)lineInfoAddress;
      }

      addr_t getEntryPoint()
      {
         return _entryPoint;
      }

      void setEntryPoint(addr_t address)
      {
         _entryPoint = address;
      }
      void setDebugInfoSize(pos_t size)
      {
         _debugInfoSize = size;
      }
      void setDebugInfo(pos_t size, addr_t debugInfoPtr)
      {
         _debugInfoSize = size;
         _debugInfoPtr = debugInfoPtr;
      }
 
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
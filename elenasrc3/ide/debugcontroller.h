//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGCONTROLLER_H
#define DEBUGCONTROLLER_H

#include "idecommon.h"
#include "ideview.h"
#include "elena.h"

namespace elena_lang
{
   // --- DebugInfoProvider ---
   class DebugInfoProvider
   {
      typedef MemoryMap<addr_t, addr_t, Map_StoreAddr, Map_GetAddr> ClassInfoMap;
      typedef MemoryMap<ustr_t, addr_t, Map_StoreUStr, Map_GetUStr> SymbolMap;

      ProjectModel*     _model;

      addr_t            _entryPoint;
      pos_t             _debugInfoSize;
      addr_t            _debugInfoPtr;

      ModuleMap         _modules;
      ClassInfoMap      _classes;
      SymbolMap         _classNames;

      ModuleBase* loadDebugModule(ustr_t reference);

      bool loadSymbol(ustr_t reference, StreamReader& addressReader, DebugProcessBase* process);

      void retrievePath(ustr_t name, PathString& path, path_t extension);

   public:
      addr_t getEntryPoint()
      {
         return _entryPoint;
      }
      addr_t getDebugInfoPtr()
      {
         return _debugInfoPtr;
      }
      pos_t getDebugInfoSize()
      {
         return _debugInfoSize;
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

         //_classes.clear();
         //_modules.clear();
         //_classNames.clear();

         //_tape.clear();
         //_tapeBookmarks.clear();
      }

      DebugInfoProvider(ProjectModel* model)
         : _modules(nullptr), _classes(INVALID_ADDR), _classNames(INVALID_ADDR)
      {
         _model = model;

         clear();
      }
   };

   // --- DebugController ---
   class DebugController : DebugControllerBase
   {
      class DebugReader : public StreamReader
      {
         DebugProcessBase* _debugger;
         addr_t            _address;
         pos_t             _position;
         pos_t             _size;

      public:
         bool eof() override
         {
            return _position >= _size;
         }

         pos_t length() const override
         {
            return _size;
         }

         pos_t position() const override
         {
            return _position;
         }

         bool read(void* s, pos_t length) override
         {
            if (_debugger->readDump(_address + _position, (char*)s, length)) {
               _position += length;

               return true;
            }
            else return false;

         }

         bool seek(pos_t position) override
         {
            _position = position;

            return true;
         }

         DebugReader(DebugProcessBase* debugger, addr_t address, pos_t position)
         {
            _debugger = debugger;
            _address = address;
            _position = position;

            _size = 0;
         }
      };

      bool              _started;
      bool              _running;
      PathString        _debuggee;
      PathString        _arguments;

      DebugProcessBase* _process;
      DebugInfoProvider _provider;

      void debugThread() override;
      void processStep();

      bool startThread();

      void onInitBreakpoint();
      void loadDebugSection(StreamReader& reader, bool starting);
      bool loadDebugData(StreamReader& reader, bool setEntryAddress = false);

   public:
      bool isStarted() const
      {
         return _started;
      }

      bool start(path_t programPath, path_t arguments, bool debugMode);

      void clearBreakpoints();

      void run();

      virtual void clearDebugInfo()
      {
         _provider.clear();
      }

      void release()
      {
         _process->reset();
         clearDebugInfo();
      }

      DebugController(DebugProcessBase* process, ProjectModel* model);
   };
   
}

#endif

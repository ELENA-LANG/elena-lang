//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGCONTROLLER_H
#define DEBUGCONTROLLER_H

#include "idecommon.h"
#include "ideview.h"
#include "elena.h"

namespace elena_lang
{
   constexpr auto DEBUG_MAX_STR_LENGTH = 260;

   // --- DebugSourceController ---
   class DebugSourceController
   {
   public:
      virtual void traceStart(ProjectModel* model) = 0;

      virtual void traceStep(SourceViewModel* sourceModel, bool found, int row) = 0;

      virtual bool selectSource(ProjectModel* model, SourceViewModel* sourceModel, 
         ustr_t moduleName, path_t sourcePath) = 0;

      virtual void traceFinish(SourceViewModel* sourceModel) = 0;

      virtual ~DebugSourceController() = default;
   };

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

      ModuleBase* getDebugModule(addr_t address);

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

      addr_t getClassAddress(ustr_t name);

      bool load(StreamReader& reader, bool setEntryAddress, DebugProcessBase* process);

      ModuleBase* resolveModule(ustr_t ns);

      addr_t findNearestAddress(ModuleBase* module, ustr_t path, int row);

      DebugLineInfo* seekDebugLineInfo(addr_t lineInfoAddress, IdentifierString& moduleName, ustr_t& sourcePath);
      DebugLineInfo* seekDebugLineInfo(size_t lineInfoAddress)
      {
         return (DebugLineInfo*)lineInfoAddress;
      }
      DebugLineInfo* getNextStep(DebugLineInfo* step, bool stepOverMode);

      DebugLineInfo* seekClassInfo(addr_t address, IdentifierString& className, addr_t vmtAddress, ref_t flags);

      void fixNamespace(NamespaceString& str);

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

      struct PostponedStart
      {
         bool             stepMode;
         bool             gotoMode;
         bool             autoNextLine;
         int              col, row;
         IdentifierString source;
         IdentifierString path;

         void clear()
         {
            stepMode = gotoMode = false;
            autoNextLine = false;
            col = row = -1;
            source.clear();
            path.clear();
         }

         void setStepMode()
         {
            clear();
            stepMode = true;
         }

         void setGotoMode(int col, int row, ustr_t source, ustr_t path)
         {
            clear();
            this->gotoMode = true;
            this->col = col;
            this->row = row;
            this->source.copy(source);
            this->path.copy(path);
         }

         PostponedStart()
         {
            clear();
         }
      };

      bool                    _started;
      bool                    _running;
      PathString              _debuggee;
      PathString              _arguments;
      bool                    _witExplicitConsole;

      DebugProcessBase*       _process;
      DebugInfoProvider       _provider;
      PostponedStart          _postponed;

      ProjectModel*           _model;
      SourceViewModel*        _sourceModel;
      DebugSourceController*  _sourceController;

      IdentifierString        _currentModule;
      ustr_t                  _currentPath;

      void debugThread() override;
      void processStep();

      bool startThread();

      void onInitBreakpoint();
      void loadDebugSection(StreamReader& reader, bool starting);
      bool loadDebugData(StreamReader& reader, bool setEntryAddress = false);

      void onCurrentStep(DebugLineInfo* lineInfo, ustr_t moduleName, ustr_t sourcePath);
      void onStop();

      void readObjectContent(ContextBrowserBase* watch, void* item, addr_t address, int level,
         DebugLineInfo* info, addr_t vmtAddress);
      void readFields(ContextBrowserBase* watch, void* parent, addr_t address, int level, DebugLineInfo* info);
      void readObjectArray(ContextBrowserBase* watch, void* parent, addr_t address, int level, DebugLineInfo* info);

      void* readObject(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level, ustr_t className = nullptr, addr_t vmtAddress = 0);
      void* readFieldValue(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level, int size, ustr_t className = nullptr);
      void* readByteLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readIntLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readUIntLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readLongLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readRealLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readByteArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readShortArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readIntArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);
      void* readRealArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level);

      void autoStepOver();

   public:
      bool isStarted() const
      {
         return _started;
      }

      bool start(path_t programPath, path_t arguments, bool debugMode, bool witExplicitConsole);

      void clearBreakpoints();

      void run();
      void stepOver();
      void stepInto();

      void stop();
      void runToCursor(ustr_t name, ustr_t path, int row);
      void toggleBreakpoint(Breakpoint* bp, bool adding);

      void readAutoContext(ContextBrowserBase* watch, int level, WatchItems* refreshedItems);
      void readContext(ContextBrowserBase* watch, void* parentItem, addr_t address, int level);

      void resolveNamespace(NamespaceString& ns);

      virtual void clearDebugInfo()
      {
         _provider.clear();
      }

      void release()
      {
         _process->reset();
         clearDebugInfo();
      }

      DebugController(DebugProcessBase* process, ProjectModel* model, 
         SourceViewModel* sourceModel, DebugSourceController* sourceController);
   };
   
}

#endif

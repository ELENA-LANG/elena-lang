//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef debugcontrollerH
#define debugcontrollerH

#include "elena.h"
#include "idecommon.h"
#include "bytecode.h"

#ifdef _WIN64

#include "winapi64\debugger.h"

#elif _WIN32

#include "winapi32\debugger.h"

#elif _LINUX32

#include "gtk-linux32/debugger.h"

#endif

namespace _ELENA_
{

#define DEBUG_MAX_LIST_LENGTH  300
#define DEBUG_MAX_STR_LENGTH   260
#define DEBUG_MAX_ARRAY_LENGTH 300

// --- DebugController ---

class DebugController : public _DebugController
{
protected:
   struct PostponedStart
   {
      bool             stepMode;
      bool             gotoMode;
      int              col, row;
      IdentifierString source;
      Path             path;

      void clear()
      {
         stepMode = gotoMode = false;
         col = row = -1;
         source.clear();
         path.clear();
      }

      void setStepMode()
      {
         clear();
         stepMode = true;
      }

      void setGotoMode(int col, int row, ident_t source, path_t path)
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

   class DebugReader : public StreamReader
   {
      Debugger* _debugger;
      size_t    _address;
      size_t    _position;
      size_t    _size;

   public:
      virtual bool Eof() { return (_position >= _size); }
      virtual pos_t Position() 
      { 
         // !! temporal : should return 0 for > 2G address
         return (pos_t)_position;
      }

      virtual bool seek(pos_t position)
      {
         //if (_address == 0) {
         //   _address = position;
         //}
         /*else */_position = position;

         return true;
      }
      virtual bool seek(pos64_t position)
      {
         _position = (size_t)position;

         return true;
      }

      virtual bool read(void* s, pos_t length)
      {
         if (_debugger->Context()->readDump(_address + _position, (char*)s, length)) {
            _position += length;

            return true;
         }
         else return false;
      }

      virtual const char* getLiteral(const char* def) { return def; } // !! temporal not supported
      virtual const wide_c* getLiteral(const wide_c* def) { return def; } // !! temporal not supported

      void setSize(size_t size)
      {
         _size = size;
      }

      DebugReader(Debugger* debugger)
      {
         _debugger = debugger;
         _address = 0;
         _position = 0;

         _size = 0;
      }
      DebugReader(Debugger* debugger, size_t address, size_t position)
      {
         _debugger = debugger;
         _address = address;
         _position = position;

         _size = 0;
      }
   };

   // class mapping between vmt ptr and debuglineinfo position in debug module
   typedef MemoryMap<size_t, size_t, false> ClassInfoMap;
   typedef MemoryMap<size_t, int, false>    TapeMap;

   Debugger          _debugger;
   DebugEventManager _events;

   ClassInfoMap      _classes;
   SymbolMap         _classNames;

   //ReferenceMap      _subjects;

   bool              _started;
   bool              _running;
   bool              _autoStepInto;
   Path              _debuggee;
   Path              _arguments;

   size_t			   _entryPoint;
   PostponedStart    _postponed;
   size_t            _debugInfoPtr;  // debug section address
   size_t            _debugInfoSize; // loaded debug section size

   ident_t           _currentModule;
   ident_t           _currentSource;

   _GUI_::_DebugListener*  _listener;
   _GUI_::_ProjectManager* _manager;

   pos_t mapDebugPTR32(void* address);
   void* unmapDebugPTR32(pos_t position);

   bool loadSymbolDebugInfo(ident_t reference, StreamReader& addressReader);
   //void loadSubjectInfo(StreamReader& addressReader);
   bool loadTapeDebugInfo(size_t objectPtr);

   bool loadDebugData(StreamReader& reader, bool setEntryAddress = false);

   _Module* getDebugModule(size_t address);

   DebugLineInfo* seekDebugLineInfo(size_t address, ident_t &moduleName, ident_t &sourcePath);
   DebugLineInfo* seekDebugLineInfo(size_t lineInfoAddress)
   {
      return /*(lineInfoAddress < _tape.Length()) ? (DebugLineInfo*)_tape.get(lineInfoAddress) :*/ (DebugLineInfo*)lineInfoAddress;
   }

   DebugLineInfo* seekClassInfo(uintptr_t address, IdentifierString &className, size_t& flags, size_t vmtAddress = 0);
   DebugLineInfo* seekLineInfo(size_t address, ident_t &moduleName, ident_t &className,
      ident_t &methodName, ident_t &path);

   DebugLineInfo* getNextStep(DebugLineInfo* step, bool stepOverMode);
   DebugLineInfo* getEndStep(DebugLineInfo* step);

   size_t findNearestAddress(_Module* module, ident_t path, int row);

   void readFields(_DebuggerWatch* watch, DebugLineInfo* self, size_t address);
   void readList(_DebuggerWatch* watch, uintptr_t* list, int length);
   void readByteArray(_DebuggerWatch* watch, size_t address, ident_t name);
   void readShortArray(_DebuggerWatch* watch, size_t address, ident_t name);
   void readIntArray(_DebuggerWatch* watch, size_t address, ident_t name);
   void readObject(_DebuggerWatch* watch, size_t selfPtr, ident_t name);
   void readObject(_DebuggerWatch* watch, size_t address, ident_t className, ident_t name);
   void readPString(size_t address, IdentifierString& string);
   void readLocalInt(_DebuggerWatch* watch, size_t selfPtr, ident_t name);
   void readLocalLong(_DebuggerWatch* watch, size_t selfPtr, ident_t name);
   void readLocalReal(_DebuggerWatch* watch, size_t selfPtr, ident_t name);
   void readParams(_DebuggerWatch* watch, size_t selfPtr, ident_t name, bool ignoreInline);
   void readMessage(_DebuggerWatch* watch, size_t selfPtr, ref_t message);

   const char* getValue(size_t address, char* value, size_t length);
   const wide_c* getValue(size_t address, wide_c* value, size_t length);

   void parseMessage(IdentifierString& messageValue, ref_t message);

   int generateTape(int* list, int length);

   bool start();

   virtual void onInitBreakpoint();
   void loadDebugSection(StreamReader& reader, bool starting);

   void processStep();

protected:
   ModuleMap  _modules;
   MemoryDump _tape;
   TapeMap    _tapeBookmarks;

   _ELENA_::_Module* resolveModule(ident_t ns);

   virtual _ELENA_::_Module* loadDebugModule(ident_t reference);

   virtual void debugThread();

   virtual void clearDebugInfo()
   {
      _classes.clear();
      _modules.clear();
      _classNames.clear();

      _tape.clear();
      _tapeBookmarks.clear();

      _debugInfoPtr = 0;
      _debugInfoSize = 0;
   }

public:
   text_t getTemporalSource(int param)
   {
      return (text_t)_tape.get(param);
   }

   void toggleBreakpoint(Breakpoint& breakpoint, bool adding);

   bool isStarted() const { return _debugger.isStarted(); }

//   int getEIP();

   void clearBreakpoints();

   virtual void readAutoContext(_DebuggerWatch* watch);
   virtual void readContext(_DebuggerWatch* watch, size_t selfPtr, size_t classPtr = 0);
   virtual void readCallStack(_DebuggerCallStack* watch);

   void release()
   {
      _debugger.reset();
      clearDebugInfo();
   }

   void showCurrentModule(DebugLineInfo* lineInfo, ident_t moduleName, ident_t sourcePath);
   void showCurrentModule()
   {
      ident_t moduleName = NULL;
      ident_t sourcePath = NULL;
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State(), moduleName, sourcePath);

      showCurrentModule(lineInfo, moduleName, sourcePath);
   }

   bool start(path_t programPath, path_t arguments, bool debugMode, List<Breakpoint>& _breakpoints);
   void run();
   void runToCursor(ident_t name, path_t path, int col, int row);
   void stepOver();
   void stepInto();
   void stop();

   void loadBreakpoints(List<Breakpoint>& breakpoints);

   void assignListener(_GUI_::_DebugListener* listener)
   {
      _listener = listener;
   }

   void assignSourceManager(_GUI_::_ProjectManager* manager)
   {
      _manager = manager;
   }

   DebugController()
      : _modules(NULL, freeobj), _tapeBookmarks(-1)
   {
      _listener = NULL;
      _manager = NULL;

      _started = false;
      _currentModule = NULL;
      _currentSource = NULL;
      _running = false;
      _debugInfoPtr = 0;
      _debugInfoSize = 0;
      _autoStepInto = false;

      //ByteCodeCompiler::loadVerbs(_verbs);
   }

   virtual ~DebugController() {}
};

}

#endif // debugcontrollerH

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef debugcontrollerH
#define debugcontrollerH

#include "elena.h"

#ifdef _WIN32

#include "winapi32\debugger.h"

#elif _LINUX32

#include "gtk-linux32/debugger.h"

#endif

namespace _ELENA_
{

#define DEBUG_MAX_LIST_LENGTH  100
#define DEBUG_MAX_STR_LENGTH   260
#define DEBUG_MAX_ARRAY_LENGTH 100

class DebugController;

// --- _DebuggerWatch ---

class _DebuggerWatch
{
public:
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, const wchar16_t* className) = 0;
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, int value) = 0;
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, double value) = 0;
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, long long value) = 0;
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, char* bytearray, int length) = 0;
   virtual void write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, short* shortarray, int length) = 0;
   virtual void write(_ELENA_::DebugController* controller, const wchar16_t* value) = 0;
   virtual void write(_ELENA_::DebugController* controller, const char* value) = 0;
   virtual void write(_ELENA_::DebugController* controller, int value) = 0;
   virtual void write(_ELENA_::DebugController* controller, double value) = 0;
   virtual void write(_ELENA_::DebugController* controller, long long value) = 0;
   virtual void write(_ELENA_::DebugController* controller, int index, int value) = 0;

   virtual ~_DebuggerWatch() {}
};

// --- _DebuggerWatch ---

class _DebuggerCallStack
{
public:
   virtual void write(const wchar16_t* moduleName, const wchar16_t* className, const wchar16_t* methodName, const wchar16_t* path, 
                        int col, int row, size_t address) = 0;
   virtual void write(size_t address) = 0;

   virtual ~_DebuggerCallStack() {}
};

// --- DebugController ---

class DebugController : _Controller
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

      void setGotoMode(int col, int row, const wchar16_t* source, const wchar16_t* path)
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
      virtual size_t Position() { return _position; }

      virtual bool seek(size_t position)
      {
         _position = position;

         return true;
      }

      virtual bool read(void* s, size_t length)
      {
         _debugger->Context()->readDump(_address + _position, (char*)s, length);

         _position += length;

         return true;
      }

      virtual const wchar16_t* getWideLiteral() { return NULL; } // !! temporal not supported
      virtual const char* getLiteral() { return NULL; } // !! temporal not supported

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
   typedef MemoryMap<ref_t, size_t, false> ClassInfoMap;

   Debugger          _debugger;
   DebugEventManager _events;

   ClassInfoMap      _classes;

   bool              _started;
   bool              _running;
   bool              _debugTape;
   Path              _debuggee;
   Path              _arguments;

   size_t			   _entryPoint;
   PostponedStart    _postponed;
   size_t            _debugInfoPtr;  // debug section address
   size_t            _debugInfoSize; // loaded debug section size

   const wchar16_t*  _currentModule;
   const wchar16_t*  _currentSource;

   bool loadSymbolDebugInfo(const wchar16_t* reference, StreamReader& addressReader);
   bool loadTapeDebugInfo(StreamReader& reader, size_t size);

   bool loadDebugData(StreamReader& reader, bool setEntryAddress = false);

   _Module* getDebugModule(size_t address);

   DebugLineInfo* seekDebugLineInfo(size_t address, const wchar16_t* &moduleName, const wchar16_t* &sourcePath);
   DebugLineInfo* seekDebugLineInfo(size_t lineInfoAddress)
   {
      return (lineInfoAddress < _tape.Length()) ? (DebugLineInfo*)_tape.get(lineInfoAddress) : (DebugLineInfo*)lineInfoAddress;
   }

   DebugLineInfo* seekClassInfo(size_t address, const wchar16_t* &className, int& flags);
   DebugLineInfo* seekLineInfo(size_t address, const wchar16_t* &moduleName, const wchar16_t* &className, 
                               const wchar16_t* &methodName, const wchar16_t* &path);

   DebugLineInfo* getNextStep(DebugLineInfo* step);
   DebugLineInfo* getEndStep(DebugLineInfo* step);

   size_t findNearestAddress(_Module* module, const tchar_t* path, size_t row, size_t col);

   void readFields(_DebuggerWatch* watch, DebugLineInfo* self, size_t address);
   void readList(_DebuggerWatch* watch, int* list, int length);
   void readByteArray(_DebuggerWatch* watch, size_t address, const wchar16_t* name);
   void readShortArray(_DebuggerWatch* watch, size_t address, const wchar16_t* name);
   void readObject(_DebuggerWatch* watch, ref_t selfPtr, const wchar16_t* name, bool ignoreInline);
   void readMessage(_DebuggerWatch* watch, ref_t reference);
   void readPString(size_t address, IdentifierString& string);
   void readLocalInt(_DebuggerWatch* watch, ref_t selfPtr, const wchar16_t* name);
   void readLocalLong(_DebuggerWatch* watch, ref_t selfPtr, const wchar16_t* name);
   void readLocalReal(_DebuggerWatch* watch, ref_t selfPtr, const wchar16_t* name);
   void readParams(_DebuggerWatch* watch, ref_t selfPtr, const wchar16_t* name, bool ignoreInline);

   const char* getValue(size_t address, char* value, size_t length);
   const wchar_t* getValue(size_t address, wchar16_t* value, size_t length);

   int generateTape(void* tape, StreamReader& reader, int breakpointCount);

   bool start();

   void processStep();

   virtual size_t findEntryPoint(const tchar_t* programPath) = 0;

protected:
   ModuleMap  _modules;
   MemoryDump _tape;

   virtual _ELENA_::_Module* loadDebugModule(const wchar16_t* reference) = 0;

   virtual void debugThread();

   virtual void onStart() = 0;
   virtual void onLoadModule(const wchar16_t* name, const wchar16_t* path) = 0;
   virtual void onLoadTape(const wchar16_t* name, int tapePtr) = 0;
   virtual void onStep(const wchar_t* ns, const wchar16_t* source, int row, int disp, int length) = 0;
   virtual void onStop(bool failed) = 0;
   virtual void onCheckPoint(const wchar16_t* message) = 0;
   virtual void onNotification(const wchar16_t* message, size_t address, int code) = 0;
   virtual void onInitBreakpoint();

   virtual void clearDebugInfo()
   {
      _classes.clear();
      _modules.clear();

      _tape.clear();

      _debugInfoPtr = 0;
      _debugInfoSize = 0;
   }

public:
   void allowTapeDebug()
   {
      _debugTape = true;
   }

   const wchar16_t* getTemporalSource(int param)
   {
      return (const wchar16_t*)_tape.get(param);
   }

   void toggleBreakpoint(Breakpoint& breakpoint, bool adding);

   bool isStarted() const { return _debugger.isStarted(); }

   int getEIP();

   void clearBreakpoints();

   void readAutoContext(_DebuggerWatch* watch);
   void readContext(_DebuggerWatch* watch, size_t selfPtr);
   //void readRegisters(_DebuggerWatch* watch);
   void readCallStack(_DebuggerCallStack* watch);

   void release()
   {
      _debugger.reset();
      clearDebugInfo();
   }

   void showCurrentModule(DebugLineInfo* lineInfo, const wchar16_t* moduleName, const wchar16_t* sourcePath);
   void showCurrentModule()
   {
      const wchar16_t* moduleName = NULL;
      const wchar16_t* sourcePath = NULL;
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State(), moduleName, sourcePath);

      showCurrentModule(lineInfo, moduleName, sourcePath);
   }

   bool start(const tchar_t* programPath, const tchar_t* arguments, DebugMode debugMode, List<Breakpoint>& _breakpoints);
   void run();
   void runToCursor(const tchar_t* name, const tchar_t* path, int col, int row);
   void stepOver();
   void stepInto();
   void stepOverLine();
   void stop();

   void loadBreakpoints(List<Breakpoint>& breakpoints);

   DebugController()
      : _modules(NULL, freeobj)
   {
      _started = false;
      _currentModule = NULL;
      _currentSource = NULL;
      _running = false;
      _debugInfoPtr = 0;
      _debugInfoSize = 0;
      _debugTape = false;
   }

   virtual ~DebugController() {}
};

}

#endif // debugcontrollerH

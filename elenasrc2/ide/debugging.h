//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the common Debugger interfaces
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef debuggingH
#define debuggingH

#include "elena.h"

namespace _ELENA_
{

class _DebuggerWatch;

// --- _DebuggerWatch ---

class _DebuggerCallStack
{
public:
   virtual void write(ident_t moduleName, ident_t className, ident_t methodName, ident_t path, int col, int row, size_t address) = 0;
   virtual void write(size_t address) = 0;

   virtual ~_DebuggerCallStack() {}
};

// --- Controller ---

class _DebugController
{
public:
   virtual void debugThread() = 0;

   virtual void readAutoContext(_DebuggerWatch* watch) = 0;
   virtual void readContext(_DebuggerWatch* watch, size_t selfPtr, size_t classPtr = 0) = 0;
   virtual void readCallStack(_DebuggerCallStack* watch) = 0;

   virtual ~_DebugController() {}
};

// --- _DebuggerWatch ---

class _DebuggerWatch
{
public:
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, ident_t className) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, int value) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, double value) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, long long value) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, char* bytearray, int length) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, short* shortarray, int length) = 0;
   virtual void write(_DebugController* controller, size_t address, ident_t variableName, int* intarray, int length) = 0;
   virtual void write(_DebugController* controller, const wide_c* value) = 0;
   virtual void write(_DebugController* controller, const char* value) = 0;
   virtual void write(_DebugController* controller, int value) = 0;
   virtual void write(_DebugController* controller, double value) = 0;
   virtual void write(_DebugController* controller, long long value) = 0;
   virtual void write(_DebugController* controller, int index, int value) = 0;

   virtual void append(_DebugController* controller, ident_t variableName, size_t address, size_t vmtAddress) = 0;

   virtual ~_DebuggerWatch() {}
};

// --- Breakpoint ---

struct Breakpoint
{
   int              row;
   IdentifierString source;
   IdentifierString module;
   void*            param;

   Breakpoint()
   {
      param = NULL;
   }
   Breakpoint(ident_t module, ident_t source, int row, void* param)
   {
      this->module.copy(module);
      this->source.copy(source);
      this->row = row;
      this->param = param;
   }
};

}
#endif // debuggingH

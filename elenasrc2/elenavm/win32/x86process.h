//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//---------------------------------------------------------------------------

#ifndef processH
#define processH 1

#include "elena.h"
#include <windows.h>

namespace _ELENA_
{

// --- x86Process ---

class x86Process : public _Memory
{
   LPVOID      _code;
   LPVOID      _codeEnd;
   size_t      _used, _allocated;
   size_t      _size;

   SYSTEM_INFO _sysInfo;

   bool allocate(size_t size);

   int getProtectedMode(bool writeAccess, bool executeAccess);

public:
   virtual size_t Length() const { return _used; }

   virtual void* get(size_t position) const { return (void*)((size_t)_code + position); }

   virtual bool read(size_t position, void* s, size_t length);

   virtual bool write(size_t position, const void* s, size_t length);

   virtual void insert(size_t position, const void* s, size_t length);

   virtual bool writeBytes(size_t position, char value, size_t length);

   virtual bool exportFunction(path_t rootPath, size_t position, path_t dllName, ident_t funName);

   virtual void trim(size_t size)
   {
      _used = size;
   }

   void protect(bool writeAccess, bool executeAccess);

   x86Process(size_t size, bool writeAccess, bool executeAccess);
   x86Process(size_t size, int address, bool writeAccess, bool executeAccess);
   virtual ~x86Process();
};

} // _ELENA_

#endif // processH

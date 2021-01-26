//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef amd64processH
#define amd64processH 1

#include "elena.h"
#include <windows.h>

namespace _ELENA_
{

// --- AMD64Process ---

class AMD64Process : public _Memory
{
   LPVOID      _code;
   LPVOID      _codeEnd;
   pos_t      _used, _allocated;
   pos_t      _size;

   SYSTEM_INFO _sysInfo;

   bool allocate(size_t size);

   int getProtectedMode(bool writeAccess, bool executeAccess);

public:
   virtual pos_t Length() const { return _used; }

   //virtual void* getLong(pos64_t position) const
   //{
   //   if (position < INT_MAX)
   //   {
   //      return get((pos_t)position);
   //   }
   //   else return NULL;
   //}

   virtual void* get(pos_t position) const 
   { 
      return (void*)((uintptr_t)_code + position);
   }

   virtual bool read(pos_t position, void* s, pos_t length);

   virtual bool readLong(pos64_t position, void* s, pos64_t length)
   {
      if (position < INT_MAX && length < INT_MAX) {
         return read((pos_t)position, s, (pos_t)length);
      }
      else return false;
   }

   virtual bool write(pos_t position, const void* s, pos_t length);

   virtual void insert(pos_t position, const void* s, pos_t length);

   virtual bool writeBytes(pos_t position, char value, pos_t length);

   virtual bool exportFunction(path_t rootPath, pos_t position, path_t dllName, ident_t funName);

   virtual void trim(pos_t size)
   {
      _used = size;
   }
   virtual void trimLong(pos64_t size)
   {
      if (size < INT_MAX)
      {
         _used = (size_t)size;
      }
   }

   void protect(bool writeAccess, bool executeAccess);

   AMD64Process(size_t size, bool writeAccess, bool executeAccess, size_t allocated = 0);
   AMD64Process(size_t size, uintptr_t address, bool writeAccess, bool executeAccess, size_t allocated = 0);
   virtual ~AMD64Process();
};

} // _ELENA_

#endif // amd64processH

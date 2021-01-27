//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "amd64process.h"

using namespace _ELENA_;

// --- AMD64Process ---

AMD64Process:: AMD64Process(size_t size, bool writeAccess, bool executeAccess, size_t allocated)
{
   _allocated = _used = 0;
   _size = size;
   _code = ::VirtualAlloc(NULL, size, MEM_RESERVE, getProtectedMode(writeAccess, executeAccess));
   ::GetSystemInfo(&_sysInfo); 

   if (allocated != 0)
      allocate(allocated);
}

AMD64Process :: AMD64Process(size_t size, uintptr_t address, bool writeAccess, bool executeAccess, size_t allocated)
{
   _allocated = _used = 0;
   _size = size;
   _code = ::VirtualAlloc((LPVOID)address, size, MEM_RESERVE, getProtectedMode(writeAccess, executeAccess));
   ::GetSystemInfo(&_sysInfo); 

   if (allocated != 0)
      allocate(allocated);
}

AMD64Process :: ~AMD64Process()
{
   ::VirtualFree(_code, 0, MEM_RELEASE);
}

int AMD64Process :: getProtectedMode(bool writeAccess, bool executeAccess)
{
   int mode = PAGE_READONLY;
   if (executeAccess) {
      mode = writeAccess ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
   }
   else if (writeAccess) {
      mode = PAGE_EXECUTE_READ;
   }

   return mode;
}

void AMD64Process :: protect(bool writeAccess, bool executeAccess)
{
   DWORD oldprotect;
   ::VirtualProtect((LPVOID)_code, _size, getProtectedMode(writeAccess, executeAccess), &oldprotect);
}

bool AMD64Process:: allocate(size_t size)
{
   if (_used + size > _size)
      return false;

   size_t blockSize = align(size, _sysInfo.dwPageSize);

   ::VirtualAlloc((LPVOID)((size_t)_code + _used), blockSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

   _allocated += blockSize;

   return true;
}

bool AMD64Process :: write(pos_t position, const void* s, pos_t length)
{
   size_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated)
         allocate(length);

      _used = newSize;
   }

   memcpy((LPVOID)((size_t)_code + position), s, length);

   return true;
}

bool AMD64Process:: writeBytes(pos_t position, char value, pos_t length)
{
   size_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated)
         allocate(length);

      _used = newSize;
   }

   memset((LPVOID)((size_t)_code + position), value, length);

   return true;
}

void AMD64Process :: insert(pos_t position, const void* s, pos_t length)
{
   if (_allocated - _used < length)
      allocate(length);

   memmove((LPVOID)((size_t)_code + position + length), (LPVOID)((size_t)_code + position), _used - position - length);
   memcpy((LPVOID)((size_t)_code + position), s, length);
   
   _used += length;
}

bool AMD64Process :: read(pos_t position, void* s, pos_t length)
{
   if (position < _used && _used >= position + length) {
      memcpy(s, (LPVOID)((size_t)_code + position), length);

      return true;
   }
   else return false;
}

bool AMD64Process :: exportFunction(path_t rootPath, pos_t position, path_t dllName, ident_t funName)
{
   HMODULE handle = ::LoadLibrary(dllName);
   // if dll is not found, use root path
   if (handle == NULL) {
      Path dllPath(rootPath);
      dllPath.combine(dllName);

      handle = ::LoadLibrary(dllPath);
   }

   String<char, 200> lpFunName(funName);
   uintptr_t address = (uintptr_t)::GetProcAddress(handle, lpFunName);
   if (address == 0)
      return false;
   
   return write(position, &address, sizeof(uintptr_t));
}

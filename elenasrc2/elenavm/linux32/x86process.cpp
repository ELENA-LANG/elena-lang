//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86process.h"
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>

using namespace _ELENA_;

// --- x86Process ---

x86Process :: x86Process(size_t size, bool writeAccess, bool executeAccess, size_t allocated)
{
   _allocated = _used = 0;
   _size = size;

   _code = mmap(NULL, size, getProtectedMode(writeAccess, executeAccess),
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (_code == (void*)INVALID_REF) {
/*      IdentifierString s;
      s.appendInt(errno);

      ident_t pstr = s;
      for(int i = 0; i < getlength(s); i++)
         putchar(pstr[i]);

      fflush(stdout);
*/
      ::exit(errno);
   }

   if (allocated != 0)
      allocate(allocated);
}

x86Process :: x86Process(size_t size, int address, bool writeAccess, bool executeAccess, size_t allocated)
{
   _allocated = _used = 0;
   _size = size;

   _code = mmap((void*)address, size, getProtectedMode(writeAccess, executeAccess),
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (allocated != 0)
      allocate(allocated);
}

x86Process :: ~x86Process()
{
   munmap(_code, _size);
}

int x86Process :: getProtectedMode(bool writeAccess, bool executeAccess)
{
   int mode = 0;
   if (executeAccess) {
      mode = PROT_READ | PROT_WRITE | PROT_EXEC;
   }
   else mode = PROT_READ | PROT_WRITE;
/*   if (executeAccess) {
      mode |= PROT_EXEC;
   }
   else if (writeAccess) {
      mode |= PROT_WRITE;
   }
*/
   return mode;
}

void x86Process :: protect(bool writeAccess, bool executeAccess)
{
//   /*int retVal = */::mprotect(_code, _size, getProtectedMode(writeAccess, executeAccess));
   /*if (retVal == -1) {
      IdentifierString s;
      s.appendInt(errno);

      ident_t pstr = s;
      for(int i = 0; i < getlength(s); i++)
         putchar(pstr[i]);

      fflush(stdout);
   }*/

}

bool x86Process :: allocate(size_t size)
{
   if (_used + size > _size)
      return false;

   size_t blockSize = align(size, sysconf(_SC_PAGE_SIZE));

   _allocated += blockSize;

   return true;
}

bool x86Process :: write(pos_t position, const void* s, pos_t length)
{
   size_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated)
         allocate(length);

      _used = newSize;
   }

   memcpy((void*)((size_t)_code + position), s, length);

   return true;
}

bool x86Process :: writeBytes(pos_t position, char value, pos_t length)
{
   size_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated)
         allocate(length);

      _used = newSize;
   }

   memset((void*)((size_t)_code + position), value, length);

   return true;
}

void x86Process :: insert(pos_t position, const void* s, pos_t length)
{
   if (_allocated - _used < length)
      allocate(length);

   memmove((void*)((size_t)_code + position + length), (void*)((size_t)_code + position), _used - position - length);
   memcpy((void*)((size_t)_code + position), s, length);

   _used += length;
}

bool x86Process :: read(pos_t position, void* s, pos_t length)
{
   if (position < _used && _used >= position + length) {
      memcpy(s, (void*)((size_t)_code + position), length);

      return true;
   }
   else return false;
}

bool x86Process :: exportFunction(path_t rootPath, size_t position, path_t dllName, ident_t funName)
{
   void* handle = dlopen(dllName, RTLD_LAZY);
   //// if dll is not found, use root path
   //if (handle == NULL) {
   //   Path dllPath(rootPath);
   //   dllPath.combine(dllName);

   //   handle = ::LoadLibrary(dllPath);
   //}

   String<char, 200> lpFunName(funName);
   ref_t address = (ref_t)dlsym(handle, funName);
   if (address == 0)
      return false;

   return write(position, &address, 4);
}

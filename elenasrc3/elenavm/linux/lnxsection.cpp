//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA *nix Image Section implementation
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "linux/lnxsection.h"
#include "langcommon.h"

#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

using namespace elena_lang;

// --- WinImageSection ---

UnixImageSection :: UnixImageSection(pos_t size, bool writeAccess, bool executeAccess, pos_t allocated)
{
   _size = size;
   _allocated = _used = 0;

   _code = mmap(nullptr, size, getProtectedMode(writeAccess, executeAccess),
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (_code == (void*)INVALID_ADDR) {
      ::exit(errno);
   }

   if (allocated != 0)
      allocate(allocated);
}

UnixImageSection :: ~UnixImageSection()
{
   munmap(_code, _size);
}

int UnixImageSection :: getProtectedMode(bool writeAccess, bool executeAccess)
{
   int mode = 0;
   if (executeAccess) {
      mode = PROT_READ | PROT_WRITE | PROT_EXEC;
   }
   else mode = PROT_READ | PROT_WRITE;

   return mode;
}

bool UnixImageSection :: allocate(pos_t size)
{
   if (_used + size > _size)
      return false;

   size_t blockSize = align(size, sysconf(_SC_PAGE_SIZE));

   _allocated += blockSize;

   return true;
}

void* UnixImageSection :: get(pos_t position) const
{
   return (void*)((uintptr_t)_code + position);
}

bool UnixImageSection :: insert(pos_t position, const void* s, pos_t length)
{
   if (_allocated - _used < length) {
      if (!allocate(length))
         return false;
   }

   memmove((void*)((size_t)_code + position + length), (void*)((size_t)_code + position), _used - position - length);
   memcpy((void*)((size_t)_code + position), s, length);

   _used += length;

   return true;
}

pos_t UnixImageSection :: length() const
{
   return (pos_t)_used;
}

bool UnixImageSection :: read(pos_t position, void* s, pos_t length) const
{
   if (position < _used && _used >= position + length) {
      memcpy(s, (void*)((size_t)_code + position), length);

      return true;
   }
   else return false;
}

void UnixImageSection :: trim(pos_t size)
{
   _used = size;
}

bool UnixImageSection :: write(pos_t position, const void* s, pos_t length)
{
   size_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated) {
         if (!allocate(length))
            return false;
      }

      _used = newSize;
   }

   memcpy((void*)((size_t)_code + position), s, length);

   return true;
}

void UnixImageSection :: protect(bool writeAccess, bool executeAccess)
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

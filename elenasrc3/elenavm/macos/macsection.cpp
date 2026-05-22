//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA macOS Image Section implementation
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "macos/macsection.h"
#include "langcommon.h"

#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace elena_lang;

// --- MacOSImageSection ---

static int getMappingMode(bool executeAccess)
{
   return MAP_PRIVATE | MAP_ANONYMOUS | (executeAccess ? MAP_JIT : 0);
}

MacOSImageSection :: MacOSImageSection(pos_t size, bool writeAccess, bool executeAccess, pos_t allocated)
{
   _size = size;
   _allocated = _used = 0;

   _code = mmap(nullptr, size, getProtectedMode(writeAccess, executeAccess),
      getMappingMode(executeAccess), -1, 0);

   if (_code == (void*)INVALID_ADDR) {
      ::exit(errno);
   }

   if (executeAccess)
      pthread_jit_write_protect_np(false);

   if (allocated != 0)
      allocate(allocated);
}

MacOSImageSection :: ~MacOSImageSection()
{
   munmap(_code, _size);
}

int MacOSImageSection :: getProtectedMode(bool writeAccess, bool executeAccess)
{
   int mode = 0;
   if (executeAccess) {
      mode = PROT_READ | PROT_WRITE | PROT_EXEC;
   }
   else mode = PROT_READ | PROT_WRITE;

   return mode;
}

bool MacOSImageSection :: allocate(pos_t size)
{
   if (_used + size > _size)
      return false;

   size_t blockSize = align(size, sysconf(_SC_PAGE_SIZE));

   _allocated += blockSize;

   return true;
}

void* MacOSImageSection :: get(pos_t position) const
{
   return (void*)((uintptr_t)_code + position);
}

bool MacOSImageSection :: insert(pos_t position, const void* s, pos_t length)
{
   if (_allocated - _used < length) {
      if (!allocate(length))
         return false;
   }

   memmove((void*)((size_t)_code + position + length), (void*)((size_t)_code + position), _used - position);
   memcpy((void*)((size_t)_code + position), s, length);

   _used += length;

   return true;
}

pos_t MacOSImageSection :: length() const
{
   return (pos_t)_used;
}

bool MacOSImageSection :: read(pos_t position, void* s, pos_t length) const
{
   if (position < _used && _used >= position + length) {
      memcpy(s, (void*)((size_t)_code + position), length);

      return true;
   }
   else return false;
}

void MacOSImageSection :: trim(pos_t size)
{
   _used = size;
}

bool MacOSImageSection :: write(pos_t position, const void* s, pos_t length)
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

void MacOSImageSection :: protect(bool writeAccess, bool executeAccess)
{
   if (executeAccess) {
      pthread_jit_write_protect_np(true);
   }
   else if (writeAccess) {
      pthread_jit_write_protect_np(false);
   }
}

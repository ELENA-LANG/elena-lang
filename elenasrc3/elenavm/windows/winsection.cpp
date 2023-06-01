//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows Image Section implementation
//
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "windows/winsection.h"
#include "langcommon.h"

using namespace elena_lang;

// --- WinImageSection ---

WinImageSection :: WinImageSection(pos_t size, bool writeAccess, bool executeAccess, pos_t allocated)
{
   _size = size;
   _allocated = _used = 0;

   _section = ::VirtualAlloc(nullptr, size, MEM_RESERVE, getProtectedMode(writeAccess, executeAccess));
   ::GetSystemInfo(&_sysInfo);

   if (allocated != 0)
      allocate(allocated);
}

int WinImageSection :: getProtectedMode(bool writeAccess, bool executeAccess)
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

bool WinImageSection :: allocate(pos_t size)
{
   if (_allocated + size > _size)
      return false;

   pos_t blockSize = align(size, _sysInfo.dwPageSize);

   LPVOID retVal = ::VirtualAlloc((LPVOID)((uintptr_t)_section + _allocated), blockSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
   if (retVal == nullptr)
      return false;

   _allocated += blockSize;

   return true;
}

void* WinImageSection :: get(pos_t position) const
{
   return (void*)((uintptr_t)_section + position);
}

bool WinImageSection :: insert(pos_t position, const void* s, pos_t length)
{
   if (_allocated - _used < length) {
      if (!allocate(length))
         return false;
   }

   memmove((LPVOID)((uintptr_t)_section + position + length), (LPVOID)((uintptr_t)_section + position), _used - position - length);
   memcpy((LPVOID)((uintptr_t)_section + position), s, length);

   return true;
}

pos_t WinImageSection :: length() const
{
   return _used;
}

bool WinImageSection :: read(pos_t position, void* s, pos_t length) const
{
   if (position < _used && _used >= position + length) {
      memcpy(s, (LPVOID)((uintptr_t)_section + position), length);

      return true;
   }
   else return false;
}

void WinImageSection :: trim(pos_t size)
{
   _used = size;
}

bool WinImageSection :: write(pos_t position, const void* s, pos_t length)
{
   pos_t newSize = position + length;

   // check if the operation insert data to the end
   if (newSize > _used) {
      if (newSize > _allocated) {
         if (!allocate(length))
            return false;
      }

      _used = newSize;
   }

   memcpy((LPVOID)((size_t)_section + position), s, length);

   return true;
}

void WinImageSection :: protect(bool writeAccess, bool executeAccess)
{
   DWORD oldprotect;
   ::VirtualProtect(_section, _size, getProtectedMode(writeAccess, executeAccess), &oldprotect);

}

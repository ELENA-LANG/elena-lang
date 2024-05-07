//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows Image Section declaration
//
//                                             (C)2022-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSECTION_H
#define WINSECTION_H

#include "elena.h"
#include <windows.h>

namespace elena_lang
{
   // --- WinImageSection ---
   class WinImageSection : public MemoryBase
   {
      pos_t       _size;
      pos_t       _allocated;
      pos_t       _used;

      void*       _section;

      SYSTEM_INFO _sysInfo;

      int getProtectedMode(bool writeAccess, bool executeAccess);

      bool allocate(pos_t size);

   public:
      void* get(pos_t position) const override;

      bool insert(pos_t position, const void* s, pos_t length) override;

      pos_t length() const override;

      bool read(pos_t position, void* s, pos_t length) const override;

      void trim(pos_t position) override;

      bool write(pos_t position, const void* s, pos_t length) override;

      void protect(bool writeAccess, bool executeAccess);

      WinImageSection(pos_t size, bool writeAccess, bool executeAccess, pos_t allocated = 0);
   };
}

#endif

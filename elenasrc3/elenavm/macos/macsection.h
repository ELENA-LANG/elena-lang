//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA macOS Image Section declaration
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACSECTION_H
#define MACSECTION_H

#include "elena.h"

namespace elena_lang
{
   // --- MacOSImageSection ---
   class MacOSImageSection : public MemoryBase
   {
      void*      _code;
      size_t     _used, _allocated;
      size_t     _size;

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

      MacOSImageSection(pos_t size, bool writeAccess, bool executeAccess, pos_t allocated = 0);
      ~MacOSImageSection();
   };
}

#endif

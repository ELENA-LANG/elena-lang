//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows Image Section declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSECTION_H
#define WINSECTION_H

#include "elena.h"

namespace elena_lang
{
   // --- WinImageSection ---
   class WinImageSection : public MemoryBase
   {
   public:
      void* get(pos_t position) const override;

      bool insert(pos_t position, const void* s, pos_t length) override;

      pos_t length() const override;

      bool read(pos_t position, void* s, pos_t length) override;

      void trim(pos_t position) override;

      bool write(pos_t position, const void* s, pos_t length) override;
   };
}

#endif

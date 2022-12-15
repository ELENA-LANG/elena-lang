//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Windows Image Section implementation
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "windows/winsection.h"
#include "langcommon.h"

using namespace elena_lang;

// --- WinImageSection ---

void* WinImageSection::get(pos_t position) const
{
   throw InternalError(errVMBroken);
}

bool WinImageSection::insert(pos_t position, const void* s, pos_t length)
{
   throw InternalError(errVMBroken);
}

pos_t WinImageSection::length() const
{
   throw InternalError(errVMBroken);
}

bool WinImageSection::read(pos_t position, void* s, pos_t length)
{
   throw InternalError(errVMBroken);
}

void WinImageSection::trim(pos_t position)
{
   throw InternalError(errVMBroken);
}

bool WinImageSection::write(pos_t position, const void* s, pos_t length)
{
   throw InternalError(errVMBroken);
}

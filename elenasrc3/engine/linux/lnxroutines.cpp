//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux ELENA System Routines
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <sys/mman.h>
#include <errno.h>

using namespace elena_lang;

uintptr_t SystemRoutineProvider::NewHeap(int totalSize, int committedSize)
{
   void* allocPtr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (allocPtr == (void*)INVALID_REF) {
      /*IdentifierString s;
      s.appendInt(errno);

      ident_t pstr = s;
      for(int i = 0; i < getlength(s); i++)
         putchar(pstr[i]);*/

      ::exit(errno);
   }

   return (uintptr_t)allocPtr;
}

void SystemRoutineProvider :: Exit(int exitCode)
{
   ::exit(exitCode);
}
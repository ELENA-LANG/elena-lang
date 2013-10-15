//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Linux32 Command line syntax generator header
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "textsource.h"
#include "parsertable.h"
//#include "syntax.h"

#include <stdarg.h>

#define DEFAULT_ENCODING 0

void printLine(const char* msg, ...)
{
   va_list argptr;

   va_start(argptr, msg);
   vprintf(msg, argptr);
   va_end(argptr);
   fflush(stdout);
}

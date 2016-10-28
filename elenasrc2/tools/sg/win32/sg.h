//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                WIN32 Command line syntax generator header
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "textsource.h"
#include "parsertable.h"

#include <windows.h>
#include <stdarg.h>

#define DEFAULT_ENCODING CP_OEMCP

void print(const wchar_t* msg, ...)
{
   va_list argptr;

   va_start(argptr, msg);
   vwprintf(msg, argptr);
   va_end(argptr);
   fflush(stdout);
}

void printLine(const char* msg)
{
   print(_ELENA_::WideString(msg));
}

void printLine(const char* msg, int id)
{
   print(_ELENA_::WideString(msg), id);
}

void printLine(const char* msg, int param1, int param2, wchar_t ch)
{
   print(_ELENA_::WideString(msg), param1, param2, ch);
}

void printLine(const char* msg, const char* param)
{
   print(_ELENA_::WideString(msg), (const wchar_t*)_ELENA_::WideString(param));
}

void printLine(const char* msg, const wchar_t* param)
{
   print(_ELENA_::WideString(msg), param);
}

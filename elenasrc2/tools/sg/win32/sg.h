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
   _ELENA_::String<wchar16_t, 255> wsMsg(msg);

   print(wsMsg);
}

void printLine(const char* msg, int id)
{
   _ELENA_::String<wchar16_t, 255> wsMsg(msg);

   print(wsMsg, id);
}

void printLine(const char* msg, int param1, int param2, wchar_t ch)
{
   _ELENA_::String<wchar16_t, 255> wsMsg(msg);

   print(wsMsg, param1, param2, ch);
}

void printLine(const char* msg, const wchar_t* param)
{
   _ELENA_::String<wchar16_t, 255> wsMsg(msg);

   print(wsMsg, param);
}

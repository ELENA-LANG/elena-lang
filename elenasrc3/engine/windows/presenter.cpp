//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the Windows Presenter implementation
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "presenter.h"
#include <cstdarg>

using namespace elena_lang;

void print(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);

   fflush(stdout);
}

void printLine(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void WinConsolePresenter :: readLine(char* buffer, size_t length)
{
   int maxSize = static_cast<int>(length);

   // !! fgets is used instead of fgetws, because there is a strange bug in fgetws implementation
   fgets(buffer, maxSize, stdin);
}

#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))

inline void adjustConstantForGCC(WideMessage& wstr)
{
   size_t index = (*wstr).findStr(L"%s");
   while (index != NOTFOUND_POS) {
      wstr.insert(L"l", index + 1);

      index = (*wstr).findStr(L"%s");
   }
}

#endif


void WinConsolePresenter :: print(ustr_t msg, ustr_t arg)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif
   WideMessage warg(arg);

   ::print(wstr.str(), warg.str());
}

void WinConsolePresenter :: print(ustr_t msg, ustr_t arg1, ustr_t arg2)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg1(arg1);
   WideMessage warg2(arg2);

   ::print(wstr.str(), warg1.str(), warg2.str());
}

void WinConsolePresenter :: print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg1(arg1);
   WideMessage warg2(arg2);
   WideMessage warg3(arg3);

   ::print(wstr.str(), warg1.str(), warg2.str(), warg3.str());
}

void WinConsolePresenter :: print(ustr_t msg, int arg1, int arg2, int arg3)
{
   WideMessage wstr(msg);

   ::print(wstr.str(), arg1, arg2, arg3);
}

void WinConsolePresenter :: printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg4(arg4);

   ::print(wstr.str(), arg1.str(), arg2, arg3, warg4.str());
}

void WinConsolePresenter :: print(ustr_t msg, int arg1)
{
   WideMessage wstr(msg);

   ::print(wstr.str(), arg1);
}

void WinConsolePresenter :: print(ustr_t msg, int arg1, int arg2)
{
   WideMessage wstr(msg);

   ::print(wstr.str(), arg1, arg2);
}

void WinConsolePresenter :: printPath(ustr_t msg, path_t arg)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   ::print(wstr.str(), arg.str());
}

void WinConsolePresenter :: print(ustr_t msg)
{
   size_t len = msg.length();
   if (len < MESSAGE_LEN) {
      WideMessage wstr(msg);

      ::print(wstr.str());
   }
   else {
      while (len > 0) {
         WideMessage wstr;
         size_t sublen = _min(msg.length(), MESSAGE_LEN);

         wstr.appendUstr(msg, sublen);
         ::print(wstr.str());

         len -= sublen;
         msg = msg + sublen;
      }
   }
}

void WinConsolePresenter :: print(ustr_t msg, ustr_t path, int col, int row, ustr_t s)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage wpath(path);
   WideMessage ws(s);

   ::print(wstr.str(), wpath.str(), row, col, ws.str());
}

void WinConsolePresenter::print(ustr_t msg, ustr_t path, int col, int row, ustr_t s, ustr_t arg)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage wpath(path);
   WideMessage ws(s);
   WideMessage warg(arg);

   ::print(wstr.str(), wpath.str(), row, col, ws.str(), warg.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, ustr_t arg)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg(arg);

   ::printLine(wstr.str(), warg.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, ustr_t arg1, ustr_t arg2)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg1(arg1);
   WideMessage warg2(arg2);

   ::printLine(wstr.str(), warg1.str(), warg2.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg1(arg1);
   WideMessage warg2(arg2);
   WideMessage warg3(arg3);

   ::printLine(wstr.str(), warg1.str(), warg2.str(), warg3.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, int arg1, int arg2, int arg3)
{
   WideMessage wstr(msg);

   ::printLine(wstr.str(), arg1, arg2, arg3);
}

void WinConsolePresenter::printLine(ustr_t msg, int arg1, int arg2, int arg3, ustr_t arg4) //version support print
{
	WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg4(arg4);

	::printLine(wstr.str(), arg1, arg2, arg3, warg4.str());
}

void WinConsolePresenter :: printPathLine(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage warg4(arg4);

   ::printLine(wstr.str(), arg1.str(), arg2, arg3, warg4.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, int arg1)
{
   WideMessage wstr(msg);

   ::printLine(wstr.str(), arg1);
}

void WinConsolePresenter :: printLine(ustr_t msg, int arg1, int arg2)
{
   WideMessage wstr(msg);

   ::printLine(wstr.str(), arg1, arg2);
}

void WinConsolePresenter :: printPathLine(ustr_t msg, path_t arg)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   ::printLine(wstr.str(), arg.str());
}

void WinConsolePresenter :: printLine(ustr_t msg)
{
   WideMessage wstr(msg);

   ::printLine(wstr.str());
}

void WinConsolePresenter :: printLine(ustr_t msg, ustr_t path, int col, int row, ustr_t s)
{
   WideMessage wstr(msg);
#if defined(__GNUC__) && (defined(_WIN32) || defined(__WIN32__))
   adjustConstantForGCC(wstr);
#endif

   WideMessage wpath(path);
   WideMessage ws(s);

   ::printLine(wstr.str(), wpath.str(), row, col, ws.str());
}

void WinConsolePresenter::showProgress()
{
   ::print(L".");
}

void WinConsolePresenter :: stopProgress()
{
   ::print(L"\n");
}

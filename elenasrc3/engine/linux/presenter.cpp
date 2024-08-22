//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the Windows Presenter implementation
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "presenter.h"
#include <cstdarg>

using namespace elena_lang;

void print(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);

   fflush(stdout);
}

void printLine(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void LinuxConsolePresenter :: readLine(char* buffer, size_t length)
{
   fgets(buffer, length, stdin);
}

void LinuxConsolePresenter :: print(ustr_t msg, ustr_t arg)
{
   ::print(msg.str(), arg.str());
}

void LinuxConsolePresenter :: print(ustr_t msg, ustr_t arg1, ustr_t arg2)
{
   ::print(msg.str(), arg1.str(), arg2.str());
}

void LinuxConsolePresenter :: print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3)
{
   ::print(msg.str(), arg1.str(), arg2.str(), arg3.str());
}

void LinuxConsolePresenter :: print(ustr_t msg, int arg1, int arg2, int arg3)
{
   ::print(msg.str(), arg1, arg2, arg3);
}

void LinuxConsolePresenter :: printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4)
{
   ::print(msg.str(), arg1.str(), arg2, arg3, arg4.str());
}

void LinuxConsolePresenter :: print(ustr_t msg, int arg1)
{
   ::print(msg.str(), arg1);
}

void LinuxConsolePresenter :: print(ustr_t msg, int arg1, int arg2)
{
   ::print(msg.str(), arg1, arg2);
}

void LinuxConsolePresenter :: printPath(ustr_t msg, path_t arg)
{
   ::print(msg.str(), arg.str());
}

void LinuxConsolePresenter :: print(ustr_t msg)
{
   ::print(msg.str());
}

void LinuxConsolePresenter :: print(ustr_t msg, ustr_t path, int col, int row, ustr_t s)
{
   ::print(msg.str(), path.str(), row, col, s.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, ustr_t arg)
{
   ::printLine(msg.str(), arg.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, ustr_t arg1, ustr_t arg2)
{
   ::printLine(msg.str(), arg1.str(), arg2.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3)
{
   ::printLine(msg.str(), arg1.str(), arg2.str(), arg3.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, int arg1, int arg2, int arg3, ustr_t arg4)
{
   ::printLine(msg.str(), arg1, arg2, arg3, arg4.str());
}

void LinuxConsolePresenter::printLine(ustr_t msg, int arg1, int arg2, int arg3, ustr_t arg4)
{
   ::printLine(msg.str(), arg1, arg2, arg3, arg4.str());
}

void LinuxConsolePresenter::printLine(ustr_t msg, ustr_t path, int col, int row, ustr_t s)
{
   ::printLine(msg.str(), path.str(), row, col, s.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, int arg1, int arg2, int arg3)
{
   ::printLine(msg.str(), arg1, arg2, arg3);
}

void LinuxConsolePresenter :: printPathLine(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4)
{
   ::printLine(msg.str(), arg1.str(), arg2, arg3, arg4.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg, int arg1)
{
   ::printLine(msg.str(), arg1);
}

void LinuxConsolePresenter :: printLine(ustr_t msg, int arg1, int arg2)
{
   ::printLine(msg.str(), arg1, arg2);
}

void LinuxConsolePresenter :: printPathLine(ustr_t msg, path_t arg)
{
   ::printLine(msg.str(), arg.str());
}

void LinuxConsolePresenter :: printLine(ustr_t msg)
{
   ::printLine(msg.str());
}

void LinuxConsolePresenter::showProgress()
{
   ::print(".");
}

void LinuxConsolePresenter :: stopProgress()
{
   ::print("\n");
}

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ConsoleHelper implementation.
//		Supported platforms: WinAPI
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "consolehelper.h"
#include <windows.h>
#include <conio.h> 

using namespace _ELENA_;

bool ConsoleHelper :: getConsoleSize(int& columns, int& rows)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;

   if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
      columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

      return true;
   }
   else return false;
}

char ConsoleHelper :: getChar()
{
   return _getch();
}
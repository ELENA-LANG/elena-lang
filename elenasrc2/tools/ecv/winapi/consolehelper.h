//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains Console Helper.
//		Supported platforms: WinAPI
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef consolehelperH
#define consolehelperH 1

namespace _ELENA_
{

class ConsoleHelper
{
public:
   static bool getConsoleSize(int& columns, int& rows);

   static char getChar();
};

} // _ELENA_

#endif // consolehelperH

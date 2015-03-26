//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Output class header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winoutputH
#define winoutputH

#include "winapi32\wincommon.h"

namespace _GUI_
{

class Output : public Control
{
protected:
   int    _postponedAction;

   HANDLE _hStdoutRead;
   HANDLE _hProcess;
   HANDLE _hEvtStop;		// event to notify the redir thread to exit
   HANDLE _hThread;		// thread to receive the output of the child process
   DWORD  _dwThreadId;		// id of the redir thread

   HWND   _receptor;    // notify receptor

   static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);

   bool execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, 
					HANDLE hStdOut);

   int redirectStdout();
   
public:
   bool execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, int postponedAction);
   void close();

   wchar_t* getOutput();
   void clear();
   
   Output(Control* owner, Control* receptor);
   ~Output();
};

} // _GUI_

#endif // winoutputH

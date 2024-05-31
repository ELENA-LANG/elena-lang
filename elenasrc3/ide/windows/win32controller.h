//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 OS Controller class and its helpers header
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32CONTROLLER_H
#define WIN32CONTROLLER_H

#include "idecommon.h"
#include <windows.h>

namespace elena_lang
{
   constexpr int BUF_SIZE = 512;

   // -- Win32Process ---
   class Win32Process : public ProcessBase
   {
      HANDLE _hThread;     // thread to receive the output of the child process
      HANDLE _hEvtStop;    // event to notify the redir thread to exit
      DWORD  _dwThreadId;		// id of the redir thread

      DWORD  _dwWaitTime;  // wait time to check the status of the child process

      int    _extraArg;

      char   _buffer[BUF_SIZE];
      size_t _offset;

   protected:
      HANDLE _hStdinWrite;	// write end of child's stdin pipe
      HANDLE _hStdoutRead;	// read end of child's stdout pipe
      HANDLE _hChildProcess;

      static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);

      bool start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir,
         HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr);

      void close();

      int redirectStdout();

      virtual void writeStdOut(const char* output);
      virtual void writeStdError(const char* error);
      virtual void afterExecution(DWORD exitCode);

   public:
      bool start(path_t path, path_t commandLine, path_t curDir, bool readOnly, int extraArg) override;

      void stop(int exitCode = 0) override;

      bool write(wchar_t ch) override;
      bool write(const char* line, size_t length) override;

      void flush();

      Win32Process(int waitTime);
      virtual ~Win32Process();
   };
}

#endif // WIN32CONTROLLER_H
//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Redirect Class
//                                              (C)2005-2016, by Alexei Rakov
//                                              (C)2001, by Jeff Lee
//---------------------------------------------------------------------------

#ifndef wiredirectH
#define wiredirectH

#include "wincommon.h"

namespace _GUI_
{

class RedirectorListener
{
public:
   virtual void afterExecution(DWORD exitCode) = 0;
   virtual void onOutput(const char* text) = 0;

   virtual void clear() = 0;
};

class Redirector
{
   bool   _readOnly;

   HANDLE _hThread;		// thread to receive the output of the child process
   HANDLE _hEvtStop;		// event to notify the redir thread to exit
   DWORD  _dwThreadId;		// id of the redir thread
   DWORD  _dwWaitTime;		// wait time to check the status of the child process

protected:
   HANDLE _hStdinWrite;	// write end of child's stdin pipe
   HANDLE _hStdoutRead;	// read end of child's stdout pipe
   HANDLE _hChildProcess;

   static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);

   bool start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir,
      HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr);

   int redirectStdout();

   virtual void writeStdOut(const char* output) = 0;
   virtual void writeStdError(const char* error) = 0;

   virtual void afterExecution(DWORD exitCode) = 0;

public:
   bool execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir);
   virtual void close();

   bool write(const wchar_t* line);

   Redirector(bool readOnly, size_t waitTime);
   virtual ~Redirector();
};

class WindowRedirector : public Redirector
{
protected:
   RedirectorListener* _target;

   virtual void writeStdOut(const char* output);
   virtual void writeStdError(const char* error);

   virtual void afterExecution(DWORD exitCode);

public:
   WindowRedirector(RedirectorListener* target, bool readOnly, size_t waitTime);
};

};

#endif // wiredirectH
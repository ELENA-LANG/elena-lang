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

   virtual ~RedirectorListener() {}
};

const int BUF_SIZE = 512;

class Redirector
{
   bool   _readOnly;

   HANDLE _hThread;		// thread to receive the output of the child process
   HANDLE _hEvtStop;		// event to notify the redir thread to exit
   DWORD  _dwThreadId;		// id of the redir thread
   DWORD  _dwWaitTime;		// wait time to check the status of the child process

   char   _buffer[BUF_SIZE];
   size_t _offset;

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

   void close();

public:
   bool start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir);   
   void stop(int exitCode = 0);

   bool write(const char* line, size_t length);
   bool write(wchar_t ch);

   void flush();

   Redirector(bool readOnly, int waitTime);
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
   WindowRedirector(RedirectorListener* target, bool readOnly, int waitTime);
};

};

#endif // wiredirectH